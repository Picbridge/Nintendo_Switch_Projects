/****************************************************************************/
/*!
\file   CameraMovement.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   11/18/2019
\brief
Packs up the node of waypoints made. 
And enables camera movement based on smoothed rail nodes and character movement
Apply to parent of the set of waypoints.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraManager : MonoBehaviour
{
    public Transform[] nodes;
    private int nodeCount;

    // Use this for initialization
    void Start()
    {
        nodeCount = transform.childCount;
        nodes = new Transform[nodeCount];

        for (int i = 0; i < nodeCount; i++)
        {
            nodes[i] = transform.GetChild(i);
        }
    }

    // Update is called once per frame
    void Update()
    {

        for (int i = 0; i < nodeCount - 1; i++)
        {
            Debug.DrawLine(nodes[i].position, nodes[i + 1].position, Color.green);
        }

    }
    
    public Vector3 CatmullPosition(int seg, float ratio)
    {

        Vector3 p1, p2, p3, p4;

        if (seg == 0)
        {
            p1 = nodes[seg].position;
            p2 = p1;
            p3 = nodes[seg + 1].position;
            p4 = nodes[seg + 2].position;
        }
        else if (seg == nodes.Length - 2)
        {
            p1 = nodes[seg - 1].position;
            p2 = nodes[seg].position;
            p3 = nodes[seg + 1].position;
            p4 = p3;
        }
        else if (seg >= nodes.Length - 1)
        {
            p1 = nodes[seg-1].position;
            p2 = nodes[nodes.Length - 1].position;
            p3 = p2;
            p4 = p2;
        }
        else if (seg < 0)
        {
            p1 = nodes[0].position;
            p2 = p1;
            p3 = p1;
            p4 = p1;
        }
        else
        {
            p1 = nodes[seg - 1].position;
            p2 = nodes[seg].position;
            p3 = nodes[seg + 1].position;
            p4 = nodes[seg + 2].position;
        }
        Debug.Log(seg);
        float t2 = ratio * ratio;
        float t3 = t2 * ratio;

        float x = 0.5f * ((2.0f * p2.x)
            + (-p1.x + p3.x)
            * ratio + (2.0f * p1.x - 5.0f * p2.x + 4 * p3.x - p4.x)
            * t2 + (-p1.x + 3.0f * p2.x - 3.0f * p3.x + p4.x)
            * t3);

        float y = 0.5f * ((2.0f * p2.y)
            + (-p1.y + p3.y)
            * ratio + (2.0f * p1.y - 5.0f * p2.y + 4 * p3.y - p4.y)
            * t2 + (-p1.y + 3.0f * p2.y - 3.0f * p3.y + p4.y)
            * t3);

        float z = 0.5f * ((2.0f * p2.z)
            + (-p1.z + p3.z)
            * ratio + (2.0f * p1.z - 5.0f * p2.z + 4 * p3.z - p4.z)
            * t2 + (-p1.z + 3.0f * p2.z - 3.0f * p3.z + p4.z)
            * t3);

        return new Vector3(x, y, z);
    }
}
