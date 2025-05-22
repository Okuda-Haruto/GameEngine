#include "Text.h"

#include "GetDescriptorHandle.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"
#include "ConvertString.h"

#include <wchar.h>

Text::~Text(){
    //解放
    //フォント情報を元に戻しておく
    SelectObject(hdc_, oldFont_);
    DeleteObject(hFont_);
    DeleteObject(oldFont_);
    ReleaseDC(hwnd_, hdc_);
    RemoveFontResourceEx(filePath_.c_str(), FR_NOT_ENUM, nullptr);
}

void Text::Initialize(LONG fontSize, LONG fontWeight, const std::string& filePath, const std::string& fontName, HWND hwnd) {

    //ウィンドウハンドルを保存
    hwnd_ = hwnd;

    //テキストのデータの数を0にしておく
    textData_.clear();

    //パスが正しいか確認
    filePath_ = ConvertString(filePath);
    DWORD fileAttr = GetFileAttributes(filePath_.c_str());
    assert(fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));

    // フォントを一時的に追加
    AddFontResourceEx(filePath_.c_str(), FR_NOT_ENUM, nullptr);

    // フォントを作成
    lf_ = { fontSize, 0, 0, 0, fontWeight, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN, {} };

    //フォント名を変更
    std::wstring fontNameW = ConvertString(fontName);
    wcscpy_s(lf_.lfFaceName, 32, fontNameW.c_str());
    
    //論理フォントの作成
    hFont_ = CreateFontIndirectW(&lf_);

    //デバイスコンテキストの入手
    hdc_ = GetDC(hwnd_);

    //元のフォントの状態を保存
    oldFont_ = (HFONT)SelectObject(hdc_, hFont_);
}

void Text::GetTextData(wchar_t text,
    Microsoft::WRL::ComPtr <ID3D12Device> device,
    Microsoft::WRL::ComPtr <ID3D12CommandQueue>& commandQueue,
    Microsoft::WRL::ComPtr <ID3D12CommandAllocator>& commandAllocator,
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList,
    Microsoft::WRL::ComPtr <ID3D12Fence>& fence,
    uint64_t& fenceValue,
    HANDLE& fenceEvent,
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>& srvDescriptorHeap,
    uint32_t descriptorSizeSRV,
    uint32_t& CPUIndex,
    uint32_t& GPUIndex) {

    TextData textData;

    //描画する文字
    UINT code = (UINT)text;

    //使用するフォントの情報
    TEXTMETRICW tm;
    GetTextMetricsW(hdc_, &tm);
    //描画する文字の位置情報
    GLYPHMETRICS gm;
    //変換行列  (単位行列)
    CONST MAT2 mat = { {0,1}, {0,0}, {0,0}, {0,1} };

    //GGO_GRAY4_BITMAPを使用
    const UINT format = GGO_GRAY4_BITMAP;
    DWORD size = GetGlyphOutlineW(hdc_, code, format, &gm, 0, nullptr, &mat);
    assert(size != GDI_ERROR);

    //データの入力
    textData.textOrigin.x = float(gm.gmptGlyphOrigin.x);
    textData.textOrigin.y = float(gm.gmptGlyphOrigin.y);
    textData.textSize.x = float(gm.gmCellIncX);
    textData.textSize.y = float(tm.tmHeight);


    //改行はsizeがある場合ので別途用意
    if (text == L'\n') {
        textData.isLineBreak = true;
        textData_.push_back(textData);
        return;
    //サイズが無い等のデータはここでreturnすること
    } else if (size == 0) {
        textData.isBlank = true;
        textData_.push_back(textData);
        length_++;
        return;
    }

    //ビットマップデータを格納するためのメモリ確保とデータ取得
    BYTE* pMono = new BYTE[size];
    assert(GetGlyphOutlineW(hdc_, code, format, &gm, size, pMono, &mat) != GDI_ERROR);

    //幅と高さを取得
    int fontWidth = gm.gmCellIncX;
    int fontHeight = tm.tmHeight;

    //テクスチャの作成
    DirectX::ScratchImage fontImage;
    HRESULT hr = fontImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, fontWidth, fontHeight, 1, 1);
    assert(SUCCEEDED(hr));

    //ビットマップの情報
    uint8_t* pPixels = fontImage.GetImages()->pixels;
    //ビットマップの区切り
    size_t RowPitch = fontImage.GetImages()->rowPitch;

    //ビットマップのピクセルデータをテクスチャに書き込む
    //xの原点
    int iOfs_x = gm.gmptGlyphOrigin.x;
    //yの原点
    int iOfs_y = tm.tmAscent - gm.gmptGlyphOrigin.y;
    //ビットマップの行の開始位置
    int glyphRowPitch = (gm.gmBlackBoxX + 3) & ~3; // 4の倍数に切り上げ

    // ビットマップデータの書き込み
    for (unsigned int y = 0; y < gm.gmBlackBoxY; ++y) {
        for (unsigned int x = 0; x < gm.gmBlackBoxX; ++x) {
            BYTE gray = pMono[y * glyphRowPitch + x];
            uint8_t alpha = static_cast<uint8_t>(gray * 255 / 16);
            uint32_t color = (alpha << 24) | (B_ << 16) | (G_ << 8) | R_;
            memcpy(pPixels + RowPitch * (y + iOfs_y) + 4 * (x + iOfs_x), &color, sizeof(uint32_t));
        }
    }
    delete[] pMono;

    const DirectX::TexMetadata& metadata = fontImage.GetMetadata();
    textData.textureResource_ = CreateTextureResource(device, metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textData.textureResource_.Get(), fontImage, device.Get(), commandList.Get());
    //コマンドリストの内容を確定させる。すべてのコマンドを詰んでからCloseすること
    hr = commandList->Close();
    assert(SUCCEEDED(hr));

    //GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    //Fenceの値を更新
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    //Fenceの値が指定したSignal値に辿り着いているか確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        //指定したSignalに辿り着いていないので、辿り着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        //イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    //次のフレーム用のコマンドリストを準備
    hr = commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(hr));

    intermediateResource->Release();

    //metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    //SRVを作成するDescriptorHeapの場所を決める。ImGuiが最初を使うのでその次を使う
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, CPUIndex);
    textData.textureSrvHandleGPU_ = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, GPUIndex);
    //SRVの生成
    device->CreateShaderResourceView(textData.textureResource_.Get(), &srvDesc, textureSrvHandleCPU);

    CPUIndex++;
    GPUIndex++;

    textData_.push_back(textData);
    length_++;
}

void Text::SetColor(uint8_t R, uint8_t G, uint8_t B) {
    R_ = R;
    G_ = G;
    B_ = B;
}

void Text::SetLogfont(LOGFONTW lf) {

    lf_ = lf;

    //一時解放
    SelectObject(hdc_, oldFont_);
    DeleteObject(hFont_);
    DeleteObject(oldFont_);

    //論理フォントの作成
    hFont_ = CreateFontIndirectW(&lf_);

    //デバイスコンテキストの入手
    hdc_ = GetDC(hwnd_);

    //元のフォントの状態を保存
    oldFont_ = (HFONT)SelectObject(hdc_, hFont_);
}