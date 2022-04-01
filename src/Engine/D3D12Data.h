#pragma once
#include<d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include<wrl.h>
#include <cassert>
#include"Vec.h"

#define ASSERT_MSG(msg) {printf("%s", msg); assert(0);}

static const enum DESC_HANDLE_TYPE { CBV, SRV, UAV, RTV, DSV, DESC_HANDLE_NUM };

//�f�B�X�N���v�^�n���h��
class DescHandles
{
private:
	//����������Ă��邩
	bool invalid = true;
	//CPU�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	//GPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

public:
	//�R���X�g���N�^
	DescHandles() {}
	DescHandles(const DescHandles& tmp)
	{
		if (!tmp)ASSERT_MSG("�R�s�[�R���X�g���N�^���Ăяo����܂������A�R�s�[���̏�����������Ă��܂���\n");
		Initialize(tmp.cpuHandle, tmp.gpuHandle);
	}
	DescHandles(DescHandles&& tmp)
	{
		if (!tmp)ASSERT_MSG("���[�u�R���X�g���N�^���Ăяo����܂������A���[�u���̏�����������Ă��܂���\n");
		Initialize(tmp.cpuHandle, tmp.gpuHandle);
	}
	DescHandles(const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHandle) { Initialize(CPUHandle, GPUHandle); }

	//������
	void Initialize(const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHandle)
	{
		cpuHandle = CPUHandle;
		gpuHandle = GPUHandle;
		invalid = false;
	}

	// if ���ɂ��̂܂܂����ď����ς݂�����o����悤��
	operator bool()const { return !invalid; }
	//������Z�q
	void operator=(const DescHandles& rhs)
	{
		if (rhs.invalid)ASSERT_MSG("������Z�q���Ăяo����܂������A�������f�B�X�N���v�^�n���h��������������Ă��܂���\n");
		cpuHandle = rhs.cpuHandle;
		gpuHandle = rhs.gpuHandle;
		invalid = false;
	}

	operator const D3D12_CPU_DESCRIPTOR_HANDLE& ()const
	{
		if (invalid)ASSERT_MSG("CPU�f�B�X�N���v�^�n���h�����擾���悤�Ƃ��܂������A����������Ă��܂���\n");
		return cpuHandle;
	}
	operator const D3D12_CPU_DESCRIPTOR_HANDLE* ()const
	{
		if (invalid)ASSERT_MSG("CPU�f�B�X�N���v�^�n���h���̃|�C���^���擾���悤�Ƃ��܂������A����������Ă��܂���\n");
		return &cpuHandle;
	}
	operator const D3D12_GPU_DESCRIPTOR_HANDLE& ()const
	{
		if (invalid)ASSERT_MSG("GPU�f�B�X�N���v�^�n���h�����擾���悤�Ƃ��܂������A����������Ă��܂���\n");
		return gpuHandle;
	}
	operator const D3D12_GPU_DESCRIPTOR_HANDLE* ()const
	{
		if (invalid)ASSERT_MSG("GPU�f�B�X�N���v�^�n���h���̃|�C���^���擾���悤�Ƃ��܂������A����������Ă��܂���\n");
		return &gpuHandle;
	}
};

class DescHandlesContainer
{
private:
	DescHandles handles[DESC_HANDLE_NUM];

public:
	void Initialize(const DESC_HANDLE_TYPE& Type, const DescHandles& Handle)
	{
		handles[Type] = Handle;
	}
	const DescHandles& GetHandle(const DESC_HANDLE_TYPE& Type) { return handles[Type]; }
};

//GPU���\�[�X�i�f�[�^�R���e�i�j
class GPUResource
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GPUResource() = delete;
	GPUResource(const GPUResource& tmp) = delete;
	GPUResource(GPUResource&& tmp) = delete;

	void* buffOnCPU = nullptr;//CPU������A�N�Z�X�ł��邷��o�b�t�@�̃A�h���X
	bool mapped = false;
	ComPtr<ID3D12Resource1>buff = nullptr;	//���\�[�X�o�b�t�@
	D3D12_RESOURCE_STATES barrier = D3D12_RESOURCE_STATE_COMMON;	//���\�[�X�o���A�̏��

