/*--------------------------------------------------------------------------------*
  Copyright Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

/**
* @examplesource{AudioRenderer.cpp,PageSampleAudioAudioRenderer}
*
* @brief
*  Sample program for the audio renderer.
*/

/**
* @page PageSampleAudioAudioRenderer  Audio Renderer Sample
* @tableofcontents
*
* @brief
*  Sample program for the audio renderer.
*
* @section PageSampleAudioAudioRenderer_SectionBrief  Overview
*  This sample uses the audio renderer and performs audio playback.
*
* @section PageSampleAudioAudioRenderer_SectionFileStructure  File Structure
*  This sample program is located in
*  <tt>@link ../../../Samples/Sources/Applications/AudioRenderer Samples/Sources/Applications/AudioRenderer/ @endlink</tt>.
*
* @section PageSampleAudioAudioRenderer_SectionNecessaryEnvironment  Required Environment
*  You must be able to use audio output.
*
* @section PageSampleAudioAudioRenderer_SectionHowToOperate  How to Use
*  When the sample program is executed, the BGM and sound effects will be played back automatically.
*  You can use input from the keyboard and DebugPad during playback to manipulate the playing BGM, sound effects, and sine waves.
*
*  <p>
*  <table>
*  <tr><th> Input </th><th> Operation </th></tr>
*  <tr><td> [A]             </td><td> Plays <tt>SampleSe0</tt> once.                           </td></tr>
*  <tr><td> [B]             </td><td> Plays <tt>SampleSe1</tt> once.                           </td></tr>
*  <tr><td> [X]             </td><td> Plays <tt>SampleSe2</tt> once.                           </td></tr>
*  <tr><td> [Y]             </td><td> Plays <tt>SampleSe3</tt> once.                           </td></tr>
*  <tr><td> [L]             </td><td> Enables or disables the biquad filter.           </td></tr>
*  <tr><td> [R]             </td><td> Switches the SampleBgm0-2ch playback state.            </td></tr>
*  <tr><td> [AnalogStickL]  </td><td> Raises and lowers the volume of the sine wave.                       </td></tr>
*  <tr><td> [Left][Right]   </td><td> Changes the volume for the SampleBgm0-2ch left channel.  </td></tr>
*  <tr><td> [Up][Down]      </td><td> Changes the volume for the SampleBgm0-2ch right channel.  </td></tr>
*  <tr><td> [AnalogStickR]  </td><td> Raises and lowers the pitch of the sine wave.                     </td></tr>
*  <tr><td> [Start/Space]   </td><td> Exits the program.                               </td></tr>
*  </table>
*  </p>
*
* @section PageSampleAudioAudioRenderer_SectionPrecaution  Cautions
*  Nothing specific.
*
* @section PageSampleAudioAudioRenderer_SectionHowToExecute  Execution Procedure
*  Build the sample program and then run it.
*
* @section PageSampleAudioAudioRenderer_SectionDetail  Description
*  This sample program performs audio rendering using multiple waveforms.
*
*  This sample program has the following flow.
*
*  - Initialize the file system.
*  - Initialize the audio renderer.
*  - Prepare <tt>FinalMix</tt> and prepare the bus routing using <tt>BufferMixer</tt>.
*  - Set the output device.
*  - Prepare the input waveform, allocate <tt>Voice</tt>, and set parameters.
*  - Initialize audio rendering.
*  - Change parameters and implement state acquisition.
*  - End rendering.
*  - Close the audio renderer.
*  - Free the input waveform.
*
*  The file system is initialized first.
*  Use a resource file that is converted during build.
*  Appropriately change the resource directory and use it.
*
*  Initialize the audio renderer.
*  Before initialization, parameter settings and work buffer preparations must be performed in advance.
*  This work buffer is used for audio rendering processes.
*  To use the audio renderer, you must prepare <tt>nn::audio::AudioRendererConfig</tt> at the same time.
*  This structure is used for managing the audio renderer parameters.
*  The work buffer must also be prepared for <tt>nn::audio::AudioRendererConfig</tt>.
*
*  The audio renderer specifies the output destination of the final output with <tt>Sink</tt>.
*  This sample specifies a <tt>DeviceSinkType</tt> named MainAudioOut.
*  (In the current version of the SDK, sound is output from the Windows <em>prescribed device</em> by specifying this name.)
*
*  Next, effects are prepared. In this demo, bus routing is set using <tt>nn::audio::BufferMixerType</tt>.
*  The effect is set in <tt>nn::audio::FinalMixType</tt>.
*
*  The input waveform and parameter can be specified using <tt>nn::audio::VoiceType</tt>.
*  In this sample, a dynamically generated sine wave and BGM and SE loaded from a file are played.
*  The input waveform is specified with <tt>nn::audio::WaveBuffer</tt>. Up to four buffers can be specified at a time for
*  a <tt>Voice</tt> and they can be played continuously. In addition, the following parameters can be specified.
*
*  - Volume
*  - Pitch
*  - Biquad filter
*  - Mix volume
*
*  For <tt>nn::audio::VoiceType</tt>, the output destination must be specified after allocation.
*
*  The various settings are applied to actual processing by calling the <tt>nn::audio::RequestUpdateAudioRenderer()</tt>
*  function after they have been set in the <tt>nn::audio::AudioRendererConfig</tt> structure.
*
*  Parameter changes, state acquisition, and buffer adding are performed in the main loop.
*  This sample loops BGM while streaming the sine wave data.
*  During playback, you can use input from the keyboard and DebugPad for the operations described in the How to Operate section.
*
*  When a request to exit the program is made, this repeated process is exited and playback stops,
*  the audio renderer is closed, and the memory is freed.
*/

