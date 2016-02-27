#include <cstdio>
#include <vector>
#include <array>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <cstdint>
#include "BVH.h"
#include "Sphere.h"
#include "Triangle.h"
using std::vector;

// Return a random number in [0,1]
float rand01() {
	return rand() * (1.f / RAND_MAX);
}

// Return a random vector with each component in the range [-1,1]
float3 randVector3() {
	return float3(rand01(), rand01(), rand01())*2.f - float3(1, 1, 1);
}

float lmap(float val, float inMin, float inMax, float outMin, float outMax) {
	return outMin + ((outMax - outMin) * (val - inMin)) / (inMax - inMin);
}

int __cdecl main(int argc, char **argv)
{
	// Create a spheres packed in the space of a cube
#ifdef _DEBUG
	const unsigned int N = 1000;
#else
	const unsigned int N = 1000000;
#endif
	vector<BVHObject*> objects;
	objects.reserve(N);
	printf("Constructing %d BVHObjects...\n", N);
	for (size_t i = 0; i < N; ++i) {
		float3 pos(randVector3());
		if (i % 2)
		{
			objects.push_back(new Sphere(pos, .005f));
		}
		else 
		{
			objects.push_back(new Triangle(
				pos + (randVector3()*.025),
				pos + (randVector3()*.025),
				pos + (randVector3()*.025)));
		}
	}

	// Compute a BVH for this object set
	BVH bvh(&objects);

	// Allocate space for some image pixels
	const unsigned int width = 800, height = 800;
	std::unique_ptr<float[]> pixels( new float[width*height*3] );

	// Create a camera from position and focus point
	const float3 camera_position(1.6f, 1.3f, 1.6f);
	const float3 camera_focus(0.f, 0.f, 0.f);
	const float3 camera_up(0.f, 1.f, 0.f);

	// Camera tangent space
	const float3 camera_dir = normalize(camera_focus - camera_position);
	const float3 camera_u = normalize(cross(camera_dir, camera_up));
	const float3 camera_v = normalize(cross(camera_u, camera_dir));
	const float fov = .5f / tanf(70.f * 3.14159265f*.5f / 180.f);

	float maxDist = std::numeric_limits<float>().min();
	float minDist = std::numeric_limits<float>().max();

	printf("Rendering image (%dx%d)...\n", width, height);
	// Raytrace over every pixel
#pragma omp parallel for
	for (int i = 0; i<width; ++i) {
		for (int j = 0; j<height; ++j) {
			int index = 3 * (width * j + i);

			float u = (i + .5f) / (float)(width - 1) - .5f;
			float v = (height - 1 - j + .5f) / (float)(height - 1) - .5f;

			// This is only valid for square aspect ratio images
			Ray ray(camera_position, normalize(u*camera_u + v*camera_v + fov*camera_dir));

			IntersectionInfo I;
			bool hit = bvh.getIntersection(ray, I, false);

			if (hit) {
				// min/max distances
				auto iPt = ray.o + ray.d * I.t;
				auto d = length(iPt);
				minDist = std::min(d, minDist);
				maxDist = std::max(d, maxDist);
			}
		}
	}

#pragma omp parallel for
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			int index = 3 * (width * j + i);

			float u = (i + .5f) / (float)(width - 1) - .5f;
			float v = (height - 1 - j + .5f) / (float)(height - 1) - .5f;

			// This is only valid for square aspect ratio images
			Ray ray(camera_position, normalize(u*camera_u + v*camera_v + fov*camera_dir));

			IntersectionInfo I;
			bool hit = bvh.getIntersection(ray, I, false);

			if (!hit) {
				pixels[index] = pixels[index + 1] = pixels[index + 2] = 0.f;
			}
			else 
			{
				// Just for fun, we'll make the color based on the normal
				auto iPt = ray.o + ray.d * I.t;

				auto d = length(iPt);
				auto sVal = lmap(d, minDist, maxDist, 0.f, 1.f);
				const float3 color(fabs(sVal), fabs(sVal), fabs(sVal));

				pixels[index + 0] = color.x();
				pixels[index + 1] = color.y();
				pixels[index + 2] = color.z();
			}
		}
	}

	// Output image file (PPM Format)
	printf("Writing out image file: \"render.ppm\"\n");
	FILE *image = fopen("render.ppm", "w");
	fprintf(image, "P6\n%d %d\n255\n", width, height);
	for (size_t j = 0; j < height; ++j) {
		for (size_t i = 0; i < width; ++i) {
			size_t index = 3 * (width * j + i);
			unsigned char r = std::max(std::min(pixels[index + 0] * 255.f, 255.f), 0.f);
			unsigned char g = std::max(std::min(pixels[index + 1] * 255.f, 255.f), 0.f);
			unsigned char b = std::max(std::min(pixels[index + 2] * 255.f, 255.f), 0.f);
			fprintf(image, "%c%c%c", r, g, b);
		}
	}
	fclose(image);

	// Cleanup
	pixels.reset();
}
