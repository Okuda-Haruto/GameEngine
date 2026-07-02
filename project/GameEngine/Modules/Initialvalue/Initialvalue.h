#pragma once
#include <Windows.h>

#include <dxgi1_6.h>
#include <dxcapi.h>
#include <d3d12.h>

#include <fstream>
#include <wrl.h>

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Triangle_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Triangle_NoDepth_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Triangle_Instancing_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Particle_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Particle_AddBlend_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Sprite_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Line_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Line_NoDepth_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Effect_Cylinder_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Screen_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);

//PSOを生成する
Microsoft::WRL::ComPtr <ID3D12PipelineState> Cubemap_PipelineStateInitialvalue(ID3D12Device* device,
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature,
	IDxcBlob* vertexShaderBlob,
	IDxcBlob* pixelShaderBlob);