public:
	GPUResource(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier,const wchar_t* Name = nullptr) 
	{
		buff = Buff;
		if (Name != nullptr)buff.Get()->SetName(Name);

		barrier = Barrier;
	}
	~GPUResource()
	{
		if (mapped)buff->Unmap(0, nullptr);
	}

	//�o�b�t�@���̃Z�b�^
	void SetName(const wchar_t* Name)
	{
		buff->SetName(Name);
	}

	//�o�b�t�@�擾
	const ComPtr<ID3D12Resource1>& GetBuff() { return buff; }
	//�}�b�s���O
	void Mapping(const size_t& DataSize, const int& ElementNum, const void* SendData);
	//���\�[�X�o���A�̕ύX
	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& NewBarrier);
	//�o�b�t�@�̃R�s�[�i�C���X�^���X�͕ʕ��A�����ɃR�s�[���j
	void CopyGPUResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& CopySource);

	template<typename T>
	const T* GetBuffOnCpu() { return (T*) buffOnCPU; }
};

//�����_�����O�̍ۂɃZ�b�g����o�b�t�@�i�f�B�X�N���v�^�o�^�̕K�v������^�C�v�̃f�[�^�j
class DescriptorData
{
	DescriptorData() = delete;
	DescriptorData(const DescriptorData& tmp) = delete;
	DescriptorData(DescriptorData&& tmp) = delete;
protected:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::shared_ptr<GPUResource> resource;	//�萔�o�b�t�@
	DescHandlesContainer handles;	//�f�B�X�N���v�^�n���h��

	DescriptorData(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier) :resource(std::make_shared<GPUResource>(Buff, Barrier)) {}
	DescriptorData(const std::shared_ptr<GPUResource>& GPUResource) :resource(GPUResource) {}	//�������̂�����

	//�o�b�t�@�Z�b�g�̃^�C�~���O�ŌĂ΂��֐��A���\�[�X�o���A��ς���Ȃ�
	virtual void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type) = 0;
public:
	//�o�b�t�@�Z�b�g
	void SetGraphicsDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam);
	void SetComputeDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam);
};

//�萔�o�b�t�@
class ConstantBuffer : public DescriptorData
{
	ConstantBuffer() = delete;
	ConstantBuffer(const ConstantBuffer& tmp) = delete;
	ConstantBuffer(ConstantBuffer&& tmp) = delete;
private:
	const size_t dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

public:
	ConstantBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& CBVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), dataSize(DataSize), elementNum(ElementNum) 
	{
		handles.Initialize(CBV, CBVHandles);
	}
	void Mapping(const void* SendData)
	{
		resource->Mapping(dataSize, elementNum, SendData);
	}
};

//�\�����o�b�t�@�i��������͒萔�o�b�t�@�Ɠ����j
class StructuredBuffer : public DescriptorData
{
	StructuredBuffer() = delete;
	StructuredBuffer(const StructuredBuffer& tmp) = delete;
	StructuredBuffer(StructuredBuffer&& tmp) = delete;

private:
	const size_t dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
public:
	StructuredBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& SRVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), dataSize(DataSize), elementNum(ElementNum) 
	{
		handles.Initialize(SRV, SRVHandles);
	}
	void Mapping(void* SendData)
	{
		resource->Mapping(dataSize, elementNum, SendData);
	}
};

//�o�͐�o�b�t�@
class RWStructuredBuffer : public DescriptorData
{
	RWStructuredBuffer() = delete;
	RWStructuredBuffer(const RWStructuredBuffer& tmp) = delete;
	RWStructuredBuffer(RWStructuredBuffer&& tmp) = delete;

private:
	const size_t dataSize;	//���[�U��`�f�[�^�̂P�v�f�T�C�Y
	const int elementNum = 0;	//�v�f��

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
public:
	RWStructuredBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& UAVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(Buff, Barrier), dataSize(DataSize), elementNum(ElementNum) 
	{
		handles.Initialize(UAV, UAVHandles);
	}
	RWStructuredBuffer(const std::shared_ptr<GPUResource>& GPUResource, const DescHandles& UAVHandles, const size_t& DataSize, const int& ElementNum)
		:DescriptorData(GPUResource), dataSize(DataSize), elementNum(ElementNum)
	{
		handles.Initialize(UAV, UAVHandles);
	}

	void CopyBuffOnGPU(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& Dest) { Dest.CopyGPUResource(CmdList, *this->resource); }

	std::weak_ptr<GPUResource>GetResource() { return resource; }
};

