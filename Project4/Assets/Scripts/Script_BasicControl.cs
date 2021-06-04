using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_BasicControl : MonoBehaviour
{
    public float speed;
    Vector3 movement;
    // Use this for initialization
    void Start()
    {
        //Hide the curser and lock the cursor inside the game
        //Cursor.lockState = CursorLockMode.Locked;
    }

    // Update is called once per frame
    void Update()
    {

        float translation = Input.GetAxisRaw("Vertical");
        float straffe = Input.GetAxisRaw("Horizontal");

        movement = new Vector3(-translation, 0, straffe);

        if (movement != Vector3.zero)
            transform.rotation = Quaternion.LookRotation(movement);

        //translation *= Time.deltaTime;
        //straffe *= Time.deltaTime;

        transform.Translate(movement * speed * Time.deltaTime, Space.World);
    }
}
