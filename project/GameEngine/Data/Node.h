#pragma once
#include <Matrix4x4.h>
#include <string>
#include <vector>
#include <KeyFrame.h>
#include <QuaternionTransform.h>
#include <memory>

struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;	//ノードの変換行列
	std::string name;	//ノード名
	std::vector<std::shared_ptr<Node>> children;	//子ノード
};