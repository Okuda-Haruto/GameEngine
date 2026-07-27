#include "Model.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "LoadGLTFFile.h"
#include "Animation/Animation.h"

Model::~Model() {

}

void Model::Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	//モデル読み込み
	if (filename.find(".obj") != std::string::npos) {
		modelData_ = LoadObjFile(directoryPath, filename);
	} else if (filename.find(".gltf") != std::string::npos) {
		modelData_ = LoadGLTFFile(directoryPath, filename);
	} else {
		assert(0);
	}

	//ワールド座標
	for (Bone& bone : modelData_.bones) {
		bone.offsetMatrix = bone.offsetMatrix;	//bone.node.lock()->localMatrix = 前ノードとの差分

		bone.localMatrix = bone.node.lock()->localMatrix;
	}
	for (Bone& bone : modelData_.bones) {
		bone.finalMatrix = std::make_shared<Matrix4x4>();
		*bone.finalMatrix = Inverse(modelData_.rootNode->localMatrix) * bone.offsetMatrix * SetWorldMatrix(modelData_.rootNode, modelData_.bones, bone);
	}

	if (modelData_.rootNode) {
		skeleton_ = CreateSkelton(modelData_.rootNode);
	}
}

//ボーンアニメーション
void Model::BoneAnimation(Skeleton& skeleton, float time, std::string animationName, AnimationInterpolation interpolation) {
	//読み込み済みアニメーションを検索
	if (modelData_.animations.contains(animationName)) {
		ApplyAnimation(skeleton, modelData_.animations[animationName], interpolation, time);

		for (Joint& joint : skeleton.joints) {
			joint.localMatrix = MakeQuaternionMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
			if (joint.parent) {	//親がいれば親の行列を掛ける
				joint.skeltonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeltonSpaceMatrix;
			}
			else {
				joint.skeltonSpaceMatrix = joint.localMatrix;
			}
		}

		return;
	}

	//存在しない場合assert
	assert(false);

	return;
}




//階層構造の行列変換
Matrix4x4 Model::Model::SetWorldMatrix(std::shared_ptr<Node> node, std::vector<Bone>& bones, Bone bone) {

	Matrix4x4 current = FindBoneLocalMatrix(bones, node->name);
	if (current.m[3][3] == 0) {
		current = node->localMatrix;
	}

	// 目的ノードなら bone.localMatrix を使って返す
	if (node->name == bone.name) {
		return bone.localMatrix;
	}

	// 子を探索し、見つかったら current * childMatrix を返す
	for (std::shared_ptr<Node>& child : node->children) {
		Matrix4x4 childMatrix = SetWorldMatrix(child, bones, bone);
		// childMatrix がゼロ行列でなければ見つかったと判断
		if (childMatrix.m[3][3] != 0) {
			return childMatrix * current;
		}
	}

	// 見つからなかったらゼロ行列を返す
	return {};
}

Matrix4x4 Model::FindBoneLocalMatrix(std::vector<Bone>& bones, std::string boneName) {

	for (Bone bone : bones) {
		if (bone.name == boneName) {
			return bone.localMatrix;
		}
	}

	return {};
}