#include "LoadGLTFFile.h"
#include "LoadMaterialTemplateFile.h"
#include "TextureManager/TextureManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <list>
#include <GameEngine.h>

Matrix4x4 mirrorX =
{
	-1, 0, 0, 0,
	 0, 1, 0, 0,
	 0, 0, 1, 0,
	 0, 0, 0, 1
};

//.gltfファイルからModelDataを構築する
ModelData LoadGLTFFile(const std::string& directoryPath, const std::string& filename) {

	ModelData modelData;	//構築するModelData

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
	assert(scene->HasMeshes());	//メッシュがないのは対応しない

	//Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());	//法線がないMeshは今回未対応
		assert(mesh->HasTextureCoords(0));	//TexcoordがないMeshは今回未対応

		//オフセット開始地点
		Offset offset;
		offset.vertexStart = UINT(modelData.vertices.size());
		offset.indexStart = UINT(modelData.indexes.size());

		//Vertexの解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			ObjectVertexData vertexData{};
			vertexData.position = { position.x,position.y,position.z,1.0f };
			vertexData.normal = { normal.x,normal.y,normal.z };
			vertexData.texcoord = { texcoord.x,texcoord.y };
			// aiProcess_MakeLeftHandedはz*=-1.0fで右手->左手に変換するので手動で対処
			vertexData.position.x *= -1.0f;
			vertexData.normal.x *= -1.0f;
			modelData.vertices.push_back(vertexData);
		}

		//Faceの中身の解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);	//三角形のみサポート

			//Indexの解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t index = face.mIndices[element];
				modelData.indexes.push_back(index);
			}
		}

		//Boneの解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* boneData = mesh->mBones[boneIndex];
			Bone bone;
			bone.offsetMatrix = mirrorX * Transpose(aiMatrix4x4ToMatrix4x4(boneData->mOffsetMatrix)) * mirrorX;
			bone.name = boneData->mName.C_Str();
			//Weightの解析
			for (uint32_t weightIndex = 0; weightIndex < boneData->mNumWeights; ++weightIndex) {
				aiVertexWeight weight = boneData->mWeights[weightIndex];
				SetVertexWeight(
					modelData.vertices[weight.mVertexId].boneID,
					modelData.vertices[weight.mVertexId].boneWeight,
					boneIndex,
					weight.mWeight);
			}
			modelData.bones.push_back(bone);
		}

		for (int index = 0; index < modelData.vertices.size(); index++) {
			modelData.vertices[index].boneWeight = NormalizeWeights(modelData.vertices[index].boneWeight);
		}

		//オフセット記録
		offset.vertexCount = UINT(modelData.vertices.size()) - offset.vertexStart;
		offset.indexCount = UINT(modelData.indexes.size()) - offset.indexStart;
		modelData.offset.push_back(offset);
	}

	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {	//最後に見つかったmaterialのTextureを使用
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilepath = directoryPath + "/" + textureFilePath.C_Str();
			modelData.textureIndex.push_back(TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilepath));
		}
	}

	//ノードの取得
	modelData.rootNode = ReadNode(scene->mRootNode, modelData);
	//ノードとボーンを関係付ける
	for (Bone& bone : modelData.bones) {
		bone.node = modelData.nodeMap[bone.name];
	}

	//Animationの解析
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		aiAnimation* animation = scene->mAnimations[animationIndex];
		AnimationData& animationData = modelData.animations[animation->mName.C_Str()];
		//時間の単位を秒に変換
		animationData.duration = float(animation->mDuration / animation->mTicksPerSecond);

		//Channelの解析
		for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
			aiNodeAnim* channel = animation->mChannels[channelIndex];
			
			NodeAnimation& nodeAnimation = animationData.nodeAnimations[channel->mNodeName.C_Str()];

			//Scalingのキーフレームの解析
			for (uint32_t scalingKeyIndex = 0; scalingKeyIndex < channel->mNumScalingKeys; ++scalingKeyIndex) {
				aiVectorKey scalingKey = channel->mScalingKeys[scalingKeyIndex];
				Keyframe_Vector3 transformKeyFrame;
				transformKeyFrame.time = float(scalingKey.mTime / animation->mTicksPerSecond);
				transformKeyFrame.value = Vector3{ scalingKey.mValue.x,scalingKey.mValue.y,scalingKey.mValue.z };

				nodeAnimation.scale.keyframes.push_back(transformKeyFrame);
			}

			//Rotationのキーフレームの解析
			for (uint32_t rotationKeyIndex = 0; rotationKeyIndex < channel->mNumRotationKeys; ++rotationKeyIndex) {
				aiQuatKey rotationKey = channel->mRotationKeys[rotationKeyIndex];
				Keyframe_Quaternion transformKeyFrame;
				transformKeyFrame.time = float(rotationKey.mTime / animation->mTicksPerSecond);
				transformKeyFrame.value = Quaternion{ -rotationKey.mValue.x,rotationKey.mValue.y,rotationKey.mValue.z,rotationKey.mValue.w };	//右手->左手

				nodeAnimation.rotate.keyframes.push_back(transformKeyFrame);
			}

			//Positionのキーフレームの解析
			for (uint32_t positionKeyIndex = 0; positionKeyIndex < channel->mNumPositionKeys; ++positionKeyIndex) {
				aiVectorKey positionKey = channel->mPositionKeys[positionKeyIndex];
				Keyframe_Vector3 transformKeyFrame;
				transformKeyFrame.time = float(positionKey.mTime / animation->mTicksPerSecond);
				transformKeyFrame.value = Vector3{ -positionKey.mValue.x,positionKey.mValue.y,positionKey.mValue.z };	//右手->左手

				nodeAnimation.translate.keyframes.push_back(transformKeyFrame);
			}

		}
	}

	return modelData;
}

