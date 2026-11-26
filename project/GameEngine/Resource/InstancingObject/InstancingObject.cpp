#include "InstancingObject.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "SpriteManager/SpriteManager.h"

void InstancingObject::Initialize(std::weak_ptr<Model> model) {

	model_ = model;

	//パーツの数をオフセットの数に合わせる
	parts_.resize(model_.lock()->GetOffsets().size());

	for (int i = 0; i < parts_.size(); i++) {
		//初期値としてモデルのテスクチャを得る
		parts_[i].textureIndex = model_.lock()->GetTextureIndex(i);

		parts_[i].material = std::make_shared<Material>();
		parts_[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		parts_[i].material->reflection = REFLECTION_HalfLambert;
		parts_[i].material->shininess = 40.0f;
		parts_[i].material->environmentCoefficient = 0.0f;

		parts_[i].transform = std::make_shared<SRT>();
		*parts_[i].transform = {};
		parts_[i].transform->scale = { 1.0f,1.0f,1.0f };

		parts_[i].UVtransform = {};
		parts_[i].UVtransform.scale = { 1.0f,1.0f,1.0f };

		parts_[i].material->shading = SHADING_Blinn_Phong;
	}

	std::weak_ptr<Camera> defaultCamera = Object::GetDefaultCamera();

	if (defaultCamera.lock() != nullptr) {
		camera_ = defaultCamera;
	}
}

void InstancingObject::Update() {

}

void InstancingObject::Draw3D() {
	GameEngine::DrawInstancingObject_3D(this, directionalLight_.lock(), pointLight_.lock(), spotLight_.lock());
}

void InstancingObject::AddNewObject(const std::string objectName) {

	//同一名Spriteが存在しないなら生成
	auto [it, inserted] = dataMap_.try_emplace(objectName);

	//すでに同一名Spriteが存在するならreturn
	if (!inserted) {
		return;
	}

	//初期値
	dataMap_[objectName].transform.scale = { 1.0f,1.0f,1.0f };
	dataMap_[objectName].transform.rotate = {};
	dataMap_[objectName].transform.translate = {};

	dataMap_[objectName].color = { 1.0f,1.0f,1.0f,1.0f };

	dataMap_[objectName].uvTransform.scale = {1.0f,1.0f,1.0f};
	dataMap_[objectName].uvTransform.rotate = {};
	dataMap_[objectName].uvTransform.translate = {};
}

void InstancingObject::DeleteObject(const std::string objectName) {
	//削除
	dataMap_.erase(objectName);
}

void InstancingObject::SetReflection(UINT reflection) {
	for (int i = 0; i < parts_.size(); i++) {
		parts_[i].material->reflection = reflection;
	}
}

void InstancingObject::SetShading(UINT shading) {
	for (int i = 0; i < parts_.size(); i++) {
		parts_[i].material->shading = shading;
	}
}

void InstancingObject::SetShininess(float shininess) {
	for (int i = 0; i < parts_.size(); i++) {
		parts_[i].material->shininess = shininess;
	}
}