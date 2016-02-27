#ifndef Sphere_h_
#define Sphere_h_

#include <cmath>
#include "Object.h"

//! For the purposes of demonstrating the BVH, a simple sphere
struct Sphere : public BVHObject {
	float3 center; // Center of the sphere
	float r, r2; // Radius, Radius^2

	Sphere(const float3& center, float radius)
		: center(center), r(radius), r2(radius*radius) { }

	bool getIntersection(const Ray& ray, IntersectionInfo* I) const {
		float3 s = center - ray.o;
		float sd = dot(s, ray.d);
		float ss = dot(s, s);

		// Compute discriminant
		float disc = sd*sd - ss + r2;

		// Complex values: No intersection
		if (disc < 0.f) return false;

		// Assume we are not in a sphere... The first hit is the lesser valued
		I->object = this;
		I->t = sd - sqrt(disc);
		return true;
	}

	__forceinline float3 getNormal(const IntersectionInfo& I) const {
		return normalize(I.hit - center);
	}

	__forceinline BBox getBBox() const {
		return BBox(center - float3(r, r, r), center + float3(r, r, r));
	}

	__forceinline float3 getCentroid() const {
		return center;
	}

};

#endif
