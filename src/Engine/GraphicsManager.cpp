#include "GraphicsManager.h"

void GraphicsManager::SetRenderTargetsCommand::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
	for (auto ptr : renderTargets)
	{
		rtvs.emplace_back(ptr.lock()->AsRTV(CmdList));
	}

	//�f�v�X�X�e���V��������ꍇ
	if (auto ptr = depthStencil.lock())
	{
		CmdList->OMSetRenderTargets(rtvs.size(), &rtvs[0], FALSE, ptr->AsDSV(CmdList));
	}
	//�Ȃ��ꍇ
	else
	{
		CmdList->OMSetRenderTargets(rtvs.size(), &rtvs[0], FALSE, nullptr);
	}
}

void GraphicsManager::ClearRTVCommand::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	renderTarget.lock()->Clear(CmdList);
}

void GraphicsManager::ClearDSVCommand::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	depthStencil.lock()->Clear(CmdList);
}

void GraphicsManager::RenderCommand::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�f�B�X�N���v�^�Z�b�g
	for (int i = 0; i < descDatas.size(); ++i)
	{
		descDatas[i].lock()->SetGraphicsDescriptorBuffer(CmdList, types[i], i);
	}

	//���_�r���[�Z�b�g
	CmdList->IASetVertexBuffers(0, 1, &vertexBuff.lock()->GetVBView());

	//�C���f�b�N�X�Ȃ�
	if (idxBuff.expired())
	{
		CmdList->DrawInstanced(vertexBuff.lock()->sendVertexNum, instanceNum, 0, 0);
	}
	//�C���f�b�N�X����
	else
	{
		CmdList->IASetIndexBuffer(&idxBuff.lock()->GetIBView());
		CmdList->DrawIndexedInstanced(idxBuff.lock()->indexNum, instanceNum, 0, 0, 0);
	}
}

void GraphicsManager::SetPostEffect::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	postEffect->Excute(CmdList, sourceTex.lock());
}

void GraphicsManager::SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget>>& RTs, const std::shared_ptr<DepthStencil>& DS)
{
	if (!renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	gCommands.emplace_back(std::make_shared<SetRenderTargetsCommand>(ConvertToWeakPtrArray(RTs), DS));
}

void GraphicsManager::SetPipeline(const std::shared_ptr<GraphicsPipeline>& Pipeline)
{
	//�V�����p�C�v���C���̃n���h���擾
	const int newPipelineHandle = Pipeline->GetPipelineHandle();

	//���ɃZ�b�g����Ă������̂Ɠ����Ȃ�X���[
	if (newPipelineHandle == recentPipelineHandle)return;

	if (!renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	gCommands.emplace_back(std::make_shared<SetPipelineCommand>(Pipeline));

	//�p�C�v���C���n���h���L�^
	recentPipelineHandle = newPipelineHandle;
}

void GraphicsManager::ClearRenderTarget(const std::shared_ptr<RenderTarget>& RenderTarget)
{
	if (!renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	gCommands.emplace_back(std::make_shared<ClearRTVCommand>(RenderTarget));
}

void GraphicsManager::ClearDepthStencil(const std::shared_ptr<DepthStencil>& DepthStencil)
{
	if (!renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	gCommands.emplace_back(std::make_shared<ClearDSVCommand>(DepthStencil));
}

void GraphicsManager::ExcutePostEffect(PostEffect* PostEffect, const std::shared_ptr<TextureBuffer>& SourceTex)
{
	recentPipelineHandle = -1;

	if (!renderCommands.empty())StackRenderCommands();	//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
	gCommands.emplace_back(std::make_shared<SetPostEffect>(PostEffect, SourceTex));
}

void GraphicsManager::ObjectRender(const std::shared_ptr<VertexBuffer>& VertexBuff, const std::vector<std::shared_ptr<DescriptorData>>& DescDatas,
	const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
	const float& Depth, const bool& TransFlg, const int& InstanceNum)
{
	//�\�[�g����̂� gCommands �ł͂Ȃ��ꎞ�I��renderCommands�ɐς�
	renderCommands.emplace_back(std::make_shared<RenderCommand>(VertexBuff, ConvertToWeakPtrArray(DescDatas), DescHandleTypes, Depth, TransFlg, InstanceNum));
}

void GraphicsManager::ObjectRender(const std::shared_ptr<VertexBuffer>& VertexBuff,
	const std::shared_ptr<IndexBuffer>& IndexBuff,
	const std::vector<std::shared_ptr<DescriptorData>>& DescDatas,
	const std::vector<DESC_HANDLE_TYPE>& DescHandleTypes,
	const float& Depth, const bool& TransFlg, const int& InstanceNum)
{
	//�\�[�g����̂� gCommands �ł͂Ȃ��ꎞ�I��renderCommands�ɐς�
	renderCommands.emplace_back(std::make_shared<RenderCommand>(VertexBuff, IndexBuff, ConvertToWeakPtrArray(DescDatas), DescHandleTypes, Depth, TransFlg, InstanceNum));
}


void GraphicsManager::StackRenderCommands()
{
	//�\�[�g
	//Z�l��蓧�߂��邩�ǂ������D��x����
	renderCommands.sort([](std::shared_ptr<RenderCommand> a, std::shared_ptr<RenderCommand> b)
		{
			if (a->trans == b->trans)
			{
				//Z�\�[�g�i�Y�������� = ���j
				return b->depth < a->depth;
			}
			else
			{
				return !a->trans && b->trans;
			}
		});

	//�ꊇ�X�^�b�N
	for (auto ptr : renderCommands)
	{
		//�L���X�g���ăO���t�B�b�N�R�}���h�ɐς�
		gCommands.emplace_back(std::static_pointer_cast<GraphicsCommandBase>(ptr));
	}

	//�����_�����O�R�}���h�N���A
	renderCommands.clear();
}

void GraphicsManager::CommandsExcute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�Ō�ɐςݏグ��ꂽ�̂������_�����O�R�}���h�������ꍇ
	if (!renderCommands.empty())
	{
		//Z�o�b�t�@�����߂��邩�ǂ����Ń\�[�g���ăO���t�B�b�N�X�R�}���h���X�g�Ɉꊇ�X�^�b�N
		StackRenderCommands();
	}

	for (auto itr = gCommands.begin(); itr != gCommands.end(); ++itr)
	{
		(*itr)->Excute(CmdList);
	}

	//�R�}���h���X�g�N���A
	gCommands.clear();
	recentPipelineHandle = -1;
}