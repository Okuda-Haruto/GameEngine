#include "Object_2D.h"
#include "GameEngine.h"

#include "Matrix4x4_operation.h"
#include "Vector3_operation.h"

#include "CreateBufferResource.h"

#pragma region Sprite_2D

Sprite_2D::~Sprite_2D() {
	vertexData_ = nullptr;
	wvpData_.clear();
	materialData_.clear();
}

void Sprite_2D::Initialize() {

	device_ = GameEngine::GetDevice();

	kWindowWidth_ = GameEngine::GetWindowWidth();
	kWindowHeight_ = GameEngine::GetWindowHeight();

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;

}

void Sprite_2D::Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_Single_Data& data) {

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { 0.0f,spriteHeight_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { spriteWidth_,spriteHeight_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { spriteWidth_,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
	wvpData_[index_]->World = worldMatrix;
	wvpData_[index_]->WorldInverseTranspose = Inverse(worldMatrix);

	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	wvpData_[index_]->WVP = worldViewProjectionMatrix;

	wvpResource_[index_]->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

	materialData_[index_]->color = data.material.color;
	materialData_[index_]->uvTransform = MakeAffineMatrix(data.material.uvTransform.scale, data.material.uvTransform.rotate, data.material.uvTransform.translate);
	materialData_[index_]->reflection = false;
	materialData_[index_]->enableDirectionalLighting = false;
	materialData_[index_]->enablePointLighting = false;
	materialData_[index_]->shininess = 0.0f;

	materialResource_[index_]->Unmap(0, nullptr);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GameEngine::RootSignature().Get());
	commandList->SetPipelineState(GameEngine::TrianglePSO());	//PSOを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	commandList->SetGraphicsRootDescriptorTable(2, GameEngine::TextureGet(data.material.textureIndex));
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ドローコール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex_) {
		index_ = 0;
	}
}

void Sprite_2D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}

#pragma endregion

#pragma region Text_2D

Text_2D::~Text_2D() {
	vertexBufferView_.clear();
	vertexData_ = nullptr;
	wvpData_.clear();
	materialData_.clear();
}

void Text_2D::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight) {

	device_ = device;

	kWindowWidth_ = kWindowWidth;
	kWindowHeight_ = kWindowHeight;

	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	vertexBufferView_.resize(kMaxIndex_);
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;

}

void Text_2D::Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_Single_Data& data) {

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 4 * int(textData_.size()));

	Vector3 textPosition{};

	for (int i = 0; i < textData_.size(); i++) {
		//例外
		if (textData_[i].isBlank) {	//空白
			textPosition = Add(textPosition, { textData_[i].textSize.x,0.0f,0.0f });
		} else if (textData_[i].isLineBreak) {	//改行
			textPosition = Add(textPosition, { 0.0f,textData_[i].textSize.y,0.0f });
			textPosition.x = 0;
		//描画
		} else {
			//頂点のローカル座標系を設定
			vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

			//左下
			vertexData_[i * 4].position = { 0.0f,textData_[i].textSize.y,0.0f,1.0f };
			vertexData_[i * 4].texcoord = { 0.0f,1.0f };
			vertexData_[i * 4].normal = { 0.0f,0.0f,-1.0f };
			//左上
			vertexData_[i * 4 + 1].position = { 0.0f,0.0f,0.0f,1.0f };
			vertexData_[i * 4 + 1].texcoord = { 0.0f,0.0f };
			vertexData_[i * 4 + 1].normal = { 0.0f,0.0f,-1.0f };
			//右下
			vertexData_[i * 4 + 2].position = { textData_[i].textSize.x,textData_[i].textSize.y,0.0f,1.0f };
			vertexData_[i * 4 + 2].texcoord = { 1.0f,1.0f };
			vertexData_[i * 4 + 2].normal = { 0.0f,0.0f,-1.0f };
			//右上
			vertexData_[i * 4 + 3].position = { textData_[i].textSize.x,0.0f,0.0f,1.0f };
			vertexData_[i * 4 + 3].texcoord = { 1.0f,0.0f };
			vertexData_[i * 4 + 3].normal = { 0.0f,0.0f,-1.0f };

			vertexResource_->Unmap(0, nullptr);

			//頂点バッファビューを作成する
			//リソースの先頭のアドレスから使う
			vertexBufferView_[i].BufferLocation = vertexResource_->GetGPUVirtualAddress() + i * sizeof(VertexData) * 4;
			//使用するリソースのサイズは頂点は4つサイズ
			vertexBufferView_[i].SizeInBytes = sizeof(VertexData) * 4;
			//1頂点あたりのサイズ
			vertexBufferView_[i].StrideInBytes = sizeof(VertexData);

			Vector3 origin = { textData_[i].textOrigin.x,0.0f,0.0f };
			Vector3 translate = Add(Add(data.transform.translate, origin), textPosition);
			textPosition = Add(textPosition, { textData_[i].textSize.x,0.0f,0.0f });

			Matrix4x4 viewMatrix = MakeIdentity4x4();
			Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

			//WVPデータを更新
			wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

			Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, translate);
			wvpData_[index_]->World = worldMatrix;
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			wvpData_[index_]->WVP = worldViewProjectionMatrix;

			wvpResource_[index_]->Unmap(0, nullptr);

			//マテリアルデータを更新
			materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

			materialData_[index_]->color = data.material.color;
			materialData_[index_]->uvTransform = MakeAffineMatrix(data.material.uvTransform.scale, data.material.uvTransform.rotate, data.material.uvTransform.translate);
			materialData_[index_]->reflection = false;
			materialData_[index_]->enableDirectionalLighting = false;
			materialData_[index_]->enablePointLighting = false;
			materialData_[index_]->shininess = 0.0f;

			materialResource_[index_]->Unmap(0, nullptr);

			//Spriteの描画。変更が必要なものだけ変更する
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);	//VBVを設定
			commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
			commandList->SetGraphicsRootDescriptorTable(2, textData_[i].textureSrvHandleGPU_);
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
			//TransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
			//ドローコール
			commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//最大値を超えたら最初から
			index_++;
			if (index_ >= kMaxIndex_) {
				index_ = 0;
			}
		}
	}
}

