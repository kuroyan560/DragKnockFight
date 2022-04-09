#pragma once
//DirectX12�֘A�̋@�\���~���ɗ��p���邽�߂̃N���X================================================
#include"D3D12Data.h"
#include"DescriptorHeap.h"
#include"Color.h"

#include<map>
#include<functional>
#include<DirectXTex.h>
using namespace DirectX;

#include"Swapchain.h"

//D3D12App�𗘗p���̃N���X
class D3D12AppUser
{
protected:
	friend class D3D12App;
	D3D12AppUser() {}
	virtual void Render() = 0;	//�����_�����O�֘A�̏���
};

class D3D12App
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	static D3D12App* INSTANCE;
	//�_�u���o�b�t�@
	static const UINT FRAME_BUFFER_COUNT = 2;

	//���[�h�p�̃����_���̂��߂̌^
	using LoadLambda_t = std::function<HRESULT(const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)>;
	static std::map<std::string, LoadLambda_t>loadLambdaTable;

public:
	static D3D12App* Instance()
	{
		if (INSTANCE == nullptr)ASSERT_MSG("D3D12App�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
		return INSTANCE;
	}

private:
	//DirectX12�f�o�C�X
	ComPtr<ID3D12Device> device;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory6>dxgiFactory = nullptr;
	//�R�}���h�A���P�[�^
	std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue;

	//�f�B�X�N���v�^�q�[�v
	std::unique_ptr<DescriptorHeap_CBV_SRV_UAV> descHeapCBV_SRV_UAV;
	std::unique_ptr<DescriptorHeap_RTV> descHeapRTV;
	std::unique_ptr<DescriptorHeap_DSV> descHeapDSV;

	//�X���b�v�`�F�C��
	std::unique_ptr<Swapchain>swapchain;
	DXGI_FORMAT backBuffFormat = DXGI_FORMAT_UNKNOWN;	//�o�b�N�o�b�t�@�̃t�H�[�}�b�g

	//���������J���[�e�N�X�`���L�^�p
	struct ColorTexture
	{
		Color color;
		int width;
		std::shared_ptr<TextureBuffer>tex;
	};
	std::list<ColorTexture>colorTextures;

	//���������摜�e�N�X�`���L�^�p
	struct LoadImgTexture
	{
		std::string path;
		std::vector<std::shared_ptr<TextureBuffer>>textures;
	};
	std::list<LoadImgTexture>loadImgTextures;

	//�摜�𕪊�����p�C�v���C��
	struct SplitImgConstData
	{
		Vec2<int> imgNum = { 0,0 };
		Vec2<int> splitSize;
	};
	int splitTexBuffCount = 0;	//SpliteTexBuffer���Ă΂ꂽ��
	std::vector<std::shared_ptr<ConstantBuffer>>splitImgConstBuff;
	std::shared_ptr<ComputePipeline>splitImgPipeline;

	void Initialize(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen);

public:
	D3D12App(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen = false)
	{
		if (INSTANCE != nullptr)ASSERT_MSG("���ɃC���X�^���X������܂��BD3D12App�͂P�̃C���X�^���X�������Ă܂���\n");
		INSTANCE = this;
		Initialize(Hwnd, ScreenSize, UseHDR, ClearValue, IsFullScreen);
	}

	//�Q�b�^
	const ComPtr<ID3D12Device>& GetDevice() { return device; }
	const ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return commandList; }
	const DXGI_FORMAT& GetBackBuffFormat() { return backBuffFormat; }
	const std::shared_ptr<RenderTarget>& GetBackBuffRenderTarget() { return swapchain->GetBackBufferRenderTarget(); }

	//�f�B�X�N���v�^�f�[�^�q�[�v�̃Z�b�g(�R���s���[�g�V�F�[�_�[�𗘗p����Ƃ��̓��[�U�[������Ăяo����悤��)
	void SetDescHeaps();
	//�����_�����O�iD3D12AppUser���p������ KuroEngine �ȊO�ł͌Ăяo���Ȃ��j
	void Render(D3D12AppUser* User);

	//���_�o�b�t�@����
	std::shared_ptr<VertexBuffer>GenerateVertexBuffer(const size_t& VertexSize, const int& VertexNum, void* InitSendData = nullptr, const char* Name = nullptr, const bool& RWStructuredBuff = false);
	//�C���f�b�N�X�o�b�t�@����
	std::shared_ptr<IndexBuffer>GenerateIndexBuffer(const int& IndexNum, void* InitSendData = nullptr, const char* Name = nullptr, const DXGI_FORMAT& IndexFormat = DXGI_FORMAT_R32_UINT);

	//�萔�o�b�t�@����
	std::shared_ptr<ConstantBuffer>GenerateConstantBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData = nullptr, const char* Name = nullptr);
	//�\�����o�b�t�@����
	std::shared_ptr<StructuredBuffer>GenerateStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData = nullptr, const char* Name = nullptr);
	//�o�͗p�o�b�t�@�쐬
	std::shared_ptr<RWStructuredBuffer>GenerateRWStructuredBuffer(const size_t& DataSize, const int& ElementNum, const char* Name = nullptr);
	//�e�N�X�`���o�b�t�@����
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const Color& Color, const DXGI_FORMAT& Format = DXGI_FORMAT_R32G32B32A32_FLOAT, const int& Width = 1);	//�P�F�h��Ԃ��e�N�X�`������
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const std::string& LoadImgFilePath);	//�摜�t�@�C��
	std::shared_ptr<TextureBuffer>GenerateTextureBuffer(const Vec2<int>&Size,const DXGI_FORMAT& Format,const char* Name = nullptr);	//�R���s���[�g�V�F�[�_�[�ł̕`�����ݐ�p�iUAV���쐬����j
	/// <summary>
	/// �摜��ǂݍ���ŕ��� (LoadDivGraph)
	/// </summary>
	/// <param name="LoadImgFilePath">�摜�t�@�C���p�X</param>
	/// <param name="AllNum">��������</param>
	/// <param name="SplitNum">�c���̕�����</param>
	/// <param name="SplitSize">������̃T�C�Y</param>
	/// <returns></returns>
	std::vector<std::shared_ptr<TextureBuffer>>GenerateTextureBuffer(const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum);
	void GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum);
	//�����_�[�^�[�Q�b�g����
	std::shared_ptr<RenderTarget>GenerateRenderTarget(const DXGI_FORMAT& Format, const Color& ClearValue, const Vec2<int>Size,
		const wchar_t* TargetName = nullptr, D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_RENDER_TARGET, int MipLevel = 1, int ArraySize = 1);
	//�f�v�X�X�e���V���쐬
	std::shared_ptr<DepthStencil>GenerateDepthStencil(const Vec2<int>& Size, const DXGI_FORMAT& Format = DXGI_FORMAT_D32_FLOAT, const float& ClearValue = 1.0f);

	//�V�F�[�_�[�̃R���p�C��
	ComPtr<ID3DBlob>CompileShader(const std::string& FilePath, const std::string& EntryPoint, const std::string& ShaderModel);

	//�p�C�v���C������
	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	/// <param name="Option">�p�C�v���C���e��ݒ�</param>
	/// <param name="ShaderInfo">�V�F�[�_�[���</param>
	/// <param name="InputLayout">���V���C�A�E�g</param>
	/// <param name="RootParams">���[�g�p�����[�^�i�f�B�X�N���v�^���r���[����ɂȂ�̂Œ��Ӂj</param>
	/// <param name="RenderTargetFormat">�`��惌���_�[�^�[�Q�b�g�ݒ�</param>
	/// <param name="SamplerDesc">�T���v���[</param>
	std::shared_ptr<GraphicsPipeline>GenerateGraphicsPipeline(
		const PipelineInitializeOption& Option,
		const Shaders& ShaderInfo,
		const std::vector<InputLayoutParam>& InputLayout,
		const std::vector<RootParam>& RootParams,
		const std::vector<RenderTargetInfo>& RenderTargetFormat,
		const WrappedSampler& Sampler);


	//�R���s���[�g�p�C�v���C������
	std::shared_ptr<ComputePipeline>GenerateComputePipeline(
		const ComPtr<ID3DBlob>& ComputeShader,
		const std::vector<RootParam>& RootParams,
		const WrappedSampler& Sampler);

	//�r���[�|�[�g�ƃV�U�[�̃Z�b�g
	void SetViewPortsAndScissorRects(const Vec2<float>& Size);
	//�o�b�N�o�b�t�@�����_�[�^�[�Q�b�g���Z�b�g
	void SetBackBufferRenderTarget();

	//�e�N�X�`���𕪊�����
	void SplitTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name);
	std::vector<std::shared_ptr<TextureBuffer>>SplitTextureBuffer(const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name);
};