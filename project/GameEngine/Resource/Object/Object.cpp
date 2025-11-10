#include "Object.h"
#include <GameEngine.h>
#include <LoadObjFile.h>

Object::~Object() {
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

		parts_[i].transform = {};
		parts_[i].transform.scale = { 1.0f,1.0f,1.0f };

		parts_[i].UVtransform = {};
		parts_[i].UVtransform.scale = { 1.0f,1.0f,1.0f };
	}

	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
}

void Object::Draw3D(Camera* camera, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) {
	GameEngine::DrawObject_3D(this, camera, directionalLight, pointLight, spotLight);
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