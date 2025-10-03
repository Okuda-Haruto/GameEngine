#pragma once
#include <SRT.h>
#include <Vector3.h>
#include <vector4.h>
#include <Windows.h>
#include <list>

struct Particle {
	SRT transform;
	Vector3 velocity;
	Vector4 beforeColor;
	Vector4 afterColor;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct Particles {
	std::list<Particle> particle;
	UINT texture;
};