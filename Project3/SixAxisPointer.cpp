/*--------------------------------------------------------------------------------*
  Copyright Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

#include <nn/nn_Assert.h>
#include <nn/nn_Common.h>
#include <nn/nn_Macro.h>
#include <nn/gfx/util/gfx_DebugFontTextWriter.h>
#include <nn/hid/hid_SixAxisSensor.h>
#include <nn/util/util_Vector.h>

#include "SixAxisPointer.h"

namespace {

	template <typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (max < value)
		{
			value = max;
		}

		return value;
	}

	template <typename T>
	T LinearInterpolation(T x1, T x2, T coefficient)
	{
		return x1 * (1 - coefficient) + x2 * coefficient;
	}

	static float CalculateVerticalAngle(const ::nn::util::Vector3f& dir)
	{
		// Up is 0 degrees, and down is 180 degrees.
		float v = Clamp(::nn::util::VectorGetY(dir), -1.0f, 1.0f);

		float degree = nn::util::RadianToDegree(nn::util::AcosEst(v));

		// Up is -90 degrees, and down is 90 degrees.
		degree -= 90.0f;

		// Up is 90 degrees, and down is -90 degrees.
		return -degree;
	}

	::nn::util::Vector3f GetBaseDirection(const ::nn::util::Float3& directionY)
	{
		return ::nn::util::Vector3f(-directionY.x, directionY.z, directionY.y);
	}

	const ::nn::util::Float3 InitialFrontVector = { {{ 0.0f, 0.0f, 1.0f }} };

	// Screen size.
	const float Width = 1280.0f;
	const float Height = 720.0f;

	// Center coordinates of the cursor.
	const float CursorCenterX = Width * 0.5f;
	const float CursorCenterY = Height * 0.5f;

	// Bias.
	const float BiasX = 6.0f;
	const float BiasY = 5.0f;

} // Namespace.

SixAxisSensorPointer::SixAxisSensorPointer() NN_NOEXCEPT
	: m_BaseAngle(0.0f)
	, m_Front(InitialFrontVector)
	, m_Cursor()
	, m_Direction()
{
	// Does nothing.
}

void SixAxisSensorPointer::Update(const nn::hid::DirectionState& direction) NN_NOEXCEPT
{
	m_Direction = direction;

	// Convert to screen coordinates.

	::nn::util::Vector3f pointingDirection = GetBaseDirection(direction.y);
	::nn::util::Vector3f pointingHorizontalDirection = pointingDirection;

	::nn::util::VectorSetY(&pointingHorizontalDirection, 0.0f);
	pointingHorizontalDirection.Normalize();

	float x = 0.0f;
	float y = 0.0f;

	// y = From front vector (dir_z).
	{
		float verticalAngle = CalculateVerticalAngle(pointingDirection);

		// Relative to the reference value.
		verticalAngle -= m_BaseAngle;
		verticalAngle = Clamp(verticalAngle, -85.0f, 85.0f);

		y = nn::util::TanEst(nn::util::DegreeToRadian(verticalAngle));
		y = y * BiasY;
		y = y * 0.5f + 0.5f;
	}

	// x =
	{
		::nn::util::Vector3f front;
		::nn::util::VectorLoad(&front, m_Front);

		// The reference forward direction vector.
		::nn::util::Vector3f rightDirection = front;
		rightDirection = rightDirection.Cross(::nn::util::Vector3f::UnitY());

		// The difference in the angle relative to the front.
		float cos = front.Dot(pointingHorizontalDirection);
		cos = Clamp(cos, -1.0f, 1.0f);

		// 0 degrees to 180 degrees.
		float horizontalAngle = nn::util::RadianToDegree(nn::util::AcosEst(cos));

		// Sign.
		if (rightDirection.Dot(pointingHorizontalDirection) < 0.0f)
		{
			horizontalAngle = -horizontalAngle;
		}

		horizontalAngle = Clamp(horizontalAngle, -85.0f, 85.0f);

		x = nn::util::TanEst(nn::util::DegreeToRadian(horizontalAngle));
		x = x * BiasX;
		x = x * 0.5f + 0.5f;
	}

	// Convert to ranges x = -640 to 640 and y = -360 to 360.
	m_Cursor.x = LinearInterpolation(-Width * 0.5f, Width * 0.5f, x);
	m_Cursor.y = LinearInterpolation(-Height * 0.5f, Height * 0.5f, y);
}

void SixAxisSensorPointer::Reset() NN_NOEXCEPT
{
	::nn::util::Vector3f baseDirection = GetBaseDirection(m_Direction.y);

	// Correct to the center.
	m_Front.x = ::nn::util::VectorGetX(baseDirection);
	m_Front.y = 0.0f;
	m_Front.z = ::nn::util::VectorGetZ(baseDirection);

	::nn::util::Vector3f front;
	::nn::util::VectorLoad(&front, m_Front);

	if (!front.Normalize())
	{
		front = ::nn::util::Vector3f::UnitZ();
	}
	::nn::util::VectorStore(&m_Front, front);

	m_BaseAngle = CalculateVerticalAngle(baseDirection);
	m_Cursor.x = 0.0f;
	m_Cursor.y = 0.0f;
}

::nn::util::Vector3f SixAxisSensorPointer::GetCursor() const NN_NOEXCEPT
{
	::nn::util::Vector3f pointer;
	pointer.SetX(CursorCenterX + m_Cursor.x);
	pointer.SetY(CursorCenterY - m_Cursor.y);
	return pointer;
}
