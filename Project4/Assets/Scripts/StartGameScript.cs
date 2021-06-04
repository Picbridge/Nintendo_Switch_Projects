/****************************************************************
 * File: StartGameScript.cs
 * Author: Matthew Adams
 * 
 * ©️ 2019 DigiPen (USA) Corporation, all rights reserved.
 ***************************************************************/

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using nn.hid;

public class StartGameScript : MonoBehaviour
{
    public RectTransform button;
    float minX, minY, maxX, maxY;

    // Start is called before the first frame update
    void Start()
    {
        Time.timeScale = 0;

        minX = button.position.x - (button.rect.width / 2);
        minY = button.position.y - (button.rect.height / 2);
        maxX = button.position.x + (button.rect.width / 2);
        maxY = button.position.y + (button.rect.height / 2);
    }

    // Update is called once per frame
    void Update()
    {
        GestureState gs = NpadInputManager.GetGestureState();

        if (gs.type == nn.hid.GestureType.Touch)
        {
            if (gs.x >= minX && gs.x <= maxX && gs.y >= minY && gs.y <= maxY)
            {
                Time.timeScale = 1;
                Destroy(gameObject);
            }

        }
    }
}
