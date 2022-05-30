#include "CRT.h"
#include"KuroEngine.h"
#include"SpriteMesh.h"

CRT::CRT()
{
	result = D3D12App::Instance()->GenerateRenderTarget(D3D12App::Instance()->GetBackBuffFormat(),
		Color(0, 0, 0, 0), D3D12App::Instance()->GetBackBuffRenderTarget()->GetGraphSize(), L"CRT_Result");
	crtInfoBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Info), 1, &crtInfo, "CRT - Info");
	mesh = std::make_shared<SpriteMesh>("CRT - Mesh");
	mesh->SetSize(WinApp::Instance()->GetExpandWinSize());
}

void CRT::Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const std::shared_ptr<TextureBuffer>& SourceTex)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	//�p�C�v���C��������
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;
		PIPELINE_OPTION.depthWriteMask = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/CRT.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/CRT.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�\�[�X�摜�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"CRT���")
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None) };
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
	}

	KuroEngine::Instance().Graphics().SetRenderTargets({ result });
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE);
	mesh->Render({ KuroEngine::Instance().GetParallelMatProjBuff(),SourceTex,crtInfoBuff }, { CBV,SRV,CBV });
}

void CRT::DrawResult(const AlphaBlendMode& AlphaBlend)
{
	PostEffect::GetWinSizeSprite()->SetTexture(result);
	PostEffect::GetWinSizeSprite()->Draw(AlphaBlend);
}
