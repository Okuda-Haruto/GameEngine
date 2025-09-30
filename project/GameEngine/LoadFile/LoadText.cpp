#include "LoadText.h"
#include "ConvertString.h"
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include <wchar.h>

/*DirectX::ScratchImage LoadText(std::string text, std::wstring& fontName, HWND hwnd) {

    DirectX::ScratchImage fontImage;
    HRESULT hr = fontImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, fontWidth, fontHeight, 1, 1);
    assert(SUCCEEDED(hr));

    uint8_t* pPixels = fontImage.GetImages()->pixels;
    size_t RowPitch = fontImage.GetImages()->rowPitch;

    // ビットマップのピクセルデータをテクスチャに書き込む
    int iOfs_x = gm.gmptGlyphOrigin.x;
    int iOfs_y = tm.tmAscent - gm.gmptGlyphOrigin.y;
    int glyphRowPitch = (gm.gmBlackBoxX + 3) & ~3; // 4の倍数に切り上げ

    // グリフのアルファ値をそのままRGBAに変換して書き込む
    // pMono のメモリ確保とデータ取得
    size = GetGlyphOutlineW(hdc, code, format, &gm, 0, nullptr, &mat);
    if (size == GDI_ERROR || size == 0) {
        OutputDebugStringW(L"GetGlyphOutlineW failed.\n");
        assert(0);
    }
    BYTE* pMono = new BYTE[size];
    if (GetGlyphOutlineW(hdc, code, format, &gm, size, pMono, &mat) == GDI_ERROR) {
        OutputDebugStringW(L"GetGlyphOutlineW data fetch failed.\n");
        delete[] pMono;
        assert(0);
    }
    // ビットマップデータの書き込み
    for (unsigned int y = 0; y < gm.gmBlackBoxY; ++y) {
        for (unsigned int x = 0; x < gm.gmBlackBoxX; ++x) {
            BYTE gray = pMono[y * glyphRowPitch + x];
            uint8_t alpha = static_cast<uint8_t>(gray * 255 / 16);
            uint32_t color = (alpha << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF;
            memcpy(pPixels + RowPitch * (y + iOfs_y) + 4 * (x + iOfs_x), &color, sizeof(uint32_t));
        }
    }
    delete[] pMono;

	//ミップマップ付きのデータを返す
	return fontImage;
}*/