#include <cstdlib>
#include <cmath>
#include <new>

#include <nns/nns_Log.h>
#include <nn/nn_Abort.h>
#include <nn/os.h>
#include <nn/mem.h>
#include <nn/fs.h>
#include <nn/nn_TimeSpan.h>

#include <nn/audio.h>
#include <nns/audio/audio_HidUtilities.h>
#include <nns/audio/audio_WavFormat.h>

#include <nn/hid.h>
#include <nn/hid/hid_KeyboardKey.h>
#include <nn/hid/hid_Npad.h>

#include <nn/settings/settings_DebugPad.h>

namespace {

// Select the rendering engine sample rate.
//const int RenderRate = 48000;
const int RenderRate = 32000;
const int RenderCount = (RenderRate / 200);

// Select the number of files to play.
const int BgmCount = 1;
const int SeCount = 4;

const char Title[] = "AudioRenderer";

// - Add or remove these files from the files lists.
const char* g_BgmFileNames[BgmCount] =
{
    "Contents:/AudioRenderer/SampleBgm0-2ch.wav",
};

const char* g_SeFileNames[SeCount] =
{
    "Contents:/AudioCommon/SampleSe0.adpcm",
    "Contents:/AudioCommon/SampleSe1.adpcm",
    "Contents:/AudioCommon/SampleSe2.adpcm",
    "Contents:/AudioCommon/SampleSe3.adpcm",
};

NN_ALIGNAS(4096) char g_WorkBuffer[8 * 1024 * 1024];
NN_AUDIO_ALIGNAS_MEMORY_POOL_ALIGN char g_WaveBufferPoolMemory[14 * 1024 * 1024];

nn::mem::StandardAllocator g_Allocator;
nn::mem::StandardAllocator g_WaveBufferAllocator;

char* g_MountRomCacheBuffer = NULL;

}

void* Allocate(size_t size)
{
    return g_Allocator.Allocate(size);
}

void Deallocate(void* p, size_t size)
{
    NN_UNUSED(size);
    g_Allocator.Free(p);
}

std::size_t GenerateSineWave(void** data, int sampleRate, int frequency, int sampleCount)
{
    // The entire memory region managed with g_WaveBufferAllocator is added to the memory pool, waveBufferMemoryPool.
    int16_t* p = static_cast<int16_t*>(g_WaveBufferAllocator.Allocate(sampleCount * sizeof(int16_t), nn::audio::BufferAlignSize));
    NN_ABORT_UNLESS_NOT_NULL(p);
    const float Pi = 3.1415926535897932384626433f;
    for (auto i = 0; i < sampleCount; ++i)
    {
        p[i] = static_cast<int16_t>(std::numeric_limits<int16_t>::max() * sinf(2 * Pi * frequency * i / sampleRate));
    }
    *data = p;
    return sampleCount * sizeof(int16_t);
}

std::size_t ReadAdpcmFile(nn::audio::AdpcmHeaderInfo* header, void** adpcmData, const char* filename)
{
    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle, filename, nn::fs::OpenMode_Read);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    int64_t size;
    uint8_t adpcmheader[nn::audio::AdpcmHeaderSize];

    result = nn::fs::GetFileSize(&size, handle);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    *adpcmData = g_WaveBufferAllocator.Allocate(static_cast<std::size_t>(size) - sizeof(adpcmheader), nn::audio::BufferAlignSize);
    NN_ABORT_UNLESS_NOT_NULL(*adpcmData);

    result = nn::fs::ReadFile(handle, 0, adpcmheader, sizeof(adpcmheader));
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    result = nn::fs::ReadFile(handle, sizeof(adpcmheader), *adpcmData, static_cast<size_t>(size) - sizeof(adpcmheader));
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    nn::fs::CloseFile(handle);

    nn::audio::ParseAdpcmHeader(header, adpcmheader, sizeof(adpcmheader));

    return static_cast<std::size_t>(size) - sizeof(adpcmheader);
}

