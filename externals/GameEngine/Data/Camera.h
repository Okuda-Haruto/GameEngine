#pragma once
#include "Matrix4x4.h"

//カメラ
struct Camera{
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};