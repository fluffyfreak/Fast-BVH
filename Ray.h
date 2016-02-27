#ifndef Ray_h
#define Ray_h

#include "Vector3.h"

struct Ray {
	float3 o; // Ray Origin
	float3 d; // Ray Direction
	float3 inv_d; // Inverse of each Ray Direction component

	Ray(const float3& o, const float3& d)
		: o(o), d(d), inv_d(float3(1, 1, 1)/d) { }
};

#endif
