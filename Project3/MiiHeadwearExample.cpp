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
* @examplesource{MiiHeadwearExample.cpp,PageSampleMiiHeadwearExample}
*
* @brief
*  Sample program showing how to display Mii characters wearing accessories, hats, and other headwear.
*/

/**
* @page PageSampleMiiHeadwearExample  Displaying Mii Characters Wearing Headwear Sample
* @tableofcontents
*
* @brief
*  This sample program displays Mii characters wearing accessories, hats, and other headwear.
*
* @section PageSampleMiiHeadwearExample_SectionBrief  Overview
*  This sample displays Mii characters wearing accessories, hats, and other headwear.
*  This sample shows how to get transforms for headwear from <tt>nn::mii::CharModel</tt>.
*
* @section PageSampleMiiHeadwearExample_SectionFileStructure  File Structure
*  This sample program is located in
*  <tt>@link ../../../Samples/Sources/Applications/MiiHeadwearExample Samples/Sources/Applications/MiiHeadwearExample/ @endlink</tt>.
*
* @section PageSampleMiiHeadwearExample_SectionNecessaryEnvironment  Required Environment
*  Nothing specific.
*
* @section PageSampleMiiHeadwearExample_SectionHowToOperate  Using the Demo
*  Nothing specific.
*
* @section PageSampleMiiHeadwearExample_SectionPrecaution  Cautions
*  Nothing specific.
*
* @section PageSampleMiiHeadwearExample_SectionHowToExecute  Execution Procedure
*  Build the sample program and then run it.
*
* @section PageSampleMiiHeadwearExample_SectionDetail  Description
*  In this sample program, Mii characters are displayed wearing accessories, hats, and other headwear.
*
* @subsection PageSampleMiiHeadwearExample_SectionDetail_Convert  Conversion Process
*
*  - <tt>/Samples/Sources/Applications/MiiHeadwearExample/Resources/SampleShader.glsl</tt>
*
*  A simple shader for rendering Mii characters and headwear.
*
* @subsection PageSampleMiiHeadwearExample_SectionDetail_HeadwearModelData  Headwear Model Data
*
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Acce_front.nfsb</tt>
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Acce_side.nfsb</tt>
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Acce_top.nfsb</tt>
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Cap_sample00.nfsb</tt>
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Cap_sample01.nfsb</tt>
*  - <tt>Samples/Sources/Applications/MiiHeadwearExample/Resources/Headwear.nfsb</tt>
*
*  The following six types of sample headwear model data (NFSB files) have been prepared.
*  NFSB files contain binary data in a proprietary format.
*
* @subsection PageSampleMiiHeadwearExample_SectionDetail_RuntimeFlow  Processes of the Sample Program
*  This sample program has the following flow.
*
*  - Initialize the various resources for drawing.
*  - Create a sampler for the Mii.
*  - Get <tt>nn::mii::CharInfo</tt>.
*  - Initialize the shader for the <tt>Faceline</tt> and <tt>Mask</tt> textures (<tt>nn::mii::TextureShader</tt>).
*  - Initialize the Mii resource (<tt>nn::mii::Resource</tt>).
*  - Initialize <tt>nn::mii::CharModel</tt>.
*  - Initialize <tt>nn::mii::Faceline</tt>.
*  - Initialize <tt>nn::mii::Mask</tt>.
*  - Assign descriptor slots.
*  - Initialize <tt>nn::mii::FacelineGpuBuffer</tt>.
*  - Initialize <tt>nn::mii::MaskGpuBuffer</tt>.
*  - Render the <tt>Faceline</tt> texture.
*  - Render the <tt>Mask</tt> texture.
*  - Initialize the shader for rendering the Mii and headwear models..
*  - Initialize the drawing process for Mii rendering.
*  - Set the constant buffers for Mii rendering.
*  - Load a headwear model file.
*  - Initialize vertex state for the headwear model.
*  - Create a buffer for vertex attributes for the headwear model.
*  - Set the constant buffers for rendering the headwear model.
*  - Render the headwear model.
*  - Render the Mii.
*/

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <new>

#include <nn/nn_Assert.h>

#include <nn/os.h>
#include <nn/fs.h>
#include <nn/init.h>
#include <nn/gfx.h>
#include <nn/vi.h>
#include <nn/mii.h>
#include <nn/util/util_Matrix.h>
#include <nns/nns_Log.h>
#include <nn/nn_Abort.h>
#include <nn/os.h>
#include <nn/mem.h>
#include <nn/nn_TimeSpan.h>
#include <nn/audio.h>
#include <nns/audio/audio_HidUtilities.h>
#include <nns/audio/audio_WavFormat.h>
#include <nn/hid.h>
#include <nn/hid/hid_KeyboardKey.h>
#include <nn/hid/hid_Npad.h>
#include <nn/settings/settings_DebugPad.h>
#include <ctime>
#if defined(WIN32)
#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <nn/nn_Windows.h>

#include <nn/hws/hws_Message.h>
#endif

#if NN_GFX_IS_TARGET_NVN
    #include <nvn/nvn.h>
    #include <nvn/nvn_FuncPtrInline.h>
#endif

#if defined( NN_BUILD_CONFIG_OS_SUPPORTS_HORIZON ) && defined( NN_BUILD_CONFIG_SPEC_NX )
#include <nv/nv_MemoryManagement.h>
#endif
#include"SixAxis.h"

using namespace SixAxis;
namespace {

const size_t VisiblePoolMemorySize = 32 * 1024 * 1024;
const size_t CachedPoolMemorySize = 16 * 1024 * 1024;
const size_t CachedCompressPoolMemorySize = 16 * 1024 * 1024;
const size_t MemoryHeapSize = 16 * 1024 * 1024;
const size_t CommandBufferControlMemorySize = 1024;
const size_t CommandBufferMemoryPoolSize = 1024 * 1024;
const size_t SamplerDescriptorCount = 1;
const size_t TextureDescriptorCount = 100;
const nn::gfx::ImageFormat DepthStencilFormat = nn::gfx::ImageFormat_D24_Unorm_S8_Uint;
const int ScanBufferCount = 2;

///  The <tt>CharInfo</tt> path.
const char* CharInfoPath = "Contents:/Common/charinfo/TestCharInfo025.dat";
///  The TextureShader path.
const char* TextureShaderPath = "Contents:/Platform/shader/TextureShader.bnsh";
#if defined( NN_BUILD_CONFIG_SPEC_NX )
#if defined( NN_BUILD_CONFIG_OS_HORIZON )
///  The path to the texture resource for NX.
const char* TextureResourcePath = "Contents:/Common/resource/NXTextureMidSRGB.dat";
#else
///  The path to the texture resource for WinNvn.
const char* TextureResourcePath = "Contents:/Common/resource/WinNvnTextureMidSRGB.dat";
#endif
# else
///  The path to the OpenGL resource.
const char* TextureResourcePath = "Contents:/Common/resource/WinGenericTextureMidSRGB.dat";
#endif
///  The path to the shape resource.
const char* ShapeResourcePath = "Contents:/Common/resource/ShapeMid.dat";
///  Simple shader for rendering the Mii.
const char* SampleShaderPath = "Contents:/Sample/SampleShader.bnsh";

///  Resolution of the mask texture and faceline texture.
const size_t TextureResolution = 512;

///  Number of mipmaps for the mask texture and faceline texture.
const int TextureMipCount = 1;

///  Format for the mask texture and faceline texture.
const nn::gfx::ImageFormat TextureFormat = nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm;

///  Flag for the expression to create.
const int ExpressionFlags = nn::mii::ExpressionFlag_Normal | nn::mii::ExpressionFlag_Smile;

///  Settings for the model to create.
const int CreateFlag = nn::mii::CreateFlag_Normal | nn::mii::CreateFlag_Hat
| nn::mii::CreateFlag_FaceOnly |nn::mii::CreateFlag_NoseNormal;
const nn::mii::CreateNoseType CreateNoseType = nn::mii::CreateNoseType_Normal;

///  The <tt>SampleShader</tt> constant buffer matrix.
struct ConstantBufferMatrix
{
    nn::util::FloatColumnMajor4x3 modelView;
    nn::util::FloatColumnMajor4x4 projection;
};

///  The <tt>Modulate</tt> constant buffer for <tt>SampleShader</tt>.
struct ConstantBufferModulate
{
    int32_t modulateType; ///<  @ref ModulateType
    int32_t pad[3];
    nn::util::Float4 constantColor[3]; ///<  Z-values are not used.
    nn::util::Float3 lightDir;
};

///  Flag for whether to enable vertical flipping of Faceline and Mask.
const bool IsVerticalFlip = false;

///  The light direction.
const nn::util::Float3 LightDirection = NN_UTIL_FLOAT_3_INITIALIZER(-0.1f, 0.1f, 0.5f);

const clock_t begin = clock();
///  The camera position.
const nn::util::Vector3fType CameraPos = NN_UTIL_VECTOR_3F_INITIALIZER(0.0f, 30.0f*(clock()- begin), 150.0f);//NN_UTIL_VECTOR_3F_INITIALIZER(0.0f, 30.0f, 150.0f);
///  The camera viewpoint.
const nn::util::Vector3fType CameraTarget = NN_UTIL_VECTOR_3F_INITIALIZER(0.0f, 30.0f, 0.0f);
nn::util::Vector3fType RotAngle = NN_UTIL_VECTOR_3F_INITIALIZER(36.0f, 45.0f, 0.0f);
///  The type of accessory modulation.
const nn::mii::DrawParam::ModulateType HeadwearModulateType = nn::mii::DrawParam::ModulateType_Constant;

///  Enumeration for the types of headwear.
enum HeadwearType {
    HeadwearType_Front,     ///<  0: For <tt>set_front</tt>.
    HeadwearType_Side,      ///<  1: For <tt>set_side</tt>.
    HeadwearType_Top,       ///<  2: For <tt>set_top</tt>.
    HeadwearType_Cap0,      ///<  3: Hat (cap).
    HeadwearType_Cap1,      ///<  4: Hat (knit cap).
    HeadwearType_Headgear,  ///<  5: Headgear.
    HeadwearType_End,       ///<  6: End.

    HeadwearType_Min = HeadwearType_Front,
    HeadwearType_Max = HeadwearType_Headgear,
};

///  The file path to the headwear.
const char* HeadwearModelPathList[] =
{
    "Contents:/Sample/Acce_front.nfsb",
    "Contents:/Sample/Acce_side.nfsb",
    "Contents:/Sample/Acce_top.nfsb",
    "Contents:/Sample/Cap_sample00.nfsb",
    "Contents:/Sample/Cap_sample01.nfsb",
    "Contents:/Sample/Headgear.nfsb",
};
NN_STATIC_ASSERT((sizeof(HeadwearModelPathList) / sizeof(HeadwearModelPathList[0])) == HeadwearType_End);

///  The type of Mii face model for each kind of headgear.
nn::mii::CreateModelType HeadwearCreateModelTypeList[] =
{
    nn::mii::CreateModelType_Normal,
    nn::mii::CreateModelType_Normal,
    nn::mii::CreateModelType_Normal,
    nn::mii::CreateModelType_Hat,
    nn::mii::CreateModelType_Hat,
    nn::mii::CreateModelType_FaceOnly,
};
NN_STATIC_ASSERT((sizeof(HeadwearCreateModelTypeList) / sizeof(HeadwearCreateModelTypeList[0])) == HeadwearType_End);

///  Set the color for each kind of headgear.
nn::util::Color4f HeadwearColorList[] =
{
    nn::util::Color4f(1.0f,0.0f,0.0f,0.0f),
    nn::util::Color4f(0.0f,1.0f,0.0f,0.0f),
    nn::util::Color4f(0.0f,0.0f,1.0f,0.0f),
    nn::util::Color4f(1.0f,1.0f,0.0f,0.0f),
    nn::util::Color4f(0.0f,1.0f,1.0f,0.0f),
    nn::util::Color4f(1.0f,0.0f,1.0f,0.0f),
};
NN_STATIC_ASSERT((sizeof(HeadwearColorList) / sizeof(HeadwearColorList[0])) == HeadwearType_End);

///  Header for the headwear model file.
struct HeadwearResourceHeader
{
    uint32_t positionOffset;  ///<  Vertex buffer offset (raw data offset).
    uint32_t normalOffset;    ///<  Normal buffer offset (raw data offset).
    uint32_t uvOffset;        ///<  UV buffer offset (raw data offset).

    uint32_t padding_0[2];

    uint32_t positionSize;    ///<  Vertex buffer size (in bytes).
    uint32_t normalSize;      ///<  Normal buffer size (in bytes).
    uint32_t uvSize;          ///<  UV buffer size (in bytes).

    uint32_t padding_1[2];

    uint32_t indexOffset;     ///<  Index buffer offset (raw data offset).
    uint32_t indexSize;       ///<  Index size (in bytes).

