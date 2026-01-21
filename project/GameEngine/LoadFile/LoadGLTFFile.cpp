#include "LoadGLTFFile.h"
#include "LoadMaterialTemplateFile.h"
#include "TextureManager/TextureManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <list>
#include <GameEngine.h>

//.gltfファイルからModelDataを構築する
ModelData LoadGLTFFile(const std::string& directoryPath, const std::string& filename) {

	ModelData modelData;	//構築するModelData

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());	//メッシュがないのは対応しない

	//Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());	//法線がないMeshは今回未対応
		assert(mesh->HasTextureCoords(0));	//TexcoordがないMeshは今回未対応

		//オフセット開始地点
		Offset offset;
		offset.vertexStart = UINT(modelData.vertices.size());

		//Faceの中身の解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);	//三角形のみサポート

			//Vertexの解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				ObjectVertexData vertex{};
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				// aiProcess_MakeLeftHandedはz*=-1.0fで右手->左手に変換するので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}
		}

		//Boneの解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* boneData = mesh->mBones[boneIndex];
			Bone bone;
			bone.offsetMatrix = aiMatrix4x4ToMatrix4x4(boneData->mOffsetMatrix);
			bone.name = boneData->mName.C_Str();
			//Weightの解析
			for (uint32_t weightIndex = 0; weightIndex < boneData->mNumWeights; ++weightIndex) {
				aiVertexWeight weight = boneData->mWeights[weightIndex];
				/*SetVertexWeight(
					modelData.vertices[weight.mVertexId].boneID,
					modelData.vertices[weight.mVertexId].boneWeight,
					boneIndex,
					weight.mWeight);*///
			}
			modelData.bones.push_back(bone);
		}

		//オフセット記録
		offset.vertexCount = UINT(modelData.vertices.size()) - offset.vertexStart;
		offset.indexStart = offset.vertexStart;
		offset.indexCount = offset.vertexCount;
		modelData.offset.push_back(offset);
	}

	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {	//最後に見つかったmaterialのTextureを使用
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilepath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	//ノードの取得
	modelData.rootNode = ReadNode(scene->mRootNode);
	//ノードとボーンを関係付ける
	for (Bone& bone : modelData.bones) {
		bone.node = FindNode(modelData.rootNode, bone.name);
	}

	//Animationの解析
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		aiAnimation* animation = scene->mAnimations[animationIndex];
		AnimationData animationData;
		animationData.duration = animation->mDuration;
		animationData.FPS = animation->mTicksPerSecond;

		//Channelの解析
		for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
			aiNodeAnim* channel = animation->mChannels[channelIndex];
			std::shared_ptr<Node>& node = FindNode(modelData.rootNode, channel->mNodeName.C_Str());
			//Scalingのキーフレームの解析
			for (uint32_t scalingKeyIndex = 0; scalingKeyIndex < channel->mNumScalingKeys; ++scalingKeyIndex) {
				aiVectorKey scalingKey = channel->mScalingKeys[scalingKeyIndex];
				KeyFrame transformKeyFrame;
				transformKeyFrame.time = scalingKey.mTime;
				transformKeyFrame.vector = Vector3{ scalingKey.mValue.x,scalingKey.mValue.y,scalingKey.mValue.z };

				node->scaleKeyFrame.push_back(transformKeyFrame);
			}
			//Rotationのキーフレームの解析
			for (uint32_t rotationKeyIndex = 0; rotationKeyIndex < channel->mNumRotationKeys; ++rotationKeyIndex) {
				aiQuatKey rotationKey = channel->mRotationKeys[rotationKeyIndex];
				KeyFrame transformKeyFrame;
				transformKeyFrame.time = rotationKey.mTime;
				transformKeyFrame.vector = Vector3{ rotationKey.mValue.x,rotationKey.mValue.y,rotationKey.mValue.z };

				node->rotateKeyFrame.push_back(transformKeyFrame);
			}
			//Positionのキーフレームの解析
			for (uint32_t positionKeyIndex = 0; positionKeyIndex < channel->mNumPositionKeys; ++positionKeyIndex) {
				aiVectorKey positionKey = channel->mPositionKeys[positionKeyIndex];
				KeyFrame transformKeyFrame;
				transformKeyFrame.time = positionKey.mTime;
				transformKeyFrame.vector = Vector3{ positionKey.mValue.x,positionKey.mValue.y,positionKey.mValue.z };
				//右手→左手
				transformKeyFrame.vector.x *= -1.0f;

				node->translateKeyFrame.push_back(transformKeyFrame);
			}
		}

		modelData.animations.push_back(animationData);
	}

	return modelData;
}

std::shared_ptr<Node> ReadNode(aiNode* node) {
	std::shared_ptr<Node> result = std::make_shared<Node>();
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;	//nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();	//列ベクトル形式を行ベクトル形式に転置
	for (auto i = 0; i < 4; i++) {
		for (auto j = 0; j < 4; j++) {
			result->localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}
	result->name = node->mName.C_Str();	//Node名を格納
	result->children.resize(node->mNumChildren);	//子の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		//再帰的に読んで階層構造を作っていく
		result->children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

std::shared_ptr<Node>& FindNode(std::shared_ptr<Node>& node, const std::string& name) {
    if (node->name == name) {
        return node;
    }
    for (std::shared_ptr<Node>& child : node->children) {
		std::shared_ptr<Node>& result = FindNode(child, name);
        if (result->name == name) {
            return result;
        }
    }
    return node;
}

void SetVertexWeight(UINT4& ids, Vector4& weights, UINT id, float weight) {
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