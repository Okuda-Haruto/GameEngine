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

//RootSignature作成
Microsoft::WRL::ComPtr <ID3D12RootSignature> TriangleRootSignatureInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device>& device, std::ofstream& logStream);

//RootSignature作成
Microsoft::WRL::ComPtr <ID3D12RootSignature> InstancingRootSignatureInitialvalue(Microsoft::WRL::ComPtr<ID3D12Device>& device, std::ofstream& logStream);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> TrianglePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> NoDepthAddBlendTrianglePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> LinePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> SpritePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);