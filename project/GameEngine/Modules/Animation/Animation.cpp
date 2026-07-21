#include "Animation.h"
#include "LerpKeyFrame/LerpKeyFrame.h"
#include "Operation/Operation.h"
#include <PrimitiveManager/PrimitiveManager.h>

void ApplyAnimation(Skeleton& skeleton, AnimationData& animationData, AnimationInterpolation interpolation, float time) {
	for (Joint& joint : skeleton.joints) {
		joint.transform = {
			{1,1,1},
			IdentityQuaternion(),
			{0,0,0},
		};

		if (!animationData.nodeAnimations[joint.name].scale.keyframes.empty())joint.transform.scale = LerpKeyFrame(animationData.nodeAnimations[joint.name].scale.keyframes, interpolation, time);
		if (!animationData.nodeAnimations[joint.name].rotate.keyframes.empty())joint.transform.rotate = LerpKeyFrame(animationData.nodeAnimations[joint.name].rotate.keyframes, interpolation, time);
		if (!animationData.nodeAnimations[joint.name].translate.keyframes.empty())joint.transform.translate = LerpKeyFrame(animationData.nodeAnimations[joint.name].translate.keyframes, interpolation, time);
	}
}

int32_t CreateJoint(std::shared_ptr<Node>& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node->name;
	joint.localMatrix = node->localMatrix;
	joint.skeltonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node->transform;
	joint.index = int32_t(joints.size());	//現在登録されてる数をIndexに
	joint.parent = parent;

	joints.push_back(joint);
	for (std::shared_ptr<Node>& child : node->children) {
		//子ノードを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	//自身のindexを返す
	return joint.index;
}

Skeleton CreateSkelton(std::shared_ptr<Node>& rootNode) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とインデックスのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}
