#pragma once
#include <QuaternionTransform.h>
#include <Matrix4x4.h>
#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <map>

struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 offsetMatrix;	//オフセット行列
	Matrix4x4 skeltonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t>jointMap;
	std::vector<Joint> joints;
};