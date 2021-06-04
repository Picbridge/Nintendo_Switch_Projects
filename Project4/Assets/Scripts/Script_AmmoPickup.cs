/****************************************************************
 * File: Script_AmmoPickup.cs
 * Author: Matthew Adams
 * 
 * ©️ 2019 DigiPen (USA) Corporation, all rights reserved.
 ***************************************************************/

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_AmmoPickup : MonoBehaviour
{
    public const int resupplyAmount = 3;
    bool isValid = true;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (!isValid) return;
        if (collision.collider.tag == "Player")
        {
            isValid = false;
            PlayerController pC = collision.collider.GetComponent<PlayerController>();
            pC.ammo += resupplyAmount;
            pC.ammoUI.text = pC.ammo.ToString();
            Destroy(gameObject);
        }
    }
}
