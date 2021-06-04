using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BombRandomGenerate : MonoBehaviour
{
    [SerializeField]
    private GameObject bombPrefab;
    [SerializeField]
    private GameObject warnPrefab;
    private float timeStamp;
    [SerializeField]
    private float cooldown;
    private bool isOnCool;
    private float randomX;
    private float randomZ;
    // Use this for initialization
    void Start()
    {
        isOnCool = false; ;
        randomX = 0;
        randomZ = 0;
    }

    // Update is called once per frame
    void Update()
    {
        Generate();

    }

    private void Generate()
    {
        if (!isOnCool)
        {
            timeStamp = Time.time + cooldown;
            isOnCool = true;
        }

        if (timeStamp <= Time.time)
        {
            Vector2 newPos = Random.insideUnitCircle;
            randomX = newPos.x * 4;
            randomZ = newPos.y * 4;
            Instantiate(bombPrefab,new Vector3(randomX, 13, randomZ),Quaternion.identity);
            Instantiate(warnPrefab, new Vector3(randomX, 3, randomZ), Quaternion.identity);
            isOnCool = false;
        }

    }
}
