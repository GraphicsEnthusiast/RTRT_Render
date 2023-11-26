#pragma once

#include "gdt/math/vec.h"
#include "../include/LaunchParams.h"
#include "math.h"

#define EPS 1e-4f
#define M_PIf 3.14159265358979323846f
#define M_PI_2f 1.57079632679489661923f
#define M_1_PIf	0.318309886183790671538f
#define M_2PIf 6.28318530717958647692f

__forceinline__ __device__ constexpr float sqr(float x) {
    return x * x;
}

__forceinline__ __device__ vec3f reflect(const vec3f& v, const vec3f& n) {
    return normalize(v - 2.0f * dot(v, n) * n);
}

struct Ray {
    vec3f origin;
    vec3f direction;
    float tmax = FLT_MAX;
};

struct Interaction {
    float distance;
    vec3f position;
    vec3f geomNormal;
    vec3f shadeNormal;
    bool frontFace;
    Material material;
};

// 将单位向量从世界坐标系转换到局部坐标系
// dir 待转换的单位向量
// up 局部坐标系的竖直向上方向在世界坐标系下的方向
__forceinline__ __device__ vec3f ToLocal(const vec3f& dir, const vec3f& up) {
	auto B = vec3f(0.0f), C = vec3f(0.0f);
	if (abs(up.x) > abs(up.y)) {
		float len_inv = 1.0f / sqrt(up.x * up.x + up.z * up.z);
		C = vec3f(up.z * len_inv, 0.0f, - up.x * len_inv);
	}
	else {
		float len_inv = 1.0f / sqrt(up.y * up.y + up.z * up.z);
		C = vec3f(0.0f, up.z * len_inv, -up.y * len_inv);
	}
	B = cross(C, up);

	return vec3f(dot(dir, B), dot(dir, C), dot(dir, up));
}

// 将单位向量从局部坐标系转换到世界坐标系
// dir 待转换的单位向量
// up 局部坐标系的竖直向上方向在世界坐标系下的方向
__forceinline__ __device__ vec3f ToWorld(const vec3f& dir, const vec3f& up) {
	auto B = vec3f(0.0f), C = vec3f(0.0f);
	if (abs(up.x) > abs(up.y)) {
		float len_inv = 1.0f / sqrt(up.x * up.x + up.z * up.z);
		C = vec3f(up.z * len_inv, 0.0f, - up.x * len_inv);
	}
	else {
		float len_inv = 1.0f / sqrt(up.y * up.y + up.z * up.z);
		C = vec3f(0.0f, up.z * len_inv, -up.y * len_inv);
	}
	B = cross(C, up);

	return normalize(dir.x * B + dir.y * C + dir.z * up);
}