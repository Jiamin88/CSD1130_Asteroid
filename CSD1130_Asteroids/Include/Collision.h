/* Start Header **************************************************************/
/*!
\file	Collision.h
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


#ifndef CS230_COLLISION_H_
#define CS230_COLLISION_H_

#include "AEEngine.h"

/**************************************************************************/
/*!

 */
/**************************************************************************/
struct AABB
{
	//AEVec2	c; // center
	//float  r[2]; // holds half width and half height
	
	AEVec2	min;
	AEVec2	max;
};

bool CollisionIntersection_RectRect(const AABB & aabb1, const AEVec2 & vel1, 
									const AABB & aabb2, const AEVec2 & vel2);


#endif // CS230_COLLISION_H_