//�e�N�X�`�����\�[�X���N���X
class TextureBuffer : public DescriptorData
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	TextureBuffer() = delete;
	TextureBuffer(const TextureBuffer& tmp) = delete;
	TextureBuffer(TextureBuffer&& tmp) = delete;
protected:
	CD3DX12_RESOURCE_DESC texDesc;	//�e�N�X�`�����i���A�����Ȃǁj

	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

public:
	TextureBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& SRVHandles, const CD3DX12_RESOURCE_DESC& Desc)
		:DescriptorData(Buff, Barrier), texDesc(Desc) 
	{
		handles.Initialize(SRV, SRVHandles);
	}
	TextureBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const DescHandles& SRVHandles, const D3D12_RESOURCE_DESC& Desc)
		:DescriptorData(Buff, Barrier), texDesc(CD3DX12_RESOURCE_DESC(Desc)) 
	{
		handles.Initialize(SRV, SRVHandles);
	}

	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& Barrier) { resource->ChangeBarrier(CmdList, Barrier); }
	void CopyTexResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, TextureBuffer* CopySource);
	const CD3DX12_RESOURCE_DESC& GetDesc() { return texDesc; }
	void SetUAVHandles(const DescHandles& UAVHandles) { handles.Initialize(UAV, UAVHandles); }
	Vec2<int>GetGraphSize()
	{
		return Vec2<int>(static_cast<int>(texDesc.Width), static_cast<int>(texDesc.Height));
	}
};

//�����_�[�^�[�Q�b�g
class RenderTarget : public TextureBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	RenderTarget() = delete;
	RenderTarget(const RenderTarget& tmp) = delete;
	RenderTarget(RenderTarget&& tmp) = delete;
private:
	float clearValue[4] = { 0.0f };	//�N���A�l
	//�s�N�Z���V�F�[�_�[���\�[�X�Ƃ��Ďg����
	void OnSetDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type)override
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
public:
	RenderTarget(const ComPtr<ID3D12Resource1>& Buff,
		const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& SRVHandles,
		const DescHandles& RTVHandles,
		const CD3DX12_RESOURCE_DESC& Desc,
		const float* ClearValue = nullptr)
		:TextureBuffer(Buff, Barrier, SRVHandles, Desc)
	{
		handles.Initialize(RTV, RTVHandles);
		if (ClearValue != nullptr)
		{
			clearValue[0] = ClearValue[0];
			clearValue[1] = ClearValue[1];
			clearValue[2] = ClearValue[2];
			clearValue[3] = ClearValue[3];
		}
	}

	//�����_�[�^�[�Q�b�g���N���A
	void Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//���\�[�X�o���A�ύX
	void ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& Barrier) { resource->ChangeBarrier(CmdList, Barrier); }

	//�����_�[�^�[�Q�b�g�Ƃ��ăZ�b�g���鏀��
	const DescHandles& AsRTV(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		return handles.GetHandle(RTV);
	}
};

//�f�v�X�X�e���V��
class DepthStencil
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	DepthStencil() = delete;
	DepthStencil(const DepthStencil& tmp) = delete;
	DepthStencil(DepthStencil&& tmp) = delete;
private:
	const CD3DX12_RESOURCE_DESC desc;
	float clearValue = 1.0f;

	GPUResource resource;
	DescHandlesContainer handles;	//�f�B�X�N���v�^�n���h��

public:
	DepthStencil(const ComPtr<ID3D12Resource1>& Buff,
		const D3D12_RESOURCE_STATES& Barrier,
		const DescHandles& DSVHandles,
		const CD3DX12_RESOURCE_DESC& Desc,
		const float& ClearValue = 1.0f)
		:resource(Buff, Barrier), desc(Desc), clearValue(ClearValue)
	{
		handles.Initialize(DSV, DSVHandles);
	}

	//�f�v�X�X�e���V�����N���A
	void Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//DSV�擾
	const DescHandles& AsDSV(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		resource.ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		return handles.GetHandle(DSV);
	}
};

//���_�o�b�t�@
class VertexBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	VertexBuffer() = delete;
	VertexBuffer(const VertexBuffer& tmp) = delete;
	VertexBuffer(VertexBuffer&& tmp) = delete;

	//���_�o�b�t�@
	std::shared_ptr<GPUResource> resource;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//�ǂݎ���p
	std::shared_ptr<RWStructuredBuffer>rwBuff;
