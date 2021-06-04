/*--------------------------------------------------------------------------------*
  Copyright Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

#pragma once

#include <nn/nn_Common.h>
#include <nn/nn_Macro.h>
#include <nn/hid/hid_SixAxisSensor.h>
#include <nn/util/util_Vector.h>

class SixAxisSensorPointer
{
	NN_DISALLOW_COPY(SixAxisSensorPointer);
	NN_DISALLOW_MOVE(SixAxisSensorPointer);

private:
	float m_BaseAngle;                        // Base angle.
	::nn::util::Float3 m_Front;               // Forward direction vector.
	::nn::util::Float3 m_Cursor;              // Screen coordinates (x, y, 0).
	::nn::hid::DirectionState m_Direction;    // Current orientation.

public:
	SixAxisSensorPointer() NN_NOEXCEPT;

	void Update(const nn::hid::DirectionState& direction) NN_NOEXCEPT;

	void Reset() NN_NOEXCEPT;

	::nn::util::Vector3f GetCursor() const NN_NOEXCEPT;
};
