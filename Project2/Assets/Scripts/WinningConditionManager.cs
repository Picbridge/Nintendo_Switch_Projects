using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class WinningConditionManager : MonoBehaviour
{
    MultiplayerManager instance;
    [SerializeField]
    private GameObject P1 = null, P2 = null, P3 = null, P4 = null;

    [SerializeField]
    private GameObject winState;
    [SerializeField]
    private Text t_P1 = null, t_P2 = null, t_P3 = null, t_P4 = null, t_Nobody = null;

    private int playerLeft;
    private int totalPlayer;
    private bool[] isPlayerLost;
    private bool isGameOver;
    // Use this for initialization
    void Start ()
    {
        totalPlayer = 2;//instance.numOfPlayer;
        playerLeft = totalPlayer;
        isPlayerLost = new bool[playerLeft];
        for (int i = 0;i< playerLeft;i++)
        {
            isPlayerLost[i] = false;
        }
        isGameOver = false;
    }
	
	// Update is called once per frame
	void Update ()
    {
        if (!isGameOver)
        {
            CheckAlivePlayer();
            SetWinCondition();
        }
    }
    private void CheckAlivePlayer()
    {
        
        if(!P1.activeSelf && !isPlayerLost[0])
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
        if(playerLeft == 1)
        {
            winState.SetActive(true);
            isGameOver = true;
            if (P1.activeSelf)
            {
                //P1 Won
                P1.SetActive(false);
                t_P1.enabled = true;
            }
            else if (P2.activeSelf)
            {
                //P2 Won
                P2.SetActive(false);
                t_P2.enabled = true;
            }
            else if (P3.activeSelf)
            {
                //P3 Won
                P3.SetActive(false);
                t_P3.enabled = true;
            }
            else if (P4.activeSelf)
            {
                //P4 Won
                P4.SetActive(false);
                t_P4.enabled = true;
            }
        }
        else if(playerLeft == 0)
        {
            winState.SetActive(true);
            isGameOver = true;
            t_Nobody.enabled = true;
        }
    }
    public void RestartLevel()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }
}