void Text_2D::Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_Single_Data& data,int index) {

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 4 * int(textData_.size()));

	Vector3 textPosition{};

	int gradationIndex = index;

	for (int i = 0; i < gradationIndex; i++) {
		//範囲外を描画しようとしだしたら終了
		if (textData_.size() <= i) { return; };
		//例外
		if (textData_[i].isBlank) {	//空白
			textPosition = Add(textPosition, { textData_[i].textSize.x,0.0f,0.0f });
		} else if (textData_[i].isLineBreak) {	//改行
			textPosition = Add(textPosition, { 0.0f,textData_[i].textSize.y,0.0f });
			textPosition.x = 0;
			gradationIndex++;
			//描画
		} else {
			//頂点のローカル座標系を設定
			vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

			//左下
			vertexData_[i * 4].position = { 0.0f,textData_[i].textSize.y,0.0f,1.0f };
			vertexData_[i * 4].texcoord = { 0.0f,1.0f };
			vertexData_[i * 4].normal = { 0.0f,0.0f,-1.0f };
			//左上
			vertexData_[i * 4 + 1].position = { 0.0f,0.0f,0.0f,1.0f };
			vertexData_[i * 4 + 1].texcoord = { 0.0f,0.0f };
			vertexData_[i * 4 + 1].normal = { 0.0f,0.0f,-1.0f };
			//右下
			vertexData_[i * 4 + 2].position = { textData_[i].textSize.x,textData_[i].textSize.y,0.0f,1.0f };
			vertexData_[i * 4 + 2].texcoord = { 1.0f,1.0f };
			vertexData_[i * 4 + 2].normal = { 0.0f,0.0f,-1.0f };
			//右上
			vertexData_[i * 4 + 3].position = { textData_[i].textSize.x,0.0f,0.0f,1.0f };
			vertexData_[i * 4 + 3].texcoord = { 1.0f,0.0f };
			vertexData_[i * 4 + 3].normal = { 0.0f,0.0f,-1.0f };

			vertexResource_->Unmap(0, nullptr);

			//頂点バッファビューを作成する
			//リソースの先頭のアドレスから使う
			vertexBufferView_[i].BufferLocation = vertexResource_->GetGPUVirtualAddress() + i * sizeof(VertexData) * 4;
			//使用するリソースのサイズは頂点は4つサイズ
			vertexBufferView_[i].SizeInBytes = sizeof(VertexData) * 4;
			//1頂点あたりのサイズ
			vertexBufferView_[i].StrideInBytes = sizeof(VertexData);

			Vector3 origin = { textData_[i].textOrigin.x,0.0f,0.0f };
			Vector3 translate = Add(Add(data.transform.translate, origin), textPosition);
			textPosition = Add(textPosition, { textData_[i].textSize.x,0.0f,0.0f });

			Matrix4x4 viewMatrix = MakeIdentity4x4();
			Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

			//WVPデータを更新
			wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

			Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, translate);
			wvpData_[index_]->World = worldMatrix;
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			wvpData_[index_]->WVP = worldViewProjectionMatrix;

			wvpResource_[index_]->Unmap(0, nullptr);

			//マテリアルデータを更新
			materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

			materialData_[index_]->color = data.material.color;
			materialData_[index_]->uvTransform = MakeAffineMatrix(data.material.uvTransform.scale, data.material.uvTransform.rotate, data.material.uvTransform.translate);
			materialData_[index_]->reflection = false;
			materialData_[index_]->enableDirectionalLighting = false;
			materialData_[index_]->enablePointLighting = false;
			materialData_[index_]->shininess = 0.0f;

			materialResource_[index_]->Unmap(0, nullptr);

			//Spriteの描画。変更が必要なものだけ変更する
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);	//VBVを設定
			commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
			commandList->SetGraphicsRootDescriptorTable(2, textData_[i].textureSrvHandleGPU_);

			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
			//TransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
			commandList->SetPipelineState(GameEngine::TrianglePSO());	//PSOを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//ドローコール
			commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//最大値を超えたら最初から
			index_++;
			if (index_ >= kMaxIndex_) {
				index_ = 0;
			}
		}
	}
}

void Text_2D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}

#pragma endregion