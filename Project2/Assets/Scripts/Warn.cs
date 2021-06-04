using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Warn : MonoBehaviour
{
    private float duration;
    private float timeStamp;
    // Use this for initialization
    void Start()
    {
        duration = 1.5f;
        timeStamp = Time.time + duration;
    }

    // Update is called once per frame
    void Update()
    {
        DeleteAfterDuration();

    }

    private void DeleteAfterDuration()
    {
        if (timeStamp <= Time.time)
        {
            timeStamp += Mathf.Infinity;
            gameObject.SetActive(false);
        }
    }
}
