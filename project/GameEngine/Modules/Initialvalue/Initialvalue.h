#pragma once
#include <Windows.h>

#include <dxgi1_6.h>
#include <dxcapi.h>
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
Microsoft::WRL::ComPtr <ID3D12PipelineState> SpritePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> LinePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> NoDepthLinePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);