#pragma once
#include "Matrix4x4.h"
#include "SRT.h"

//カメラ
struct Camera{
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	SRT transform;
};