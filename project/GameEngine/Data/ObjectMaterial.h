#pragma once
#include <SRT.h>
#include <Vector4.h>
#include <wrl.h>

struct ObjectMaterial {
	SRT uvTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	UINT textureIndex = 0;
};