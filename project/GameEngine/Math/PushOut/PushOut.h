#pragma once
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "AABB.h"
#include "OBB.h"
#include "Capsule.h"

//OBBにぶつかったSphreの押し戻し
Vector3 PushOut(Sphere& sphere, const Vector3& velocity, const OBB& obb);