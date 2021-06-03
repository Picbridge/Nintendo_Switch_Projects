using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Script_CameraManager : MonoBehaviour
{
    //Limits the Y axis of player view based on the perspective
    private const float THIRD_Y_AXIS_MAX = 70f;
    private const float THIRD_Y_AXIS_MIN = 5f;

    private const float Y_AXIS_MIN = -30f;
    private const float Y_AXIS_MAX = 70f;
    //Time taken when perspective changes
    private const float CAM_CHANGE_TIME = .6f;

    Vector2 smoothV;

    //Coroutine switchingPerspective;
    [SerializeField] private float mouseX, mouseY;
    [SerializeField] private float pevMouseY;
    //Mouse sensitivity and screen smoothing(First person view)
    public float sensitivity = 1f;
    public float smoothing = 2f;

    public GameObject character;

    private float lerpTime = CAM_CHANGE_TIME;
    private float currentLerpTime;

    private Vector3 thirdViewPos;

    //Components of third view perspective direction&view
    private Vector3 tDir;
    private Quaternion tRotation;

    [SerializeField] private float damping = 100.0f;

    //Auto zoom used on third person view
    [SerializeField] private float thirdZoom = 5f;
    private float thirdZoomspeed;

    // Use this for initialization
    void Start()
    {
        //Initialize the third-view zoom
        tDir = new Vector3(thirdZoom, 7, character.transform.rotation.z);
    }

    // Update is called once per frame
    void Update()
    {

        thirdViewPos = character.transform.position + tDir;

        currentLerpTime += Time.deltaTime;
        if (currentLerpTime >= lerpTime)
        {
            currentLerpTime = lerpTime;
        }

        ThirdViewCamOn();
    }


    void ThirdViewCamOn()
    {
        pevMouseY = mouseY;
        //Update only if it is third person view
        mouseX += Input.GetAxis("Mouse X");
        mouseY -= Input.GetAxis("Mouse Y");


        thirdZoom = Mathf.SmoothDamp(thirdZoom, -mouseY * thirdZoomspeed, ref damping, thirdZoomspeed);

        mouseY = Mathf.Clamp(mouseY, THIRD_Y_AXIS_MIN, THIRD_Y_AXIS_MAX);

        ThirdLookUpdate();
    }

    void ThirdLookUpdate()
    {
        transform.position = thirdViewPos;
        transform.LookAt(character.transform.position);
        character.transform.localRotation = Quaternion.AngleAxis(mouseX, character.transform.up);

    }
}