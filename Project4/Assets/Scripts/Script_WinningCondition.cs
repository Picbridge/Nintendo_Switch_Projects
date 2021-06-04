/****************************************************************************/
/*!
\file   Script_WinningCondition.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   12/10/2019
\brief
Checks if there's winner. 
Count all the characters that are gotten rid of.
Apply to empty game object.
Apply the empty game object to camera object in order to zoom in when won.

Gets all individual characters as parameter.
Gets all UI sprites as parameter.

©️ 2019 DigiPen (USA) Corporation, all rights reserved.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using nn.hid;

public class Script_WinningCondition : MonoBehaviour
{
    [SerializeField]
    private GameObject P1 = null, P2 = null, P3 = null, P4 = null;

    [SerializeField]
    private GameObject winState=null;

    [SerializeField]
    private Camera viewport;

    [SerializeField]
    private Text t_P1 = null, t_P2 = null, t_P3 = null, t_P4 = null, t_Nobody = null;

    [SerializeField]
    private Script_GoalLine goalLine = null;

    public Transform winner;
    private int playerLeft;
    private int totalPlayer;
    private bool[] isPlayerLost;
    public bool isGameOver;
    public bool isGoneThroughGoal;


    //private int controllers;
    //private NpadId[] npadIds = { NpadId.Handheld, NpadId.No1, NpadId.No2, NpadId.No3, NpadId.No4 };
    // Use this for initialization
    void Start()
    {
        //DetectControllerConnection();
        //if (controllers > 2)
        //    totalPlayer = controllers;
        //else
        totalPlayer = 4;
        playerLeft = totalPlayer;

        isPlayerLost = new bool[playerLeft];
        for (int i = 0; i < playerLeft; i++)
        {
            isPlayerLost[i] = false;
        }
        isGameOver = false;
        isGoneThroughGoal = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!isGameOver)
        {
            CheckAlivePlayer();
            SetWinCondition();
        }
    }
    private void CheckAlivePlayer()
    {
        
        if (!P1.activeSelf && !isPlayerLost[0])
        {
            playerLeft -= 1;
            isPlayerLost[0] = true;
        }
        if (!P2.activeSelf && !isPlayerLost[1])
        {
            playerLeft -= 1;
            isPlayerLost[1] = true;
        }
        if (!P3.activeSelf && totalPlayer >= 3 && !isPlayerLost[2])
        {
            playerLeft -= 1;
            isPlayerLost[2] = true;
        }
        if (!P4.activeSelf && totalPlayer >= 4 && !isPlayerLost[3])
        {
            playerLeft -= 1;
            isPlayerLost[3] = true;
        }
    }
    private void SetWinCondition()
    {
        if (playerLeft == 1)
        {
            winState.SetActive(true);
            isGameOver = true;
            if (P1.activeSelf)
            {
                //P1 Won
                winner = P1.transform;
                //P1.SetActive(false);
                t_P1.enabled = true;
            }
            else if (P2.activeSelf)
            {
                //P2 Won
                winner = P2.transform;
                //P2.SetActive(false);
                t_P2.enabled = true;
            }
            else if (P3.activeSelf)
            {
                //P3 Won
                winner = P3.transform;
                //P3.SetActive(false);
                t_P3.enabled = true;
            }
            else if (P4.activeSelf)
            {
                //P4 Won
                winner = P4.transform;
                //P4.SetActive(false);
                t_P4.enabled = true;
            }
        }
        else if (playerLeft == 0)
        {
            winState.SetActive(true);
            isGameOver = true;
            t_Nobody.enabled = true;
        }
        if(goalLine.winner!=null)
        {
            winner = goalLine.winner.transform;
            isGoneThroughGoal = true;
        }
    }

    //private void DetectControllerConnection()
    //{
    //    controllers = npadIds.Length;
    //    for (int i = 0; i < npadIds.Length; i++)
    //    {
    //        NpadId npadId = npadIds[i];
    //        NpadStyle npadStyle = Npad.GetStyleSet(npadId);
    //        if (npadStyle == NpadStyle.None) { controllers--; }
    //    }
    //}
    public void RestartLevel()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }
}
