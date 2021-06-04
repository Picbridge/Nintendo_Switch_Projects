using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    private float duration;
    private float timeStamp;
    private bool isOnCool;
    // Use this for initialization
    void Start ()
    {
        isOnCool = false;
        duration = 13;
        timeStamp = Time.time + duration;
    }
	
	// Update is called once per frame
	void Update ()
    {
        DeleteAfterDuration();

    }
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            for (int i = 0; i < collision.gameObject.transform.childCount; ++i)
            {
                collision.gameObject.transform.GetChild(i).gameObject.AddComponent<Rigidbody>();
                collision.gameObject.transform.GetChild(i).gameObject.AddComponent<BoxCollider>();
            }

            collision.gameObject.transform.DetachChildren();
            collision.gameObject.SetActive(false);
            gameObject.SetActive(false);
        }
    }

    private void DeleteAfterDuration()
    {
        if (timeStamp <= Time.time)
        {
            isOnCool = true;
            timeStamp += Mathf.Infinity;
            gameObject.transform.GetChild(1).GetComponent<ParticleSystem>().Play();
            //gameObject.SetActive(false);
        }
        if (isOnCool&&gameObject.transform.GetChild(1).GetComponent<ParticleSystem>().isStopped)
        {
            gameObject.SetActive(false);
        }
    }
}
