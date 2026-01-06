#include "Object.h"
#include <GameEngine.h>
#include <LoadObjFile.h>

shared_ptr<Camera> Object::DefaultCamera;

Object::~Object() {

}

void Object::Initialize(shared_ptr<Model> model) {

	model_ = model;

	//パーツの数をオフセットの数に合わせる
	parts_.resize(model_->GetOffsets().size());

	for (int i = 0; i < parts_.size(); i++) {
		//初期値としてモデルのテスクチャを得る
		parts_[i].textureIndex = model_->GetTextureIndex(i);

		parts_[i].material = make_shared<Material>();
		parts_[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		parts_[i].material->reflection = REFLECTION_HalfLambert;
		parts_[i].material->shininess = 40.0f;

		parts_[i].transform = make_shared<SRT>();
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
	GameEngine::DrawObject_3D(this, directionalLight_.lock(), pointLight_.lock(), spotLight_.lock());
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