    uint32_t padding_2[6];
};

///  Format of vertex attributes for the headwear model.
const nn::gfx::AttributeFormat HeadwearVertexAttributeFormat = nn::gfx::AttributeFormat_32_32_32_Float;
const nn::gfx::AttributeFormat HeadwearUvAttributeFormat = nn::gfx::AttributeFormat_32_32_Float;
const nn::gfx::AttributeFormat HeadwearNormalAttributeFormat = nn::gfx::AttributeFormat_10_10_10_2_Snorm;
///  Index format for the headwear model.
const nn::gfx::IndexFormat HeadwearIndexFormat = nn::gfx::IndexFormat_Uint16;
///  Vertex attribute strides for the headwear model.
const size_t HeadwearPositionStride = sizeof(float) * 3;
const size_t HeadwearUvStride = sizeof(float) * 2;
const size_t HeadwearNormalStride = 4;
///  The number of vertex attributes for the headwear model.
const int HeadwearAttributeNum = 3;

int g_TextureDescriptorBaseIndex = 0;
int g_SamplerDescriptorBaseIndex = 0;
nn::util::Quaternion angle;

//-----------------------------------------------------------------------------
// Memory
nn::util::BytePtr g_pMemoryHeap( NULL );
nn::util::BytePtr g_pMemory( NULL );


void* g_pVisiblePoolMemory = NULL;
void* g_pCachedPoolMemory = NULL;
void* g_pCachedCompressPoolMemory = NULL;
void* g_pMemoryPoolStart = NULL;
ptrdiff_t g_MemoryPoolOffset = 0;
void* g_pCachedMemoryPoolStart = NULL;
ptrdiff_t g_CachedMemoryPoolOffset = 0;
void* g_pCachedCompressMemoryPoolStart = NULL;
ptrdiff_t g_CachedCompressMemoryPoolOffset = 0;

// Initialize the device.
nn::gfx::Device g_Device;
void InitializeDevice()
{
    nn::gfx::Device::InfoType info;
    info.SetDefault();
    info.SetApiVersion( nn::gfx::ApiMajorVersion, nn::gfx::ApiMinorVersion );
    g_Device.Initialize( info );
}

nn::gfx::MemoryPool g_MemoryPool;
void InitializeMemoryPool()
{
    nn::gfx::MemoryPool::InfoType info;
    info.SetDefault();
    info.SetMemoryPoolProperty( nn::gfx::MemoryPoolProperty_CpuUncached
        | nn::gfx::MemoryPoolProperty_GpuCached );

    // Determine bufferAlignment to allocate space on a bufferAlignment boundary.
    size_t alignment = nn::gfx::MemoryPool::GetPoolMemoryAlignment( &g_Device, info );
    g_pVisiblePoolMemory = malloc( VisiblePoolMemorySize + alignment );

    g_pMemoryPoolStart = nn::util::BytePtr( g_pVisiblePoolMemory ).AlignUp( alignment ).Get();
    info.SetPoolMemory( g_pMemoryPoolStart, nn::util::align_down( VisiblePoolMemorySize,
        nn::gfx::MemoryPool::GetPoolMemorySizeGranularity( &g_Device, info ) ) );
    g_MemoryPool.Initialize( &g_Device, info );

    g_MemoryPoolOffset = 0;
}

nn::gfx::MemoryPool g_CachedMemoryPool;
void InitializeCachedMemoryPool()
{
    nn::gfx::MemoryPool::InfoType info;
    info.SetDefault();
    info.SetMemoryPoolProperty( nn::gfx::MemoryPoolProperty_CpuCached
        | nn::gfx::MemoryPoolProperty_GpuCached );

    // Determine bufferAlignment to allocate space on a bufferAlignment boundary.
    size_t alignment = nn::gfx::MemoryPool::GetPoolMemoryAlignment( &g_Device, info );
    g_pCachedPoolMemory = malloc( CachedPoolMemorySize + alignment );

    g_pCachedMemoryPoolStart = nn::util::BytePtr( g_pCachedPoolMemory ).AlignUp( alignment ).Get();
    info.SetPoolMemory( g_pCachedMemoryPoolStart, nn::util::align_down( CachedPoolMemorySize,
        nn::gfx::MemoryPool::GetPoolMemorySizeGranularity( &g_Device, info ) ) );
    g_CachedMemoryPool.Initialize( &g_Device, info );

    g_CachedMemoryPoolOffset = 0;
}

nn::gfx::MemoryPool g_CachedCompressMemoryPool;
void InitializeCachedCompressMemoryPool()
{
    nn::gfx::MemoryPool::InfoType info;
    info.SetDefault();
    info.SetMemoryPoolProperty(nn::gfx::MemoryPoolProperty_CpuCached
        | nn::gfx::MemoryPoolProperty_GpuCached
        | nn::gfx::MemoryPoolProperty_Compressible);

    // Determine the alignment to allocate space on an alignment boundary.
    size_t alignment = nn::gfx::MemoryPool::GetPoolMemoryAlignment(&g_Device, info);
    g_pCachedCompressPoolMemory = malloc(CachedCompressPoolMemorySize + alignment);

    g_pCachedCompressMemoryPoolStart = nn::util::BytePtr(g_pCachedCompressPoolMemory).AlignUp(alignment).Get();
    info.SetPoolMemory(g_pCachedCompressMemoryPoolStart, nn::util::align_down(CachedCompressPoolMemorySize,
        nn::gfx::MemoryPool::GetPoolMemorySizeGranularity(&g_Device, info)));
    g_CachedCompressMemoryPool.Initialize(&g_Device, info);

    g_CachedCompressMemoryPoolOffset = 0;
}

// Initialize the layer.
nn::vi::Display* g_pDisplay;
nn::vi::Layer* g_pLayer;
int g_DisplayWidth = 1280;
int g_DisplayHeight = 720;
void InitializeLayer()
{
    nn::Result result = nn::vi::OpenDefaultDisplay(&g_pDisplay);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
    NN_UNUSED(result);

    result = nn::vi::CreateLayer(&g_pLayer, g_pDisplay);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    result = nn::vi::SetLayerScalingMode(g_pLayer, nn::vi::ScalingMode_FitToLayer);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
}

// Initialize the swap chain.
nn::gfx::SwapChain g_SwapChain;
nn::gfx::ColorTargetView* g_pScanBufferViews[ScanBufferCount];
void InitializeSwapChain()
{
    nn::gfx::SwapChain::InfoType info;

    info.SetDefault();
    info.SetLayer( g_pLayer );
    info.SetWidth( g_DisplayWidth );
    info.SetHeight( g_DisplayHeight );
    info.SetFormat( nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm );
    info.SetBufferCount( ScanBufferCount );
    if( NN_STATIC_CONDITION( nn::gfx::SwapChain::IsMemoryPoolRequired ) )
    {
        size_t size = g_SwapChain.CalculateScanBufferSize( &g_Device, info );
        g_CachedCompressMemoryPoolOffset = nn::util::align_up( g_CachedCompressMemoryPoolOffset,
            nn::gfx::SwapChain::GetScanBufferAlignment( &g_Device, info ) );
        g_SwapChain.Initialize( &g_Device, info, &g_CachedCompressMemoryPool, g_CachedCompressMemoryPoolOffset, size );
        g_CachedCompressMemoryPoolOffset += size;
    }
    else
    {
        g_SwapChain.Initialize( &g_Device, info, NULL, 0, 0 );
    }

    g_SwapChain.GetScanBufferViews(g_pScanBufferViews, ScanBufferCount);
}

// Initialize the queue.
nn::gfx::Queue g_Queue;
void InitializeQueue()
{
    nn::gfx::Queue::InfoType info;
    info.SetDefault();
    info.SetCapability( nn::gfx::QueueCapability_Graphics );
    g_Queue.Initialize( &g_Device, info );
}

// Initialize the command buffer.
nn::gfx::CommandBuffer g_CommandBuffer;
void* g_pCommandBufferControlMemory;
ptrdiff_t g_CommandBufferMemoryPoolOffset;
void InitializeCommandBuffer()
{
    nn::gfx::CommandBuffer::InfoType info;
    info.SetDefault();
    info.SetQueueCapability( nn::gfx::QueueCapability_Graphics );
    info.SetCommandBufferType( nn::gfx::CommandBufferType_Direct );
    g_CommandBuffer.Initialize( &g_Device, info );

    ///  Allocate control memory.
    g_pMemory.AlignUp(256);
    g_pCommandBufferControlMemory = g_pMemory.Get();
    g_pMemory.Advance(CommandBufferControlMemorySize);

    ///  Allocate a memory pool.
    g_MemoryPoolOffset = nn::util::align_up( g_MemoryPoolOffset,
        nn::gfx::CommandBuffer::GetCommandMemoryAlignment( &g_Device ) );
    g_CommandBufferMemoryPoolOffset = g_MemoryPoolOffset;
    g_MemoryPoolOffset += CommandBufferMemoryPoolSize;
}

// Initialize the viewport scissor.
nn::gfx::ViewportScissorState g_ViewportScissor;
void InitializeViewport()
{
    nn::gfx::ViewportScissorState::InfoType info;
    info.SetDefault();
    info.SetScissorEnabled( true );
    nn::gfx::ViewportStateInfo viewportInfo;
    {
        viewportInfo.SetDefault();
        viewportInfo.SetWidth( static_cast< float >( g_DisplayWidth ) );
        viewportInfo.SetHeight( static_cast< float >( g_DisplayHeight ) );
    }
    nn::gfx::ScissorStateInfo scissorInfo;
    {
        scissorInfo.SetDefault();
        scissorInfo.SetWidth( g_DisplayWidth );
        scissorInfo.SetHeight( g_DisplayHeight );
    }
    info.SetViewportStateInfoArray( &viewportInfo, 1 );
    info.SetScissorStateInfoArray( &scissorInfo, 1 );
    g_ViewportScissor.Initialize( &g_Device, info );
}

nn::gfx::DescriptorPool g_SamplerDescriptorPool;
void InitializeSamplerDescriptorPool()
{
    nn::gfx::DescriptorPool::InfoType info;
    info.SetDefault();
    info.SetDescriptorPoolType( nn::gfx::DescriptorPoolType_Sampler );
    info.SetSlotCount( g_SamplerDescriptorBaseIndex + SamplerDescriptorCount );
    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize( &g_Device, info );
    g_CachedMemoryPoolOffset = nn::util::align_up( g_CachedMemoryPoolOffset,
        nn::gfx::DescriptorPool::GetDescriptorPoolAlignment( &g_Device, info ) );
    g_SamplerDescriptorPool.Initialize( &g_Device, info,
        &g_CachedMemoryPool, g_CachedMemoryPoolOffset, size );
    g_CachedMemoryPoolOffset += size;
}

nn::gfx::DescriptorPool g_TextureDescriptorPool;
void InitializeTextureDescriptorPool()
{
    nn::gfx::DescriptorPool::InfoType info;
    info.SetDefault();
    info.SetDescriptorPoolType( nn::gfx::DescriptorPoolType_TextureView );
    info.SetSlotCount( g_TextureDescriptorBaseIndex + TextureDescriptorCount );
    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize( &g_Device, info );
    g_CachedMemoryPoolOffset = nn::util::align_up( g_CachedMemoryPoolOffset,
        nn::gfx::DescriptorPool::GetDescriptorPoolAlignment( &g_Device, info ) );
    g_TextureDescriptorPool.Initialize( &g_Device, info, &g_CachedMemoryPool, g_CachedMemoryPoolOffset, size );
    g_CachedMemoryPoolOffset += size;
}

nn::gfx::Texture g_DepthStencilTexture;
nn::gfx::DepthStencilView g_DepthStencilView;
void InitializeDepthStencilTextureAndView()
{
    {
        nn::gfx::Texture::InfoType info;
        info.SetDefault();
        info.SetImageStorageDimension(nn::gfx::ImageStorageDimension_2d);
        info.SetImageFormat(DepthStencilFormat);
        info.SetGpuAccessFlags(nn::gfx::GpuAccess_DepthStencil);
        info.SetTileMode(nn::gfx::TileMode_Optimal);
        info.SetWidth(g_DisplayWidth);
        info.SetHeight(g_DisplayHeight);

        size_t alignment = nn::gfx::Texture::CalculateMipDataAlignment(&g_Device, info);
        size_t size = nn::gfx::Texture::CalculateMipDataSize(&g_Device, info);

        g_CachedCompressMemoryPoolOffset = nn::util::align_up( g_CachedCompressMemoryPoolOffset, alignment );

        g_DepthStencilTexture.Initialize( &g_Device, info
            , &g_CachedCompressMemoryPool
            , g_CachedCompressMemoryPoolOffset
            , size );

        g_CachedCompressMemoryPoolOffset += size;
    }

    {
        nn::gfx::DepthStencilViewInfo info;
        info.SetDefault();
        info.SetImageDimension(nn::gfx::ImageDimension_2d);
        info.SetMipLevel(0);
        info.SetTexturePtr(&g_DepthStencilTexture);
        g_DepthStencilView.Initialize(&g_Device, info);
    }
}

nn::gfx::Semaphore g_DisplaySemaphore;
void InitializeSemaphore()
{
    nn::gfx::Semaphore::InfoType info;
    info.SetDefault();
    g_DisplaySemaphore.Initialize( &g_Device, info );
}

nn::gfx::Fence g_DisplayFence;
nn::gfx::Fence g_GpuDoneFence;
void InitializeFence()
{
    nn::gfx::Fence::InfoType info;
    info.SetDefault();
    g_DisplayFence.Initialize( &g_Device, info );
    g_GpuDoneFence.Initialize( &g_Device, info );
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
	map.buttonA = nn::hid::KeyboardKey::A::Index;
	map.buttonB = nn::hid::KeyboardKey::B::Index;
	map.buttonX = nn::hid::KeyboardKey::X::Index;
	map.buttonY = nn::hid::KeyboardKey::Y::Index;
	map.buttonL = nn::hid::KeyboardKey::L::Index;
	map.buttonR = nn::hid::KeyboardKey::R::Index;
	map.buttonZL = nn::hid::KeyboardKey::U::Index;
	map.buttonZR = nn::hid::KeyboardKey::V::Index;
	map.buttonLeft = nn::hid::KeyboardKey::LeftArrow::Index;
	map.buttonRight = nn::hid::KeyboardKey::RightArrow::Index;
	map.buttonUp = nn::hid::KeyboardKey::UpArrow::Index;
	map.buttonDown = nn::hid::KeyboardKey::DownArrow::Index;
	map.buttonStart = nn::hid::KeyboardKey::Space::Index;
	nn::settings::SetDebugPadKeyboardMap(map);
}

void InitializeResources()
{
    // Memory
    g_pMemoryHeap.Reset( malloc( MemoryHeapSize ) );
    g_pMemory = g_pMemoryHeap;

    // Initialize the layer.
    nn::vi::Initialize();
    InitializeLayer();

    // Initialize the gfx library.
    nn::gfx::Initialize();

    InitializeDevice();

#if NN_GFX_IS_TARGET_NVN
    nn::gfx::Device::DataType& deviceData = nn::gfx::AccessorToData( g_Device );
    nvnDeviceGetInteger( deviceData.pNvnDevice,
        NVN_DEVICE_INFO_RESERVED_TEXTURE_DESCRIPTORS, &g_TextureDescriptorBaseIndex );
    nvnDeviceGetInteger( deviceData.pNvnDevice,
        NVN_DEVICE_INFO_RESERVED_SAMPLER_DESCRIPTORS, &g_SamplerDescriptorBaseIndex );
#endif

    InitializeMemoryPool();
    InitializeCachedMemoryPool();
    InitializeCachedCompressMemoryPool();

    InitializeSwapChain();
    InitializeQueue();

    InitializeCommandBuffer();
    InitializeViewport();

    InitializeSamplerDescriptorPool();
    InitializeTextureDescriptorPool();

    InitializeDepthStencilTextureAndView();

    InitializeSemaphore();
    InitializeFence();
}

void FinalizeResources()
{
    // Free each object.
    g_GpuDoneFence.Finalize( &g_Device );
    g_DisplayFence.Finalize( &g_Device );
    g_DisplaySemaphore.Finalize( &g_Device );

    g_DepthStencilView.Finalize(&g_Device);
    g_DepthStencilTexture.Finalize(&g_Device);

    g_TextureDescriptorPool.Finalize( &g_Device );
    g_SamplerDescriptorPool.Finalize( &g_Device );

    g_ViewportScissor.Finalize( &g_Device );
    g_CommandBuffer.Finalize( &g_Device );
    g_SwapChain.Finalize( &g_Device );
    g_Queue.Finalize( &g_Device );
    g_CachedCompressMemoryPool.Finalize(&g_Device);
    g_CachedMemoryPool.Finalize( &g_Device );
    g_MemoryPool.Finalize( &g_Device );
    g_Device.Finalize();

    // Finalize the library.
    nn::gfx::Finalize();

    nn::vi::DestroyLayer(g_pLayer);
    nn::vi::CloseDisplay(g_pDisplay);
    nn::vi::Finalize();

    free( g_pMemoryHeap.Get() );
    free( g_pVisiblePoolMemory );
    free( g_pCachedPoolMemory );
    free( g_pCachedCompressPoolMemory );
}

void* Allocate( size_t size )
{
    return malloc( size );
}

void Deallocate( void* ptr, size_t )
{
    free( ptr );
}

#if defined( NN_BUILD_CONFIG_OS_SUPPORTS_HORIZON ) && defined( NN_BUILD_CONFIG_SPEC_NX )
//------------------------------------------------------------------------------
// Functions for Allocating and Freeing Graphics System Memory
//------------------------------------------------------------------------------
static void* NvAllocateFunction(size_t size, size_t alignment, void* userPtr)
{
    NN_UNUSED(userPtr);
    return aligned_alloc(alignment, nn::util::align_up(size, alignment));
}
static void NvFreeFunction(void* addr, void* userPtr)
{
    NN_UNUSED(userPtr);
    free(addr);
}
static void* NvReallocateFunction(void* addr, size_t newSize, void* userPtr)
{
    NN_UNUSED(userPtr);
    return realloc(addr, newSize);
}
#endif

nn::gfx::Sampler g_Sampler;
void InitializeSampler()
{
    nn::gfx::Sampler::InfoType info;

    ///  Set the sampler so it can be used with the Mii.
    nn::mii::GetSamplerInfo(&info);

    ///  Initialize the sampler.
    g_Sampler.Initialize(&g_Device, info);
}

nn::mii::CharInfo g_CharInfo;
void LoadCharInfo()
{
    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle,CharInfoPath,nn::fs::OpenMode_Read);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    result = nn::fs::ReadFile(handle, 0, &g_CharInfo, sizeof(g_CharInfo));
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    nn::fs::CloseFile(handle);