std::shared_ptr<Node> ReadNode(aiNode* node, ModelData& modelData) {
	std::shared_ptr<Node> result = std::make_shared<Node>();
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;	//nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();	//列ベクトル形式を行ベクトル形式に転置
	for (auto i = 0; i < 4; i++) {
		for (auto j = 0; j < 4; j++) {
			result->localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}
	result->localMatrix = mirrorX * result->localMatrix * mirrorX;
	result->name = node->mName.C_Str();	//Node名を格納
	result->children.resize(node->mNumChildren);	//子の数だけ確保
	modelData.nodeMap[result->name] = result;
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		//再帰的に読んで階層構造を作っていく
		result->children[childIndex] = ReadNode(node->mChildren[childIndex], modelData);
	}
	return result;
}

void SetVertexWeight(UINT4& ids, Vector4& weights, UINT id, float weight) {
	//weightが満杯なら
	if (weights.x > 0.0f &&
		weights.y > 0.0f &&
		weights.z > 0.0f &&
		weights.w > 0.0f) {

		//全てのweightより小さいなら切り捨て
		if (weights.x > weight &&
			weights.y > weight &&
			weights.z > weight &&
			weights.w > weight) {
			return;
		}

		//最小値のweightのIndexを求める
		UINT minIndex = ids.x;
		if (weights.x < weights.y && weights.x < weights.z && weights.x < weights.w) {
			minIndex = ids.x;
		} else if (weights.y < weights.x && weights.y < weights.z && weights.y < weights.w) {
			minIndex = ids.y;
		} else if (weights.z < weights.x && weights.z < weights.y && weights.z < weights.w) {
			minIndex = ids.z;
		} else if (weights.w < weights.x && weights.w < weights.y && weights.w < weights.z) {
			minIndex = ids.w;
		}

		//最小値のweightを上書き
		if (ids.x == minIndex) {
			ids.x = id;
			weights.x = weight;
			weights = NormalizeWeights(weights);
			return;
		}
		if (ids.y == minIndex) {
			ids.y = id;
			weights.y = weight;
			weights = NormalizeWeights(weights);
			return;
		}
		if (ids.z == minIndex) {
			ids.z = id;
			weights.z = weight;
			weights = NormalizeWeights(weights);
			return;
		}
		if (ids.w == minIndex) {
			ids.w = id;
			weights.w = weight;
			weights = NormalizeWeights(weights);
			return;
		}

	} else {

		//空いてるところから入れていく
		if (weights.x <= 0.0f) {
			ids.x = id;
			weights.x = weight;
			return;
		}
		if (weights.y <= 0.0f) {
			ids.y = id;
			weights.y = weight;
			return;
		}
		if (weights.z <= 0.0f) {
			ids.z = id;
			weights.z = weight;
			return;
		}
		if (weights.w <= 0.0f) {
			ids.w = id;
			weights.w = weight;
			return;
		}
	}
}