#include "GaussianBlur.h"
#include"D3D12App.h"

GaussianBlur::GaussianBlur(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& BlurPower)
{
    //�d�݃e�[�u���萔�o�b�t�@
    weightConstBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), NUM_WEIGHTS, nullptr, "GaussianBlur - weight");
    SetBlurPower(BlurPower);

    //�p�C�v���C��
    {
        std::vector<RootParam>rootParam =
        {
            RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�d�݃e�[�u��"),
            RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�e�N�X�`�����"),
            RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�\�[�X�摜�o�b�t�@"),
            RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�`�����ݐ�o�b�t�@")
        };

        auto cs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/GaussianBlur.hlsl", "XBlur", "cs_5_0");
        xBlurPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParam, WrappedSampler(false, true));
        cs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/GaussianBlur.hlsl", "YBlur", "cs_5_0");
        yBlurPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParam, WrappedSampler(false, true));
        cs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/GaussianBlur.hlsl", "Final", "cs_5_0");
        finalPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParam, WrappedSampler(false, true));
    }

    //�e�N�X�`�����
    struct TexInfo
    {
        Vec2<int>sourceTexSize;
        Vec2<int>xBlurTexSize;
        Vec2<int>yBlurTexSize;
        Vec2<int>pad;
    }texInfo;
    texInfo.sourceTexSize = Size;
    texInfo.xBlurTexSize = { Size.x / 2,Size.y };
    texInfo.yBlurTexSize = { Size.x / 2,Size.y / 2 };
    texInfoConstBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(TexInfo), 1, &texInfo, "GaussianBlur - TexInfo");

    //�c���u���[�̌��ʕ`���
    xBlurResult = D3D12App::Instance()->GenerateTextureBuffer(texInfo.xBlurTexSize, Format, "HorizontalBlur");
    yBlurResult = D3D12App::Instance()->GenerateTextureBuffer(texInfo.yBlurTexSize, Format, "VerticalBlur");

    //�ŏI�������ʕ`���
    finalResult = D3D12App::Instance()->GenerateTextureBuffer(texInfo.sourceTexSize, Format, "GaussianBlur");
}

void GaussianBlur::SetBlurPower(const float& BlurPower)
{
    // �d�݂̍��v���L�^����ϐ����`����
    float total = 0;

    // ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���
    // ���[�v�ϐ���x����e�N�Z������̋���
    for (int x = 0; x < NUM_WEIGHTS; x++)
    {
        weights[x] = expf(-0.5f * (float)(x * x) / BlurPower);
        total += 2.0f * weights[x];
    }

    // �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���
    for (int i = 0; i < NUM_WEIGHTS; i++)
    {
        weights[i] /= total;
    }

    weightConstBuff->Mapping(&weights[0]);
}

void GaussianBlur::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const std::shared_ptr<TextureBuffer>& SourceTex)
{
    const auto& sDesc = SourceTex->GetDesc();
    const auto& fDesc = finalResult->GetDesc();
    if (sDesc.Width != fDesc.Width || sDesc.Height != fDesc.Height || sDesc.Format != fDesc.Format)
    {
        ASSERT_MSG("�K�E�V�A���u���[�����s���悤�Ƃ��܂������A�\�[�X�ƂȂ�e�N�X�`���ݒ�ƃK�E�V�A���u���[�ݒ肪�����܂���\n");
    }

    static const int DIV = 4;

    //X�u���[
    xBlurPipeline->SetPipeline(CmdList);
    weightConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 0);
    texInfoConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 1);
    SourceTex->SetComputeDescriptorBuffer(CmdList, SRV, 2);
    xBlurResult->SetComputeDescriptorBuffer(CmdList, UAV, 3);
    CmdList->Dispatch(xBlurResult->GetDesc().Width / DIV, xBlurResult->GetDesc().Height / DIV, 1);

    //Y�u���[
    yBlurPipeline->SetPipeline(CmdList);
    weightConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 0);
    texInfoConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 1);
    xBlurResult->SetComputeDescriptorBuffer(CmdList, SRV, 2);
    yBlurResult->SetComputeDescriptorBuffer(CmdList, UAV, 3);
    CmdList->Dispatch(yBlurResult->GetDesc().Width / DIV, yBlurResult->GetDesc().Height / DIV, 1);

    //�ŏI���ʍ���
    finalPipeline->SetPipeline(CmdList);
    weightConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 0);
    texInfoConstBuff->SetComputeDescriptorBuffer(CmdList, CBV, 1);
    yBlurResult->SetComputeDescriptorBuffer(CmdList, SRV, 2);
    finalResult->SetComputeDescriptorBuffer(CmdList, UAV, 3);
    CmdList->Dispatch(finalResult->GetDesc().Width / DIV, finalResult->GetDesc().Height / DIV, 1);
}

#include"KuroEngine.h"
void GaussianBlur::DrawResult(const AlphaBlendMode& AlphaBlend)
{
    KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
    PostEffect::GetWinSizeSprite()->SetTexture(finalResult);
    PostEffect::GetWinSizeSprite()->Draw(AlphaBlend);
}