    ///  Check the contents.
    NN_ASSERT(nn::mii::CharInfoAccessor(g_CharInfo).IsValid());
}
nn::gfx::ShaderCodeType GetShaderCodeType(const nn::gfx::ResShaderContainer* pContainer)
{
    for (int i = 0; i < nn::gfx::ShaderCodeType_End;++i)
    {
        auto codeType = nn::gfx::ShaderCodeType(i);
        if(pContainer->GetResShaderVariation(0)->GetResShaderProgram(codeType) != nullptr)
        {
            return codeType;
        }
    }
    NN_ASSERT(0);
    return nn::gfx::ShaderCodeType_Source;
}

nn::gfx::ResShaderFile* InitializeResShaderFile(nn::gfx::ShaderCodeType* pCodeType,const char* pPath)
{
    void* pFile;
    {
        ///  Read the shader file.
        nn::fs::FileHandle handle;
        nn::Result result = nn::fs::OpenFile(&handle,pPath,nn::fs::OpenMode_Read);
        NN_ABORT_UNLESS_RESULT_SUCCESS( result );

        ///  Get the file size.
        int64_t size;
        result = nn::fs::GetFileSize(&size, handle);
        NN_ABORT_UNLESS_RESULT_SUCCESS( result );

        size_t alignment;
        {
            ///  Get the alignment.
            nn::util::BinaryFileHeader header;
            result = nn::fs::ReadFile(handle, 0, &header,sizeof(header));
            NN_ABORT_UNLESS_RESULT_SUCCESS( result );
            alignment = header.GetAlignment();
        }

        ///  Allocate memory for a resource.
        g_pMemory.AlignUp(alignment);
        pFile = g_pMemory.Get();
        g_pMemory.Advance(ptrdiff_t(size));

        ///  Read a resource.
        result = nn::fs::ReadFile(handle, 0, pFile,size_t(size));
        NN_ABORT_UNLESS_RESULT_SUCCESS( result );

        ///  Close.
        nn::fs::CloseFile(handle);
    }

    ///  Cast.
    nn::gfx::ResShaderFile* pShaderFile = nn::gfx::ResShaderFile::ResCast(pFile);

    ///  Initialize the shader.
    nn::gfx::ResShaderContainer* pContainer = pShaderFile->GetShaderContainer();
    *pCodeType = GetShaderCodeType(pContainer);
    pContainer->Initialize(&g_Device);
    for (int i = 0; i < pContainer->GetShaderVariationCount();++i)
    {
        nn::gfx::ResShaderVariation* pVariation = pContainer->GetResShaderVariation(i);
        nn::gfx::ShaderInitializeResult result
            = pVariation->GetResShaderProgram(*pCodeType)->Initialize(&g_Device);
        NN_ASSERT(result == nn::gfx::ShaderInitializeResult_Success);
    }

    return pShaderFile;
}

nn::gfx::ResShaderFile* g_TextureShaderFile;
nn::gfx::ShaderCodeType g_TextureShaderCodeType;
nn::mii::TextureShader g_TextureShader;
nn::mii::TextureShaderInfo g_TextureShaderInfo;
void InitializeTextureShader()
{
    g_TextureShaderFile = InitializeResShaderFile(&g_TextureShaderCodeType, TextureShaderPath);

    ///  Set the format.
    g_TextureShaderInfo.SetFacelineImageFormat(TextureFormat);
    g_TextureShaderInfo.SetMaskImageFormat(TextureFormat);

    ///  Get the alignment and size.
    size_t alignment = nn::mii::TextureShader::CalculateMemoryAlignment(*g_TextureShaderFile, g_TextureShaderCodeType, g_TextureShaderInfo, &g_Device);
    size_t size = nn::mii::TextureShader::CalculateMemorySize(*g_TextureShaderFile, g_TextureShaderCodeType, g_TextureShaderInfo, &g_Device);

    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    nn::Result result = g_TextureShader.Initialize(
        pMemory,size, &g_Device,*g_TextureShaderFile, g_TextureShaderCodeType, g_TextureShaderInfo);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
}

void* ReadFile(size_t* pSize, const char* aPath, size_t alignement)
{
    int64_t size;

    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle, aPath, nn::fs::OpenMode_Read);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Get the file size.
    result = nn::fs::GetFileSize(&size, handle);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Allocate memory.
    g_pMemory.AlignUp(alignement);
    void* ptr = g_pMemory.Get();
    g_pMemory.Advance(ptrdiff_t(size));

    ///  Read the file.
    result = nn::fs::ReadFile(handle, 0, ptr, size_t(size));
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Close.
    nn::fs::CloseFile(handle);

    *pSize = size_t(size);
    return ptr;
}

nn::mii::Resource g_Resource;
void* m_ShapeResourceFile;
void* m_TextureResourceFile;
void InitializeMiiResource()
{
    ///  Read the texture resource file.
    size_t textureFileSize;
    m_TextureResourceFile =
        ReadFile(&textureFileSize, TextureResourcePath, nn::mii::ResourceMemoryAlignment);
    ///  Read the shape resource file.
    size_t shapeFileSize;
    m_ShapeResourceFile =
        ReadFile(&shapeFileSize, ShapeResourcePath, nn::mii::ResourceMemoryAlignment);

    nn::mii::ResourceInfo info;
    info.SetDefault();
    info.SetGammaType(nn::mii::GammaType_Srgb);

    ///  Get the size and alignment.
    size_t alignment = nn::mii::Resource::CalculateMemoryAlignment(
        m_TextureResourceFile, textureFileSize,
        m_ShapeResourceFile, shapeFileSize,
        info);
    size_t size = nn::mii::Resource::CalculateMemorySize(
        m_TextureResourceFile, textureFileSize,
        m_ShapeResourceFile, shapeFileSize,
        info);

    ///  Allocate memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);


    ///  Initialize the resources.
    nn::Result result = g_Resource.Initialize(pMemory,size,
        m_TextureResourceFile, textureFileSize,
        m_ShapeResourceFile, shapeFileSize,
        info, &g_Device);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
}

///  Get the expression of the nth index for the specified flag.
nn::mii::Expression GetExpression(int flag,int index)
{
    int count = 0;
    for (int i = 0; i < nn::mii::Expression_End;++i)
    {
        if ((flag & (1 << i)) != 0)
        {
            if (count == index)
            {
                return nn::mii::Expression(i);
            }
            ++count;
        }
    }
    ///  Does not reach to here.
    return nn::mii::Expression_Normal;
}

