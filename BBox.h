#ifndef BBox_h
#define BBox_h

#include "Ray.h"
#include "Vector3.h"
#include <stdint.h>

struct BBox {
	float3 mMin, mMax, mExtent;
	BBox() { }
	BBox(const float3& min, const float3& max);
	BBox(const float3& p);

	bool intersect(const Ray& ray, float *tnear, float *tfar) const;
	void expandToInclude(const float3& p);
	void expandToInclude(const BBox& b);
	uint32_t maxDimension() const;
	float surfaceArea() const;
};

#endif
