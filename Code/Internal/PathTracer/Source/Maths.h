#ifndef __MATHS_H
#define __MATHS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>


namespace Math
{
	inline float toRadians(float i_degrees)
	{		
		return((180 / M_PI)*i_degrees);
	}

	inline float toDegrees(float i_radians)
	{
		return((M_PI / 180)*i_radians);
	}
}

#endif