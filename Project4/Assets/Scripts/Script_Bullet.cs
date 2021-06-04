/****************************************************************
 * File: Script_Bullet.cs
 * Author: Matthew Adams
 * 
 * ©️ 2019 DigiPen (USA) Corporation, all rights reserved.
 ***************************************************************/

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_Bullet : MonoBehaviour
{
    public int playerThatFiredMe;
    public float lifeTime = 5.0f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        lifeTime -= Time.fixedDeltaTime;
        if (lifeTime <= 0)
            Destroy(gameObject);
    }

    private void OnCollisionEnter(Collision collision)
    {
        PlayerController pC = collision.collider.GetComponent<PlayerController>();
        if (pC)
        {
            if (pC.playerNumber != playerThatFiredMe)
                Destroy(gameObject);
        }
    }
}
