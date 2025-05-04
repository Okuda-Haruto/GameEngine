#include <Windows.h>
#include <cstdint>
#include <format>
#include <sstream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#include "SRT.h"
#include "ModelData.h"
#include "Material.h"
#include "MaterialData.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "ResourceObject.h"
#include "LoadTexture.h"
#include "LoadObjFile.h"
#include "CreateBufferResource.h"
#include "CompileShader.h"
#include "CreateDescriptorHeap.h"
#include "CreateTextureResource.h"
#include "CreateDepthStencilTextureResource.h"
#include "UploadTextureData.h"
#include "GetDescriptorHandle.h"
#include "LoadMaterialTemplateFile.h"
#include "D3DResourceLeakChecker.h"
#include "ExportDump.h"

#include "Window.h"
#include "Log.h"
#include "ConvertString.h"
#include "Initialvalue.h"

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#pragma comment(lib,"dxgi.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>
#pragma comment(lib,"Dbghelp.lib")
#include <DbgHelp.h>
#include <cassert>
#include <wrl.h>
#include <strsafe.h>

#include "GameEngine.h"

//ウィンドウの幅
const int32_t kWindowWidth = 1280;
//ウィンドウの高さ
const int32_t kWindowHeight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	/*
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	//左下2
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//上2
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	//右下2
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	//分割数
	const uint32_t kSubdivision = 16;

	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * kSubdivision * kSubdivision * 4);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * kSubdivision * kSubdivision * 4;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//経度分割1つ分の角度φd
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	//緯度分割1つ分の角度θd
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;	//θ
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;	//φ
			//頂点にデータを入力する
			//左下
			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//左上
			vertexData[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start + 1].position.y = sin(lat + kLatEvery);
			vertexData[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			//右下
			vertexData[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start + 2].position.y = sin(lat);
			vertexData[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//右上
			vertexData[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData[start + 3].position.y = sin(lat + kLatEvery);
			vertexData[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);

		}
	}

	//法線の定義の追加
	for (uint32_t index = 0; index < kSubdivision * kSubdivision * 4; ++index) {
		vertexData[index].normal.x = vertexData[index].position.x;
		vertexData[index].normal.y = vertexData[index].position.y;
		vertexData[index].normal.z = vertexData[index].position.z;
	}

	//Sprite用のインデックスリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * kSubdivision * kSubdivision * 6);

	//インデックスバッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;	//θ
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t startVertex = (latIndex * kSubdivision + lonIndex) * 4;
			indexData[start] = 0 + startVertex;
			indexData[start + 1] = 1 + startVertex;
			indexData[start + 2] = 2 + startVertex;
			indexData[start + 3] = 1 + startVertex;
			indexData[start + 4] = 3 + startVertex;
			indexData[start + 5] = 2 + startVertex;
		}
	}
	*/

	GameEngine* gameEngine = new GameEngine();

	gameEngine->Intialize(L"CG2");

	Audio* audio = new Audio();

	gameEngine->LoadAudio(audio, "resources/fanfare.wav",true);

	Audio* audio2 = new Audio();

	gameEngine->LoadAudio(audio2, "resources/Alarm01.wav",true);

	Object_3D* object1 = new Object_3D();

	gameEngine->LoadObject(object1, "resources", "axis.obj");

	Object_3D* object2 = new Object_3D();

	gameEngine->LoadObject(object2, "resources", "plane.obj");

	Object_2D* sprite = new Object_2D();

	gameEngine->LoadObject(sprite);

	Light* light = new Light();

	gameEngine->LoadLight(light);

	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//Transform変数を作る
	Transform transform2{ {1.0f,1.0f,1.0f},
		{0.0f,180.0f * float(M_PI) / 180,0.0f},
		{1.0f,0.0f,0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//カメラ変数を作る。zが-10の位置でz+の方向を向いている
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };


	//テクスチャ読み込み
	uint32_t kLastCPUIndex = 1;
	uint32_t kLastGPUIndex = 1;

	Texture* spriteTexture = new Texture();

	gameEngine->LoadTexture(spriteTexture, "resources/monsterBall.png");

	Texture* object3DTexture1 = new Texture();

	gameEngine->LoadTexture(object3DTexture1, (object1->ModelData()).material.textureFilePath);

	Texture* object3DTexture2 = new Texture();

	gameEngine->LoadTexture(object3DTexture2, (object2->ModelData()).material.textureFilePath);


	//audio->SoundPlayWave();
	//audio2->SoundPlayWave();

	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
	float lightIntensity = 1.0f;

	Vector4 Color1 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Color2 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 ColorSprite = { 1.0f,1.0f,1.0f,1.0f };

	float AudioVolume = 1.0f;

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {
			
			//Keybord keybord = gameEngine->GetKeybord();

			//Mouse mouse = gameEngine->GetMouse();

			//Pad pad = gameEngine->GetPad();

			//ImGui
			ImGui::Text("Camera");
			ImGui::DragFloat3("cameraTranslate", &(cameraTransform.translate).x);
			ImGui::SliderAngle("cameraRatateX", &(cameraTransform.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("cameraRatateY", &(cameraTransform.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("cameraRatateZ", &(cameraTransform.rotate).z, -180.0f, 180.0f);
			ImGui::Text("material");
			ImGui::DragFloat3("materialTranslate", &(transform.translate).x, 0.01f);
			ImGui::SliderAngle("materialRatateX", &(transform.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateY", &(transform.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateZ", &(transform.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("materialTextureColor", &Color1.x);
			ImGui::Text("material2");
			ImGui::DragFloat3("material2Translate", &(transform2.translate).x, 0.01f);
			ImGui::SliderAngle("material2RatateX", &(transform2.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateY", &(transform2.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateZ", &(transform2.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("material2TextureColor", &Color2.x);
			ImGui::Text("sprite");
			ImGui::DragFloat3("Position", &(transformSprite.translate).x);
			ImGui::ColorPicker4("SpriteColor", &ColorSprite.x);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			ImGui::Text("directionalLiight");
			ImGui::DragFloat("intensity", &lightIntensity, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat3("direction", &lightDirection.x, 0.01f, -1.0f, 1.0f);
			ImGui::ColorPicker4("color", &lightColor.x);
			float sqrtNumber = sqrtf(sqrtf(powf(lightDirection.x, 2) + powf(lightDirection.y, 2)) + powf(lightDirection.z, 2));
			lightDirection.x = lightDirection.x / sqrtNumber;
			lightDirection.y = lightDirection.y / sqrtNumber;
			lightDirection.z = lightDirection.z / sqrtNumber;
			ImGui::Text("Audio");
			ImGui::SliderFloat("AudioVolume", &AudioVolume,0.0f,1.0f);
			if (ImGui::Button("Play")) {
				audio->SoundPlayWave();
			}
			if (ImGui::Button("Stop")) {
				audio->SoundStopWave();
			}
			if (ImGui::Button("End")) {
				audio->SoundEndWave();
			}

			object1->SetTransform(transform);
			object1->SetUVTransform(uvTransformSprite);
			object1->SetColor(Color1);
			object1->SetCamera(cameraTransform);
			object2->SetTransform(transform2);
			object2->SetUVTransform(uvTransformSprite);
			object2->SetColor(Color2);
			object2->SetCamera(cameraTransform);
			sprite->SetTransform(transformSprite);
			sprite->SetUVTransform(uvTransformSprite);
			sprite->SetColor(ColorSprite);

			light->SetColor(lightColor);
			light->SetDirection(lightDirection);
			light->SetIntensity(lightIntensity);

			audio->SetVolume(AudioVolume);

			//
			//	描画処理
			//

			gameEngine->PreDraw();

			object1->Draw(gameEngine->GetCommandList(), light->directionalLightResource(), object3DTexture1->textureSrvHandleGPU());

			object2->Draw(gameEngine->GetCommandList(), light->directionalLightResource(), object3DTexture2->textureSrvHandleGPU());

			sprite->Draw(gameEngine->GetCommandList(), spriteTexture->textureSrvHandleGPU());

			gameEngine->PostDraw();
		}
	}

	delete audio;
	delete audio2;
	delete object1;
	delete object2;
	delete sprite;
	delete light;

	delete spriteTexture;
	delete object3DTexture1;
	delete object3DTexture2;

	delete gameEngine;

	return 0;
}
