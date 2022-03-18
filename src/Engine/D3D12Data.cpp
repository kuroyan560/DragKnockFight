#include"D3D12Data.h""

void GPUResource::MapBuffOnCPU()
{
	//�܂��}�b�s���O���Ă��Ȃ���΃}�b�s���O
	if (!mapped)
	{
		//�}�b�v�A�A���}�b�v�̃I�[�o�[�w�b�h���y�����邽�߂ɂ͂��̃C���X�^���X�������Ă���Ԃ�Unmap���Ȃ�
		auto hr = buff->Map(0, nullptr, (void**)&buffOnCPU);
		if (FAILED(hr))ASSERT_MSG("�f�[�^�̃}�b�s���O�Ɏ��s\n");
		mapped = true;
	}
}

void GPUResource::Mapping(const size_t& DataSize, const int& ElementNum, void* SendData)
{
	//����f�[�^��null�Ȃ牽�����Ȃ�
	if (SendData == nullptr)ASSERT_MSG("�f�[�^�̃}�b�s���O�Ɏ��s�A������nullptr�ł�\n");

	MapBuffOnCPU();

	memcpy(buffOnCPU, SendData, DataSize * ElementNum);
}

void GPUResource::ChangeBarrier(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const D3D12_RESOURCE_STATES& NewBarrier)
{
    //���\�[�X�o���A�ω��Ȃ�
    if (barrier == NewBarrier)return;

    //���\�[�X�o���A�ύX
    CmdList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            buff.Get(),
            barrier,
            NewBarrier));

    //���\�[�X�o���A��ԋL�^
    barrier = NewBarrier;
}

void GPUResource::CopyGPUResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, GPUResource& CopySource)
{
	//�R�s�[���̃��\�[�X�̏�Ԃ��L�^
	auto oldBarrierCopySource = CopySource.barrier;
	//�R�s�[���̃��\�[�X�o���A���R�s�[���p�ɕύX
	CopySource.ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//�R�s�[��ł��鎩�g�̃��\�[�X�̏�Ԃ��L�^
	auto oldBarrierThis = barrier;
	//�R�s�[��ł��鎩�g�̃��\�[�X�o���A���R�s�[��p�ɕύX
	this->ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_COPY_DEST);

	//�R�s�[���s
	CmdList->CopyResource(this->buff.Get(), CopySource.buff.Get());

	//�R�s�[�������̃��\�[�X�o���A�ɖ߂�
	CopySource.ChangeBarrier(CmdList, oldBarrierCopySource);

	//�R�s�[��ł��鎩�g�����̃��\�[�X�o���A�ɖ߂�
	this->ChangeBarrier(CmdList, oldBarrierThis);
}

void DescriptorData::SetGraphicsDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam)
{
	OnSetDescriptorBuffer(CmdList,Type);
	CmdList->SetGraphicsRootDescriptorTable(RootParam, handles.GetHandle(Type));
}

void DescriptorData::SetComputeDescriptorBuffer(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const DESC_HANDLE_TYPE& Type, const int& RootParam)
{
	OnSetDescriptorBuffer(CmdList,Type);
	CmdList->SetComputeRootDescriptorTable(RootParam, handles.GetHandle(Type));
}

void TextureBuffer::CopyTexResource(const ComPtr<ID3D12GraphicsCommandList>& CmdList, TextureBuffer* CopySource)
{
	if (texDesc != CopySource->texDesc)
	{
		ASSERT_MSG("�V�F�[�_�[���\�[�X���R�s�[���悤�Ƃ��܂������A�t�H�[�}�b�g���Ⴂ�܂�\n");
	}
	resource.CopyGPUResource(CmdList, CopySource->resource);
}

void RenderTarget::Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	resource.ChangeBarrier(CmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CmdList->ClearRenderTargetView(
		handles.GetHandle(RTV),
		&clearValue[0],
		0,
		nullptr);
}

void DepthStencil::Clear(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	CmdList->ClearDepthStencilView(
		handles.GetHandle(DSV),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		clearValue,
		0,
		0,
		nullptr);
}

int GraphicsPipeline::PIPELINE_NUM = 0;

void GraphicsPipeline::SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	CmdList->SetPipelineState(pipeline.Get());
	//���[�g�V�O�l�`���̐ݒ�
	CmdList->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	CmdList->IASetPrimitiveTopology(topology);
}

void ComputePipeline::SetPipeline(const ComPtr<ID3D12GraphicsCommandList>& CmdList)
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	CmdList->SetPipelineState(pipeline.Get());
	//���[�g�V�O�l�`���̐ݒ�
	CmdList->SetComputeRootSignature(rootSignature.Get());
}

