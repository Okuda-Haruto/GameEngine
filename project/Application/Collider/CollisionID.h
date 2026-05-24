#pragma once

// 1桁目	プレイヤー側か
// 2桁目	敵側か
// 3行目	アイテムか
// 4桁目	衝突するか
enum CollisionID {
	CollisionID_Player_Body = 0b1001,
	CollisionID_Player_Attack = 0b0001,
	CollisionID_Enemy_Body = 0b1010,
	CollisionID_Enemy_Attack = 0b0010,
	CollisionID_Item_Body = 0b1100,
	CollisionID_Item_Attack = 0b0100,
	CollisionID_Anything_Body = 0b1000,
	CollisionID_Anything_Attack = 0b0000,
};