std::size_t ReadWavFile(nns::audio::WavFormat* format, void** data, const char* filename)
{
    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle, filename, nn::fs::OpenMode_Read);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    int64_t size;

    result = nn::fs::GetFileSize(&size, handle);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    *data = g_WaveBufferAllocator.Allocate(static_cast<std::size_t>(size), nn::audio::BufferAlignSize);
    NN_ABORT_UNLESS_NOT_NULL(*data);

    // The DATA chunk must be read, but this sample assumes that it is found to be 1024 bits or less.
    const std::size_t WavHeaderDataSize = 1024;

    result = nn::fs::ReadFile(handle, 0, *data, WavHeaderDataSize);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    nns::audio::WavResult wavResult = nns::audio::ParseWavFormat(format, *data, WavHeaderDataSize);
    NN_ABORT_UNLESS_EQUAL(wavResult, nns::audio::WavResult_Success);
    NN_ABORT_UNLESS_EQUAL(format->bitsPerSample, 16);  // This sample assumes 16-bit PCM.

    result = nn::fs::ReadFile(handle, static_cast<std::size_t>(format->dataOffset), *data, static_cast<std::size_t>(format->dataSize));
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    nn::fs::CloseFile(handle);

    return static_cast<std::size_t>(format->dataSize);
}

void InitializeFileSystem()
{
    nn::fs::SetAllocator(Allocate, Deallocate);

    size_t cacheSize = 0;
    NN_ABORT_UNLESS_RESULT_SUCCESS(nn::fs::QueryMountRomCacheSize(&cacheSize));
    g_MountRomCacheBuffer = new(std::nothrow) char[cacheSize];
    NN_ABORT_UNLESS_NOT_NULL(g_MountRomCacheBuffer);

    NN_ABORT_UNLESS_RESULT_SUCCESS(
        nn::fs::MountRom("Contents", g_MountRomCacheBuffer, cacheSize)
    );
}

void FinalizeFileSystem()
{
    nn::fs::Unmount("Contents");

    delete[] g_MountRomCacheBuffer;
    g_MountRomCacheBuffer = NULL;
}

void InitializeHidDevices()
{
    nn::hid::InitializeDebugPad();
    nn::hid::InitializeNpad();
    const nn::hid::NpadIdType npadIds[2] = { nn::hid::NpadId::No1, nn::hid::NpadId::Handheld };
    nn::hid::SetSupportedNpadStyleSet(nn::hid::NpadStyleFullKey::Mask | nn::hid::NpadStyleHandheld::Mask);
    nn::hid::SetSupportedNpadIdType(npadIds, sizeof(npadIds) / sizeof(npadIds[0]));

    //Map keyboard keys to DebugPad buttons.
    nn::settings::DebugPadKeyboardMap map;
    nn::settings::GetDebugPadKeyboardMap(&map);
    map.buttonA     = nn::hid::KeyboardKey::A::Index;
    map.buttonB     = nn::hid::KeyboardKey::B::Index;
    map.buttonX     = nn::hid::KeyboardKey::X::Index;
    map.buttonY     = nn::hid::KeyboardKey::Y::Index;
    map.buttonL     = nn::hid::KeyboardKey::L::Index;
    map.buttonR     = nn::hid::KeyboardKey::R::Index;
    map.buttonZL    = nn::hid::KeyboardKey::U::Index;
    map.buttonZR    = nn::hid::KeyboardKey::V::Index;
    map.buttonLeft  = nn::hid::KeyboardKey::LeftArrow::Index;
    map.buttonRight = nn::hid::KeyboardKey::RightArrow::Index;
    map.buttonUp    = nn::hid::KeyboardKey::UpArrow::Index;
    map.buttonDown  = nn::hid::KeyboardKey::DownArrow::Index;
    map.buttonStart = nn::hid::KeyboardKey::Space::Index;
    nn::settings::SetDebugPadKeyboardMap(map);
}

