using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BladesSpin : MonoBehaviour
{
    [SerializeField]
    private float rotationSpeed;
    Vector3 rotation;
	// Use this for initialization
	void Start ()
    {
        //rotationSpeed = 1;
        //rotation.y = rotationSpeed;
    }
	
	// Update is called once per frame
	void Update ()
    {
        rotation.y += Time.deltaTime*rotationSpeed;
        transform.localRotation = Quaternion.Euler(rotation);
	}
}
