#pragma once

//アニメーション補完方法
enum class AnimationInterpolation {
	Linear,			//線形補間
	Step,			//キーフレームのみ
	Cubic_Spline	//スプライン曲線
};