#pragma once
#include <Windows.h>

#include <dxgi1_6.h>
#include <dxcapi.h>
#include <d3d12.h>

#include <fstream>
#include <wrl.h>

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> TrianglePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> NoDepthTrianglePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> InstancingTrianglePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> ParticlePipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> AddBlendParticlePipelineStateInitialvalue(ID3D12Device* device,
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