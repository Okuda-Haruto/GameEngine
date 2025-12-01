#include "ParticleEditor.h"
#include "imgui.h"
#include "PrimitiveManager/PrimitiveManager.h"
#include "Operation/Operation.h"

void ParticleEditor::Initialize(ParticleEmitter* particleEmitter) {
	particleEmitter_ = particleEmitter;

	emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_.spawnRange.min = { -0.5f,-0.5f,-0.5f };
	emitter_.spawnRange.max = { 0.5f,0.5f,0.5f };
	emitter_.angleBase = {-1.0f,0.0f,0.0f};
	emitter_.angleRange = {0.0f,0.0f,0.0f};	//方向範囲
	emitter_.speedBase = 0.4f;	//基礎速度
	emitter_.speedRange = 0.2f;	//速度範囲
	emitter_.count = 2;
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	emitter_.lifeTime = 1.0f;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	ParticleManager::GetInstance()->SetEmitter(particleEmitter_->GetName(), emitter_);
	field_.area.min = { -0.5f,-0.5f,-0.5f };
	field_.area.max = { 0.5f,0.5f,0.5f };
	field_.acceleration = { 0.0f,0.0f,0.0f };
	ParticleManager::GetInstance()->SetField(particleEmitter_->GetName(), field_);
}

void ParticleEditor::Update() {
#ifdef USE_IMGUI
	ImGui::Begin(particleEmitter_->GetName().c_str());
	if (ImGui::CollapsingHeader("Emitter")) {
		int count = emitter_.count;
		ImGui::InputInt("Emitter Count", &count);
		emitter_.count = count;
		ImGui::DragFloat("Emitter LifeTime", &emitter_.lifeTime, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Emitter Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::DragFloat3("Emitter Transform Scale", &emitter_.transform.scale.x, 0.01f, 0.01f, 10.0f);
			ImGui::SliderAngle("Emitter Transform RotateX", &emitter_.transform.rotate.x, 0.0f, 360.0f);
			ImGui::SliderAngle("Emitter Transform RotateY", &emitter_.transform.rotate.y, 0.0f, 360.0f);
			ImGui::SliderAngle("Emitter Transform RotateZ", &emitter_.transform.rotate.z, 0.0f, 360.0f);
			ImGui::DragFloat3("Emitter Transform Translate", &emitter_.transform.translate.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("SpawnRange")) {
			ImGui::DragFloat3("Emitter SpawnRange Min", &emitter_.spawnRange.min.x, 0.01f);
			ImGui::DragFloat3("Emitter SpawnRange Max", &emitter_.spawnRange.max.x, 0.01f);
			if (emitter_.spawnRange.max.x < emitter_.spawnRange.min.x)emitter_.spawnRange.max.x = emitter_.spawnRange.min.x;
			if (emitter_.spawnRange.max.y < emitter_.spawnRange.min.y)emitter_.spawnRange.max.y = emitter_.spawnRange.min.y;
			if (emitter_.spawnRange.max.z < emitter_.spawnRange.min.z)emitter_.spawnRange.max.z = emitter_.spawnRange.min.z;
		}
		if (ImGui::CollapsingHeader("Angle")) {
			ImGui::DragFloat3("Emitter AngleBase", &emitter_.angleBase.x, 0.01f, -1.0f, 1.0f);
			emitter_.angleBase = Normalize(emitter_.angleBase);
			ImGui::DragFloat3("Emitter angleRange", &emitter_.angleRange.x, 0.01f, -1.0f, 1.0f);
		}
		if (ImGui::CollapsingHeader("Speed")) {
			ImGui::DragFloat("Emitter speedBase", &emitter_.speedBase, 0.01f, 0.01f, 10.0f);
			ImGui::DragFloat("Emitter speedRange", &emitter_.speedRange, 0.01f, 0.01f, 10.0f);
		}
		if (ImGui::CollapsingHeader("Color")) {
			ImGui::ColorPicker4("Emitter BeforeColor", &emitter_.beforeColor.x);
			ImGui::ColorPicker4("Emitter AfterColor", &emitter_.afterColor.x);
		}
	}
	if (ImGui::CollapsingHeader("Field")) {
		if (ImGui::CollapsingHeader("Acceleration")) {
			ImGui::DragFloat3("Field Acceleration Scale", &field_.acceleration.scale.x, 0.01f, 0.01f, 10.0f);
			ImGui::SliderAngle("Field Acceleration RotateX", &field_.acceleration.rotate.x, 0.0f, 360.0f);
			ImGui::SliderAngle("Field Acceleration RotateY", &field_.acceleration.rotate.y, 0.0f, 360.0f);
			ImGui::SliderAngle("Field Acceleration RotateZ", &field_.acceleration.rotate.z, 0.0f, 360.0f);
			ImGui::DragFloat3("Field Acceleration Translate", &field_.acceleration.translate.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("Area")) {
			ImGui::DragFloat3("Field Min", &field_.area.min.x, 0.01f);
			ImGui::DragFloat3("Field Max", &field_.area.max.x, 0.01f);
			if (field_.area.max.x < field_.area.min.x)field_.area.max.x = field_.area.min.x;
			if (field_.area.max.y < field_.area.min.y)field_.area.max.y = field_.area.min.y;
			if (field_.area.max.z < field_.area.min.z)field_.area.max.z = field_.area.min.z;
		}
	}
	ImGui::End();
#endif

	particleEmitter_->SetEmitter(emitter_);
	particleEmitter_->SetField(field_);

	particleEmitter_->Update();
}

void ParticleEditor::Draw() {
#ifdef USE_IMGUI
	PrimitiveManager::GetInstance()->AddPoint(emitter_.transform.translate);
	AABB aabb = emitter_.spawnRange;
	aabb.min = emitter_.spawnRange.min + emitter_.transform.translate;
	aabb.max = emitter_.spawnRange.max + emitter_.transform.translate;
	PrimitiveManager::GetInstance()->AddAABB(aabb);
	Line line;
	line.origin = emitter_.transform.translate;
	line.diff = emitter_.angleBase * emitter_.speedBase;
	PrimitiveManager::GetInstance()->AddLine(line);

	PrimitiveManager::GetInstance()->AddAABB(field_.area);
	line.origin = {
		(field_.area.min.x + field_.area.max.x) / 2,
		(field_.area.min.y + field_.area.max.y) / 2,
		(field_.area.min.z + field_.area.max.z) / 2,
	};
	line.diff = field_.acceleration.translate;
	PrimitiveManager::GetInstance()->AddLine(line);
#endif

	particleEmitter_->Draw();
}