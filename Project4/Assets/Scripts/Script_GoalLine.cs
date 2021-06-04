/****************************************************************************/
/*!
\file   Script_GoalLine.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   12/10/2019
\brief
A goal line that indicates which player is the winner when collision occurs.
Apply to the goal line with trigger collider.
Does not get any parameter. Parameter winner is used for handing player info.

©️ 2019 DigiPen (USA) Corporation, all rights reserved.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_GoalLine : MonoBehaviour
{
    public GameObject winner;
    // Start is called before the first frame update
    void Start()
    {
        winner = null;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == "Player")
        {
            winner = other.gameObject;
        }
    }
}
