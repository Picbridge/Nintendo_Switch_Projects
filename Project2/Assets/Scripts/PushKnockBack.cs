using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PushKnockBack : MonoBehaviour
{
    public float slapSpeed;
    public enum Slap { IDLE, HIT, MISSED };
    public Slap state;
    [SerializeField]
    private ParticleSystem p_Slap;
    // Use this for initialization
    void Start ()
    {
        state = Slap.IDLE;
    }
	
	// Update is called once per frame
	void Update ()
    {
        transform.eulerAngles = transform.parent.eulerAngles;   
        if (state == Slap.MISSED)
        {
            state = Slap.IDLE;
            transform.parent.parent.GetComponent<Rigidbody>().constraints = 0;
            transform.parent.parent.gameObject.GetComponent<Rigidbody>().AddForce(transform.parent.parent.forward * slapSpeed *5);  
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            p_Slap.Play();
            Vector3 direction = (transform.position - collision.transform.position).normalized;
            collision.rigidbody.constraints = 0;
            state = Slap.HIT;
            collision.gameObject.GetComponent<Rigidbody>().AddForce(-direction * slapSpeed + transform.up*slapSpeed*5);
        }
    }

}
