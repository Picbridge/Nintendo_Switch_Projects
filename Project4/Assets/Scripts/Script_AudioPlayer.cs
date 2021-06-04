/****************************************************************************/
/*!
\file   Script_AudioPlayer.cs
\author Brian Lu
\par    email: b.lu@digipen.edu
\date   12/11/2019
\brief
Static class used to play audio from wherever using an enum.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public static class Script_AudioPlayer
{
    public enum Sounds
    {
        ButtonPress,
        TankMovement,
        TankShoot,
        TankHit,
        Victory1,
        Victory2,
    }
    public static GameObject soundObject;
    public static AudioSource audioSource;
    private static Dictionary<Sounds, float> soundDelay;

    //plays a sounds from the enum, volume is 0 to 1
    public static void PlaySound(Sounds sound, float volume)
    {
        //float minPitch = .6f;
        //float maxPitch = 1f;
        //float minVolume = .5f;
        //float maxVolume = 1f;

        if (soundObject == null)
        {
            soundObject = new GameObject("Audio");
            audioSource = soundObject.AddComponent<AudioSource>();
        }

        //audioSource.pitch = Random.Range(minPitch, maxPitch);
        audioSource.volume = Random.Range(volume-.2f, volume+.2f);

        audioSource.PlayOneShot(GetAudioClip(sound));
    }

    //retrieves the corresponding sounds from the enum
    private static AudioClip GetAudioClip(Sounds sound)
    {
        foreach (Script_AudioManager.AudioClipSounds clip in Script_AudioManager.audioManager.audioClipArray)
        {
            if (clip.sound == sound)
            {
                return clip.audioClip;
            }
        }
        Debug.LogError("sound not found: " + sound);
        return null;
    }

}
