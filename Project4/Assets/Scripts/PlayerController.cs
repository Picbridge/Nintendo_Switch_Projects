/****************************************************************
 * File: PlayerController.cs
 * Author: Matthew Adams
 * 
 * ©️ 2019 DigiPen (USA) Corporation, all rights reserved.
 ***************************************************************/

using UnityEngine;
using UnityEngine.UI;

public class PlayerController : MonoBehaviour
{
    public int playerNumber = 0;

    public float zeroForwardRotation = 1.0f;

    public float defaultSpeed = 2;
    private float speed;
    public float maxSpeed = 0.5f;
    public float turnSpeed = 60;
    public float strafeSpeed = 3.0f;
    public float turretTurnSpeed = 2;

    public float jumpHeight = 5;
    public float jumpMovementThreshold = 0.4f;

    public int ammo = 3;
    //public int resupplyAmount = 3;
    public float cooldown = 3.0f;
    private float shootCooldown;
    public float bulletSpeed = 3.0f;
    public float bulletSlowSpeed = 0.35f;
    public float bulletSlowLength = 2.0f;
    private float slowTime = 0;

    private float upwardVelocity = 0;
    private CharacterController charController;
    public GameObject turret;
    public GameObject bullet;
    public GameObject treadParticles;
    public Camera cam;
    public GameObject score;

    public Vector3 testVec;

    public Text ammoUI;

    // Start is called before the first frame update
    void Start()
    {
        charController = GetComponent<CharacterController>();
        shootCooldown = cooldown;
        ammoUI.text = ammo.ToString();
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (slowTime <= 0)
            speed = defaultSpeed;
        else
            slowTime -= Time.fixedDeltaTime;

        Vector3 lookAt = transform.position - cam.transform.position;
        Vector3 scoreRotation = Vector3.RotateTowards(score.transform.forward, lookAt, float.MaxValue, float.MaxValue);
        score.transform.forward = scoreRotation;


        /*
            // Jump. Using touch screen.
            nn.hid.GestureState gs = NpadInputManager.GetGestureState();
            if (charController.isGrounded && gs.type == nn.hid.GestureType.Touch)
                upwardVelocity = jumpHeight;

            // Jump. By Pressing the B button.
            if (charController.isGrounded && NpadInputManager.isKeyPressed(NpadInputManager.KeyCode.B, playerNumber))
                upwardVelocity = jumpHeight;

            // Jump. By flicking the right joycon up.
            nn.hid.SixAxisSensorState sensorState = NpadInputManager.GetMotionControlState(playerNumber, false);
            if (charController.isGrounded && sensorState.angularVelocity.x > jumpMovementThreshold)
                upwardVelocity = jumpHeight;
        */

// forward/back and rotation movement using motion controls in left joycon
nn.hid.SixAxisSensorState sensorState = NpadInputManager.GetMotionControlState(playerNumber, true);

        // rotate with left/right rotation of joycon (in horizontal mode)
        if (sensorState.acceleration.y > 0.05f)
            transform.Rotate(transform.up, -sensorState.acceleration.y * turnSpeed * Time.fixedDeltaTime);
        else if (sensorState.acceleration.y < -0.05f)
            transform.Rotate(transform.up, -sensorState.acceleration.y * turnSpeed * Time.fixedDeltaTime);

        // get forward/back movement magnitude by rotating away/toward you (in horizontal mode)
        float acc = sensorState.acceleration.x + zeroForwardRotation;
        acc = Mathf.Clamp(acc, -maxSpeed, maxSpeed);
        float moveX = acc * speed * Time.fixedDeltaTime;

        if (acc > 0.3f)
            treadParticles.SetActive(true);
        else
            treadParticles.SetActive(false);

        // Movement in direction of forward vector
        upwardVelocity += Physics.gravity.y * Time.fixedDeltaTime;
        Vector3 movement = transform.forward * moveX;

        if (movement.magnitude > 0)
            Script_AudioPlayer.PlaySound(Script_AudioPlayer.Sounds.TankMovement, .5f);

        movement.y += upwardVelocity;
        GetComponent<CharacterController>().Move(movement);
        
        // Rotate turret using left stick
        Vector2 leftStick = NpadInputManager.GetAxis(true, playerNumber);
        if (leftStick.magnitude > 0)
        {
            float turretAngle = Vector2.SignedAngle(new Vector2(0, 1), leftStick) * Mathf.Deg2Rad;
            float sin = Mathf.Sin(turretAngle);
            float cos = Mathf.Cos(turretAngle);
            turret.transform.rotation = Quaternion.LookRotation(new Vector3(transform.forward.x * cos - transform.forward.z * sin, 0, transform.forward.x * sin + transform.forward.z * cos));
        }

        shootCooldown -= Time.fixedDeltaTime;
        if (ammo > 0 && shootCooldown <= 0 && NpadInputManager.isKeyPressed(NpadInputManager.KeyCode.A, playerNumber))
            FireCannon();
    }

    void FireCannon()
    {
        GameObject b = Instantiate(bullet);
        Vector3 pos = b.transform.localPosition;
        b.transform.parent = null;
        b.transform.localScale *= 36;
        pos.y += 0.2f;
        b.transform.position = transform.position + pos;
        
        Vector3 dir = turret.transform.forward; dir.y += 0.12f;
        b.GetComponent<Rigidbody>().velocity = dir.normalized * bulletSpeed;
        b.transform.position += dir * bulletSpeed * Time.fixedDeltaTime;

        b.GetComponent<Script_Bullet>().playerThatFiredMe = playerNumber;

        b.SetActive(true);

        shootCooldown = cooldown;
        --ammo;
        ammoUI.text = ammo.ToString();
        Script_AudioPlayer.PlaySound(Script_AudioPlayer.Sounds.TankShoot, .5f);
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.collider.tag == "Bullet")
        {
            if (collision.collider.GetComponent<Script_Bullet>().playerThatFiredMe != playerNumber)
            {
                Script_AudioPlayer.PlaySound(Script_AudioPlayer.Sounds.TankHit, .5f);

                speed = bulletSlowSpeed;
                slowTime = bulletSlowLength;
            }
        }
        //else if (collision.collider.tag == "Resupply")
        //{
        //    ammo += 3;// resupplyAmount;
        //    Destroy(collision.collider.gameObject);
        //    ammoUI.text = ammo.ToString();
        //}
    }
}
