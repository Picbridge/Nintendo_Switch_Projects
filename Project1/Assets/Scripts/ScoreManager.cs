using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class ScoreManager : MonoBehaviour
{
    public static int score;
    public static ScoreManager instance;
    public Text scoreText;
    public static bool isGameOver;
    [SerializeField]
    private GameObject winState;
    [SerializeField]
    private GameObject loseState;
    // Game Instance Singleton
    public static ScoreManager Instance
    {
        get
        {
            return instance;
        }
    }

    // Use this for initialization
    void Start ()
    {
        isGameOver = false;
        score = 0;
    }
	
	// Update is called once per frame
	void Update ()
    {
        scoreText.text = score.ToString();

        if(score<=-20&&!isGameOver)
        {
           // winState.SetActive(false);
            loseState.SetActive(true);
            isGameOver = true;
        }
        else if(score>=50&&!isGameOver)
        {
            //loseState.SetActive(false);
            winState.SetActive(true);
            isGameOver = true;
        }

    }

    public void RestartGame()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);//"SampleScene");
        
    }
}
