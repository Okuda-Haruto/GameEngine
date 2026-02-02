#pragma once
#include <Matrix4x4.h>
#include <string>
#include <vector>
#include <KeyFrame.h>
#include <QuaternionKeyFlame.h>
#include <memory>

struct Node {
	Matrix4x4 localMatrix;	//ノードの変換行列
	std::string name;	//ノード名
	std::vector<std::shared_ptr<Node>> children;	//子ノード

	std::vector<std::vector<KeyFrame>> scaleKeyFrame;	//拡縮キーフレーム
	std::vector<std::vector<QuaternionKeyFlame>> rotateKeyFrame;	//回転キーフレーム
	std::vector<std::vector<KeyFrame>> translateKeyFrame;//移動キーフレーム
};