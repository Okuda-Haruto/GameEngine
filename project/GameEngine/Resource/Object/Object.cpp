#include "Object.h"
#include <GameEngine.h>
#include <LoadObjFile.h>

shared_ptr<Camera> Object::DefaultCamera;

Object::~Object() {

}

void Object::Initialize(shared_ptr<Model> model) {

	model_ = model;
	
	//頂点リソースを作る
	objectResource_ = ObjectManager::GetInstance()->MakeObjectDataSRVResource();

	objectResource_->Map(0, nullptr, reinterpret_cast<void**>(&objectData_));

	objectData_->allocation = ObjectManager::GetInstance()->MakeNewOffsetAllocation(model_);

	objectResource_->Unmap(0, nullptr);

	processedResource_ = ObjectManager::GetInstance()->MakeObjectDataUAVResource();

	materialResource_ = ObjectManager::GetInstance()->MakeObjectDataSRVResource();

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shading = SHADING_Blinn_Phong;

	materialResource_->Unmap(0, nullptr);

	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	uvTransform_ = {};
	uvTransform_.scale = { 1.0f,1.0f,1.0f };

	if (DefaultCamera != nullptr) {
		camera_ = DefaultCamera;
	}

	isUseAnimation_ = false;
	animationName_ = {};
	isLoopAnimation_ = false;
	animationTime_ = 0.0f;

	bones_ = model_->GetBones();

	skeleton_ = model_->GetSkeleton();

	//アニメーションがあるなら得る
	if (model_->GetAnimationDatas().size() > 0) {
		animationName_ = model_->GetAnimationDatas().begin()->first;
	}
}

void Object::Update() {


	//アニメーションするなら
	if (isUseAnimation_) {
		animationTime_ += 1.0f / 60.0f;
		if (isLoopAnimation_) {
			AnimationData animationData = model_->GetAnimationData(animationName_);
			animationTime_ = std::fmod(animationTime_,animationData.duration);
		}
		model_->BoneAnimation(skeleton_, animationTime_, animationName_, interpolation_);

		for (int i = 0; i < bones_.size(); i++) {
			if (i > 128)break;
			*bones_[i].finalMatrix = bones_[i].offsetMatrix * skeleton_.joints[skeleton_.jointMap[bones_[i].name]].skeltonSpaceMatrix;
		}

	}
}

void Object::Draw3D() {
	GameEngine::DrawObject_3D(this, directionalLight_.lock(), pointLight_.lock(), spotLight_.lock(),0, animationTime_);
}

void Object::Draw3D(uint32_t index) {
	GameEngine::DrawParts_3D(this, index, directionalLight_.lock(), pointLight_.lock(), spotLight_.lock());
}

void Object::Draw2D() {
	GameEngine::DrawObject_2D(this, directionalLight_.lock());
}

void Object::Draw2D(uint32_t index) {
	GameEngine::DrawParts_2D(this, index, directionalLight_.lock());
}