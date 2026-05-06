#pragma once

namespace ColorChange {

	enum ColorMode {
		COLORMODE_GRAYSCALE,	//グレースケール
		COLORMODE_SEPIATONE,	//セピア調
	};

	struct ColorChangeState {
		int colorMode;
		float intensity;
	};

}