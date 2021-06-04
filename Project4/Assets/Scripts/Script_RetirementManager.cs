/****************************************************************************/
/*!
\file   Script_RetirementManager.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   11/18/2019
\brief
Packs up the node of characters made 
and enables camera to check if character is in the viewport. 
Inactivate all the characters out of viewport
Apply to parent of the set of characters.
Get main camera as parameter.

©️ 2019 DigiPen (USA) Corporation, all rights reserved.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_RetirementManager : MonoBehaviour
{
    public Transform[] characters;
    [SerializeField]
    private Camera viewport = null;
    private bool[] isRetired;
    // Start is called before the first frame update
    void Start()
    {
        characters = new Transform[transform.childCount];
        isRetired = new bool[transform.childCount];
        for (int i = 0; i < characters.Length; i++)
        {
            characters[i] = transform.GetChild(i);
            isRetired[i] = false;
        }
    }

    // Update is called once per frame
    void Update()
    {
        RetirementManage();
    }

    void RetirementManage()
    {
        for (int i = 0; i < characters.Length; i++)
        {
           
            if(!isRetired[i]&&!IsInViewport(characters[i].position))
            {
                characters[i].gameObject.transform.GetChild(0).GetComponent<ParticleSystem>().Play();
                isRetired[i] = true;
            }
            if (isRetired[i]&&characters[i].gameObject.transform.GetChild(0).GetComponent<ParticleSystem>().isStopped)
            {
                characters[i].gameObject.SetActive(false);
            }

        }
    }

    bool IsInViewport(Vector3 pos)
    {
        Vector3 screenPoint = viewport.WorldToViewportPoint(pos);
        bool isInViewport = screenPoint.z > 0 && screenPoint.x > 0 && screenPoint.x < 1 && screenPoint.y > 0 && screenPoint.y < 1;

        return isInViewport;
    }
}