public:
	//���_�T�C�Y
	const size_t vertexSize;
	//�����������_��
	const unsigned int vertexNum;
	//���M���钸�_��
	unsigned int sendVertexNum;

	VertexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_VERTEX_BUFFER_VIEW& VBView)
		:vbView(VBView), vertexSize(VBView.StrideInBytes), vertexNum(VBView.SizeInBytes / VBView.StrideInBytes), sendVertexNum(vertexNum) 
	{
		resource = std::make_shared<GPUResource>(Buff, Barrier);
	}
	VertexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_VERTEX_BUFFER_VIEW& VBView, const DescHandles& UAVHandle)
		:vbView(VBView), vertexSize(VBView.StrideInBytes), vertexNum(VBView.SizeInBytes / VBView.StrideInBytes), sendVertexNum(vertexNum)
	{
		resource = std::make_shared<GPUResource>(Buff, Barrier);
		rwBuff = std::make_shared<RWStructuredBuffer>(resource, UAVHandle, vertexSize, sendVertexNum);
	}
	void Mapping(void* SendData)
	{
		resource->Mapping(vertexSize, sendVertexNum, SendData);
	}
	void SetName(const wchar_t* Name)
	{
		resource->SetName(Name);
	}
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }

	//�ǂݎ���p�\�����o�b�t�@�擾
	std::weak_ptr<RWStructuredBuffer>GetRWStructuredBuff()
	{
		if (!rwBuff)ASSERT_MSG("���_�o�b�t�@�̕`�����ݗp�\�����o�b�t�@�͖������ł�\n");
		return rwBuff;
	}
	//���_�o�b�t�@�Ƃ��Ďg�����߂Ƀ��\�[�X�o���A�ύX
	void ChangeBarrierForVertexBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
	{
		resource->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
};

//�C���f�b�N�X�o�b�t�@
class IndexBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IndexBuffer() = delete;
	IndexBuffer(const IndexBuffer& tmp) = delete;
	IndexBuffer(IndexBuffer&& tmp) = delete;

	//�C���f�b�N�X�o�b�t�@
	GPUResource resource;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};

public:
	//�C���f�b�N�X�T�C�Y
	const size_t indexSize;
	//�C���f�b�N�X��
	const unsigned int indexNum;

	IndexBuffer(const ComPtr<ID3D12Resource1>& Buff, const D3D12_RESOURCE_STATES& Barrier, const D3D12_INDEX_BUFFER_VIEW& IBView, const size_t& IndexSize)
		:resource(Buff, Barrier), ibView(IBView), indexSize(IndexSize), indexNum(IBView.SizeInBytes / IndexSize) {}
	void Mapping(void* SendData)
	{
		resource.Mapping(indexSize, indexNum, SendData);
	}
	void SetName(const wchar_t* Name)
	{
		resource.SetName(Name);
	}
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
};

//�V�F�[�_�[���
class Shaders
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	ComPtr<ID3DBlob>vs;	//���V�V�F�[�_�[
	ComPtr<ID3DBlob>ps;	//�s�N�Z���V�F�[�_�[
	ComPtr<ID3DBlob>ds;	//�h���C���V�F�[�_�[
	ComPtr<ID3DBlob>hs;	//�n���V�F�[�_�[
	ComPtr<ID3DBlob>gs;	//�W�I���g���V�F�[�_�[
};

//���_���C�A�E�g�p�����[�^
class InputLayoutParam
{
	InputLayoutParam() = delete;
public:
	const std::string semantics;
	const DXGI_FORMAT format;
	InputLayoutParam(const std::string& Semantics, const DXGI_FORMAT& Format) :semantics(Semantics), format(Format) {}
};


#define D3D12_DESCRIPTOR_RANGE_TYPE_NUM D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER+1
//���[�g�p�����[�^(SRV,UAV,CBV,SAMPLER ��GPU�f�[�^�w��j
class RootParam
{
private:
	RootParam() = delete;

public:
	std::string comment;		//���߁@�����Ă��Ȃ��Ă��ǂ�
	D3D12_DESCRIPTOR_RANGE_TYPE descriptorRangeType;
	DESC_HANDLE_TYPE viewType;
	bool descriptor = false;	//�f�B�X�N���v�^�Ƃ��ēo�^����Ă��邩
	int descNum = 1;

