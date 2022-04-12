#pragma once
#include"D3D12Data.h"
#include"PostEffect.h"

//�O���t�B�b�N�X�}�l�[�W��
class GraphicsManager
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region ��p�̃O���t�B�b�N�X�R�}���h
	//�O���t�B�b�N�X�R�}���h���N���X
	class GraphicsCommandBase
	{
	public:
		virtual void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList) = 0;
	};

	//�����_�[�^�[�Q�b�g�Z�b�g�R�}���h
	class SetRenderTargetsCommand : public GraphicsCommandBase
	{
		SetRenderTargetsCommand() = delete;

		const std::vector<std::weak_ptr<RenderTarget>>renderTargets;
		const std::weak_ptr<DepthStencil>depthStencil;
	public:
		SetRenderTargetsCommand(const std::vector<std::weak_ptr<RenderTarget>>& RTs, const std::weak_ptr<DepthStencil>& DS) :renderTargets(RTs), depthStencil(DS) {}
		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�p�C�v���C���Z�b�g�R�}���h
	class SetPipelineCommand : public GraphicsCommandBase
	{
		SetPipelineCommand() = delete;

		std::weak_ptr<GraphicsPipeline> pipeline;
	public:
		SetPipelineCommand(std::weak_ptr<GraphicsPipeline> Pipeline) :pipeline(Pipeline) {}
		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override
		{
			pipeline.lock()->SetPipeline(CmdList);
		}
		const int& GetPipelineHandle() { return pipeline.lock()->GetPipelineHandle(); }
	};

	//�����_�[�^�[�Q�b�g�N���A�R�}���h
	class ClearRTVCommand : public GraphicsCommandBase
	{
		ClearRTVCommand() = delete;
		std::weak_ptr<RenderTarget>renderTarget;

	public:
		ClearRTVCommand(const std::weak_ptr<RenderTarget>& RenderTarget) :renderTarget(RenderTarget) {}
		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�f�v�X�X�e���V���N���A�R�}���h
	class ClearDSVCommand : public GraphicsCommandBase
	{
		ClearDSVCommand() = delete;
		std::weak_ptr<DepthStencil>depthStencil;
	public:
		ClearDSVCommand(const std::weak_ptr<DepthStencil>& DepthStencil) :depthStencil(DepthStencil) {}
		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�����_�����O���i�����_�[�R�}���h�ɓn�����j
	//�I�u�W�F�N�g�̃����_�����O�R�}���h
	class RenderCommand : public GraphicsCommandBase
	{
		RenderCommand() = delete;

		std::weak_ptr<VertexBuffer>vertexBuff;
		std::weak_ptr<IndexBuffer>idxBuff;
		const std::vector<std::weak_ptr<DescriptorData>> descDatas;	//�f�B�X�N���v�^�iCBV,SRV�Ȃǁj
		const std::vector<DESC_HANDLE_TYPE> types;	//�Z�b�g����f�B�X�N���v�^�^�C�v
		const int instanceNum = 1;	//�C���X�^���X���i�C���X�^���V���O�`��p�j

	public:
		const float depth = 0.0f;	//Z�\�[�g�p
		const bool trans = false;	//���߃I�u�W�F�N�g

		//�C���f�b�N�X�Ȃ�
		RenderCommand(const std::weak_ptr<VertexBuffer>& VertexBuff,
			const std::vector<std::weak_ptr<DescriptorData>>& DescDatas,
			const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
			const float& Depth,
			const bool& TransFlg,
			const int& InstanceNum = 1)
			:vertexBuff(VertexBuff), descDatas(DescDatas), types(DescHandleTypes), depth(Depth), trans(TransFlg), instanceNum(InstanceNum) {}

		//�C���f�b�N�X����
		RenderCommand(const std::weak_ptr<VertexBuffer>& VertexBuff,
			const std::weak_ptr<IndexBuffer>& IndexBuff,
			const std::vector<std::weak_ptr<DescriptorData>>& DescDatas,
			const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
			const float& Depth,
			const bool& TransFlg,
			const int& InstanceNum = 1)
			:vertexBuff(VertexBuff), idxBuff(IndexBuff), descDatas(DescDatas), types(DescHandleTypes), depth(Depth), trans(TransFlg), instanceNum(InstanceNum) {}

		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�|�X�g�G�t�F�N�g
	class SetPostEffect :public GraphicsCommandBase
	{
		SetPostEffect() = delete;

		PostEffect* postEffect;
		std::weak_ptr<TextureBuffer>sourceTex;
	public:
		SetPostEffect(PostEffect* PostEffect, const std::weak_ptr<TextureBuffer>& SourceTex)
			:postEffect(PostEffect), sourceTex(SourceTex) {}

		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�e�N�X�`���̃R�s�[
	class CopyTex : public GraphicsCommandBase
	{
		std::weak_ptr<TextureBuffer>destTex;
		std::weak_ptr<TextureBuffer>srcTex;
	public:
		CopyTex(const std::weak_ptr<TextureBuffer>& DestTex, const std::weak_ptr<TextureBuffer>& SrcTex)
			:destTex(DestTex), srcTex(SrcTex) {}

		void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

#pragma endregion

private:
	//�O���t�B�b�N�X�R�}���h���X�g
	std::list<std::shared_ptr<GraphicsCommandBase>>gCommands;

	//�����_�����O�R�}���h���X�g�i�\�[�g�̂��߈ꎞ�O���t�B�b�N�X�R�}���h���X�g�Ƃ͕ʂŐςݏグ��j
	std::list<std::shared_ptr<RenderCommand>>renderCommands;

	//�Ō�ɃZ�b�g���ꂽ�p�C�v���C���n���h��
	int recentPipelineHandle = -1;

	//Z�o�b�t�@�A���߂��邩�ǂ����Ń\�[�g
	void StackRenderCommands();

	//shader_ptr�z���weak_ptr�z��ɕϊ�
	template<typename T>
	std::vector<std::weak_ptr<T>>ConvertToWeakPtrArray(const std::vector<std::shared_ptr<T>>& Array)
	{
		std::vector<std::weak_ptr<T>>result;
		for (int i = 0; i < Array.size(); ++i)
		{
			result.emplace_back(std::weak_ptr<T>(Array[i]));
		}
		return result;
	}

public:
	//�����_�[�^�[�Q�b�g�̃Z�b�g�R�}���h�ςݏグ
	void SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget>>& RTs, const std::shared_ptr<DepthStencil>& DS = std::shared_ptr<DepthStencil>());

	//�p�C�v���C���̃Z�b�g�R�}���h�ςݏグ
	void SetPipeline(const std::shared_ptr<GraphicsPipeline>& Pipeline);

	//�����_�[�^�[�Q�b�g�̃N���A�R�}���h�ςݏグ
	void ClearRenderTarget(const std::shared_ptr<RenderTarget>& RenderTarget);

	//�f�v�X�X�e���V���̃N���A�R�}���h�ςݏグ
	void ClearDepthStencil(const std::shared_ptr<DepthStencil>& DepthStencil);

	//�|�X�g�G�t�F�N�g�R�}���h�ςݏグ
	void ExcutePostEffect(PostEffect* PostEffect, const std::shared_ptr<TextureBuffer>& SourceTex);

	//�e�N�X�`���R�s�[�R�}���h�ςݏグ
	void CopyTexture(const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex);

	//�I�u�W�F�N�g�̃����_�����O�R�}���h�ςݏグ�i�C���f�b�N�X�Ȃ��j
	void ObjectRender(const std::shared_ptr<VertexBuffer>& VertexBuff,
		const std::vector<std::shared_ptr<DescriptorData>>& DescDatas,
		const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
		const float& Depth, const bool& TransFlg, const int& InstanceNum = 1);

	//�I�u�W�F�N�g�̃����_�����O�R�}���h�ςݏグ�i�C���f�b�N�X�Ȃ��j
	void ObjectRender(const std::shared_ptr<VertexBuffer>& VertexBuff, const std::shared_ptr<IndexBuffer>& IndexBuff,
		const std::vector<std::shared_ptr<DescriptorData>>& DescDatas,
		const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
		const float& Depth, const bool& TransFlg, const int& InstanceNum = 1);

	//�R�}���h���X�g�S���s
	void CommandsExcute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CmdList);
};