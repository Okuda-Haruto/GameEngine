#pragma once

// 1桁目	プレイヤー側か
// 2桁目	ボス側か
// 3桁目	キャラクターか
enum CollisionID {
	CollisionID_Player_Body = 0b101,
	CollisionID_Player_Attack = 0b001,
	CollisionID_Enemy_Body = 0b110,
	CollisionID_Enemy_Attack = 0b010,
	CollisionID_Item_Body = 0b100,
	CollisionID_Item_Attack = 0b000,
};