void PrintUsage()
{
    NNS_LOG("--------------------------------------------------------\n");
    NNS_LOG("%s Sample\n", Title);
    NNS_LOG("--------------------------------------------------------\n");
    NNS_LOG("[A]             StartSound                   (SampleSe0)\n");
    NNS_LOG("[B]             StartSound                   (SampleSe1)\n");
    NNS_LOG("[X]             StartSound                   (SampleSe2)\n");
    NNS_LOG("[Y]             StartSound                   (SampleSe3)\n");
    NNS_LOG("[L]             Toggle Biquad filter enabled (SampleBgm0-2ch)\n");
    NNS_LOG("[R]             Toggle BGM Play state        (SampleBgm0-2ch)\n");
    NNS_LOG("[AnalogStickL]  ControlVolume                (SineWave)\n");
    NNS_LOG("[Left][Right]   ControlVolume                (SineWave)\n");
    NNS_LOG("[AnalogStickR]  ControlPitch                 (SineWave)\n");
    NNS_LOG("[Up][Down]      ControlPitch                 (SineWave)\n");
    NNS_LOG("[ZLeft/U]       PanVolume (Left)             (SampleBgm0-2ch)\n");
    NNS_LOG("[ZRight/V]      PanVolume (Right)            (SampleBgm0-2ch)\n");
    NNS_LOG("[Start/Space]   Shut down sample program\n");
    NNS_LOG("-------------------------------------------------------\n");
}

