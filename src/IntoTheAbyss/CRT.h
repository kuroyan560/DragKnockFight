#pragma once
#include"PostEffect.h"
class GraphicsPipeline;
class RenderTarget;
class ConstantBuffer;
class SpriteMesh;

class CRT : public PostEffect
{
	struct Info
	{
		float noiseX = 0.0f;
		float rgbNoise = 0.0f;
		//float sinNoiseScale = 1.0f;
		float sinNoiseScale = 0.0f;
		//float sinNoiseWidth = 1.0f;
		float sinNoiseWidth = 0.0f;

		//float sinNoiseOffset = 1.0f;
		float sinNoiseOffset = 0.0f;
		float scanLineSpeed = 100.0f;
		float scanLineTail = 0.5f;
		float _time = 0;

		Vec2<float>offset = { 0,0 };
		float _screenParamX;
		float pad;
		//float barrelPower = 1;
	}crtInfo;

	std::shared_ptr<SpriteMesh>mesh;
	std::shared_ptr<ConstantBuffer>crtInfoBuff;
	std::shared_ptr<RenderTarget>result;

public:
	CRT();
	//実行
	void Excute(const ComPtr<ID3D12GraphicsCommandList>& CmdList, const std::shared_ptr<TextureBuffer>& SourceTex)override;
	//結果のテクスチャ取得
	std::shared_ptr<RenderTarget>& GetResult() { return result; }
	//ウィンドウサイズで結果を描画
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};