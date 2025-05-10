#pragma once

#include <SRT.h>
#include <Vector4.h>
#include <Camera.h>

struct Object_3D_Data {
	SRT transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	SRT uvTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	Camera camera;
};

struct Object_2D_Data {
	SRT transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	SRT uvTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
};