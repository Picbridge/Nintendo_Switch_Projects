/****************************************************************************/
/*!
\file   Script_AudioPlayer.cs
\author Brian Lu
\par    email: b.lu@digipen.edu
\date   12/11/2019
\brief
Small class to identify current checkpoint and lap
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_PlayerInfo : MonoBehaviour
{
    private int currentCheckpoint = 0;
    private int currentLap = 0;
    private GameObject prevCheckpoint;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public int GetCurrentCheckpoint()
    {
        return currentCheckpoint;
    }
    public int GetCurrentLap()
    {
        return currentLap;
    }
    private void OnTriggerEnter(Collider other)
    {
        //checks the last checkpoint hit
        if(prevCheckpoint != other.gameObject)
        {
            currentCheckpoint++;
            //Debug.Log(gameObject.name + "current checkpoint: " + currentCheckpoint);
        }
        prevCheckpoint = other.gameObject;       
    }
}
