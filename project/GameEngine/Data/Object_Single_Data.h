#pragma once
#include <SRT.h>
#include <Material.h>

struct Object_Single_Data {
	SRT transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	Material material;
};