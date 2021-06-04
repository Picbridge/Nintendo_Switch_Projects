using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Elevator : MonoBehaviour
{
    Vector3 elevatePos;
    float startPointX;
    public GameObject ball;
    public GameObject allocateLocation;
    public float speed;
    public float rotSpeed;
    public bool isElevatorOn;
    public bool isArrived;
    public bool isGoing;
    public bool isDropped;
    // Use this for initialization
    void Start ()
    {
        elevatePos = new Vector3(0, 0, 0);
        isElevatorOn = false;
        isArrived = true;
        startPointX = transform.position.x;
        isDropped = false;
    }
	
	// Update is called once per frame
	void Update ()
    {
        
        if (Input.GetKeyDown(KeyCode.Mouse0/*JoystickButton1*/)&&!ScoreManager.isGameOver&&!SplashManager.isSplashOn)
        {
            if(isGoing)
            {
                isElevatorOn = !isElevatorOn;
                transform.GetChild(0).gameObject.SetActive(false);
                isDropped = true;
            }
            else
            {
                isElevatorOn = !isElevatorOn;
                isGoing = true;
            }
            
        }

        if (isElevatorOn)
        {
            elevatePos.x = speed;
            transform.position += elevatePos;

            if (transform.position.x >= 5.5f)
            {
                transform.GetChild(0).gameObject.SetActive(false);
                isDropped = true;
                isElevatorOn = false;
                isGoing = false;
                
            }
           
        }

        if(isDropped)
        {
            if(transform.position.x <= startPointX)
            {
                transform.GetChild(0).gameObject.SetActive(true);
                isDropped = false;
                isGoing = false;
                isArrived = true;
            }
            else
            transform.position -= elevatePos;
        }
        if(isArrived)
        {
            Instantiate(ball,allocateLocation.transform.position,Quaternion.identity);
            isArrived = false;
        }

    }
}
