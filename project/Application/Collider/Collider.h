#pragma once
#include <cstdint>

//CollidersからIsCollisionを呼び出すためだけのClass
class Collider {
public:
	virtual void IsCollision(uint8_t sourceId) = 0;
};