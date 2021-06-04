using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MultiplayerManager : MonoBehaviour
{
    MultiplayerManager instance;

    [SerializeField]
    private GameObject third;
    [SerializeField]
    private GameObject fourth;
    string[] controllers;

    public int numOfPlayer;
    // Use this for initialization
    void Start ()
    {
		
	}
	
	// Update is called once per frame
	void Update ()
    {
        DetectControllerConnection();
    }

    private void DetectControllerConnection()
    {
        controllers = Input.GetJoystickNames();
        numOfPlayer = controllers.Length;
        if (controllers.Length < 3)
        {
            third.SetActive(false);
            fourth.SetActive(false);
        }
        else if(controllers.Length < 4)
        {
            fourth.SetActive(false);
        }
    }
}
