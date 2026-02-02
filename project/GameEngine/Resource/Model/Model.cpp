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

	//頂点リソースを作る
	vertexResource_ = dxCommon_->CreateBufferResources(sizeof(ObjectVertexData) * modelData_.vertices.size());

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(ObjectVertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(ObjectVertexData);

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));	//書き込むためのアドレスを取得

	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(ObjectVertexData) * modelData_.vertices.size());	//頂点データにリソースにコピー

	vertexResource_->Unmap(0, nullptr);

	//Sprite用のインデックスリソースを作る
	indexResource_ = dxCommon_->CreateBufferResources(sizeof(uint32_t) * modelData_.indexes.size());

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスのサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indexes.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	std::memcpy(indexData_, modelData_.indexes.data(), sizeof(uint32_t) * modelData_.indexes.size());	//インデックスデータにリソースにコピー

	indexResource_->Unmap(0, nullptr);

	vertexIndex_ = UINT(modelData_.vertices.size());

	//ワールド座標
	for (Bone& bone : modelData_.bones) {
		bone.offsetMatrix = bone.offsetMatrix;	//bone.node.lock()->localMatrix = 前ノードとの差分

		bone.localMatrix = bone.node.lock()->localMatrix;
	}
	for (Bone& bone : modelData_.bones) {
		bone.finalMatrix = Inverse(modelData_.rootNode->localMatrix) * bone.offsetMatrix * SetWorldMatrix(modelData_.rootNode, modelData_.bones, bone);
	}
}

//ボーンアニメーション
void Model::BoneAnimation(std::vector<Bone>& bones, float time, UINT animationIndex, AnimationInterpolation interpolation) {
	assert(animationIndex < modelData_.animations.size());

	//ローカル座標
	for (Bone& bone : bones) {
		QuaternionTransform animationTransform = GetAnimationTransform(bone.node, modelData_.animations, animationIndex, interpolation, time);
		bone.localMatrix = MakeQuaternionMatrix(animationTransform.scale, animationTransform.rotate, animationTransform.translate);
	}

	//ワールド座標
	for (Bone& bone : bones) {
		bone.finalMatrix = bone.offsetMatrix * SetWorldMatrix(modelData_.rootNode, bones, bone);
	}

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