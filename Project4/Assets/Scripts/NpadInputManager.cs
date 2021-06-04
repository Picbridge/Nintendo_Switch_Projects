/****************************************************************
 * File: NpadInputManager.cs
 * Author: Matthew Adams
 * 
 * ©️ 2019 DigiPen (USA) Corporation, all rights reserved.
 ***************************************************************/

 using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NpadInputManager : MonoBehaviour
{
    //------------------------------------------------------
    //
    //  Public Interface
    //
    //------------------------------------------------------

    public const byte minNumberOfPlayers = 2;
    public const byte maxNumberOfPlayers = 4;

    public struct KeyState
    {
        public KeyState(bool d, bool p, bool r) { keyDown = d; keyWasPressed = p; keyWasReleased = r; }
        public bool keyDown;
        public bool keyWasPressed;
        public bool keyWasReleased;
    }

    public enum KeyCode
    {
        A = 0,
        B,
        X,
        Y,
        L,
        R,
        ZL,
        ZR,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        START,
        SELECT,
        LEFTSTICK,
        RIGHTSTICK
    }

    public static KeyState GetKeyState(KeyCode k, int controllerId)
    {
        bool key = (keyState[controllerId] & ((uint)1 << (int)k)) != 0;
        bool trigger = (triggerState[controllerId] & ((uint)1 << (int)k)) != 0;
        return new KeyState(
                key,
                trigger && key,
                trigger && !key
            );
    }
    public static bool isKeyPressed(KeyCode k, int controllerId)
    {
        return (keyState[controllerId] & ((uint)1 << (int)k)) != 0;
    }
    public static bool wasKeyPressed(KeyCode k, int controllerId)
    {
        bool key = (keyState[controllerId] & ((uint)1 << (int)k)) != 0;
        bool trigger = (triggerState[controllerId] & ((uint)1 << (int)k)) != 0;
        return trigger && key;
    }
    public static bool wasKeyReleased(KeyCode k, int controllerId)
    {
        bool key = (keyState[controllerId] & ((uint)1 << (int)k)) != 0;
        bool trigger = (triggerState[controllerId] & ((uint)1 << (int)k)) != 0;
        return trigger && !key;
    }

    //public void Rumble(int controllerId)
    //{
    //    nn.hid.VibrationDeviceHandle vibe = new nn.hid.VibrationDeviceHandle();
    //    
    //    nn.hid.Vibration.GetDeviceHandles()
    //}

    public static nn.hid.GestureState GetGestureState()
    {
        return instance.gestureState;
    }

    public static nn.hid.SixAxisSensorState GetMotionControlState(int player, bool isLeft)
    {
        int index = (isLeft ? 0 : 1);
        
        //if (index < instance.handleCount[player])
            return instance.state[player * 2 + index];

        //return new nn.hid.SixAxisSensorState();
    }

    public static Vector2 GetAxis(bool leftStick, int controllerId)
    {
#if UNITY_SWITCH
#if !UNITY_EDITOR
        nn.hid.NpadStyle style = nn.hid.Npad.GetStyleSet((nn.hid.NpadId)controllerId);
        if (style == nn.hid.NpadStyle.JoyDual)
        {
            nn.hid.NpadJoyDualState n_state = new nn.hid.NpadJoyDualState();
            nn.hid.NpadJoyDual.GetState(ref n_state, (nn.hid.NpadId)controllerId);
            if (leftStick)
                return new Vector2(n_state.analogStickL.x, n_state.analogStickL.y) / nn.hid.AnalogStickState.Max;
            else
                return new Vector2(n_state.analogStickR.x, n_state.analogStickR.y) / nn.hid.AnalogStickState.Max;
        }
        else if (style == nn.hid.NpadStyle.JoyLeft)
        {
            nn.hid.NpadJoyLeftState n_state = new nn.hid.NpadJoyLeftState();
            nn.hid.NpadJoyLeft.GetState(ref n_state, (nn.hid.NpadId)controllerId);
        
            if (nn.hid.NpadJoy.GetHoldType() == nn.hid.NpadJoyHoldType.Vertical)
                return new Vector2(n_state.analogStickL.x, n_state.analogStickL.y) / nn.hid.AnalogStickState.Max;
            else
                return new Vector2(-n_state.analogStickL.y, n_state.analogStickL.x) / nn.hid.AnalogStickState.Max;
        }
        else if (style == nn.hid.NpadStyle.JoyRight)
        {
            nn.hid.NpadJoyRightState n_state = new nn.hid.NpadJoyRightState();
            nn.hid.NpadJoyRight.GetState(ref n_state, (nn.hid.NpadId)controllerId);

            if (nn.hid.NpadJoy.GetHoldType() == nn.hid.NpadJoyHoldType.Vertical)
                return new Vector2(n_state.analogStickR.x, n_state.analogStickR.y) / nn.hid.AnalogStickState.Max;
            else
                return new Vector2(n_state.analogStickR.y, -n_state.analogStickR.x) / nn.hid.AnalogStickState.Max;
        }
        style = nn.hid.Npad.GetStyleSet(nn.hid.NpadId.Handheld);
        if ((style & nn.hid.NpadStyle.Handheld) != 0)
        {
            nn.hid.NpadHandheldState nh_state = new nn.hid.NpadHandheldState();
            nn.hid.NpadHandheld.GetState(ref nh_state, nn.hid.NpadId.Handheld);
            if (leftStick)
                return new Vector2(nh_state.analogStickL.x, nh_state.analogStickL.y) / nn.hid.AnalogStickState.Max;
            else
                return new Vector2(nh_state.analogStickR.x, nh_state.analogStickR.y) / nn.hid.AnalogStickState.Max;
        }
#endif
#endif
        return new Vector2();
    }

    //------------------------------------------------------
    //
    //  Private Interface
    //
    //------------------------------------------------------

    private static ulong[] keyState;      // the current state of pressed keys
    private static ulong[] triggerState;  // if the current state is different (on press/release)

    private static NpadInputManager instance = null;

#if UNITY_SWITCH && !UNITY_EDITOR
    private int numNpadIds;
    private bool[] controllerConnected;
#endif
    private nn.hid.GestureState[] gestureStates;
    private nn.hid.GestureState gestureState;

    private nn.hid.SixAxisSensorHandle[][] handle = new nn.hid.SixAxisSensorHandle[4][];
    private nn.hid.SixAxisSensorState[] state = new nn.hid.SixAxisSensorState[8];
    private int[] handleCount = { 0, 0, 0, 0 };

    private void Awake()
    {
        if (instance != null) return;
        instance = this;

#if UNITY_SWITCH
#if !UNITY_EDITOR
        nn.hid.Npad.Initialize();
        //nn.hid.Npad.SetSupportedStyleSet(nn.hid.NpadStyle.FullKey | nn.hid.NpadStyle.JoyDual | nn.hid.NpadStyle.JoyLeft | nn.hid.NpadStyle.JoyRight);
        nn.hid.Npad.SetSupportedStyleSet(nn.hid.NpadStyle.JoyLeft | nn.hid.NpadStyle.JoyRight);
        nn.hid.NpadJoy.SetHoldType(nn.hid.NpadJoyHoldType.Horizontal);

        numNpadIds = maxNumberOfPlayers;
        nn.hid.NpadId[] NpadIds = new nn.hid.NpadId[numNpadIds];// = { nn.hid.NpadId.No1, nn.hid.NpadId.No2 };

        for (int i = 0; i < maxNumberOfPlayers; ++i) {
            NpadIds[i] = nn.hid.NpadId.No1 + 0x01 * i;
        }

        nn.hid.Npad.SetSupportedIdType(NpadIds, NpadIds.Length);
        keyState = new ulong[numNpadIds];
        triggerState = new ulong[numNpadIds];
        controllerConnected = new bool[numNpadIds];

        int numControllersConnected = 0;
        for (int i = 0; i < numNpadIds; ++i)
        {
            nn.hid.NpadState state = new nn.hid.NpadState();
            nn.hid.Npad.GetState(ref state, (nn.hid.NpadId)i, nn.hid.Npad.GetStyleSet((nn.hid.NpadId)i));

            controllerConnected[i] = (state.attributes & nn.hid.NpadAttribute.IsConnected) != 0;
            if (controllerConnected[i])
            {
                ++numControllersConnected;
            }

            handle[i] = new nn.hid.SixAxisSensorHandle[2];
        }

        if (numControllersConnected < minNumberOfPlayers)
            ShowControllerSupport();
        
        nn.hid.Gesture.Initialize();
        gestureStates = new nn.hid.GestureState[nn.hid.Gesture.StateCountMax];
        for (int i = 0; i < gestureStates.Length; i++) { gestureStates[i].SetDefault(); }

        gestureState.SetDefault();
        gestureState.eventNumber = -1;

        UpdateControllers();
#else
        keyState = new ulong[maxNumberOfPlayers];
        triggerState = new ulong[maxNumberOfPlayers];
#endif
#endif
    }
    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
#if UNITY_SWITCH && !UNITY_EDITOR
        UpdateControllers();
#else
        SetKeyStates(0);
#endif
    }