nn::mii::CharModel g_CharModel;
void InitializeCharModel()
{
    nn::mii::CharModelInfo info;
    info.SetDefault();
    info.SetDynamicTextureResolution(TextureResolution,TextureResolution);
    info.SetCreateFlag(CreateFlag);
    info.SetDynamicTextureFormat(TextureFormat,TextureFormat);
    info.SetMaskCount(nn::mii::GetExpressionCount(ExpressionFlags));
    info.SetDynamicTextureMipCount(TextureMipCount, TextureMipCount);


    ///  Get the memory size and alignment.
    size_t alignment = nn::mii::CharModel::CalculateMemoryAlignment(info);
    size_t size = nn::mii::CharModel::CalculateMemorySize(info);

    ///  Get the size of the temporary memory.
    size_t tempSize = nn::mii::TemporaryBufferSize;

    ///  Allocate memory and temporary memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    ///  The temporary memory can be freed after the call to CharModel::Initialize().
    nn::mii::TemporaryBuffer* pTempMemory = g_pMemory.Get<nn::mii::TemporaryBuffer>();
    g_pMemory.Advance(tempSize);

    ///  Get the size and alignment of the dynamic memory pool.
    ///  The memory pool for the Faceline and Mask textures.
    ///  Pass a memory pool set to Compress to serve as the memory pool for initializing the textures that are the rendering targets.
    size_t dynamicMemoryPoolAlignment =
        nn::mii::CharModel::CalculateDynamicMemoryPoolAlignment( &g_Device, info );
    size_t dynamicMemoryPoolSize =
        nn::mii::CharModel::CalculateDynamicMemoryPoolSize( &g_Device, info );
    ///  Get the size and alignment of the static memory pool.
    ///  The memory pool for initializing shapes and textures other than the Faceline and Mask textures.
    ///  This memory pool will not be used for initializing the textures that are the rendering targets, so it does not need to be set to Compress.
    size_t staticMemoryPoolAlignment = nn::mii::CharModel::CalculateStaticMemoryPoolAlignment(g_Resource);
    size_t staticMemoryPoolSize = nn::mii::CharModel::CalculateStaticMemoryPoolSize(g_Resource);

    ///  Adjust the memory pool alignment.
    g_CachedCompressMemoryPoolOffset = nn::util::align_up( g_CachedCompressMemoryPoolOffset, dynamicMemoryPoolAlignment );
    g_CachedMemoryPoolOffset = nn::util::align_up( g_CachedMemoryPoolOffset, staticMemoryPoolAlignment );

    ///  Initialize CharModel.
    nn::Result result = g_CharModel.Initialize(
        pMemory, size
        , &g_Device
        , &g_CachedCompressMemoryPool, g_CachedCompressMemoryPoolOffset, dynamicMemoryPoolSize
        , &g_CachedMemoryPool, g_CachedMemoryPoolOffset, staticMemoryPoolSize
        , g_Resource
        , pTempMemory
        , info, g_CharInfo
    );
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Update the offset of the memory pool.
    g_CachedCompressMemoryPoolOffset += dynamicMemoryPoolSize;
    g_CachedMemoryPoolOffset += staticMemoryPoolSize;
}

nn::mii::Faceline g_Faceline;
void InitializeFaceline()
{
    ///  Get the memory size and alignment.
    size_t alignment = nn::mii::Faceline::CalculateMemoryAlignment();
    size_t size = nn::mii::Faceline::CalculateMemorySize();

    ///  Get the size of the temporary memory.
    size_t tempSize = nn::mii::TemporaryBufferSize;

    ///  Allocate memory and temporary memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    ///  The temporary memory can be freed after the call to Faceline::Initialize().
    nn::mii::TemporaryBuffer* pTempMemory = g_pMemory.Get<nn::mii::TemporaryBuffer>();
    g_pMemory.Advance(tempSize);

    ///  Get the memory pool and its size.
    size_t memoryPoolAlignment = nn::mii::Faceline::CalculateMemoryPoolAlignment(
        &g_Device, g_Resource);
    size_t memoryPoolSize = nn::mii::Faceline::CalculateMemoryPoolSize(
        &g_Device, g_Resource);

    ///  Adjust the memory pool alignment.
    g_CachedMemoryPoolOffset = nn::util::align_up( g_CachedMemoryPoolOffset,memoryPoolAlignment);


    g_Faceline.Initialize(pMemory, size, &g_Device
        , &g_CachedMemoryPool, g_CachedMemoryPoolOffset, memoryPoolSize
        , pTempMemory
        , g_Resource, g_CharInfo, IsVerticalFlip);

    ///  Update the offset of the memory pool.
    g_CachedMemoryPoolOffset += memoryPoolSize;
}

nn::mii::Mask g_Mask;
void InitializeMask()
{
    ///  Get the memory size and alignment.
    size_t alignment = nn::mii::Mask::CalculateMemoryAlignment();
    size_t size = nn::mii::Mask::CalculateMemorySize();

    ///  Get the size of the temporary memory.
    size_t tempSize = nn::mii::TemporaryBufferSize;

    ///  Allocate memory and temporary memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    ///  The temporary memory can be freed after the call to Mask::Initialize().
    nn::mii::TemporaryBuffer* pTempMemory = g_pMemory.Get<nn::mii::TemporaryBuffer>();
    g_pMemory.Advance(tempSize);

    ///  Get the memory pool and its size.
    size_t memoryPoolAlignment = nn::mii::Mask::CalculateMemoryPoolAlignment(
        &g_Device, g_Resource, ExpressionFlags);
    size_t memoryPoolSize = nn::mii::Mask::CalculateMemoryPoolSize(
        &g_Device, g_Resource, ExpressionFlags);

    ///  Adjust the memory pool alignment.
    g_CachedMemoryPoolOffset = nn::util::align_up( g_CachedMemoryPoolOffset,memoryPoolAlignment);


    g_Mask.Initialize(pMemory, size, &g_Device
        , &g_CachedMemoryPool, g_CachedMemoryPoolOffset, memoryPoolSize
        , pTempMemory
        , g_Resource, g_CharInfo, ExpressionFlags, IsVerticalFlip);

    ///  Update the offset of the memory pool.
    g_CachedMemoryPoolOffset += memoryPoolSize;
}

nn::gfx::DescriptorSlot g_SamplerSlot;
void BindDescriptorSlot()
{
    ///  Bind the sampler.
    {
        g_SamplerDescriptorPool.BeginUpdate();
        g_SamplerDescriptorPool.SetSampler(g_SamplerDescriptorBaseIndex, &g_Sampler);
        g_SamplerDescriptorPool.GetDescriptorSlot(&g_SamplerSlot,g_SamplerDescriptorBaseIndex);
        ++g_SamplerDescriptorBaseIndex;
        g_SamplerDescriptorPool.EndUpdate();
    }

    ///  Bind the texture view.
    g_TextureDescriptorPool.BeginUpdate();


    ///  Bind CharModel textures other than the Mask texture.
    for (int i = 0; i < nn::mii::CharModel::TextureType_End;++i)
    {
        int mask = 0;
        nn::mii::CharModel::TextureType type = nn::mii::CharModel::TextureType(i);
        const nn::gfx::TextureView* pView = g_CharModel.GetTextureView(type, mask);

        if (pView == 0)
        {
            continue;
        }

        if (type == nn::mii::CharModel::TextureType_Mask)
        {
            continue;
        }

        nn::gfx::DescriptorSlot slot;
        g_TextureDescriptorPool.SetTextureView(g_TextureDescriptorBaseIndex, pView);
        g_TextureDescriptorPool.GetDescriptorSlot(&slot, g_TextureDescriptorBaseIndex);
        ++g_TextureDescriptorBaseIndex;

        ///  Set the obtained slot to CharModel.
        g_CharModel.SetTextureDescriptorSlot(type, slot, mask);
    }

    ///  Bind the Mask texture of CharModel.
    for (int i = 0; i < nn::mii::GetExpressionCount(ExpressionFlags);++i)
    {
        int mask = i;
        nn::mii::CharModel::TextureType type = nn::mii::CharModel::TextureType_Mask;
        const nn::gfx::TextureView* pView = g_CharModel.GetTextureView(type, mask);

        nn::gfx::DescriptorSlot slot;
        g_TextureDescriptorPool.SetTextureView(g_TextureDescriptorBaseIndex, pView);
        g_TextureDescriptorPool.GetDescriptorSlot(&slot, g_TextureDescriptorBaseIndex);
        ++g_TextureDescriptorBaseIndex;

        ///  Set the obtained slot to CharModel.
        g_CharModel.SetTextureDescriptorSlot(type, slot, mask);
    }

    ///  Bind the Faceline texture.
    for (int i = 0; i < nn::mii::Faceline::TextureType_End;++i)
    {
        nn::mii::Faceline::TextureType type = nn::mii::Faceline::TextureType(i);
        const nn::gfx::TextureView* pView = g_Faceline.GetTextureView(type);

        if (pView == 0)
        {
            continue;
        }

        nn::gfx::DescriptorSlot slot;
        g_TextureDescriptorPool.SetTextureView(g_TextureDescriptorBaseIndex, pView);
        g_TextureDescriptorPool.GetDescriptorSlot(&slot, g_TextureDescriptorBaseIndex);
        ++g_TextureDescriptorBaseIndex;

        ///  Set the obtained slot to CharModel.
        g_Faceline.SetTextureDescriptorSlot(type, slot);
    }

    ///  Bind the Mask texture.
    for (int i = 0; i < nn::mii::Mask::TextureType_End;++i)
    {
        nn::mii::Mask::TextureType type = nn::mii::Mask::TextureType(i);
        const nn::gfx::TextureView* pView = g_Mask.GetTextureView(type);

        if (pView == 0)
        {
            continue;
        }

        nn::gfx::DescriptorSlot slot;
        g_TextureDescriptorPool.SetTextureView(g_TextureDescriptorBaseIndex, pView);
        g_TextureDescriptorPool.GetDescriptorSlot(&slot, g_TextureDescriptorBaseIndex);
        ++g_TextureDescriptorBaseIndex;

        ///  Set the obtained slot to CharModel.
        g_Mask.SetTextureDescriptorSlot(type, slot);
    }

    g_TextureDescriptorPool.EndUpdate();
}

nn::mii::FacelineGpuBuffer g_FacelineGpuBuffer;
void InitializeFacelineGpuBuffer()
{
    size_t alignment = nn::mii::FacelineGpuBuffer::CalculateMemoryAlignment();
    size_t size = nn::mii::FacelineGpuBuffer::CalculateMemorySize();

    ///  Allocate memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    ///  Get the memory pool and its size.
    size_t memoryPoolAlignment = nn::mii::FacelineGpuBuffer::CalculateMemoryPoolAlignment(
        &g_Device);
    size_t memoryPoolSize = nn::mii::FacelineGpuBuffer::CalculateMemoryPoolSize(
        &g_Device);

    ///  Adjust the memory pool alignment.
    g_MemoryPoolOffset = nn::util::align_up( g_MemoryPoolOffset,memoryPoolAlignment);

    g_FacelineGpuBuffer.Initialize(
        pMemory, size, &g_Device
        , &g_MemoryPool, g_MemoryPoolOffset, memoryPoolSize
        );

    ///  Update the offset of the memory pool.
    g_MemoryPoolOffset += memoryPoolSize;
}

nn::mii::MaskGpuBuffer g_MaskGpuBuffer;
void InitializeMaskGpuBuffer()
{
    size_t alignment = nn::mii::MaskGpuBuffer::CalculateMemoryAlignment();
    size_t size = nn::mii::MaskGpuBuffer::CalculateMemorySize();

    ///  Allocate memory.
    g_pMemory.AlignUp(alignment);
    void* pMemory = g_pMemory.Get();
    g_pMemory.Advance(size);

    ///  Get the memory pool and its size.
    size_t memoryPoolAlignment = nn::mii::MaskGpuBuffer::CalculateMemoryPoolAlignment(
        &g_Device);
    size_t memoryPoolSize = nn::mii::MaskGpuBuffer::CalculateMemoryPoolSize(
        &g_Device);

    ///  Adjust the memory pool alignment.
    g_MemoryPoolOffset = nn::util::align_up( g_MemoryPoolOffset,memoryPoolAlignment);

    g_MaskGpuBuffer.Initialize(
        pMemory, size, &g_Device
        , &g_MemoryPool, g_MemoryPoolOffset, memoryPoolSize
        );

    ///  Update the offset of the memory pool.
    g_MemoryPoolOffset += memoryPoolSize;
}

void RenderFacelineTexture()
{
    ///  Set the GPU buffer.
    g_FacelineGpuBuffer.SetColorTarget(&g_Device, g_CharModel.GetFacelineTexture()
                                       ,g_TextureShaderInfo
                                       ,TextureResolution / 2,TextureResolution, TextureMipCount);
    g_FacelineGpuBuffer.SetFaceline(g_Faceline);


    ///  Draw using the GPU.
    g_CommandBuffer.Reset();
    g_CommandBuffer.AddControlMemory(
        g_pCommandBufferControlMemory, CommandBufferControlMemorySize);
    g_CommandBuffer.AddCommandMemory(
        &g_MemoryPool,g_CommandBufferMemoryPoolOffset,CommandBufferMemoryPoolSize);

    g_CommandBuffer.Begin();

    g_CommandBuffer.SetDescriptorPool(&g_TextureDescriptorPool);
    g_CommandBuffer.SetDescriptorPool(&g_SamplerDescriptorPool);

    g_CommandBuffer.InvalidateMemory( nn::gfx::GpuAccess_Texture | nn::gfx::GpuAccess_IndexBuffer
        | nn::gfx::GpuAccess_ConstantBuffer | nn::gfx::GpuAccess_VertexBuffer );

    g_TextureShader.DrawFaceline(&g_CommandBuffer, &g_FacelineGpuBuffer, g_SamplerSlot);

    g_CommandBuffer.End();

    ///  Execute commands and wait for them to complete.
    g_Queue.ExecuteCommand(&g_CommandBuffer, 0);
    g_Queue.Sync();
}