	RootParam(const D3D12_DESCRIPTOR_RANGE_TYPE& Range, const char* Comment = nullptr, const int& DescNum = 1)
		:descriptorRangeType(Range), descriptor(true), descNum(DescNum) {
		if (Comment != nullptr)comment = Comment;
	}
	RootParam(const DESC_HANDLE_TYPE& ViewType, const char* Comment = nullptr)
		:viewType(ViewType) {
		if (viewType == RTV || viewType == DSV)ASSERT_MSG("���[�g�p�����[�^�� RTV / DSV �͐ݒ�ł��܂���\n");
		if (Comment != nullptr)comment = Comment;
	}
};

//�T���v���[
class WrappedSampler
{
public:
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	WrappedSampler(const D3D12_TEXTURE_ADDRESS_MODE& TexAddressMode)
	{
		sampler.AddressU = TexAddressMode;
		sampler.AddressV = TexAddressMode;
		sampler.AddressW = TexAddressMode;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		//sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	//��Ԃ��Ȃ��i�j�A���X�g�l�C�o�[�j
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//���
		sampler.MaxLOD = D3D12_FLOAT32_MAX;	//�~�b�v�}�b�v�ő�l
		sampler.MinLOD = 0.0f;	//�~�b�v�}�b�v�ŏ��l
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler.ShaderRegister = 0;
	}
};

//�p�C�v���C���e��ݒ�
class PipelineInitializeOption
{
	PipelineInitializeOption() = delete;
public:
	bool calling = true;	//�J�����O
	bool wireFrame = false;	//���C���[�t���[��
	bool depthTest = true;	//�[�x�e�X�g
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;	//�f�v�X�X�e���V���̃t�H�[�}�b�g
	bool depthWriteMask = true;	//�f�v�X�̏������݁i�[�x�e�X�g���s���ꍇ�j
	bool independetBlendEnable = true;		//���������_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ��邩

	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;	//�}�`�̌`��ݒ�

	//�p�C�v���C���̖��O�ŋ�ʂ���̂ŁA�K����������
	PipelineInitializeOption(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& TopologyType, const D3D_PRIMITIVE_TOPOLOGY& Topology)
		:primitiveTopologyType(TopologyType), primitiveTopology(Topology) {}
};

//�A���t�@�u�����f�B���O���[�h
static const enum AlphaBlendMode
{
	AlphaBlendMode_None,	//�A���t�@�u�����f�B���O�Ȃ�(�㏑��)�B
	AlphaBlendMode_Trans,	//����������
	AlphaBlendMode_Add,		//���Z����
	AlphaBlendModeNum
};

//�������ݐ惌���_�[�^�[�Q�b�g���
class RenderTargetInfo
{
	RenderTargetInfo() = delete;
public:
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	AlphaBlendMode blendMode = AlphaBlendMode_None;
	RenderTargetInfo(const DXGI_FORMAT& Format, const AlphaBlendMode& BlendMode) :format(Format), blendMode(BlendMode) {}
};

//�O���t�B�b�N�X�p�C�v���C�����
class GraphicsPipeline
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	static int PIPELINE_NUM;	//�p�C�v���C�����������ꂽ���i�����ʔԍ��j

private:
	int	handle = -1;	//�p�C�v���C�����ʔԍ�

	ComPtr<ID3D12PipelineState>pipeline;			//�p�C�v���C��
	ComPtr<ID3D12RootSignature>rootSignature;	//���[�g�V�O�l�`��
	D3D_PRIMITIVE_TOPOLOGY topology;
public:
	GraphicsPipeline(const ComPtr<ID3D12PipelineState>& Pipeline, const ComPtr<ID3D12RootSignature>& RootSignature, const D3D_PRIMITIVE_TOPOLOGY& Topology)
		:pipeline(Pipeline), rootSignature(RootSignature), topology(Topology), handle(PIPELINE_NUM++) {}

	void SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	const int& GetPipelineHandle() { return handle; }
};

//�R���s���[�g�p�C�v���C��
class ComputePipeline
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	ComPtr<ID3D12PipelineState>pipeline;			//�p�C�v���C��
	ComPtr<ID3D12RootSignature>rootSignature;	//���[�g�V�O�l�`��
public:
	ComputePipeline(const ComPtr<ID3D12PipelineState>& Pipeline, const ComPtr<ID3D12RootSignature>& RootSignature)
		:pipeline(Pipeline), rootSignature(RootSignature) {}

	void SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList);
};