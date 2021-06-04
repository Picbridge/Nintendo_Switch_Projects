#pragma once
/*--------------------------------------------------------------------------------*
  Copyright Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

 /**
	 @examplesource{HidNpadSixAxisSensor_Main.cpp,PageSampleHidNpadSixAxisSensor}

	 @brief
	 Sample program showing how to get the state of <tt>SixAxisSensor</tt> by using Npad.
 */

 /**
	 @page PageSampleHidNpadSixAxisSensor  Getting the State of SixAxisSensor Using Npad Sample
	 @tableofcontents

	 @brief
	 This is a description of the sample program for getting the state of <tt>SixAxisSensor</tt> by using Npad.

	 @section PageSampleHidNpadSixAxisSensor_SectionBrief  Overview
	 This sample program demonstrates how to get the latest state of <tt>SixAxisSensor</tt> for a controller.

	 @section PageSampleHidNpadSixAxisSensor_SectionFileStructure  File Structure
	 This sample program is located in <tt>@link ../../../Samples/Sources/Applications/HidNpadSixAxisSensor Samples/Sources/Applications/HidNpadSimple @endlink</tt>.

	 @section PageSampleHidNpadSixAxisSensor_SectionNecessaryEnvironment  Required Environment
	 When running in a Windows environment, connect the Bluetooth dongle to the computer ahead of time, and pair the controller to the computer.
	 When running in an SDEV environment, pair the controller to the SDEV ahead of time.

	 @section PageSampleHidNpadSixAxisSensor_SectionHowToOperate  How to Use
	 When this sample program is run in the Windows environment, the states of <tt>SixAxisSensor</tt> are displayed in a window.
	 When it is run in the NX development hardware environment, the states are displayed on the touch screen.

	 When you operate the controller, you can see how the state changes.
	 Up to four controllers can be connected at the same time.

	 The following states are displayed, in order from the left.
	 - The values for the <tt>SixAxisSensor</tt> velocity, angular velocity, rotation angle, and attitude.
	 - The XY, YZ, and ZX components of <tt>SixAxisSensorState::acceleration</tt>, shown in a two-dimensional coordinate system.
	 - The XY, YZ, and ZX components of <tt>SixAxisSensorState::angularVelocity</tt>, shown in a two-dimensional coordinate system.
	 - The rotation about the x-axis, y-axis, and z-axis for <tt>SixAxisSensorState::angle</tt> appear using a two-dimension coordinate system.
	 - The <tt>SixAxisSensorState::GetQuaternion</tt> attitude data using a three-dimensional wireframe.

	 The controllers are shown in four colors (red, orange, yellow, and green) to distinguish them,
	 based on the order they are added to <tt>ControllerManager</tt>.

	 To end the sample program, either simultaneously press the + Button and - Button on the controller,
	 or end the program via Visual Studio or Nintendo Target Manager.

	 Pressing the controller's + Button causes the following actions to be taken.
	 - Initialize the attitude of the 3D wireframe.
	 - Initialize the pointer's coordinates to the center of the screen.

	 @section PageSampleHidNpadSixAxisSensor_SectionHowToExecute  Execution Procedure
	 Build the sample program and then run it.

	 When you press any button, the latest <tt>SixAxisSensorState</tt> for the controller, corresponding to the
	 style of operation in which the button was pressed, is dumped to the console window.
	 The dumped acceleration, angular acceleration, rotation angle, and axial orientation vectors are displayed as (x, y, z) vectors.

	 @section PageSampleHidNpadSixAxisSensor_SectionDetail  Description
	 This sample program has the following flow.
	 - Get the Npad ID.
	 - Initialize the Npad.
	 - Get the <tt>SixAxisSensorHandle</tt>.
	 - Activate the six-axis sensor feature.
	 - Get the current button input state.
	 - Get the current six-axis sensor input state.
	 - Estimate the packet-loss rate from the current timestamp.
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <nn/nn_Assert.h>
#include <nn/nn_Common.h>
#include <nn/nn_Log.h>
#include <nn/nn_Macro.h>
#include <nn/nn_TimeSpan.h>
#include <nn/gfx/util/gfx_DebugFontTextWriter.h>
#include <nn/hid.h>
#include <nn/hid/hid_Npad.h>
#include <nn/hid/hid_NpadSixAxisSensor.h>
#include <nn/hid/hid_SixAxisSensor.h>
#include <nn/TargetConfigs/build_Platform.h>

#if defined(NN_BUILD_TARGET_PLATFORM_NX)
#include <nv/nv_MemoryManagement.h>
#endif

#include "SixAxisPointer.h"


namespace SixAxis{

	template<typename T, std::size_t U>
	std::size_t GetArrayLength(const T(&staticArray)[U])
	{
		NN_UNUSED(staticArray);
		return U;
	}

	const nn::hid::NpadIdType NpadIds[] = { nn::hid::NpadId::No1,
										   nn::hid::NpadId::No2,
										   nn::hid::NpadId::No3,
										   nn::hid::NpadId::No4 };

	const int NpadIdCountMax = static_cast<int>(GetArrayLength(NpadIds));


	/**
	* @brief  The identifier for the coordinate system to render.
	*/
	enum CoordinateId
	{
		CoordinateId_AccelerometerXy = 0,
		CoordinateId_AccelerometerYz,
		CoordinateId_AccelerometerZx,
		CoordinateId_AngularVelocityXy,
		CoordinateId_AngularVelocityYz,
		CoordinateId_AngularVelocityZx,
		CoordinateId_AngleX,
		CoordinateId_AngleY,
		CoordinateId_AngleZ,
	};

	//!<  Get the origin within the coordinate system.
	void GetCoordinateOrigin(nn::util::Float2* pOutOrigin, CoordinateId id)
	{
		const float X = 350;
		const float Y = 150;
		const float DeltaX = 250;
		const float DeltaY = 230;

		pOutOrigin->x = X + static_cast<int>(id) / 3 * DeltaX;
		pOutOrigin->y = Y + (static_cast<int>(id) % 3) * DeltaY;
	}

	//!<  Get the name of the coordinate system.
	void GetCoordinateName(char* pOutAxis1,
		char* pOutAxis2,
		::std::string* pOutTitle,
		CoordinateId id)
	{
		switch (id)
		{
		case CoordinateId_AccelerometerXy:
			*pOutTitle = "AccelerometerXy";
			*pOutAxis1 = 'x';
			*pOutAxis2 = 'y';
			break;
		case CoordinateId_AccelerometerYz:
			*pOutTitle = "AccelerometerYz";
			*pOutAxis1 = 'y';
			*pOutAxis2 = 'z';
			break;
		case CoordinateId_AccelerometerZx:
			*pOutTitle = "AccelerometerZx";
			*pOutAxis1 = 'z';
			*pOutAxis2 = 'x';
			break;
		case CoordinateId_AngularVelocityXy:
			*pOutTitle = "AngularVelocityXy";
			*pOutAxis1 = 'x';
			*pOutAxis2 = 'y';
			break;
		case CoordinateId_AngularVelocityYz:
			*pOutTitle = "AngularVelocityYz";
			*pOutAxis1 = 'y';
			*pOutAxis2 = 'z';
			break;
		case CoordinateId_AngularVelocityZx:
			*pOutTitle = "AngularVelocityZx";
			*pOutAxis1 = 'z';
			*pOutAxis2 = 'x';
			break;
		case CoordinateId_AngleX:
			*pOutTitle = "AngleX";
			*pOutAxis1 = ' ';
			*pOutAxis2 = ' ';
			break;
		case CoordinateId_AngleY:
			*pOutTitle = "AngleY";
			*pOutAxis1 = ' ';
			*pOutAxis2 = ' ';
			break;
		case CoordinateId_AngleZ:
			*pOutTitle = "AngleZ";
			*pOutAxis1 = ' ';
			*pOutAxis2 = ' ';
			break;
		default:
			NN_UNEXPECTED_DEFAULT;
			break;
		}
	}

	//!<  Get the button trigger input.
	nn::hid::NpadButtonSet GetTriggerButtons(const nn::hid::NpadButtonSet current,
		const nn::hid::NpadButtonSet previous)
	{
		return (current ^ previous) & current;
	}

	//!<  Determine application finalization.
	bool Terminate(const nn::hid::NpadButtonSet& buttons)
	{
		return (buttons.Test<nn::hid::NpadButton::Plus>() &&
			buttons.Test<nn::hid::NpadButton::Minus>());
	}

	/**
	* @brief  Interface that denotes the processes for each operation state.
	*/
	class INpadStyleSixAxisSensor
	{
	public:

		virtual ~INpadStyleSixAxisSensor() NN_NOEXCEPT { /* Does nothing. */ }

		virtual void Initialize() NN_NOEXCEPT = 0; //!<  Get the handle and begins sampling.

		virtual bool CanReset() NN_NOEXCEPT = 0;   //!<  Returns whether to reset sensor value attitude position.

		virtual void Reset() NN_NOEXCEPT = 0;      //!<  Resets the sensor value attitude position.

		virtual void Update() NN_NOEXCEPT = 0;     //!<  Update the input state.

		virtual bool Quit() NN_NOEXCEPT = 0;       //!<  Return <tt>true</tt> for application finalization.

		virtual ::nn::util::Vector3f GetPointer() const NN_NOEXCEPT = 0; //!<  Gets the pointer's coordinates.

		virtual void ResetPointer() NN_NOEXCEPT = 0; //!<  Resets the pointer's coordinates to the center of the screen.

		virtual bool IsConnected() const NN_NOEXCEPT = 0;

		virtual nn::util::Quaternion GetRotation() NN_NOEXCEPT = 0;
	};

	//!<  Class that denotes the processes for the <tt>NpadStyleFullKey</tt> operation state.
	class FullKeySixAxisSensor : public INpadStyleSixAxisSensor
	{
		NN_DISALLOW_COPY(FullKeySixAxisSensor);
		NN_DISALLOW_MOVE(FullKeySixAxisSensor);

	private:
		nn::hid::NpadFullKeyState    m_ButtonState[2];
		nn::hid::SixAxisSensorHandle m_Handle;
		nn::hid::SixAxisSensorState  m_State;
		nn::util::Quaternion         m_Quaternion;
		const nn::hid::NpadIdType*   m_pId;

		SixAxisSensorPointer m_Pointer;

		uint32_t     m_FramerateCounter;
		nn::os::Tick m_FramerateFirstTick;
		int64_t      m_FramerateFirstSample;
		float        m_FramerateComputation;
		float        m_PacketDropPercentage;
	public:
		static const int ResetIntervalsInFrame = 60 * 3;
		static const int UpdateIntervalsInFrame = 20;

		explicit FullKeySixAxisSensor(const nn::hid::NpadIdType& id) NN_NOEXCEPT
			: m_pId(&id)
			, m_Pointer()
		{
			m_Quaternion = nn::util::Quaternion::Identity();
		}

		virtual ~FullKeySixAxisSensor() NN_NOEXCEPT NN_OVERRIDE { /* Does nothing. */ };

		virtual void Initialize() NN_NOEXCEPT NN_OVERRIDE
		{
			int handleCount = nn::hid::GetSixAxisSensorHandles(&m_Handle,
				1,
				*m_pId,
				nn::hid::NpadStyleFullKey::Mask);
			NN_LOG("HandleCount(FullKey)=%d\n", handleCount);
			nn::hid::StartSixAxisSensor(m_Handle);
		}

		virtual bool CanReset() NN_NOEXCEPT NN_OVERRIDE
		{
			return (GetTriggerButtons(m_ButtonState[0].buttons, m_ButtonState[1].buttons) & nn::hid::NpadButton::Plus::Mask).IsAnyOn();
		}

		virtual void Reset() NN_NOEXCEPT NN_OVERRIDE
		{
			m_State.GetQuaternion(&m_Quaternion);
		}

		virtual void Update() NN_NOEXCEPT NN_OVERRIDE
		{
			m_ButtonState[1] = m_ButtonState[0];

			nn::hid::GetNpadState(&m_ButtonState[0], *m_pId);
			nn::hid::GetSixAxisSensorState(&m_State, m_Handle);

			m_Pointer.Update(m_State.direction);

			// Initializes data for packet loss at an interval of ResetIntervalsInFrame frames.
			if ((m_FramerateCounter % ResetIntervalsInFrame) == 0)
			{
				m_FramerateFirstTick = nn::os::GetSystemTick();
				m_FramerateFirstSample = m_State.samplingNumber;
			}
			nn::os::Tick currentTick = nn::os::GetSystemTick() - m_FramerateFirstTick;
			int64_t currentSample = m_State.samplingNumber - m_FramerateFirstSample;

			// Updates the packet loss at a cycle of UpdateIntervalsInFrame.
			if (m_FramerateCounter % UpdateIntervalsInFrame == (UpdateIntervalsInFrame - 1))
			{
				m_FramerateComputation = currentSample / (float(currentTick.GetInt64Value()) / nn::os::GetSystemTickFrequency());

				const float ExpectedFrameRate = 1000.0f / m_State.deltaTime.GetMilliSeconds();
				m_PacketDropPercentage = 1.0f - std::min(std::max(m_FramerateComputation / ExpectedFrameRate, 0.0f), 1.0f);

				nn::hid::NpadStyleSet style = nn::hid::GetNpadStyleSet(*m_pId);
				if (style.Test<nn::hid::NpadStyleFullKey>() == false)
				{
					m_ButtonState[0].buttons.Reset();
				}
			}
			m_FramerateCounter++;
		}

		virtual bool Quit() NN_NOEXCEPT NN_OVERRIDE
		{
			return Terminate(m_ButtonState[0].buttons);
		}


		bool IsConnected() const NN_NOEXCEPT NN_OVERRIDE
		{
			//Get the currently enabled style of operation (NpadStyleSet).
			nn::hid::NpadStyleSet style = nn::hid::GetNpadStyleSet(*m_pId);

			return style.Test<nn::hid::NpadStyleFullKey>();
		}

		::nn::util::Vector3f GetPointer() const NN_NOEXCEPT NN_OVERRIDE
		{
			return m_Pointer.GetCursor();
		}

		void ResetPointer() NN_NOEXCEPT NN_OVERRIDE
		{
			m_Pointer.Reset();
		}

		virtual nn::util::Quaternion GetRotation() NN_NOEXCEPT NN_OVERRIDE
		{
			nn::util::Quaternion currentQuaternion, releativeQuaternion;
			m_State.GetQuaternion(&currentQuaternion);
			releativeQuaternion = currentQuaternion / m_Quaternion;

			return releativeQuaternion;
		}
	};

#if defined(NN_BUILD_TARGET_PLATFORM_NX)
	const size_t GraphicsMemorySize = 8 * 1024 * 1024;

	void* NvAllocate(size_t size, size_t alignment, void* userPtr) NN_NOEXCEPT
	{
		NN_UNUSED(userPtr);
		return aligned_alloc(alignment, nn::util::align_up(size, alignment));
	}

	void NvFree(void* addr, void* userPtr) NN_NOEXCEPT
	{
		NN_UNUSED(userPtr);
		free(addr);
	}

	void* NvReallocate(void* addr, size_t newSize, void* userPtr) NN_NOEXCEPT
	{
		NN_UNUSED(userPtr);
		return realloc(addr, newSize);
	}
#endif

} // Namespace.

