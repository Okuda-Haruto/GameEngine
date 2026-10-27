#include "Target.h"
#include "Operation/Operation.h"

void Target::Initialize(std::weak_ptr<GameCamera> gameCamera) {
	gameCamera_ = gameCamera;

	sprite_.second = make_unique<Sprite>();
	sprite_.second->Initialize("resources/Sprite/Target_true.png");
	sprite_.second->SetAnchorPoint(Vector2{ 0.5f,0.5f });

	targetSprites_ = make_unique<InstancingSprite>();
	targetSprites_->Initialize("resources/Sprite/Target_false.png");
}

void Target::Update() {
	if (gameCamera_.lock()) {
		std::vector<std::weak_ptr<Sphere>> targetSphere = gameCamera_.lock()->GetTargetSpheres();
		std::weak_ptr<Sphere> targeted = gameCamera_.lock()->GetTargetedSphere();
		targetSpriteNum_ = 0;

		for (uint32_t index = 0; index < targetIndex_; index++) {
			//番号決め
			std::string str = "TargetSprite " + std::to_string(index);

			targetSprites_->SetIsDraw(str, false);
		}

		for (auto& sphere : targetSphere) {



			if (sphere.lock() == targeted.lock()) {
				//目標地点
				Vector4 worldPosition = Vector4{ sphere.lock()->center.x,sphere.lock()->center.y,sphere.lock()->center.z,1.0f };
				Matrix4x4 viewMatrix = gameCamera_.lock()->GetCamera().lock()->GetViewMatrix();
				Matrix4x4 projectionMatrix = gameCamera_.lock()->GetCamera().lock()->GetProjectionMatrix();

				Vector4 clipPosition = Transform(worldPosition, viewMatrix * projectionMatrix);

				//カメラの裏側なら描画しない
				if (clipPosition.w <= 0.0f) {
					sprite_.first = false;
					continue;
				}

				Vector2 screenPosition = { clipPosition.x / clipPosition.w,clipPosition.y / clipPosition.w };

				Vector2 position = { (screenPosition.x + 1.0f) * 0.5f * 1280, (1.0f - screenPosition.y) * 0.5f * 720 };
				sprite_.second->SetPosition(position);

				sprite_.first = true;
			} else {
				if (targetSpriteNum_ >= targetIndex_) {
					AddTargetSprite();
				}
				//番号決め
				std::string str = "TargetSprite " + std::to_string(targetSpriteNum_);

				//目標地点
				Vector4 worldPosition = Vector4{ sphere.lock()->center.x,sphere.lock()->center.y,sphere.lock()->center.z,1.0f };
				Matrix4x4 viewMatrix = gameCamera_.lock()->GetCamera().lock()->GetViewMatrix();
				Matrix4x4 projectionMatrix = gameCamera_.lock()->GetCamera().lock()->GetProjectionMatrix();

				Vector4 clipPosition = Transform(worldPosition, viewMatrix * projectionMatrix);

				//カメラの裏側なら描画しない
				if (clipPosition.w <= 0.0f) {
					continue;
				}

				Vector2 screenPosition = { clipPosition.x / clipPosition.w,clipPosition.y / clipPosition.w };

				Vector2 position = { (screenPosition.x + 1.0f) * 0.5f * 1280, (1.0f - screenPosition.y) * 0.5f * 720 };
				targetSprites_->SetPosition(str, position);

				targetSprites_->SetIsDraw(str, true);

				targetSpriteNum_++;
			}
		}
	}

	sprite_.second->Update();
	targetSprites_->Update();
}

void Target::Draw() {
	if (gameCamera_.lock()) {
		if (gameCamera_.lock()->GetTargetedSphere().lock()) {
			if (sprite_.first) {
				sprite_.second->Draw2D();
			}
		}

		targetSprites_->Draw2D();
	}
}

void Target::AddTargetSprite() {
	//番号決め
	std::string str = "TargetSprite " + std::to_string(targetIndex_);

	//追加
	targetSprites_->AddNewSprite(str);
	targetSprites_->SetAnchorPoint(str, Vector2{ 0.5f,0.5f });
	targetSprites_->SetColor(str, Vector4{ 0.5f,0.5f,0.5f,1.0f });

	targetIndex_++;
}