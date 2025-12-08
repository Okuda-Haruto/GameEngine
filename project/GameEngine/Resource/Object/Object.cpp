#include "Object.h"
#include <GameEngine.h>
#include <LoadObjFile.h>

Camera* Object::DefaultCamera = nullptr;

Object::~Object() {
	for (int i = 0; i < parts_.size(); i++) {
		delete parts_[i].transform;
	}
}

void Object::Initialize(Model* model) {

	model_ = model;

	//パーツの数をオフセットの数に合わせる
	parts_.resize(model_->GetOffsets().size());

	for (int i = 0; i < parts_.size(); i++) {
		//初期値としてモデルのテスクチャを得る
		parts_[i].textureIndex = model_->GetTextureIndex(i);

		parts_[i].material = new Material;
		parts_[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		parts_[i].material->reflection = REFLECTION_HalfLambert;
		parts_[i].material->shininess = 40.0f;

		parts_[i].transform = new SRT;
		*parts_[i].transform = {};
		parts_[i].transform->scale = { 1.0f,1.0f,1.0f };

		parts_[i].UVtransform = {};
		parts_[i].UVtransform.scale = { 1.0f,1.0f,1.0f };
	}

	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };

	if (DefaultCamera != nullptr) {
		camera_ = DefaultCamera;
	}
}

void Object::Draw3D() {
	GameEngine::DrawObject_3D(this, directionalLight_, pointLight_, spotLight_);
}

void Object::Draw2D() {
	GameEngine::DrawObject_2D(this, directionalLight_);
}

void Object::SetReflection(UINT reflection) {
	for (int i = 0; i < parts_.size(); i++) {
		parts_[i].material->reflection = reflection;
	}
}

void Object::SetShininess(float shininess) {
	for (int i = 0; i < parts_.size(); i++) {
		parts_[i].material->shininess = shininess;
	}
}