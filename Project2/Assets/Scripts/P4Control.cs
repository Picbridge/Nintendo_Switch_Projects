using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class P4Control : MonoBehaviour
{
    public float speed;
    Vector3 movement;
    public float slapSpeed;
    // Total distance between the markers.
    private float distLeft;
    private float distRight;
    // Time when the movement started.
    private Vector3 destinationRight;
    private Vector3 destinationLeft;
    float distCoveredLeft;
    float fractionLeft;
    float distCoveredRight;
    float fractionRight;

    private float armLength;
    private bool isLeftArmSlap;
    private bool isLeftReturn;
    private bool isRightArmSlap;
    private bool isRightReturn;

    private float timeStamp;
    [SerializeField]
    private float cooldown;
    private bool isOnCool;

    private bool isLost;
    // Use this for initialization
    void Start()
    {
        isLeftArmSlap = false;
        isLeftReturn = false;
        isRightArmSlap = false;
        isRightReturn = false;
        isOnCool = false;
        isLost = false;
        armLength = 0.5f;
    }

    // Update is called once per frame
    void Update()
    {
        //Check if player lost balance
        if ((transform.rotation.eulerAngles.z > (360 - 45) || transform.rotation.eulerAngles.z < 45) && !isLost)
        {
            movement = new Vector3(-Input.GetAxisRaw("JoyConVerticalP4"), 0, Input.GetAxisRaw("JoyconHorizontalP4"));
            LookUpdate();
            KeyInputUpdate();
            LeftSlapUpdate();
            RightSlapUpdate();

            transform.Translate(movement * speed * Time.deltaTime, Space.World);
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Blade")
        {
            for (int i = 0; i < transform.childCount; ++i)
            {
                transform.GetChild(i).gameObject.AddComponent<Rigidbody>();
                transform.GetChild(i).gameObject.AddComponent<BoxCollider>();
            }

            transform.DetachChildren();
            gameObject.SetActive(false);
        }
        else if (collision.gameObject.tag == "Ground" && (transform.rotation.eulerAngles.z < (360 - 85) && transform.rotation.eulerAngles.z > 85))
        {
            isLost = true;
            for (int i = 0; i < transform.childCount; ++i)
            {
                transform.GetChild(i).gameObject.AddComponent<Rigidbody>();
                Destroy(transform.GetChild(i).gameObject.GetComponent<BoxCollider>());
            }

            transform.DetachChildren();
            gameObject.SetActive(false);
        }
        else if (collision.gameObject.tag == "Boundary")
        {
            isLost = true;
            for (int i = 0; i < transform.childCount; ++i)
            {
                transform.GetChild(i).gameObject.AddComponent<Rigidbody>();
                Destroy(transform.GetChild(i).gameObject.GetComponent<BoxCollider>());
            }

            transform.DetachChildren();
            gameObject.SetActive(false);
        }
    }
    void LookUpdate()
    {
        if (gameObject.GetComponent<Rigidbody>().constraints == 0 && !isOnCool)
        {
            timeStamp = Time.time + cooldown;
            isOnCool = true;
        }
        if (movement != Vector3.zero)
        {
            transform.rotation = Quaternion.LookRotation(movement);
            if (timeStamp <= Time.time)
            {
                gameObject.GetComponent<Rigidbody>().constraints = RigidbodyConstraints.FreezeRotationX | RigidbodyConstraints.FreezeRotationZ;
                isOnCool = false;
            }
        }
    }
    void KeyInputUpdate()
    {

        if (Input.GetKeyDown("JoyConLeftSlapP4") && !isLeftReturn)
        {
            transform.GetChild(0).GetChild(0).gameObject.AddComponent<BoxCollider>();
            isLeftArmSlap = true;
        }
        if (Input.GetKeyDown("JoyConRightSlapP4") && !isRightReturn)
        {
            transform.GetChild(2).GetChild(0).gameObject.AddComponent<BoxCollider>();
            isRightArmSlap = true;
        }
    }
    void LeftSlapUpdate()
    {
        if (isLeftArmSlap)
        {

            distCoveredLeft = Time.deltaTime * slapSpeed;
            destinationLeft = transform.GetChild(0).position + transform.forward;
            distLeft = Vector3.Distance(transform.GetChild(1).position, destinationLeft);
            fractionLeft = distCoveredLeft / distLeft;
            transform.GetChild(0).position = Vector3.Lerp(transform.GetChild(0).position, destinationLeft, fractionLeft);

            transform.GetChild(0).eulerAngles = transform.GetChild(1).eulerAngles;

            if (Vector3.Distance(transform.GetChild(1).position, transform.GetChild(0).position) > armLength)
            {

                Destroy(transform.GetChild(0).GetChild(0).gameObject.GetComponent<BoxCollider>());
                destinationLeft = transform.GetChild(1).position;
                isLeftArmSlap = false;
                isLeftReturn = true;
            }
        }
        else
        {
            if (isLeftReturn)
            {
                distCoveredLeft = Time.deltaTime * slapSpeed;
                distLeft = Vector3.Distance(transform.GetChild(1).position, destinationLeft);
                fractionLeft = distCoveredLeft / distLeft;
                transform.GetChild(0).eulerAngles = transform.GetChild(1).eulerAngles;
                transform.GetChild(0).position = Vector3.Lerp(transform.GetChild(0).position, destinationLeft, fractionLeft);
                if (Vector3.Distance(destinationLeft, transform.GetChild(0).position) < 0.5f)
                {
                    if (transform.GetChild(0).GetChild(0).gameObject.GetComponent<PushKnockBack>().state != PushKnockBack.Slap.HIT)
                    {
                        transform.GetChild(0).GetChild(0).gameObject.GetComponent<PushKnockBack>().state = PushKnockBack.Slap.MISSED;
                    }
                    else
                        transform.GetChild(0).GetChild(0).gameObject.GetComponent<PushKnockBack>().state = PushKnockBack.Slap.IDLE;
                    isLeftReturn = false;
                }
            }
        }
    }
    void RightSlapUpdate()
    {
        if (isRightArmSlap)
        {

            distCoveredRight = Time.deltaTime * slapSpeed;
            destinationRight = transform.GetChild(2).position + transform.forward;
            distRight = Vector3.Distance(transform.GetChild(3).position, destinationRight);
            fractionRight = distCoveredRight / distRight;
            transform.GetChild(2).position = Vector3.Lerp(transform.GetChild(2).position, destinationRight, fractionRight);

            transform.GetChild(2).eulerAngles = transform.GetChild(3).eulerAngles;

            if (Vector3.Distance(transform.GetChild(3).position, transform.GetChild(2).position) > armLength)
            {

                Destroy(transform.GetChild(2).GetChild(0).gameObject.GetComponent<BoxCollider>());
                destinationRight = transform.GetChild(3).position;
                isRightArmSlap = false;
                isRightReturn = true;
            }
        }
        else
        {
            if (isRightReturn)
            {

                distCoveredRight = Time.deltaTime * slapSpeed;
                distRight = Vector3.Distance(transform.GetChild(3).position, destinationRight);
                fractionRight = distCoveredRight / distRight;
                transform.GetChild(2).eulerAngles = transform.GetChild(3).eulerAngles;
                transform.GetChild(2).position = Vector3.Lerp(transform.GetChild(2).position, destinationRight, fractionRight);
                if (Vector3.Distance(destinationRight, transform.GetChild(2).position) < 0.5f)
                {
                    if (transform.GetChild(2).GetChild(0).gameObject.GetComponent<PushKnockBack>().state != PushKnockBack.Slap.HIT)
                    {
                        transform.GetChild(2).GetChild(0).gameObject.GetComponent<PushKnockBack>().state = PushKnockBack.Slap.MISSED;
                    }
                    else
                        transform.GetChild(2).GetChild(0).gameObject.GetComponent<PushKnockBack>().state = PushKnockBack.Slap.IDLE;
                    isRightReturn = false;
                }
            }
        }
    }
}
