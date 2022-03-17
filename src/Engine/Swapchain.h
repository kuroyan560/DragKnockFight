#pragma once
#include"D3D12Data.h"

class DescriptorHeap_RTV;
class DescriptorHeap_CBV_SRV_UAV;
//�X���b�v�`�F�C��
class Swapchain
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDXGISwapChain4> swapchain;
	std::vector<std::shared_ptr<RenderTarget>> images;

	std::vector<UINT64> fenceValues;
	std::vector<ComPtr<ID3D12Fence1>> fences;

	DXGI_SWAP_CHAIN_DESC1 desc;

	HANDLE waitEvent;
	void SetMetadata();
public:
	Swapchain(const ComPtr<ID3D12Device>& Device,
		const ComPtr<IDXGISwapChain1>& Swapchain,
		DescriptorHeap_CBV_SRV_UAV& DescHeap_CBV_SRV_UAV,
		DescriptorHeap_RTV& DescHeapRTV,
		bool UseHDR,
		float* ClearValue);

	//�X���b�v�`�F�C���Q�b�^
	const ComPtr<IDXGISwapChain4>& GetSwapchain() { return swapchain; }

	//���̃R�}���h���ς߂�悤�ɂȂ�܂őҋ@
	void WaitPreviousFrame(const ComPtr<ID3D12CommandQueue>& CmdQueue, const int& FrameIdx);

	//���݂̃o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g
	const std::shared_ptr<RenderTarget>& GetBackBufferRenderTarget() { return images[swapchain->GetCurrentBackBufferIndex()]; }
};
