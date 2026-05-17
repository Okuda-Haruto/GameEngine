#pragma once

//CollidersからIsCollisionを呼び出すためだけのClass
class Collider {
public:
	virtual void IsCollision() = 0;
};