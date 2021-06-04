using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class controller : MonoBehaviour {

    public float speed;

    // Use this for initialization
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        float translation = Input.GetAxis("Vertical") * speed;
        float straffe = Input.GetAxis("Horizontal") * speed;

        translation *= Time.deltaTime;
        straffe *= Time.deltaTime;

        transform.Translate(-translation, 0, straffe);
    }
}