#if UNITY_SWITCH && !UNITY_EDITOR
    void UpdateControllers()
    {
        int gestureStateCount = nn.hid.Gesture.GetStates(gestureStates, gestureStates.Length);

        for (int i = gestureStateCount - 1; i >= 0; i--)
        {
            if (gestureStates[i].eventNumber <= gestureState.eventNumber) { continue; }
            gestureState = gestureStates[i];
        }

    

        for (int i = 0; i < numNpadIds; ++i)
        {
            nn.hid.NpadStyle style = nn.hid.Npad.GetStyleSet((nn.hid.NpadId)i);
            if (style == nn.hid.NpadStyle.None || style == nn.hid.NpadStyle.Invalid) continue;

            SetKeyStates(i);

            for (int j = 0; j < handleCount[i]; j++)
            {
                nn.hid.SixAxisSensor.Stop(handle[i][j]);
            }
            
            handleCount[i] = nn.hid.SixAxisSensor.GetHandles(handle[i], 2, (nn.hid.NpadId)i, style);
            for (int j = 0; j < handleCount[i]; j++)
            {
                nn.hid.SixAxisSensor.Start(handle[i][j]);
                nn.hid.SixAxisSensor.GetState(ref state[i*2 + j], handle[i][j]);

                if (style == nn.hid.NpadStyle.JoyRight) {
                    state[i * 2 + j].acceleration.x *= -1;
                    state[i * 2 + j].acceleration.y *= -1;
                    state[i * 2 + j].acceleration.z *= -1;
                }
            }
        }
    
    }
