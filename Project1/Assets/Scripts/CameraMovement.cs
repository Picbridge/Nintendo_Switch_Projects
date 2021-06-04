/****************************************************************************/
/*!
\file   CameraMovement.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   11/18/2019
\brief
Enables camera movement based on smoothed rail nodes and character movement
Apply to Camera.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraMovement : MonoBehaviour {

    public CameraManager Rail;
    public Transform PlayerTransform;
    public float zoomTimer = 0;
    public int currentSeg;
    public float transition;
    private const float distFromPlayer = 7;
    private const float cameraTransit = 1f / 2.5f;
    private Vector3 desiredPos,updatedRawPos;
    private float zoomIn = 0;
    enum Closest{ Prev,Current,Next};
    Closest updateClosest;
    // Update is called once per frame
    void Update()
    {
        if (!Rail)
            return;
        Movement();
    }

    private void Movement()
    {
        
        updateClosest = ClosestLocation();
        //Move forward if next node is closer  
        if (updateClosest == Closest.Next){ transition += Time.deltaTime * cameraTransit;}
        //Move back if prev node is closer  
        else if (updateClosest == Closest.Prev){transition -= Time.deltaTime * cameraTransit;}

        if (transition > 1 )
        {
            if (currentSeg < (Rail.nodes.Length - 2))
            {
                currentSeg++;
                transition = 0;
            }
            else
                transition = 1;
        }
        else if (transition < 0)
        {
            transition = 1;
            currentSeg--;
        }
        zoomIn += Vector3.Distance(transform.position, PlayerTransform.position)*Time.deltaTime/ zoomTimer;
        UpdatePosition();
    }
    //Returns true if first parameter is closer to the player, else false.
    private bool DistanceCompare(Vector3 l,Vector3 r)
    {
        return ((l.x - PlayerTransform.position.x)* (l.x - PlayerTransform.position.x) +
                (l.z - PlayerTransform.position.z)*(l.z - PlayerTransform.position.z) <
                (r.x - PlayerTransform.position.x) * (r.x - PlayerTransform.position.x) +
                (r.z - PlayerTransform.position.z) * (r.z - PlayerTransform.position.z)) ? true : false;
    }

    private Closest ClosestLocation()
    {
        bool currToPrev = DistanceCompare(Rail.CatmullPosition(currentSeg, transition), Rail.CatmullPosition(currentSeg, transition - Time.deltaTime * cameraTransit));
        bool currToNext = DistanceCompare(Rail.CatmullPosition(currentSeg, transition), Rail.CatmullPosition(currentSeg, transition + Time.deltaTime * cameraTransit));

        if (currToNext && currToPrev)
            return Closest.Current;
        else if (currToNext && !currToPrev)
            return Closest.Prev;
        else
            return Closest.Next;
    }

    private void UpdatePosition()
    {
        updatedRawPos = Rail.CatmullPosition(currentSeg, transition);
        desiredPos = updatedRawPos;
        desiredPos.x += distFromPlayer;// - (zoomIn + distFromPlayer/ zoomTimer);
        //desiredPos.y -= zoomIn;

        float fraction = Vector3.Distance(transform.position, desiredPos);
        transform.position = Vector3.Lerp(transform.position,desiredPos, fraction);
    }
}
