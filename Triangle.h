#pragma once

#include <cmath>
#include "Object.h"

//! For the purposes of demonstrating the BVH, a simple sphere
struct Triangle : public BVHObject 
{
	float3 a, b, c;
	float3 normal;

	Triangle(float3 _a, float3 _b, float3 _c)
		: a(_a), b(_b), c(_c) 
	{
		normal = cross((c - a), (b - a));
		
	}

	bool getIntersection(const Ray& ray, IntersectionInfo* I) const
	{
		const float nominator = dot(normal, (a - ray.o));

		const float3 v0_cross = cross((c - ray.o), (b - ray.o));
		const float3 v1_cross = cross((b - ray.o), (a - ray.o));
		const float3 v2_cross = cross((a - ray.o), (c - ray.o));

		const float v0d = dot(v0_cross, ray.d);
		const float v1d = dot(v1_cross, ray.d);
		const float v2d = dot(v2_cross, ray.d);

		if (((v0d > 0) && (v1d > 0) && (v2d > 0)) ||
			((v0d < 0) && (v1d < 0) && (v2d < 0))) 
		{
			const float dist = nominator / dot(ray.d, normal);
			if (dist < 0.0f) {
				return false;
			}
			I->object = this;
			I->t = dist;
			return true;
		}
		return false;
	}

	__forceinline float3 getNormal(const IntersectionInfo& I) const {
		return normalize(normal);
	}

	__forceinline BBox getBBox() const {
		
		return BBox(min(a, min(b, c)), max(a, max(b, c)));
	}

	__forceinline float3 getCentroid() const {
		return (a + b + c) / 3;
	}

};

