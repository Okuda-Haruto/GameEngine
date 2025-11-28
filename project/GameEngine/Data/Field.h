#pragma once
#include <Vector3.h>
#include <AABB.h>

struct AccelerationField {
	SRT acceleration;
	AABB area;
};