void RenderMaskTexture()
{
    for (int i = 0; i < nn::mii::GetExpressionCount(ExpressionFlags);++i)
    {
        ///  Get the expression.
        nn::mii::Expression expression = GetExpression(ExpressionFlags, i);
        ///  Set the GPU buffer.
        g_MaskGpuBuffer.SetColorTarget(&g_Device, g_CharModel.GetMaskTexture(i)
                                           ,g_TextureShaderInfo
                                           ,TextureResolution, TextureMipCount);
        g_MaskGpuBuffer.SetMask(g_Mask,expression);

        ///  Draw using the GPU.
        g_CommandBuffer.Reset();
        g_CommandBuffer.AddControlMemory(
            g_pCommandBufferControlMemory, CommandBufferControlMemorySize);
        g_CommandBuffer.AddCommandMemory(
            &g_MemoryPool,g_CommandBufferMemoryPoolOffset,CommandBufferMemoryPoolSize);

        g_CommandBuffer.Begin();

        g_CommandBuffer.SetDescriptorPool(&g_TextureDescriptorPool);
        g_CommandBuffer.SetDescriptorPool(&g_SamplerDescriptorPool);

        g_CommandBuffer.InvalidateMemory( nn::gfx::GpuAccess_Texture | nn::gfx::GpuAccess_IndexBuffer
            | nn::gfx::GpuAccess_ConstantBuffer | nn::gfx::GpuAccess_VertexBuffer );

        g_TextureShader.DrawMask(&g_CommandBuffer, &g_MaskGpuBuffer, g_SamplerSlot);

        g_CommandBuffer.End();

        ///  Execute commands and wait for them to complete.
        g_Queue.ExecuteCommand(&g_CommandBuffer, 0);
        g_Queue.Sync();
    }
}

nn::gfx::ResShaderFile* g_SampleShaderFile;
nn::gfx::ShaderCodeType g_SampleShaderCodeType;
int g_SampleShaderMatrixIndex;
int g_SampleShaderModulateIndex;
int g_SampleShaderSamplerIndex;
void InitializeSampleShader()
{
    g_SampleShaderFile = InitializeResShaderFile(&g_SampleShaderCodeType, SampleShaderPath);

    const nn::gfx::ResShaderVariation* pVariation
        = g_SampleShaderFile->GetShaderContainer()->GetResShaderVariation(0);
    const nn::gfx::ResShaderProgram* pProgram
        = pVariation->GetResShaderProgram(g_SampleShaderCodeType);
    const nn::gfx::Shader* pShader = pProgram->GetShader();

    g_SampleShaderMatrixIndex = pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex
                              , nn::gfx::ShaderInterfaceType_ConstantBuffer
                              , "u_Matrix");
    g_SampleShaderModulateIndex = pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel
                              , nn::gfx::ShaderInterfaceType_ConstantBuffer
                              , "u_Modulate");

    g_SampleShaderSamplerIndex = pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel
                              , nn::gfx::ShaderInterfaceType_Sampler
                              , "s_Tex");
    NN_ASSERT(g_SampleShaderMatrixIndex >= 0);
    NN_ASSERT(g_SampleShaderModulateIndex >= 0);
    NN_ASSERT(g_SampleShaderSamplerIndex >= 0);
}

nn::gfx::RasterizerState g_RasterizerState[nn::gfx::CullMode_End];
void InitializeRasterizerState()
{
    nn::gfx::RasterizerStateInfo info;
    info.SetDefault();
    info.SetPrimitiveTopologyType(nn::gfx::PrimitiveTopologyType_Triangle);
    info.SetScissorEnabled(true);
    info.SetDepthClipEnabled(false);
    for (int i = 0; i < nn::gfx::CullMode_End;++i)
    {
        nn::gfx::CullMode cullMode = nn::gfx::CullMode(i);
        info.SetCullMode(cullMode);

        g_RasterizerState[i].Initialize(&g_Device, info);
    }
}

enum DrawMode
{
    DrawMode_Opa,
    DrawMode_Xlu,
    DrawMode_End
};

nn::gfx::BlendState g_BlendState[DrawMode_End];
void InitializeBlendState()
{
    for (int i = 0; i < DrawMode_End;++i)
    {
        DrawMode drawMode = DrawMode(i);
        nn::gfx::BlendTargetStateInfo targetInfo;
        {
            targetInfo.SetDefault();
        }

        if (drawMode == DrawMode_Xlu)
        {
            targetInfo.SetBlendEnabled(true);
            targetInfo.SetColorBlendFunction(nn::gfx::BlendFunction_Add);
            targetInfo.SetSourceColorBlendFactor(nn::gfx::BlendFactor_SourceAlpha);
            targetInfo.SetDestinationColorBlendFactor(nn::gfx::BlendFactor_OneMinusSourceAlpha);
        }

        nn::gfx::BlendState::InfoType info;
        info.SetDefault();
        info.SetBlendTargetStateInfoArray( &targetInfo, 1 );


        size_t size = nn::gfx::BlendState::GetRequiredMemorySize( info );
        g_pMemory.AlignUp( nn::gfx::BlendState::RequiredMemoryInfo_Alignment );
        g_BlendState[i].SetMemory( g_pMemory.Get(), size );
        g_pMemory.Advance( size );
        g_BlendState[i].Initialize( &g_Device, info );
    }
}

nn::gfx::DepthStencilState g_DepthStencilState[DrawMode_End];
void InitializeDepthStencilState()
{
    for (int i = 0; i < DrawMode_End;++i)
    {
        nn::gfx::DepthStencilStateInfo info;
        info.SetDefault();
        info.SetDepthTestEnabled(true);
        info.SetDepthWriteEnabled(i == DrawMode_Opa);

        g_DepthStencilState[i].Initialize(&g_Device, info);
    }
}

nn::gfx::VertexState g_VertexState;
void InitializeVertexState()
{
    nn::gfx::VertexAttributeStateInfo attribs[3];
    attribs[ 0 ].SetDefault();
    attribs[ 0 ].SetBufferIndex( 0 );
    attribs[ 0 ].SetFormat( nn::mii::DrawParam::PositionFormat );
    attribs[ 0 ].SetOffset( nn::mii::DrawParam::PositionOffset );
    attribs[ 0 ].SetShaderSlot( 0 );
    attribs[ 1 ].SetDefault();
    attribs[ 1 ].SetBufferIndex( 1 );
    attribs[ 1 ].SetFormat( nn::mii::DrawParam::UvFormat );
    attribs[ 1 ].SetOffset( nn::mii::DrawParam::UvOffset );
    attribs[ 1 ].SetShaderSlot( 1 );
    attribs[ 2 ].SetDefault();
    attribs[ 2 ].SetBufferIndex( 2 );
    attribs[ 2 ].SetFormat(nn::mii::DrawParam::NormalFormat);
    attribs[ 2 ].SetOffset(nn::mii::DrawParam::NormalOffset);
    attribs[ 2 ].SetShaderSlot( 2 );

    nn::gfx::VertexBufferStateInfo buffers[3];
    buffers[0].SetDefault();
    buffers[0].SetStride(nn::mii::DrawParam::PositionStride);
    buffers[1].SetDefault();
    buffers[1].SetStride(nn::mii::DrawParam::UvStride);
    buffers[2].SetDefault();
    buffers[2].SetStride(nn::mii::DrawParam::NormalStride);

    nn::gfx::VertexState::InfoType info;
    info.SetDefault();
    info.SetVertexAttributeStateInfoArray(attribs, 3);
    info.SetVertexBufferStateInfoArray(buffers, 3);

    size_t size = nn::gfx::VertexState::GetRequiredMemorySize( info );
    g_pMemory.AlignUp( nn::gfx::VertexState::RequiredMemoryInfo_Alignment );
    g_VertexState.SetMemory( g_pMemory.Get(), size );
    g_pMemory.Advance( size );
    g_VertexState.Initialize( &g_Device, info, NULL );
}

nn::gfx::Buffer g_MiiConstantBufferMatrix;
nn::gfx::Buffer g_MiiConstantBufferModulate[nn::mii::CharModel::DrawType_End];
void InitializeConstantBuffers()
{
    {
        nn::gfx::BufferInfo info;
        info.SetDefault();
        info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
        info.SetSize(sizeof(ConstantBufferMatrix));

        size_t alignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
        g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, alignment);

        g_MiiConstantBufferMatrix.Initialize(&g_Device,info
                                          , &g_MemoryPool
                                          , g_MemoryPoolOffset
                                          , info.GetSize());

        g_MemoryPoolOffset += info.GetSize();
    }

    {
        nn::gfx::BufferInfo info;
        info.SetDefault();
        info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
        info.SetSize(sizeof(ConstantBufferModulate));
        size_t alignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
        for (int i = 0; i < nn::mii::CharModel::DrawType_End;++i)
        {
            g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, alignment);
            g_MiiConstantBufferModulate[i].Initialize(&g_Device,info
                , &g_MemoryPool
                , g_MemoryPoolOffset
                , info.GetSize());
            g_MemoryPoolOffset += info.GetSize();
        }
    }
}

///  Set the <tt>Projection</tt> and the <tt>ModelView</tt> for <tt>ConstantBufferMatrix</tt>.
void SetConstantBufferMatrixValue(ConstantBufferMatrix* pConstantBufferMatrix, const nn::util::Matrix4x3fType& mvMatrix)
{
    NN_ASSERT_NOT_NULL(pConstantBufferMatrix);
    ///  Set the projection matrix.
    const float fovy = 3.14f / 4.0f;
    const float aspect = float(g_DisplayWidth) / float(g_DisplayHeight);
    nn::util::Matrix4x4fType projection;
    nn::util::MatrixPerspectiveFieldOfViewRightHanded(&projection, fovy, aspect, 0.01f, 10000.0f);
    nn::util::MatrixStore(&pConstantBufferMatrix->projection, projection);
    ///  Set <tt><var>modelView</var></tt>.
    nn::util::Matrix4x3fType modelView;

    nn::util::MatrixLookAtRightHanded(&modelView, CameraPos, CameraTarget, 0.0f);
    nn::util::MatrixMultiply(&modelView, mvMatrix, modelView);
    nn::util::MatrixStore(&pConstantBufferMatrix->modelView, modelView);
	
}

///  Update the constant buffers for Mii rendering.
void SetupMiiConstantBuffers(nn::mii::CreateModelType modelType, nn::util::Quaternion rotation)
{
    {
        ConstantBufferMatrix* pBuffer = g_MiiConstantBufferMatrix.Map<ConstantBufferMatrix>();
        NN_ASSERT_NOT_NULL(pBuffer);

		nn::util::Vector3f vecZero;
		nn::util::VectorZero(&vecZero);

        nn::util::Matrix4x3fType mv;
        nn::util::MatrixIdentity(&mv);

		mv = nn::util::MatrixRowMajor4x3f::MakeRotation(angle);
		nn::util::MatrixSetAxisW(&mv, vecZero);
        ///  The <tt><var>projection</var></tt> and <tt><var>modelView</var></tt> settings.
        SetConstantBufferMatrixValue(pBuffer, mv);
        g_MiiConstantBufferMatrix.Unmap();
    }

    ///  Configure the various color and light settings for drawing.
    for (int i = 0; i < nn::mii::CharModel::DrawType_End;++i)
    {
        nn::mii::CharModel::DrawType drawType = nn::mii::CharModel::DrawType(i);

        const nn::mii::DrawParam* pDrawParam = g_CharModel.GetDrawParam(drawType, modelType, CreateNoseType, 0);

        if (pDrawParam == NULL)
        {
            continue;
        }

        ConstantBufferModulate* pBuffer = g_MiiConstantBufferModulate[i].Map<ConstantBufferModulate>();

        ///  Set the type of modulation.
        pBuffer->modulateType = pDrawParam->GetModulateType();

        ///  Set the constant colors.
        for (int j = 0; j < nn::mii::DrawParam::ConstantColorNum;++j)
        {
            const nn::mii::Color3* srcColor = pDrawParam->GetConstantColor(j);
            if (srcColor == NULL)
            {
                continue;
            }

            pBuffer->constantColor[j].x = srcColor->r;
            pBuffer->constantColor[j].y = srcColor->g;
            pBuffer->constantColor[j].z = srcColor->b;
        }
        ///  Set the light.
        pBuffer->lightDir = LightDirection;

        g_MiiConstantBufferModulate[i].Unmap();
    }
}

