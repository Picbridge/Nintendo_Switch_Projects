/****************************************************************************/
/*!
\file   Script_CameraManager.cs
\author Jihoon Bae
\par    email: aky2000kr\@gmail.com
\date   11/18/2019
\brief
Enables camera movement based on smoothed rail nodes and character movement
Apply to Camera.
Get parent of the rail with Script_RailManager applied as parameter.
Get character as parameter.
Get the winning condition manager object with Script_WinningCondition

©️ 2019 DigiPen (USA) Corporation, all rights reserved.
*/
/****************************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_CameraManager : MonoBehaviour
{
    public Script_RailManager Rail;
    private Transform PlayerTransform;
    public Script_RaceManager RaceManager = null;
    public Script_WinningCondition WinningCondition = null;

    public bool isSubCamera = false;
    //public float zoomTimer = 0;
    private int currentSeg;
    private float transition;
    private const float distFromPlayer = 12;
    private const float cameraTransit = 1f / 1.7f;
    private Vector3 desiredPos, updatedRawPos;
    private float zoomIn = 3;
    private float rotateSpeed = 7;
    enum Closest { Prev, Current, Next };
    Closest updateClosest;

    private void Start()
    {
        if(RaceManager)
            PlayerTransform = RaceManager.GetFirstPlayer().transform;
        Time.timeScale = 0;
    }
    // Update is called once per frame
    void Update()
    {
        if (!Rail)
            return;
        if (isSubCamera)
            PlayIntro();
        else
        {
            if (WinningCondition.isGameOver)
                rotateSpeed = 3;
            if (!WinningCondition.isGoneThroughGoal)
            {
                Movement();

                PlayerTransform = RaceManager.GetFirstPlayer().transform;
            }
            else//Zoom in to character won
            {
                Time.timeScale = 0;
                transform.LookAt(WinningCondition.winner);
                transform.Translate(Vector3.right * Time.unscaledDeltaTime * rotateSpeed);
                if (Vector3.Distance(transform.position, WinningCondition.winner.position) > zoomIn)
                    transform.Translate(Vector3.forward * Time.unscaledDeltaTime * rotateSpeed);
            }
        }
    }

    private void Movement()
    {

        updateClosest = ClosestLocation();
        //Move forward if next node is closer  
        if (updateClosest == Closest.Next) { transition += Time.unscaledDeltaTime * cameraTransit; }
        //Move back if prev node is closer  
        else if (updateClosest == Closest.Prev) { transition -= Time.unscaledDeltaTime * cameraTransit; }

        if (transition > 1)
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
        
        UpdatePosition();
    }

    private void PlayIntro()
    {

        transition += Time.unscaledDeltaTime * cameraTransit;

        if (transition > 1)
        {
            if (currentSeg < (Rail.nodes.Length - 2))
            {
                currentSeg++;
                transition = 0;
            }
            else
            {
                transition = 1;
                transform.gameObject.SetActive(false);
                Time.timeScale = 1;
            }
        }
        else if (transition < 0)
        {
            transition = 1;
            currentSeg--;
        }

        UpdateIntro();
    }

    //Returns true if first parameter is closer to the player, else false.
    private bool DistanceCompare(Vector3 l, Vector3 r)
    {
        return ((l.x - PlayerTransform.position.x) * (l.x - PlayerTransform.position.x) +
                (l.z - PlayerTransform.position.z) * (l.z - PlayerTransform.position.z) <
                (r.x - PlayerTransform.position.x) * (r.x - PlayerTransform.position.x) +
                (r.z - PlayerTransform.position.z) * (r.z - PlayerTransform.position.z)) ? true : false;
    }

    private Closest ClosestLocation()
    {
        bool currToPrev = DistanceCompare(Rail.CatmullPosition(currentSeg, transition), Rail.CatmullPosition(currentSeg, transition - Time.unscaledDeltaTime * cameraTransit));
        bool currToNext = DistanceCompare(Rail.CatmullPosition(currentSeg, transition), Rail.CatmullPosition(currentSeg, transition + Time.unscaledDeltaTime * cameraTransit));

        if (currToNext && currToPrev)
            return Closest.Current;
        else if (currToNext && !currToPrev)
            return Closest.Prev;
        else
            return Closest.Next;
    }
    private void UpdateIntro()
    {
        updatedRawPos = Rail.CatmullPosition(currentSeg, transition);
        desiredPos = updatedRawPos;
        transform.LookAt(desiredPos);
        float fraction = Vector3.Distance(transform.position, desiredPos);
        transform.position = Vector3.Lerp(transform.position, desiredPos, fraction);
    }

    private void UpdatePosition()
    {
        updatedRawPos = Rail.CatmullPosition(currentSeg, transition);
        desiredPos = updatedRawPos;
        desiredPos.x += distFromPlayer;// - (zoomIn + distFromPlayer/ zoomTimer);
        //desiredPos.y -= zoomIn;

        float fraction = Vector3.Distance(transform.position, desiredPos);
        transform.position = Vector3.Lerp(transform.position, desiredPos, fraction);
    }
}