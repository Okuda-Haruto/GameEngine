#pragma once

// 1桁目	プレイヤー側か
// 2桁目	ボス側か
// 3桁目	キャラクターか
enum CollisionID {
	CollisionID_Player_Character = 0b101,
	CollisionID_Player_Bullet = 0b001,
	CollisionID_Enemy_Character = 0b110,
	CollisionID_Enemy_Bullet = 0b010,
	CollisionID_Item_Character = 0b100,
	CollisionID_Item_Bullet = 0b000,
};