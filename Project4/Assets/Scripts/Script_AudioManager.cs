/****************************************************************************/
/*!
\file   Script_AudioManager.cs
\author Brian Lu
\par    email: b.lu@digipen.edu
\date   12/11/2019
\brief
Handles the list of sounds and connects them with their enum.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public class Script_AudioManager : MonoBehaviour
{

    private static Script_AudioManager audioManager_;

    public static Script_AudioManager audioManager
    {
        get
        {
            if (audioManager_ == null)
                audioManager_ = GameObject.FindGameObjectWithTag("AudioManager").GetComponent<Script_AudioManager>(); //loads gameobject prefab from resources folder.
            return audioManager_;
        }
    }

    public AudioClipSounds[] audioClipArray;


    [System.Serializable]
    public class AudioClipSounds
    {
        public Script_AudioPlayer.Sounds sound;
        public AudioClip audioClip;
    }

}
