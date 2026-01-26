#pragma once
#include <Matrix4x4.h>
#include <vector>
#include <string>
#include <Node.h>

struct Bone {
	std::string name;
	Matrix4x4 offsetMatrix;	//オフセット行列
	Matrix4x4 localMatrix;	//ローカル座標
	Matrix4x4 finalMatrix;	//最終ボーン行列
	std::weak_ptr<Node> node;	//ノード
};