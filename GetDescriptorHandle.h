#pragma once
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

static uint32_t kLastCPUIndex = 1;

static uint32_t kLastGPUIndex = 1;

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);