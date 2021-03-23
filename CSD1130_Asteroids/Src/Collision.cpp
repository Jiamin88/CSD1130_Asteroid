/* Start Header **************************************************************/
/*!
\file	Collision.cpp
\author Jee Jia Min , j.jiamin, and 2002144
\par	j.jiamin@digipen.edu
\date	Feb 11, 2021
\brief
This header declares functions for collision check between active game objects.

The functions include:
\li \c bool CollisionIntersection_RectRect(const AABB & aabb1, const AEVec2 & vel1,const AABB & aabb2, const AEVec2 & vel2);
	\n The first argument “aabb1” of type “AABB” holds the information of the bounding box of the first object instance.
	\n The return type bool should return 0 for “No Intersection” and 1 for “Intersection”.

Copyright (C) 20xx DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header ****************************************************************/

#include "main.h"

/**************************************************************************/
/*!

	*/
/**************************************************************************/
bool CollisionIntersection_RectRect(const AABB & aabb1, const AEVec2 & vel1, 
									const AABB & aabb2, const AEVec2 & vel2)
{
	UNREFERENCED_PARAMETER(aabb1);
	UNREFERENCED_PARAMETER(vel1);
	UNREFERENCED_PARAMETER(aabb2);
	UNREFERENCED_PARAMETER(vel2);
	float a_min_x = aabb1.min.x;
	float a_max_x = aabb1.max.x;
	float b_min_x = aabb2.min.x;
	float b_max_x = aabb2.max.x;
	float a_min_y = aabb1.min.y;
	float a_max_y = aabb1.max.y;
	float b_min_y = aabb2.min.y;
	float b_max_y = aabb2.max.y;

	// step 1
	if (!(a_min_x > b_max_x || a_max_x < b_min_x ||
		a_min_y < b_max_y || a_max_y > b_min_y)) {
		return 1;
	}
	// x-axis test
	// step 2
	AEVec2 relative_vel;
	AEVec2 unconst_vel1 = vel1;
	AEVec2 unconst_vel2 = vel2;
	AEVec2Sub(&relative_vel, &unconst_vel2, &unconst_vel1);
	float t_first = 0;
	float t_last = (float)AEFrameRateControllerGetFrameTime();
	// step 3
	if (relative_vel.x < 0) {
		// case 1
		if (a_min_x > b_max_x) {
			return 0;
		}
		// case 4 - revisited
		if (a_max_x < b_min_x) {
			t_first = max((a_max_x - b_min_x) / relative_vel.x, t_first);
		}
		if (a_min_x < b_max_x) {
			t_last = min((a_min_x - b_max_x) / relative_vel.x, t_last);
		}
	}
	else {
		// case 2 - revisited
		if (a_min_x > b_max_x) {
			t_first = max((a_min_x - b_max_x) / relative_vel.x, t_first);
		}
		if (a_max_x > b_min_x) {
			t_last = min((a_max_x - b_min_x) / relative_vel.x, t_last);
		}
		// case 3
		if (a_max_x < b_min_x) {
			return 0;
		}
	}
	// case 5
	if (t_first > t_last) {
		return 0;
	}

	// y-axis test repeat step 3
	// step 3
	if (relative_vel.y < 0) {
		// case 1
		if (a_min_y > b_max_y) {
			return 0;
		}
		// case 4 - revisited
		if (a_max_y < b_min_y) {
			t_first = max((a_max_y - b_min_y) / relative_vel.y, t_first);
		}
		if (a_min_y < b_max_y) {
			t_last = min((a_min_y - b_max_y) / relative_vel.y, t_last);
		}
	}
	else {
		// case 2 - revisited
		if (a_min_y > b_max_y) {
			t_first = max((a_min_y - b_max_y) / relative_vel.y, t_first);
		}
		if (a_max_y > b_min_y) {
			t_last = min((a_max_y - b_min_y) / relative_vel.y, t_last);
		}
		// case 3
		if (a_max_y < b_min_y) {
			return 0;
		}
	}
	// case 5
	if (t_first > t_last) {
		return 0;
	}
	return 1;
}