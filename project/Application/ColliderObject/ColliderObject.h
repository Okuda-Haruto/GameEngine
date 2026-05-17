#pragma once
#include <Collider/OBBCollider.h>
#include <Object/Object.h>

/// <summary>
/// 接触可能なオブジェクト
/// </summary>
class ColliderObject {
private:
	std::unique_ptr<Object> object_;

	std::shared_ptr<OBBCollider> collider_;

	SRT transform_;
public:
	//初期化
	void Initialize(std::shared_ptr<Model> model, SRT transform);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	std::shared_ptr<OBBCollider> GetCollider() { return collider_; }
};