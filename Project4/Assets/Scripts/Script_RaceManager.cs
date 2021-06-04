/****************************************************************************/
/*!
\file   Script_AudioPlayer.cs
\author Brian Lu
\par    email: b.lu@digipen.edu
\date   12/11/2019
\brief
Handles keeping track of current state of the race, mainly used to determine 
first player in the race for the camera.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_RaceManager : MonoBehaviour
{
    public GameObject players;
    public GameObject checkpoints;
    public int numAmmoBoxes = 5;
    public GameObject ammoBox;
    public Script_RailManager rail;

    private List<GameObject> playerList = new List<GameObject>();
    private List<GameObject> checkpointList = new List<GameObject>();
    // Start is called before the first frame update
    void Start()
    {
        foreach(Transform player in players.transform)
        {
            playerList.Add(player.gameObject); //adds each player in the list
        }
        foreach(Transform checkpoint in checkpoints.transform)
        {
            checkpointList.Add(checkpoint.gameObject);
        }

        for (int i = 0; i < numAmmoBoxes; ++i)
        {
            GameObject aB = Instantiate(ammoBox);
            aB.transform.position = rail.GetRandomPositionOnTrack();
            aB.SetActive(true);
        }
    }

    // Update is called once per frame
    void Update()
    {
        //GetFirstPlayer();
        //Debug.Log(GetFirstPlayer().name);
    }

    public GameObject GetFirstPlayer()
    {
        int firstPlace = 0;
        int index = 0; 

        for(int i = 0; i < playerList.Capacity; ++i)
        {
            // int lap = playerList[i].GetComponent<Script_PlayerInfo>().GetCurrentLap();
            int currentCheckpoint = playerList[i].GetComponent<Script_PlayerInfo>().GetCurrentCheckpoint(); //+ (lap * checkpointList.Count); handles looping track

            if (firstPlace < currentCheckpoint)
            {
                firstPlace = currentCheckpoint;
                index = i;
            }
            else if(firstPlace == currentCheckpoint && index != i) //more than one player is on the same checkpoint
            {
                //check distance from previous node -- will probably do something more precise later, might not need to since camera is fixed to track
                float dist0 = Vector3.Distance(checkpointList[currentCheckpoint].transform.position, playerList[i].transform.position); //potential leader
                float dist1 = Vector3.Distance(checkpointList[currentCheckpoint].transform.position, playerList[index].transform.position); //current leader

                //Debug.Log(playerList[i].name + " dist: " +dist0);
                //Debug.Log(playerList[index].name + " dist: " + dist1);
                if (dist0 < dist1)
                {
                    index = i;
                }
            }
        }

        return playerList[index];
    }
}