void FinalizeMii()
{
    for (int i = 0; i < nn::mii::CharModel::DrawType_End;++i)
    {
        g_MiiConstantBufferModulate[i].Finalize(&g_Device);
    }
    g_MiiConstantBufferMatrix.Finalize(&g_Device);
    g_VertexState.Finalize(&g_Device);
    for (int i = 0; i < DrawMode_End;++i)
    {
        g_BlendState[i].Finalize( &g_Device);
        g_DepthStencilState[i].Finalize(&g_Device);
    }
    for (int i = 0; i < nn::gfx::CullMode_End;++i)
    {
        g_RasterizerState[i].Finalize(&g_Device);
    }
    {
    nn::gfx::ResShaderContainer* pContainer = g_SampleShaderFile->GetShaderContainer();
    for (int i = 0; i < pContainer->GetShaderVariationCount();++i)
    {
        nn::gfx::ResShaderVariation* pVariation = pContainer->GetResShaderVariation(i);
        pVariation->GetResShaderProgram(g_SampleShaderCodeType)->Finalize(&g_Device);
    }
    pContainer->Finalize(&g_Device);
    }

    g_MaskGpuBuffer.Finalize(&g_Device);
    g_FacelineGpuBuffer.Finalize(&g_Device);
    g_Mask.Finalize(&g_Device);
    g_Faceline.Finalize(&g_Device);
    g_CharModel.Finalize(&g_Device);
    g_Resource.Finalize(&g_Device);
    g_TextureShader.Finalize(&g_Device);
    nn::gfx::ResShaderContainer* pContainer = g_TextureShaderFile->GetShaderContainer();
    for (int i = 0; i < pContainer->GetShaderVariationCount();++i)
    {
        nn::gfx::ResShaderVariation* pVariation = pContainer->GetResShaderVariation(i);
        pVariation->GetResShaderProgram(g_TextureShaderCodeType)->Finalize(&g_Device);
    }
    g_TextureShaderFile->GetShaderContainer()->Finalize(&g_Device);
    g_Sampler.Finalize(&g_Device);
}

void DrawByDrawParam(nn::gfx::CommandBuffer* pCommandBuffer,const nn::mii::DrawParam* pDrawParam)
{
    ///  If the texture is valid.
    if (pDrawParam->IsValidTexture())
    {
        pCommandBuffer->SetTextureAndSampler(
            g_SampleShaderSamplerIndex, nn::gfx::ShaderStage_Pixel
            , *pDrawParam->GetTextureDescriptorSlot()
            , g_SamplerSlot);
    }

    nn::gfx::GpuAddress gpuAddress;
    const nn::gfx::Buffer* pVertexBuffer = pDrawParam->GetAttribute(nn::mii::DrawParam::AttributeType_Position);
    size_t vertexBufferSize = pDrawParam->GetBufferSize(nn::mii::DrawParam::AttributeType_Position);
    NN_ASSERT_NOT_NULL(pVertexBuffer);
    NN_ASSERT(0 < vertexBufferSize);
    pVertexBuffer->GetGpuAddress(&gpuAddress);
    pCommandBuffer->SetVertexBuffer(0, gpuAddress, nn::mii::DrawParam::PositionStride, vertexBufferSize);

    ///  Update only if UV is valid. (In some cases, DrawParam in CharModel does not have UV.)
    if (pDrawParam->IsValidAttribute(nn::mii::DrawParam::AttributeType_Uv))
    {
        pVertexBuffer = pDrawParam->GetAttribute(nn::mii::DrawParam::AttributeType_Uv);
        vertexBufferSize = pDrawParam->GetBufferSize(nn::mii::DrawParam::AttributeType_Uv);
    }
    pVertexBuffer->GetGpuAddress(&gpuAddress);
    pCommandBuffer->SetVertexBuffer(1, gpuAddress, nn::mii::DrawParam::UvStride, vertexBufferSize);

    // Update only if <tt>Normal</tt> is valid.
    const nn::gfx::Buffer* pNormaBuffer = pDrawParam->GetAttribute(nn::mii::DrawParam::AttributeType_Normal);
    if (pNormaBuffer != nullptr)
    {
        pVertexBuffer = pDrawParam->GetAttribute(nn::mii::DrawParam::AttributeType_Normal);
        vertexBufferSize = pDrawParam->GetBufferSize(nn::mii::DrawParam::AttributeType_Normal);
    }
    pVertexBuffer->GetGpuAddress(&gpuAddress);
    pCommandBuffer->SetVertexBuffer(2, gpuAddress, nn::mii::DrawParam::NormalStride, vertexBufferSize);

    int count = pDrawParam->GetIndexCount();
    pDrawParam->GetIndexBuffer()->GetGpuAddress(&gpuAddress);
    pCommandBuffer->DrawIndexed(
        nn::mii::DrawParam::PrimitiveTopology
        , nn::mii::DrawParam::IndexFormat
        , gpuAddress
        , count, 0);

}

///  A class that manages a buffer for headwear model shapes.
class HeadwearShape
{
    NN_DISALLOW_COPY(HeadwearShape);
public:
    enum VertexAttributeType
    {
        VertexAttributeType_Position,
        VertexAttributeType_Uv,
        VertexAttributeType_Normal,

        VertexAttributeType_End,
    };
public:
    HeadwearShape() NN_NOEXCEPT
    {
    }
    ~HeadwearShape() NN_NOEXCEPT
    {
    }

    ///  Initializes the buffer for vertex attributes.
    void InitializeVertexAttributeBuffer(
        const void* pSrcData, size_t size, VertexAttributeType type) NN_NOEXCEPT
    {
        NN_ASSERT_NOT_NULL(pSrcData);
        NN_ASSERT_RANGE(type, 0, VertexAttributeType_End);

        nn::gfx::BufferInfo info;
        info.SetDefault();
        info.SetGpuAccessFlags(nn::gfx::GpuAccess_VertexBuffer);
        info.SetSize(size);
        const size_t bufferAlignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
        g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, bufferAlignment);

        nn::gfx::Buffer& buffer = m_Buffer[type];
        buffer.Initialize(&g_Device, info, &g_MemoryPool, g_MemoryPoolOffset, info.GetSize());
        g_MemoryPoolOffset += info.GetSize();
        NN_ASSERT(nn::gfx::IsInitialized(buffer));
        // Copies the data.
        void* pMap = buffer.Map();
        std::memcpy(pMap, pSrcData, size);
        buffer.Unmap();
        m_BufferSize[type] = size;
    }

    ///  Initializes the index buffer.
    void InitializeIndexBuffer(const void* pSrcData, size_t size) NN_NOEXCEPT
    {
        nn::gfx::BufferInfo info;
        info.SetDefault();
        info.SetGpuAccessFlags(nn::gfx::GpuAccess_IndexBuffer);
        info.SetSize(size);
        const size_t bufferAlignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
        g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, bufferAlignment);
        m_IndexBuffer.Initialize(&g_Device, info, &g_MemoryPool, g_MemoryPoolOffset, info.GetSize());
        g_MemoryPoolOffset += info.GetSize();
        NN_ASSERT(nn::gfx::IsInitialized(m_IndexBuffer));
        void* pMap = m_IndexBuffer.Map();
        std::memcpy(pMap, pSrcData, size);
        m_IndexBuffer.Unmap();
        m_IndexCount = int(size / sizeof(uint16_t));
        NN_STATIC_ASSERT(HeadwearIndexFormat == nn::gfx::IndexFormat_Uint16);
    }

    ///  Gets the vertex attribute buffer.
    const nn::gfx::Buffer& GetVertexAttirbuteBuffer(VertexAttributeType type) const NN_NOEXCEPT
    {
        return m_Buffer[type];
    }

    ///  Gets the size of the vertex attribute buffer.
    size_t GetVertexBufferSize(VertexAttributeType type) const NN_NOEXCEPT
    {
        return m_BufferSize[type];
    }

    ///  Gets the index buffer.
    const nn::gfx::Buffer& GetIndexBuffer() const NN_NOEXCEPT
    {
        return m_IndexBuffer;
    }

    ///  Gets the number of indices.
    size_t GetIndexBufferCount() const NN_NOEXCEPT
    {
        return m_IndexCount;
    }

    ///  Finalize.
    void Finalize() NN_NOEXCEPT
    {
        for ( int idx = 0; idx < VertexAttributeType_End; ++idx )
        {
            nn::gfx::Buffer& buffer = m_Buffer[idx];
            if ( nn::gfx::IsInitialized(buffer) )
            {
                buffer.Finalize(&g_Device);
            }
        }

        if ( nn::gfx::IsInitialized(m_IndexBuffer) )
        {
            m_IndexBuffer.Finalize(&g_Device);
        }
    }

private:
    nn::gfx::Buffer m_Buffer[VertexAttributeType_End];
    size_t          m_BufferSize[VertexAttributeType_End];
    nn::gfx::Buffer m_IndexBuffer;
    int             m_IndexCount;
};

HeadwearShape g_HeadwearShape[HeadwearType_End];
void InitializeHeadwearShape(HeadwearType headwearType)
{
    NN_ASSERT_RANGE(headwearType, HeadwearType_Min, HeadwearType_End);
    ///  Read the model file.
    nn::fs::FileHandle handle;
    nn::Result result = nn::fs::OpenFile(&handle, HeadwearModelPathList[headwearType], nn::fs::OpenMode_Read);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Get the file size.
    int64_t size = 0;
    result = nn::fs::GetFileSize(&size, handle);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Get the alignment.
    nn::util::BinaryFileHeader header;
    result = nn::fs::ReadFile(handle, 0, &header, sizeof(header));
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
    size_t alignment = header.GetAlignment();

    ///  Allocate memory for the model resource.
    g_pMemory.AlignUp(alignment);
    void* pFile = g_pMemory.Get();
    NN_ASSERT_NOT_NULL(pFile);
    g_pMemory.Advance(ptrdiff_t(size));

    ///  Read a resource.
    result = nn::fs::ReadFile(handle, 0, pFile, size_t(size));
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );

    ///  Close.
    nn::fs::CloseFile(handle);

    ///  Gets the model data header for the headwear.
    HeadwearResourceHeader* pShapeHeader = reinterpret_cast< HeadwearResourceHeader* >( pFile );
    NN_ASSERT_NOT_NULL(pShapeHeader);
    const void* pRaw = nn::util::ConstBytePtr(pShapeHeader).Advance(sizeof(HeadwearResourceHeader)).Get();

    HeadwearShape& headwearShape = g_HeadwearShape[headwearType];
    ///  Initialize the vertex buffer.
    {
        const ptrdiff_t offset = ptrdiff_t(pShapeHeader->positionOffset);
        const size_t dataSize = size_t(pShapeHeader->positionSize);
        const void* pData = reinterpret_cast<const void*>( intptr_t(pRaw) + offset );

        headwearShape.InitializeVertexAttributeBuffer(
            pData, dataSize, HeadwearShape::VertexAttributeType_Position);
    }
    ///  Initialize the normal buffer.
    {
        const int32_t offset = pShapeHeader->normalOffset;
        const size_t dataSize = size_t(pShapeHeader->normalSize);
        const void* pData = reinterpret_cast<const void*>( intptr_t(pRaw) + offset );

        headwearShape.InitializeVertexAttributeBuffer(
            pData, dataSize, HeadwearShape::VertexAttributeType_Normal);
    }
    ///  Initialize the UV buffer.
    {
        const int32_t offset = pShapeHeader->uvOffset;
        const size_t dataSize = size_t(pShapeHeader->uvSize);
        const void* pData = reinterpret_cast<const void*>( intptr_t(pRaw) + offset );

        headwearShape.InitializeVertexAttributeBuffer(
            pData, dataSize, HeadwearShape::VertexAttributeType_Uv);
    }
    ///  Initialize the index buffer.
    {
        const int32_t offset = pShapeHeader->indexOffset;
        const size_t dataSize = size_t(pShapeHeader->indexSize);
        const void* pData = reinterpret_cast<const void*>( intptr_t(pRaw) + offset );

        headwearShape.InitializeIndexBuffer(pData, dataSize);
    }
}

nn::gfx::VertexState g_HeadwearVertexState;
void InitializeHeadwearVertexState()
{
    nn::gfx::VertexAttributeStateInfo attribs[HeadwearAttributeNum];
    attribs[ 0 ].SetDefault();
    attribs[ 0 ].SetBufferIndex( 0 );
    attribs[ 0 ].SetFormat( HeadwearVertexAttributeFormat );
    attribs[ 0 ].SetOffset( 0 );
    attribs[ 0 ].SetShaderSlot( 0 );
    attribs[ 1 ].SetDefault();
    attribs[ 1 ].SetBufferIndex( 1 );
    attribs[ 1 ].SetFormat( HeadwearUvAttributeFormat );
    attribs[ 1 ].SetOffset( 0 );
    attribs[ 1 ].SetShaderSlot( 1 );
    attribs[ 2 ].SetDefault();
    attribs[ 2 ].SetBufferIndex( 2 );
    attribs[ 2 ].SetFormat( HeadwearNormalAttributeFormat );
    attribs[ 2 ].SetOffset( 0 );
    attribs[ 2 ].SetShaderSlot( 2 );

    nn::gfx::VertexBufferStateInfo buffers[HeadwearAttributeNum];
    buffers[0].SetDefault();
    buffers[0].SetStride( HeadwearPositionStride );
    buffers[1].SetDefault();
    buffers[1].SetStride( HeadwearUvStride );
    buffers[2].SetDefault();
    buffers[2].SetStride( HeadwearNormalStride );

    nn::gfx::VertexState::InfoType info;
    info.SetDefault();
    info.SetVertexAttributeStateInfoArray(attribs, HeadwearAttributeNum);
    info.SetVertexBufferStateInfoArray(buffers, HeadwearAttributeNum);

    size_t size = nn::gfx::VertexState::GetRequiredMemorySize( info );
    g_pMemory.AlignUp( nn::gfx::VertexState::RequiredMemoryInfo_Alignment );
    g_HeadwearVertexState.SetMemory( g_pMemory.Get(), size );
    g_pMemory.Advance( size );
    g_HeadwearVertexState.Initialize( &g_Device, info, NULL );
}

