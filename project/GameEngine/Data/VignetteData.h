#pragma once

struct VignetteData {
	//ビネットの強度。高いほど中心部が明るくなる
	float vignetteIntensity = 16.0f;
	//ビネットのカーブ。高いほど端の暗い範囲が広くなる。(0以下にするとビネットがかからなくなる)
	float vignetteCurve = 0.8f;

	float padding[2]{};
};