#endif

    private void SetKeyStates(int id)
    {
        ulong b = 0;
#if UNITY_EDITOR
        if (Input.GetKeyDown(UnityEngine.KeyCode.L)) b |= 1 << (int)KeyCode.A;
        if (Input.GetKeyDown(UnityEngine.KeyCode.K)) b |= 1 << (int)KeyCode.B;
        if (Input.GetKeyDown(UnityEngine.KeyCode.I)) b |= 1 << (int)KeyCode.X;
        if (Input.GetKeyDown(UnityEngine.KeyCode.J)) b |= 1 << (int)KeyCode.Y;
        if (Input.GetKeyDown(UnityEngine.KeyCode.E)) b |= 1 << (int)KeyCode.L;
        if (Input.GetKeyDown(UnityEngine.KeyCode.U)) b |= 1 << (int)KeyCode.R;
        if (Input.GetKeyDown(UnityEngine.KeyCode.Q)) b |= 1 << (int)KeyCode.ZL;
        if (Input.GetKeyDown(UnityEngine.KeyCode.O)) b |= 1 << (int)KeyCode.ZR;
        if (Input.GetKeyDown(UnityEngine.KeyCode.W)) b |= 1 << (int)KeyCode.UP;
        if (Input.GetKeyDown(UnityEngine.KeyCode.S)) b |= 1 << (int)KeyCode.DOWN;
        if (Input.GetKeyDown(UnityEngine.KeyCode.A)) b |= 1 << (int)KeyCode.LEFT;
        if (Input.GetKeyDown(UnityEngine.KeyCode.D)) b |= 1 << (int)KeyCode.RIGHT;
        if (Input.GetKeyDown(UnityEngine.KeyCode.Y)) b |= 1 << (int)KeyCode.START;
        if (Input.GetKeyDown(UnityEngine.KeyCode.R)) b |= 1 << (int)KeyCode.SELECT;
        if (Input.GetKeyDown(UnityEngine.KeyCode.X)) b |= 1 << (int)KeyCode.LEFTSTICK;
        if (Input.GetKeyDown(UnityEngine.KeyCode.Comma)) b |= 1 << (int)KeyCode.RIGHTSTICK;
#elif UNITY_SWITCH
        nn.hid.NpadStyle style = nn.hid.Npad.GetStyleSet((nn.hid.NpadId)id);
        if ((style & nn.hid.NpadStyle.JoyDual) != 0)
        {
            nn.hid.NpadJoyDualState n_state = new nn.hid.NpadJoyDualState();
            nn.hid.NpadJoyDual.GetState(ref n_state, (nn.hid.NpadId)id);
            
            if ((n_state.buttons & nn.hid.NpadButton.A) != 0) b |= 1 << (int)KeyCode.A;
            if ((n_state.buttons & nn.hid.NpadButton.B) != 0) b |= 1 << (int)KeyCode.B;
            if ((n_state.buttons & nn.hid.NpadButton.X) != 0) b |= 1 << (int)KeyCode.X;
            if ((n_state.buttons & nn.hid.NpadButton.Y) != 0) b |= 1 << (int)KeyCode.Y;
            if ((n_state.buttons & nn.hid.NpadButton.L) != 0) b |= 1 << (int)KeyCode.L;
            if ((n_state.buttons & nn.hid.NpadButton.R) != 0) b |= 1 << (int)KeyCode.R;
            if ((n_state.buttons & nn.hid.NpadButton.ZL) != 0) b |= 1 << (int)KeyCode.ZL;
            if ((n_state.buttons & nn.hid.NpadButton.ZR) != 0) b |= 1 << (int)KeyCode.ZR;
            if ((n_state.buttons & nn.hid.NpadButton.Up) != 0) b |= 1 << (int)KeyCode.UP;
            if ((n_state.buttons & nn.hid.NpadButton.Down) != 0) b |= 1 << (int)KeyCode.DOWN;
            if ((n_state.buttons & nn.hid.NpadButton.Left) != 0) b |= 1 << (int)KeyCode.LEFT;
            if ((n_state.buttons & nn.hid.NpadButton.Right) != 0) b |= 1 << (int)KeyCode.RIGHT;
            if ((n_state.buttons & nn.hid.NpadButton.Plus) != 0) b |= 1 << (int)KeyCode.START;
            if ((n_state.buttons & nn.hid.NpadButton.Minus) != 0) b |= 1 << (int)KeyCode.SELECT;
            if ((n_state.buttons & nn.hid.NpadButton.StickL) != 0) b |= 1 << (int)KeyCode.LEFTSTICK;
            if ((n_state.buttons & nn.hid.NpadButton.StickR) != 0) b |= 1 << (int)KeyCode.RIGHTSTICK;
        }
        if ((style & nn.hid.NpadStyle.JoyRight) != 0)
        {
            nn.hid.NpadJoyRightState nr_state = new nn.hid.NpadJoyRightState();
            nn.hid.NpadJoyRight.GetState(ref nr_state, (nn.hid.NpadId)id);
            if ((nr_state.buttons & nn.hid.NpadButton.X) != 0) b |= 1 << (int)KeyCode.A;
            if ((nr_state.buttons & nn.hid.NpadButton.A) != 0) b |= 1 << (int)KeyCode.B;
            if ((nr_state.buttons & nn.hid.NpadButton.Y) != 0) b |= 1 << (int)KeyCode.X;
            if ((nr_state.buttons & nn.hid.NpadButton.B) != 0) b |= 1 << (int)KeyCode.Y;
            if ((nr_state.buttons & nn.hid.NpadButton.RightSL) != 0) b |= 1 << (int)KeyCode.L;
            if ((nr_state.buttons & nn.hid.NpadButton.RightSR) != 0) b |= 1 << (int)KeyCode.R;
            if ((nr_state.buttons & nn.hid.NpadButton.Plus) != 0) b |= 1 << (int)KeyCode.START;
            if ((nr_state.buttons & nn.hid.NpadButton.StickR) != 0) b |= 1 << (int)KeyCode.LEFTSTICK;
        }
        if ((style & nn.hid.NpadStyle.JoyLeft) != 0)
        {
            nn.hid.NpadJoyLeftState nl_state = new nn.hid.NpadJoyLeftState();
            nn.hid.NpadJoyLeft.GetState(ref nl_state, (nn.hid.NpadId)id);
            if ((nl_state.buttons & nn.hid.NpadButton.Down) != 0) b |= 1 << (int)KeyCode.A;
            if ((nl_state.buttons & nn.hid.NpadButton.Left) != 0) b |= 1 << (int)KeyCode.B;
            if ((nl_state.buttons & nn.hid.NpadButton.Right) != 0) b |= 1 << (int)KeyCode.X;
            if ((nl_state.buttons & nn.hid.NpadButton.Up) != 0) b |= 1 << (int)KeyCode.Y;
            if ((nl_state.buttons & nn.hid.NpadButton.LeftSL) != 0) b |= 1 << (int)KeyCode.L;
            if ((nl_state.buttons & nn.hid.NpadButton.LeftSR) != 0) b |= 1 << (int)KeyCode.R;
            if ((nl_state.buttons & nn.hid.NpadButton.Minus) != 0) b |= 1 << (int)KeyCode.START;
            if ((nl_state.buttons & nn.hid.NpadButton.StickL) != 0) b |= 1 << (int)KeyCode.LEFTSTICK;
        }
        style = nn.hid.Npad.GetStyleSet(nn.hid.NpadId.Handheld);
        if ((style & nn.hid.NpadStyle.Handheld) != 0)
        {
            nn.hid.NpadHandheldState nh_state = new nn.hid.NpadHandheldState();
            nn.hid.NpadHandheld.GetState(ref nh_state, nn.hid.NpadId.Handheld);
            if ((nh_state.buttons & nn.hid.NpadButton.A) != 0) b |= 1 << (int)KeyCode.A;
            if ((nh_state.buttons & nn.hid.NpadButton.B) != 0) b |= 1 << (int)KeyCode.B;
            if ((nh_state.buttons & nn.hid.NpadButton.X) != 0) b |= 1 << (int)KeyCode.X;
            if ((nh_state.buttons & nn.hid.NpadButton.Y) != 0) b |= 1 << (int)KeyCode.Y;
            if ((nh_state.buttons & nn.hid.NpadButton.L) != 0) b |= 1 << (int)KeyCode.L;
            if ((nh_state.buttons & nn.hid.NpadButton.R) != 0) b |= 1 << (int)KeyCode.R;
            if ((nh_state.buttons & nn.hid.NpadButton.ZL) != 0) b |= 1 << (int)KeyCode.ZL;
            if ((nh_state.buttons & nn.hid.NpadButton.ZR) != 0) b |= 1 << (int)KeyCode.ZR;
            if ((nh_state.buttons & nn.hid.NpadButton.Up) != 0) b |= 1 << (int)KeyCode.UP;
            if ((nh_state.buttons & nn.hid.NpadButton.Down) != 0) b |= 1 << (int)KeyCode.DOWN;
            if ((nh_state.buttons & nn.hid.NpadButton.Left) != 0) b |= 1 << (int)KeyCode.LEFT;
            if ((nh_state.buttons & nn.hid.NpadButton.Right) != 0) b |= 1 << (int)KeyCode.RIGHT;
            if ((nh_state.buttons & nn.hid.NpadButton.Plus) != 0) b |= 1 << (int)KeyCode.START;
            if ((nh_state.buttons & nn.hid.NpadButton.Minus) != 0) b |= 1 << (int)KeyCode.SELECT;
            if ((nh_state.buttons & nn.hid.NpadButton.StickL) != 0) b |= 1 << (int)KeyCode.LEFTSTICK;
            if ((nh_state.buttons & nn.hid.NpadButton.StickR) != 0) b |= 1 << (int)KeyCode.RIGHTSTICK;
        }
#endif
        triggerState[id] = keyState[id] ^ b;
        keyState[id] = b;
    }

//#if UNITY_SWITCH && !UNITY_EDITOR
    nn.hid.ControllerSupportArg controllerSupportArg = new nn.hid.ControllerSupportArg();

    void ShowControllerSupport()
    {
        controllerSupportArg.SetDefault();
        controllerSupportArg.playerCountMax = maxNumberOfPlayers;
        controllerSupportArg.playerCountMin = minNumberOfPlayers;
        controllerSupportArg.enablePermitJoyDual = false;
        controllerSupportArg.enableSingleMode = true;

        controllerSupportArg.enableIdentificationColor = true;
        controllerSupportArg.identificationColor[0].Set(255, 128, 128, 255);
        controllerSupportArg.identificationColor[1].Set(128, 128, 255, 255);
        controllerSupportArg.identificationColor[2].Set(128, 255, 128, 255);
        controllerSupportArg.identificationColor[3].Set(224, 224, 128, 255);

        Debug.Log(controllerSupportArg);
        nn.Result result = nn.hid.ControllerSupport.Show(controllerSupportArg);
        if (!result.IsSuccess()) { Debug.Log(result); }
    }
    //#endif
}