extern "C" void nnMain_Sound()
{
    g_Allocator.Initialize(g_WorkBuffer, sizeof(g_WorkBuffer));
    g_WaveBufferAllocator.Initialize(g_WaveBufferPoolMemory, sizeof(g_WaveBufferPoolMemory));

    InitializeFileSystem();
    InitializeHidDevices();

    // Specify a renderer parameter.
    nn::audio::AudioRendererParameter parameter;
    nn::audio::InitializeAudioRendererParameter(&parameter);
    parameter.sampleRate = RenderRate;
    parameter.sampleCount = RenderCount;
    parameter.mixBufferCount = 6 + 2; // FinalMix(6) + SubMix(2)
    parameter.voiceCount = 24;
    parameter.subMixCount = 2;
    parameter.sinkCount = 1;
    parameter.effectCount = 2;
    parameter.performanceFrameCount = 0;

    // Define the relationship between the mix buffer and audio bus.
    int channelCount = 2;
    int8_t mainBus[2];
    mainBus[nn::audio::ChannelMapping_FrontLeft] = 4;
    mainBus[nn::audio::ChannelMapping_FrontRight] = 5;
    int8_t auxBusA[2];
    auxBusA[nn::audio::ChannelMapping_FrontLeft] = 0;
    auxBusA[nn::audio::ChannelMapping_FrontRight] = 1;

    // Check whether the parameter is supported by the system.
    NN_ABORT_UNLESS(
        nn::audio::IsValidAudioRendererParameter(parameter),
        "Invalid AudioRendererParameter specified."
    );

    // Prepare a renderer work buffer.
    size_t workBufferSize = nn::audio::GetAudioRendererWorkBufferSize(parameter);
    void* workBuffer = g_Allocator.Allocate(workBufferSize, nn::os::MemoryPageSize);
    NN_ABORT_UNLESS_NOT_NULL(workBuffer);

    nn::os::SystemEvent systemEvent;

    // Initialize the renderer.
    nn::audio::AudioRendererHandle handle;
    NN_ABORT_UNLESS(
        nn::audio::OpenAudioRenderer(&handle, &systemEvent, parameter, workBuffer, workBufferSize).IsSuccess(),
        "Failed to open AudioRenderer"
    );

    // Initialize AudioRendererConfig.
    size_t configBufferSize = nn::audio::GetAudioRendererConfigWorkBufferSize(parameter);
    void* configBuffer = g_Allocator.Allocate(configBufferSize, nn::os::MemoryPageSize);
    NN_ABORT_UNLESS_NOT_NULL(configBuffer);
    nn::audio::AudioRendererConfig config;
    nn::audio::InitializeAudioRendererConfig(&config, parameter, configBuffer, configBufferSize);

    nn::audio::FinalMixType finalMix;
    nn::audio::AcquireFinalMix(&config, &finalMix, 6);

    nn::audio::SubMixType subMix0;
    nn::audio::AcquireSubMix(&config, &subMix0, parameter.sampleRate, 1);
    nn::audio::SubMixType subMix1;
    nn::audio::AcquireSubMix(&config, &subMix1, parameter.sampleRate, 1);

    // Specify the renderer output destination.
    nn::audio::DeviceSinkType deviceSink;
    // Configure the input for the audio output device.
    // The output channel is set in conjunction with the mix buffer index specified for mainBus.
    // mainBus[nn::audio::ChannelMapping_FrontLeft] is output to the left channel and
    // mainBus[nn::audio::ChannelMapping_FrontRight] is output to the right channel.
    nn::Result result = nn::audio::AddDeviceSink(&config, &deviceSink, &finalMix, mainBus, channelCount, "MainAudioOut");
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    // Connect SubMix(0) to FinalMix.
    nn::audio::SetSubMixDestination(&config, &subMix0, &finalMix);

    // Configure so that you send the 0th buffer of SubMix(0) to mainBus[0]
    // and mainBus[1] of FinalMix, using a volume of 0.5f for the mix.
    nn::audio::SetSubMixMixVolume(&subMix0, &finalMix, 0.5f, 0, mainBus[0]);
    nn::audio::SetSubMixMixVolume(&subMix0, &finalMix, 0.5f, 0, mainBus[1]);

    // Connect SubMix(1) to SubMix(0).
    nn::audio::SetSubMixDestination(&config, &subMix1, &subMix0);

    // Configure so that you send the 0th buffer of SubMix(1) to the 0th buffer of SubMix(0)
    // and mainBus[1] of FinalMix, using a volume of 0.5f for the mix.
    nn::audio::SetSubMixMixVolume(&subMix1, &subMix0, 0.5f, 0, 0);

    // Prepare and register BufferMixer.
    nn::audio::BufferMixerType mixer1;
    result = nn::audio::AddBufferMixer(&config, &mixer1, &finalMix);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    nn::audio::SetBufferMixerInputOutput(&mixer1, auxBusA, mainBus, channelCount);

    // Set buffer mixer parameters.
    nn::audio::SetBufferMixerVolume(&mixer1, 0, 1.0f);
    nn::audio::SetBufferMixerVolume(&mixer1, 1, 1.0f);

    // The specified parameters are applied in the renderer.
    result = nn::audio::RequestUpdateAudioRenderer(handle, &config);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    // Start rendering.
    result = nn::audio::StartAudioRenderer(handle);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    // Prepare Voice.
    nn::audio::VoiceType voiceSine;
    nn::audio::WaveBuffer waveBufferSine;
    void* dataSine;

    // Prepare a memory pool to maintain the sample data added to WaveBuffer.
    nn::audio::MemoryPoolType waveBufferMemoryPool;
    auto ret = AcquireMemoryPool(&config, &waveBufferMemoryPool, g_WaveBufferPoolMemory, sizeof(g_WaveBufferPoolMemory));
    NN_ABORT_UNLESS(ret);
    ret = RequestAttachMemoryPool(&waveBufferMemoryPool);
    NN_ABORT_UNLESS(ret);

    // Prepare the waveform to play with Voice.
    {
        const int sineSampleRate = 32000;
        const int sineFrequency = 440;
        const int sineSampleCount = 32000;
        std::size_t size = GenerateSineWave(&dataSine, sineSampleRate, sineFrequency, sineSampleCount);

        nn::audio::AcquireVoiceSlot(&config, &voiceSine, sineSampleRate, 1, nn::audio::SampleFormat_PcmInt16, nn::audio::VoiceType::PriorityHighest, nullptr, 0);
        nn::audio::SetVoiceDestination(&config, &voiceSine, &subMix1);

        waveBufferSine.buffer = dataSine;
        waveBufferSine.size = size;
        waveBufferSine.startSampleOffset = 0;
        waveBufferSine.endSampleOffset = sineSampleCount;
        waveBufferSine.loop = 0;
        waveBufferSine.isEndOfStream = false;

        // Add the waveform to Voice.
        nn::audio::AppendWaveBuffer(&voiceSine, &waveBufferSine);
        nn::audio::AppendWaveBuffer(&voiceSine, &waveBufferSine);
        nn::audio::AppendWaveBuffer(&voiceSine, &waveBufferSine);
        nn::audio::AppendWaveBuffer(&voiceSine, &waveBufferSine);
        nn::audio::SetVoicePlayState(&voiceSine, nn::audio::VoiceType::PlayState_Play);
        nn::audio::SetVoiceMixVolume(&voiceSine, &subMix1, 0.707f / 2, 0, 0);
    }

    // Background music
    nn::audio::VoiceType voiceBgm[BgmCount];
    nn::audio::WaveBuffer waveBufferBgm;
    void* dataBgm[BgmCount];

    for (int i = 0; i < BgmCount; ++i)
    {
        nns::audio::WavFormat format;
        std::size_t dataSize = ReadWavFile(&format, &dataBgm[i], g_BgmFileNames[i]);

        // Specify the number of channels and get the voices for the loaded WAV file.
        // Note that if multiple channels of data are loaded, a number of voices commensurate with the number of channels is used.
        nn::audio::AcquireVoiceSlot(&config, &voiceBgm[i], format.sampleRate, format.channelCount, nn::audio::SampleFormat_PcmInt16, nn::audio::VoiceType::PriorityHighest, nullptr, 0);
        nn::audio::SetVoiceDestination(&config, &voiceBgm[i], &finalMix);

        waveBufferBgm.buffer = dataBgm[i];
        waveBufferBgm.size = dataSize;
        waveBufferBgm.startSampleOffset = 0;
        waveBufferBgm.endSampleOffset = static_cast<int32_t>(dataSize / sizeof(int16_t)) / format.channelCount;
        waveBufferBgm.loop = true;
        waveBufferBgm.isEndOfStream = false;
        waveBufferBgm.pContext = nullptr;
        waveBufferBgm.contextSize = 0;

        nn::audio::AppendWaveBuffer(&voiceBgm[i], &waveBufferBgm);
        nn::audio::SetVoicePlayState(&voiceBgm[i], nn::audio::VoiceType::PlayState_Play);
        // Set the mix volume, sending the voice's channel 0 to mainBus[0] and channel 1 to mainBus[1].
        nn::audio::SetVoiceMixVolume(&voiceBgm[i], &finalMix, 0.5f, 0, mainBus[0]);
        nn::audio::SetVoiceMixVolume(&voiceBgm[i], &finalMix, 0.5f, 1, mainBus[1]);

        // Set a 2048 Hz cutoff low-pass filter.
        nn::audio::BiquadFilterParameter firstFilter = { true, {720, 1439, 720}, {22684, -9350} };
        nn::audio::SetVoiceBiquadFilterParameter(&voiceBgm[i], 0, firstFilter);
        // Set a high-pass filter with a cutoff frequency of 1024 Hz, but leave it disabled for now.
        nn::audio::BiquadFilterParameter secondFilter = { false, {14041, -28083, 14041}, {29547, -13563} };
        nn::audio::SetVoiceBiquadFilterParameter(&voiceBgm[i], 1, secondFilter);
    }

    // Sound effects
    nn::audio::VoiceType voiceSe[SeCount];
    nn::audio::WaveBuffer waveBufferSe[SeCount];
    nn::audio::AdpcmHeaderInfo* header[SeCount];
    void* dataSe[SeCount];

    for (int i = 0; i < SeCount; ++i)
    {
        header[i] = reinterpret_cast<nn::audio::AdpcmHeaderInfo*>(g_WaveBufferAllocator.Allocate(sizeof(nn::audio::AdpcmHeaderInfo), NN_ALIGNOF(nn::audio::AdpcmHeaderInfo)));
        std::size_t dataSeSize = ReadAdpcmFile(header[i], &dataSe[i], g_SeFileNames[i]);
        nn::audio::AcquireVoiceSlot(&config, &voiceSe[i], header[i]->sampleRate, 1, nn::audio::SampleFormat_Adpcm, nn::audio::VoiceType::PriorityHighest, &header[i]->parameter, sizeof(nn::audio::AdpcmParameter));
        nn::audio::SetVoiceDestination(&config, &voiceSe[i], &finalMix);

        waveBufferSe[i].buffer = dataSe[i];
        waveBufferSe[i].size = dataSeSize;
        waveBufferSe[i].startSampleOffset = 0;
        waveBufferSe[i].endSampleOffset = header[i]->sampleCount;
        waveBufferSe[i].loop = false;
        waveBufferSe[i].isEndOfStream = false;
        waveBufferSe[i].pContext = &header[i]->loopContext;
        waveBufferSe[i].contextSize = sizeof(nn::audio::AdpcmContext);

        nn::audio::AppendWaveBuffer(&voiceSe[i], &waveBufferSe[i]);
        nn::audio::SetVoicePlayState(&voiceSe[i], nn::audio::VoiceType::PlayState_Play);
        nn::audio::SetVoiceMixVolume(&voiceSe[i], &finalMix, 0.707f / 2, 0, auxBusA[0]);
        nn::audio::SetVoiceMixVolume(&voiceSe[i], &finalMix, 0.707f / 2, 0, auxBusA[1]);
    }

    PrintUsage();

    // Wait for the waveform playback to finish and update the parameters.
    for (;;)
    {
        systemEvent.Wait();

        nn::hid::NpadButtonSet npadButtonCurrent = {};
        nn::hid::NpadButtonSet npadButtonDown = {};
        nn::hid::AnalogStickState analogStickStateL = {};
        nn::hid::AnalogStickState analogStickStateR = {};

        // Get the Nad input.
        if (nn::hid::GetNpadStyleSet(nn::hid::NpadId::No1).Test<nn::hid::NpadStyleFullKey>())
        {
            static nn::hid::NpadFullKeyState npadFullKeyState = {};
            nn::hid::NpadFullKeyState state;
            nn::hid::GetNpadState(&state, nn::hid::NpadId::No1);
            npadButtonCurrent |= state.buttons;
            npadButtonDown |= state.buttons & ~npadFullKeyState.buttons;
            analogStickStateL.x += state.analogStickL.x;
            analogStickStateL.y += state.analogStickL.y;
            analogStickStateR.x += state.analogStickR.x;
            analogStickStateR.y += state.analogStickR.y;
            npadFullKeyState = state;
        }
        if (nn::hid::GetNpadStyleSet(nn::hid::NpadId::Handheld).Test<nn::hid::NpadStyleHandheld>())
        {
            static nn::hid::NpadHandheldState npadHandheldState = {};
            nn::hid::NpadHandheldState state;
            nn::hid::GetNpadState(&state, nn::hid::NpadId::Handheld);
            npadButtonCurrent |= state.buttons;
            npadButtonDown |= state.buttons & ~npadHandheldState.buttons;
            analogStickStateL.x += state.analogStickL.x;
            analogStickStateL.y += state.analogStickL.y;
            analogStickStateR.x += state.analogStickR.x;
            analogStickStateR.y += state.analogStickR.y;
            npadHandheldState = state;
        }

        // Get the input from the DebugPad.
        {
            static nn::hid::DebugPadState debugPadState = {};
            nn::hid::DebugPadButtonSet debugPadButtonCurrent = {};
            nn::hid::DebugPadButtonSet debugPadButtonDown = {};
            nn::hid::DebugPadState state;
            nn::hid::GetDebugPadState(&state);
            debugPadButtonCurrent |= state.buttons;
            debugPadButtonDown |= state.buttons & ~debugPadState.buttons;
            analogStickStateL.x += state.analogStickL.x;
            analogStickStateL.y += state.analogStickL.y;
            analogStickStateR.x += state.analogStickR.x;
            analogStickStateR.y += state.analogStickR.y;
            debugPadState = state;
            nns::audio::ConvertDebugPadButtonsToNpadButtons(&npadButtonCurrent, debugPadButtonCurrent);
            nns::audio::ConvertDebugPadButtonsToNpadButtons(&npadButtonDown, debugPadButtonDown);
        }

        //Play sound effects. (The same SE is not overlaid.)
        for (int i = 0; i < SeCount; ++i)
        {
            // SE numbers {0, 1, 2, 3, 4, 5} correspond to buttons {A, B, X, Y, L, R}.
            if(npadButtonDown.Test(i))
            {
                if (nn::audio::GetReleasedWaveBuffer(&voiceSe[i]))
                {
                    nn::audio::AppendWaveBuffer(&voiceSe[i], &waveBufferSe[i]);
                }
            }
        }

        //Manipulate the volume of the sine wave.
        float sineVolume = nn::audio::GetVoiceVolume(&voiceSine) + 0.01f * analogStickStateL.x / (::nn::hid::AnalogStickMax + 1);

        if(npadButtonCurrent.Test< ::nn::hid::NpadButton::Right >())
        {
            sineVolume += 0.01f;
        }
        if(npadButtonCurrent.Test< ::nn::hid::NpadButton::Left >())
        {
            sineVolume -= 0.01f;
        }

        // For the Voice object's volume and mix volume, you can set any value between nn::audio::VoiceType::GetVolumeMin() and nn::audio::VoiceType::GetVolumeMax().
        // To be polite about noise, the ranges in this sample program have been limited to (0.0f, 2.0f) or (0.0f, 1.0f).
        if (sineVolume < 2.0f && sineVolume > 0.0f)
        {
            nn::audio::SetVoiceVolume(&voiceSine, sineVolume);
        }

        for (int i = 0; i < BgmCount; ++i)
        {
            if(npadButtonDown.Test< ::nn::hid::NpadButton::L >())
            {
                // Enable or disable the second biquad filter setting.
                nn::audio::BiquadFilterParameter biquadFilterParameter(nn::audio::GetVoiceBiquadFilterParameter(&voiceBgm[i], 1));
                biquadFilterParameter.enable = !biquadFilterParameter.enable;
                nn::audio::SetVoiceBiquadFilterParameter(&voiceBgm[i], 1, biquadFilterParameter);
            }

            if(npadButtonDown.Test< ::nn::hid::NpadButton::R >())
            {
                // Enable or disable BGM.
                switch (nn::audio::GetVoicePlayState(&voiceBgm[i]))
                {
                case nn::audio::VoiceType::PlayState_Play:
                    nn::audio::SetVoicePlayState(&voiceBgm[i], nn::audio::VoiceType::PlayState_Pause);
                    break;
                case nn::audio::VoiceType::PlayState_Pause:
                case nn::audio::VoiceType::PlayState_Stop:
                    nn::audio::SetVoicePlayState(&voiceBgm[i], nn::audio::VoiceType::PlayState_Play);
                    break;
                default:
                    NN_ABORT("Unexpected play state\n");
                    break;
                }
            }

            // Left channel.
            float bgmLeftVolume = nn::audio::GetVoiceMixVolume(&voiceBgm[i], &finalMix, 0, mainBus[0]);
            if(npadButtonCurrent.Test< ::nn::hid::NpadButton::ZL >())
            {
                bgmLeftVolume += 0.01f;
            }

            if(npadButtonCurrent.Test< ::nn::hid::NpadButton::ZR >())
            {
                bgmLeftVolume -= 0.01f;
            }

            if (bgmLeftVolume < 1.0f && bgmLeftVolume > 0.0f)
            {
                nn::audio::SetVoiceMixVolume(&voiceBgm[i], &finalMix, bgmLeftVolume, 0, mainBus[0]);
            }

            // Right channel.
            float bgmRightVolume = 1.0f - bgmLeftVolume;

            if (bgmRightVolume < 1.0f && bgmRightVolume > 0.0f)
            {
                nn::audio::SetVoiceMixVolume(&voiceBgm[i], &finalMix, bgmRightVolume, 1, mainBus[1]);
            }
        }

        //Manipulate the pitch of the sine wave.
        float sinePitch = nn::audio::GetVoicePitch(&voiceSine) + 0.01f * analogStickStateR.x / (::nn::hid::AnalogStickMax + 1);
        if(npadButtonCurrent.Test< ::nn::hid::NpadButton::Up >())
        {
            sinePitch += 0.01f;
        }
        if(npadButtonCurrent.Test< ::nn::hid::NpadButton::Down >())
        {
            sinePitch -= 0.01f;
        }

        if (sinePitch < nn::audio::VoiceType::GetPitchMax() && sinePitch > nn::audio::VoiceType::GetPitchMin())
        {
            nn::audio::SetVoicePitch(&voiceSine, sinePitch);
        }

        if (const nn::audio::WaveBuffer* pWaveBuffer = nn::audio::GetReleasedWaveBuffer(&voiceSine))
        {
            nn::audio::AppendWaveBuffer(&voiceSine, pWaveBuffer);
        }

        if(npadButtonDown.Test< ::nn::hid::NpadButton::Plus >())
        {
            break;
        }

        result = nn::audio::RequestUpdateAudioRenderer(handle, &config);
        NN_ABORT_UNLESS_RESULT_SUCCESS(result);

    }

    // End rendering.
    nn::audio::StopAudioRenderer(handle);
    nn::audio::CloseAudioRenderer(handle);
    nn::os::DestroySystemEvent(systemEvent.GetBase());

    // Free memory.
    if (dataSine)
    {
        g_WaveBufferAllocator.Free(dataSine);
        dataSine = nullptr;
    }
    for (int i = 0; i < BgmCount; ++i)
    {
        if (dataBgm[i])
        {
            g_WaveBufferAllocator.Free(dataBgm[i]);
            dataBgm[i] = nullptr;
        }
    }
    for (int i = 0; i < SeCount; ++i)
    {
        if (dataSe[i])
        {
            g_WaveBufferAllocator.Free(dataSe[i]);
            dataSe[i] = nullptr;
            g_WaveBufferAllocator.Free(header[i]);
            header[i] = nullptr;
        }
    }
    if (configBuffer)
    {
        g_Allocator.Free(configBuffer);
        configBuffer = nullptr;
    }
    if (workBuffer)
    {
        g_Allocator.Free(workBuffer);
        workBuffer = nullptr;
    }

    FinalizeFileSystem();
}  // NOLINT(readability/fn_size)
