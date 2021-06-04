using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SplashManager : MonoBehaviour
{
    RawImage digiPen;
    RawImage anyKey;
    RawImage main;
    float alpha;
    float mainAlpha;
    bool isDigiLogoON;
    bool anyKeyPressed;
    public static bool isSplashOn;
    public static SplashManager instance;
    
    public static SplashManager Instance
    {
        get
        {
            return instance;
        }
    }
    private void Awake()
    {
        // if the singleton hasn't been initialized yet
        if (instance != null && instance != this)
        {
            Destroy(this.gameObject);
        }

        instance = this;
        DontDestroyOnLoad(this.gameObject);
    }
    // Use this for initialization
    void Start ()
    {

        digiPen = transform.GetChild(0).GetComponent<RawImage>();
        anyKey = transform.GetChild(1).GetComponent<RawImage>();
        main = GetComponent<RawImage>();
        alpha = 0f;
        mainAlpha = 1f;
        isDigiLogoON = false;
        isSplashOn = true;
        anyKeyPressed = false;
    }
	
	// Update is called once per frame
	void Update ()
    {
		if(alpha < 1f&& isDigiLogoON == false)
        {
            alpha += 0.01f;
            digiPen.color = new Color(digiPen.color.r, digiPen.color.g, digiPen.color.b, alpha);

        }
         else if(alpha>=1f)
        {
            alpha = 0f;
            transform.GetChild(1).gameObject.SetActive(true);
            isDigiLogoON = true;
        }
         else if(isDigiLogoON)
        {
            if (alpha < 1f)
            {
                alpha += 0.01f;
                anyKey.color = new Color(anyKey.color.r, anyKey.color.g, anyKey.color.b, alpha);
            }
            else
            {
                alpha -= 0.01f;
                anyKey.color = new Color(anyKey.color.r, anyKey.color.g, anyKey.color.b, alpha);
            }
            if(Input.anyKey)
            {
                anyKeyPressed = true;

            }
            
        }
        if (anyKeyPressed)
        {
            if (mainAlpha > 0f)
            {
                mainAlpha -= 0.01f;
                main.color = new Color(main.color.r, main.color.g, main.color.b, mainAlpha);
            }
            else
            {
                gameObject.SetActive(false);
                isSplashOn = false;
            }
        }
    }
}
