using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BallManager : MonoBehaviour
{ 

    // Use this for initialization
    void Start ()
    {
    }
	
	// Update is called once per frame
	void Update () {
		
	}


    void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Blue")
        {
            ScoreManager.score -= 10;
            Destroy(gameObject);
        }
        else if (collision.gameObject.tag == "Green")
        {
            ScoreManager.score -= 2;
            Destroy(gameObject);
        }
        else if (collision.gameObject.tag == "Yellow")
        {
            ScoreManager.score += 10;
            Destroy(gameObject);
        }
        else if (collision.gameObject.tag == "Red")
        {
            ScoreManager.score += 5;
            Destroy(gameObject);
        }
    }
}