///  The constant buffer for the headwear <tt>Matrix</tt> object.
nn::gfx::Buffer g_HeadwearConstantBufferMatrix;
void InitializeHeadwearConstantBufferMatrix()
{
    nn::gfx::BufferInfo info;
    info.SetDefault();
    info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
    info.SetSize(sizeof(ConstantBufferMatrix));
    const size_t alignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
    g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, alignment);
    g_HeadwearConstantBufferMatrix.Initialize(&g_Device, info
        , &g_MemoryPool
        , g_MemoryPoolOffset
        , info.GetSize());
    g_MemoryPoolOffset += info.GetSize();
    NN_ASSERT(nn::gfx::IsInitialized(g_HeadwearConstantBufferMatrix));
}

///  Constant buffer for the <tt>Matrix</tt> to use when headwear is worn on the left side.
nn::gfx::Buffer g_HeadwearConstantBufferMatrixLeft;
void InitializeHeadwearConstantBufferMatrixLeft()
{
    nn::gfx::BufferInfo info;
    info.SetDefault();
    info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
    info.SetSize(sizeof(ConstantBufferMatrix));
    const size_t alignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
    g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, alignment);
    g_HeadwearConstantBufferMatrixLeft.Initialize(&g_Device, info
        , &g_MemoryPool
        , g_MemoryPoolOffset
        , info.GetSize());
    g_MemoryPoolOffset += info.GetSize();
    NN_ASSERT(nn::gfx::IsInitialized(g_HeadwearConstantBufferMatrixLeft));
}

///  The constant buffer for the headwear <tt>Modulate</tt> object.
nn::gfx::Buffer g_HeadwearConstantBufferModulate;
void InitializeHeadwearConstantBufferModulate()
{
    nn::gfx::BufferInfo info;
    info.SetDefault();
    info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
    info.SetSize(sizeof(ConstantBufferModulate));
    const size_t alignment = nn::gfx::Buffer::GetBufferAlignment(&g_Device, info);
    g_MemoryPoolOffset = nn::util::align_up(g_MemoryPoolOffset, alignment);
    g_HeadwearConstantBufferModulate.Initialize(&g_Device, info
                , &g_MemoryPool
                , g_MemoryPoolOffset
                , info.GetSize());
    g_MemoryPoolOffset += info.GetSize();
    NN_ASSERT(nn::gfx::IsInitialized(g_HeadwearConstantBufferModulate));
}

///  Get the translation value for the headwear.
nn::util::Float3 GetHeadwearTranslate(HeadwearType type)
{
    NN_ASSERT_RANGE(type, HeadwearType_Min, HeadwearType_End);
    NN_ASSERT(g_CharModel.IsInitialized());
    switch (type)
    {
    case HeadwearType_Front:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Front).translate;
    case HeadwearType_Side:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Right).translate;
    case HeadwearType_Top:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Top).translate;

    case HeadwearType_Cap0:
    case HeadwearType_Cap1:
    case HeadwearType_Headgear:
        return g_CharModel.GetHatPartsTranslate();
    default: NN_UNEXPECTED_DEFAULT;
    }
}

///  Get the rotation value for the headwear.
nn::util::Float3 GetHeadwearRotate(HeadwearType type)
{
    NN_ASSERT_RANGE(type, HeadwearType_Min, HeadwearType_End);
    NN_ASSERT(g_CharModel.IsInitialized());
    switch (type)
    {
    case HeadwearType_Front:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Front).rotate;

    case HeadwearType_Side:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Right).rotate;

    case HeadwearType_Top:
        return g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Top).rotate;

    case HeadwearType_Cap0:
    case HeadwearType_Cap1:
    case HeadwearType_Headgear:
        ///  Hats and headgear do not have a rotation value.
        return NN_UTIL_FLOAT_3_INITIALIZER(0.0f, 0.0f, 0.0f);
    default: NN_UNEXPECTED_DEFAULT;
    }
}

///  Update the values in the constant buffers related to the headwear models.
void SetupHeadwearConstantBuffers(HeadwearType headwearType,nn::util::Quaternion angle)
{
    NN_ASSERT_RANGE(headwearType, HeadwearType_Min, HeadwearType_End);
    {
        ConstantBufferMatrix* pBuffer = g_HeadwearConstantBufferMatrix.Map<ConstantBufferMatrix>();
        NN_ASSERT_NOT_NULL(pBuffer);

        ///  Get the translation and rotation values when headwear is being displayed.
        const nn::util::Float3 translate = GetHeadwearTranslate(headwearType);
        const nn::util::Float3 rotate = GetHeadwearRotate(headwearType);

        nn::util::Matrix4x3fType mvMtx;
        nn::util::MatrixIdentity(&mvMtx);

        //nn::util::MatrixSetRotateXyz(&mvMtx, nn::util::Vector3f(rotate));

		nn::util::Vector3f vecZero;
		nn::util::VectorZero(&vecZero);

		mvMtx = nn::util::MatrixRowMajor4x3f::MakeRotation(angle);
		nn::util::MatrixSetAxisW(&mvMtx, vecZero);
		nn::util::MatrixSetTranslate(&mvMtx, nn::util::Vector3f(translate));

        SetConstantBufferMatrixValue(pBuffer, mvMtx);

        g_HeadwearConstantBufferMatrix.Unmap();
    }
    ///  Update the constant buffers for the <tt>Matrix</tt> object on the opposite side (the left ear side) when the headgear is something set on both sides of the head.
    if (headwearType == HeadwearType_Side)
    {
        ConstantBufferMatrix* pBuffer = g_HeadwearConstantBufferMatrixLeft.Map<ConstantBufferMatrix>();
        NN_ASSERT_NOT_NULL(pBuffer);

        nn::util::Matrix4x3fType mvMtx;
        nn::util::MatrixIdentity(&mvMtx);

		nn::util::Vector3f vecZero;
		nn::util::VectorZero(&vecZero);

		mvMtx = nn::util::MatrixRowMajor4x3f::MakeRotation(angle);
		nn::util::MatrixSetAxisW(&mvMtx, vecZero);

		const nn::mii::HeadPartsTransform& transform = g_CharModel.GetHeadPartsTransform(nn::mii::HeadPartsType_Left);
		//nn::util::MatrixSetRotateXyz(&mvMtx, nn::util::Vector3f(transform.rotate));
		nn::util::MatrixSetTranslate(&mvMtx, nn::util::Vector3f(transform.translate));
        SetConstantBufferMatrixValue(pBuffer, mvMtx);

        g_HeadwearConstantBufferMatrixLeft.Unmap();
    }
    {
        ConstantBufferModulate* pBuffer = g_HeadwearConstantBufferModulate.Map<ConstantBufferModulate>();
        NN_ASSERT_NOT_NULL(pBuffer);
        ///  Set the type of modulation.
        pBuffer->modulateType = HeadwearModulateType;

        ///  Set the constant colors.
        for (int idx = 0; idx < nn::mii::DrawParam::ConstantColorNum; ++idx)
        {
            const nn::util::Color4f& color = HeadwearColorList[headwearType];
            pBuffer->constantColor[idx].x = color.GetR();
            pBuffer->constantColor[idx].y = color.GetG();
            pBuffer->constantColor[idx].z = color.GetB();
        }


        ///  Set the light.
        pBuffer->lightDir = LightDirection;

        g_HeadwearConstantBufferModulate.Unmap();
    }
}

///  Initialize the sample model for headwear.
void InitializeHeadwearModel()
{
    ///  Initialize each model.
    for ( int idx = 0; idx < HeadwearType_End; ++idx )
    {
        InitializeHeadwearShape(HeadwearType(idx));
    }
    InitializeHeadwearVertexState();
    ///  Initialize the constant buffers.
    InitializeHeadwearConstantBufferMatrix();
    InitializeHeadwearConstantBufferMatrixLeft();
    InitializeHeadwearConstantBufferModulate();
}

///  Finalize the sample model for headwear.
void FinalizeHeadwearModel()
{
    for ( int idx = 0; idx < HeadwearType_End; ++idx )
    {
        g_HeadwearShape[idx].Finalize();
    }
    g_HeadwearConstantBufferModulate.Finalize(&g_Device);
    g_HeadwearConstantBufferMatrixLeft.Finalize(&g_Device);
    g_HeadwearConstantBufferMatrix.Finalize(&g_Device);
    g_HeadwearVertexState.Finalize(&g_Device);
}

///  Render the Mii.
void DrawMii(nn::mii::CreateModelType modelType, int maskSlot)
{
    NN_ASSERT_RANGE(modelType, nn::mii::CreateModelType_Min, nn::mii::CreateModelType_End);
    NN_ASSERT(0 <= maskSlot);
    NN_ASSERT(g_CharModel.IsInitialized());
    for ( int drawTypeIndex = 0; drawTypeIndex < nn::mii::CharModel::DrawType_End; ++drawTypeIndex )
    {
        nn::mii::CharModel::DrawType drawType = nn::mii::CharModel::DrawType(drawTypeIndex);
        const nn::mii::DrawParam* pDrawParam = g_CharModel.GetDrawParam(drawType
            , modelType
            , CreateNoseType
            , maskSlot);

        if ( pDrawParam == NULL )
        {
            continue;
        }

        DrawMode drawMode = drawType >= nn::mii::CharModel::DrawType_XluMin
            ? DrawMode_Xlu : DrawMode_Opa;

        ///  Set render settings.
        g_CommandBuffer.SetRasterizerState(&g_RasterizerState[pDrawParam->GetCullMode()]);
        g_CommandBuffer.SetBlendState(&g_BlendState[drawMode]);
        g_CommandBuffer.SetDepthStencilState(&g_DepthStencilState[drawMode]);
        g_CommandBuffer.SetVertexState(&g_VertexState);
        g_CommandBuffer.SetShader(
            g_SampleShaderFile->GetShaderContainer()->GetResShaderVariation(0)
            ->GetResShaderProgram(g_SampleShaderCodeType)->GetShader()
            , nn::gfx::ShaderStageBit_All);

        ///  Bind the constant buffers.
        nn::gfx::GpuAddress gpuAddress;
        g_MiiConstantBufferMatrix.GetGpuAddress(&gpuAddress);
        g_CommandBuffer.SetConstantBuffer(
            g_SampleShaderMatrixIndex, nn::gfx::ShaderStage_Vertex
            , gpuAddress, sizeof(ConstantBufferMatrix));
        g_MiiConstantBufferModulate[drawTypeIndex].GetGpuAddress(&gpuAddress);
        g_CommandBuffer.SetConstantBuffer(
            g_SampleShaderModulateIndex, nn::gfx::ShaderStage_Pixel
            , gpuAddress, sizeof(ConstantBufferModulate));
		
        ///  Draw.
        DrawByDrawParam(&g_CommandBuffer, pDrawParam);
    }
}

int GetNextScanBufferIndexAndWaitDisplayFence()
{
    ///  Gets the index of the scan buffer that is the rendering target for the next frame.
    int nextScanBufferIndex = 0;
    const nn::gfx::AcquireScanBufferResult acquireResult = g_SwapChain.AcquireNextScanBufferIndex(
        &nextScanBufferIndex, &g_DisplaySemaphore, &g_DisplayFence);
    NN_ASSERT(acquireResult == nn::gfx::AcquireScanBufferResult_Success);
    NN_UNUSED(acquireResult);
    ///  Synchronizing getting the scan buffer (GPU) is not necessary.
    ///  g_Queue.SyncSemaphore(&g_DisplaySemaphore);

    ///  Waits until the scan buffer, that is the rendering target in the next frame, becomes available.
    const nn::gfx::SyncResult syncResult = g_DisplayFence.Sync(nn::TimeSpan::FromSeconds(1));
    NN_ASSERT(syncResult == nn::gfx::SyncResult_Success);
    NN_UNUSED(syncResult);

    return nextScanBufferIndex;
}
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
// AUDIO
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------

// Select the rendering engine sample rate.
//const int RenderRate = 48000;
const int RenderRate = 32000;
const int RenderCount = (RenderRate / 200);

// Select the number of files to play.
const int BgmCount = 1;
const int SeCount = 4;

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

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
// AUDIO
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------

#if defined( NN_BUILD_CONFIG_OS_SUPPORTS_HORIZON )
//-----------------------------------------------------------------------------
// nninitStartup() is invoked before calling nnMain().
//
extern "C" void nninitStartup()
{
    const size_t ApplicationMemorySize = 512 * 1024 * 1024;
    const size_t MallocMemorySize = 256 * 1024 * 1024;
    nn::Result result = nn::os::SetMemoryHeapSize(ApplicationMemorySize);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
    uintptr_t address;
    result = nn::os::AllocateMemoryBlock(&address, MallocMemorySize);
    NN_ABORT_UNLESS_RESULT_SUCCESS( result );
    nn::init::InitializeAllocator(reinterpret_cast<void*>(address), MallocMemorySize);
}
#endif

