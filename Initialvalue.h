#pragma once
#include <Windows.h>

#pragma comment(lib,"dxgi.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <fstream>
#include <wrl.h>

//使用するアダプターの初期値
Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapterInitialvalue(Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory, std::ofstream& logStream);

//使用するデバイスの初期値
Microsoft::WRL::ComPtr<ID3D12Device> deviceInitialvalue(Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter, std::ofstream& logStream);

//使用するコマンドキューの初期値
Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueueInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device> &device);

//使用するコマンドアロケータの初期値
Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocatorInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device> &device);

//使用するコマンドリストの初期値
Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandListInitialvalue(Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Device> &device);

//RootSignature作成
Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device>& device, std::ofstream& logStream);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device>& device,
	std::ofstream& logStream,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);