//
//  Main Function
//  The main function.
//

extern "C" void nnMain()
{
	g_Allocator.Initialize(g_WorkBuffer, sizeof(g_WorkBuffer));
	g_WaveBufferAllocator.Initialize(g_WaveBufferPoolMemory, sizeof(g_WaveBufferPoolMemory));

    InitializeFileSystem();

#if defined( NN_BUILD_CONFIG_OS_SUPPORTS_HORIZON ) && defined( NN_BUILD_CONFIG_SPEC_NX )
    // Initialize memory resources for the graphics system.
    {
        const size_t graphicsSystemMemorySize = 8 * 1024 * 1024;
        nv::SetGraphicsAllocator(NvAllocateFunction, NvFreeFunction, NvReallocateFunction, NULL);
        nv::InitializeGraphics(malloc(graphicsSystemMemorySize), graphicsSystemMemorySize);
    }
#endif
	std::vector<INpadStyleSixAxisSensor*> npadStyleSixAxisSensors;

	nn::hid::InitializeNpad();

	/// Input
	InitializeHidDevices();

    InitializeResources();

    ///  Prepares the sampler for the Mii.
    InitializeSampler();

    ///  Load <tt>CharInfo</tt> from the file.
    LoadCharInfo();

    ///  Initialize the shader for creating the Faceline and Mask textures.
    InitializeTextureShader();

    ////  Initialize the Mii resource.
    InitializeMiiResource();

    ///  Initialize CharModel.
    InitializeCharModel();

    ///  Initialize Faceline.
    InitializeFaceline();

    ///  Initialize Mask.
    InitializeMask();

    ///  Assign descriptor slots.
    BindDescriptorSlot();

    ///  Draw Faceline and Mask.
    InitializeFacelineGpuBuffer();
    InitializeMaskGpuBuffer();
    ///  g_Faceline and g_Mask can be destroyed after the textures have been rendered.
    RenderFacelineTexture();
    RenderMaskTexture();

    ///  Initialize the shader for drawing the Mii.
    InitializeSampleShader();

    ///  Initialize the process for drawing the Mii.
    InitializeRasterizerState();
    InitializeBlendState();
    InitializeDepthStencilState();
    InitializeVertexState();
    InitializeConstantBuffers();

    ///  Initialize the constant buffers and the model to use for the headwear model.
    InitializeHeadwearModel();

    ///  Get the scan buffer for the 0th frame.
    int nextScanBufferIndex = GetNextScanBufferIndexAndWaitDisplayFence();

	// Configure the Npad to use.
	nn::hid::SetSupportedNpadIdType(NpadIds, NpadIdCountMax);

	for (int i = 0; i < NpadIdCountMax; i++)
	{
		void* area = g_Allocator.Allocate(sizeof(FullKeySixAxisSensor), 16);
		npadStyleSixAxisSensors.push_back(reinterpret_cast<INpadStyleSixAxisSensor*>(new (area) FullKeySixAxisSensor(NpadIds[i])));
	}

	// Initialization for the operating state for the management target.
	for (std::vector<INpadStyleSixAxisSensor*>::iterator it = npadStyleSixAxisSensors.begin();
		it != npadStyleSixAxisSensors.end();
		++it)
	{
		(*it)->Initialize();
	}

	//Set the style of operation to use.
	nn::hid::SetSupportedNpadStyleSet(nn::hid::NpadStyleFullKey::Mask);
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

    // Draw each frame.
    for( int frame = 0; frame < 60 * 12; ++frame )
    {
		//Play sound effects. (The same SE is not overlaid.)
		for (int i = 0; i < SeCount; ++i)
		{
			// SE numbers {0, 1, 2, 3, 4, 5} correspond to buttons {A, B, X, Y, L, R}.
			if (frame % SeCount == i)
			{
				if (nn::audio::GetReleasedWaveBuffer(&voiceSe[i]))
				{
					nn::audio::AppendWaveBuffer(&voiceSe[i], &waveBufferSe[i]);
				}
			}
		}


        ///  Periodically change the facial expression.
        const int maskSlot = (frame / 30) % nn::mii::GetExpressionCount(ExpressionFlags);
        ///  Change to a different headwear model after the current one has been shown for a set amount of time.
        const HeadwearType headwearType = HeadwearType((frame / 120) % HeadwearType_End);

        NN_ASSERT_RANGE(headwearType, HeadwearType_Min, HeadwearType_End);

		for (std::vector<INpadStyleSixAxisSensor*>::iterator it = npadStyleSixAxisSensors.begin();
			it != npadStyleSixAxisSensors.end();
			++it)
		{
			if (!(*it)->IsConnected())
			{
				continue;
			}

			(*it)->Update();

			if ((*it)->CanReset())
			{
				(*it)->Reset();
				(*it)->ResetPointer();
			}
		}

		for (std::vector<INpadStyleSixAxisSensor*>::iterator it = npadStyleSixAxisSensors.begin();
			it != npadStyleSixAxisSensors.end();
			++it)
		{
			if (!(*it)->IsConnected())
			{
				continue;
			}

			angle = (*it)->GetRotation();
		}
        ///  Set the various constant buffers for drawing.
        SetupMiiConstantBuffers(HeadwearCreateModelTypeList[headwearType], angle);
        SetupHeadwearConstantBuffers(headwearType,angle);

        g_CommandBuffer.Reset();
        g_CommandBuffer.AddControlMemory(
            g_pCommandBufferControlMemory, CommandBufferControlMemorySize);
        g_CommandBuffer.AddCommandMemory(
            &g_MemoryPool,g_CommandBufferMemoryPoolOffset,CommandBufferMemoryPoolSize);

        g_CommandBuffer.Begin();

        ///  Clear.
        nn::gfx::ColorTargetView* pColorTarget = g_pScanBufferViews[nextScanBufferIndex];
        g_CommandBuffer.ClearColor(pColorTarget,0.3f,0.1f,0.1f,0.1f,NULL);
        g_CommandBuffer.ClearDepthStencil(&g_DepthStencilView, 1.0f, 255
                                          , nn::gfx::DepthStencilClearMode_DepthStencil, NULL);

        g_CommandBuffer.SetRenderTargets( 1, &pColorTarget, &g_DepthStencilView );
        g_CommandBuffer.SetViewportScissorState(&g_ViewportScissor);

        g_CommandBuffer.InvalidateMemory( nn::gfx::GpuAccess_Texture | nn::gfx::GpuAccess_IndexBuffer
            | nn::gfx::GpuAccess_ConstantBuffer | nn::gfx::GpuAccess_VertexBuffer );

        ///  Display the headwear.
        {
            ///  Set render settings.
            g_CommandBuffer.SetRasterizerState(&g_RasterizerState[nn::gfx::CullMode_Back]);
            g_CommandBuffer.SetBlendState(&g_BlendState[DrawMode_Opa]);
            g_CommandBuffer.SetDepthStencilState(&g_DepthStencilState[DrawMode_Opa]);
            g_CommandBuffer.SetVertexState(&g_HeadwearVertexState);
            g_CommandBuffer.SetShader(
                g_SampleShaderFile->GetShaderContainer()->GetResShaderVariation(0)
                ->GetResShaderProgram(g_SampleShaderCodeType)->GetShader()
                , nn::gfx::ShaderStageBit_All);

            ///  Bind the constant buffers.
            nn::gfx::GpuAddress gpuAddress;
            ///  Bind the constant buffer for the <tt>Matrix</tt> object for the headwear.
            g_HeadwearConstantBufferMatrix.GetGpuAddress(&gpuAddress);
            g_CommandBuffer.SetConstantBuffer(
                g_SampleShaderMatrixIndex, nn::gfx::ShaderStage_Vertex
                , gpuAddress, sizeof(ConstantBufferMatrix));
            ///  Bind the constant buffer for the <tt>Modulate</tt> object for the headwear.
            g_HeadwearConstantBufferModulate.GetGpuAddress(&gpuAddress);
            g_CommandBuffer.SetConstantBuffer(
                g_SampleShaderModulateIndex, nn::gfx::ShaderStage_Pixel
                , gpuAddress, sizeof(ConstantBufferModulate));
            ///  Render the model data for the headwear.
            const HeadwearShape& headwearShape = g_HeadwearShape[headwearType];
            {
                const nn::gfx::Buffer& buffer =
                    headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Position);
                const size_t size =
                    headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Position);
                buffer.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetVertexBuffer(0, gpuAddress, HeadwearPositionStride, size);
            }
            {
                const nn::gfx::Buffer& buffer =
                    headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Uv);
                const size_t size =
                    headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Uv);
                buffer.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetVertexBuffer(1, gpuAddress, HeadwearUvStride, size);
            }
            {
                const nn::gfx::Buffer& buffer =
                    headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Normal);
                const size_t size =
                    headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Normal);
                buffer.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetVertexBuffer(2, gpuAddress, HeadwearNormalStride, size);
            }
            {
                const int count = int(headwearShape.GetIndexBufferCount());
                headwearShape.GetIndexBuffer().GetGpuAddress(&gpuAddress);
                g_CommandBuffer.DrawIndexed(
                    nn::gfx::PrimitiveTopology_TriangleList, HeadwearIndexFormat,
                    gpuAddress, count, 0);
            }
            ///  If <tt>HeadwearType</tt> is <tt>Side</tt>, also display the matching one-sided model (for the left ear).
            if ( headwearType == HeadwearType_Side )
            {
                ///  Bind the constant buffer for the <tt>Matrix</tt> object for the left ear part.
                g_HeadwearConstantBufferMatrixLeft.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetConstantBuffer(
                    g_SampleShaderMatrixIndex, nn::gfx::ShaderStage_Vertex
                    , gpuAddress, sizeof(ConstantBufferMatrix));
                ///  Bind the constant buffer for the <tt>Modulate</tt> object.
                g_HeadwearConstantBufferModulate.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetConstantBuffer(
                    g_SampleShaderModulateIndex, nn::gfx::ShaderStage_Pixel
                    , gpuAddress, sizeof(ConstantBufferModulate));
                ///  Render the model for the left ear. (The model data itself is the same as that for the right ear.)
                {
                    const nn::gfx::Buffer& buffer =
                        headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Position);
                    const size_t size =
                        headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Position);
                    buffer.GetGpuAddress(&gpuAddress);
                    g_CommandBuffer.SetVertexBuffer(0, gpuAddress, HeadwearPositionStride, size);
                }
                {
                    const nn::gfx::Buffer& buffer =
                        headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Uv);
                    const size_t size =
                        headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Uv);
                    buffer.GetGpuAddress(&gpuAddress);
                    g_CommandBuffer.SetVertexBuffer(1, gpuAddress, HeadwearUvStride, size);
                }
                {
                    const nn::gfx::Buffer& buffer =
                        headwearShape.GetVertexAttirbuteBuffer(HeadwearShape::VertexAttributeType_Normal);
                    const size_t size =
                        headwearShape.GetVertexBufferSize(HeadwearShape::VertexAttributeType_Normal);
                    buffer.GetGpuAddress(&gpuAddress);
                    g_CommandBuffer.SetVertexBuffer(2, gpuAddress, HeadwearNormalStride, size);
                }
                {
                    const int count = int(headwearShape.GetIndexBufferCount());
                    headwearShape.GetIndexBuffer().GetGpuAddress(&gpuAddress);
                    g_CommandBuffer.DrawIndexed(
                        nn::gfx::PrimitiveTopology_TriangleList, HeadwearIndexFormat,
                        gpuAddress, count, 0);
                }
            }
        }

        ///  Render the Mii.
        DrawMii(HeadwearCreateModelTypeList[headwearType], maskSlot);
        g_CommandBuffer.End();

        // Execute the commands.
        g_Queue.ExecuteCommand(&g_CommandBuffer, &g_GpuDoneFence);

        // Display the results.
        g_Queue.Present(&g_SwapChain, 1);

        ///  Get the scan buffer that is the rendering target for the next frame.
        ///  Waits until the scan buffer, that is the rendering target in the next frame, becomes available.
        nextScanBufferIndex = GetNextScanBufferIndexAndWaitDisplayFence();
        ///  Waits for the rendering currently being performed.
        const nn::gfx::SyncResult syncResult = g_GpuDoneFence.Sync(nn::TimeSpan::FromSeconds(1));
        NN_ASSERT(syncResult == nn::gfx::SyncResult_Success);

		result = nn::audio::RequestUpdateAudioRenderer(handle, &config);
		NN_ABORT_UNLESS_RESULT_SUCCESS(result);

#ifdef WIN32
        nn::hws::ProcessMessage();
#endif
    }

    FinalizeHeadwearModel();
    FinalizeMii();
    FinalizeResources();
    FinalizeFileSystem();
}//NOLINT(impl/function_size)
