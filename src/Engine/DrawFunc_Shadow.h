#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

class LightManager;

static class DrawFunc_Shadow
{
	//_ΐWpΜθobt@
	static std::shared_ptr<ConstantBuffer>EYE_POS_BUFF;
	//Μx^hθeNX`
	static std::shared_ptr<TextureBuffer>DEFAULT_TEX;
	// (0,0,-1) Μx^hθm[}}bv
	static std::shared_ptr<TextureBuffer>DEFAULT_NORMAL_MAP;
	//Μx^hθeNX`
	static std::shared_ptr<TextureBuffer>DEFAULT_EMISSIVE_MAP;

	//DrawExtendGraph
	static int DRAW_EXTEND_GRAPH_COUNT;

	//DrawRotaGraph
	static int DRAW_ROTA_GRAPH_COUNT;

	static void StaticInit();
public:
	static void SetEyePos(Vec3<float> EyePos);

public:
	//ΔΡo΅JEgZbg
	static void CountReset()
	{
		DRAW_EXTEND_GRAPH_COUNT = 0;
		DRAW_ROTA_GRAPH_COUNT = 0;
	}

	/// <summary>
	/// gεk¬`ζ
	/// </summary>
	/// <param name="LeftUpPos">ι`ΜΆγΐW</param>
	/// <param name="RightBottomPos">ι`ΜEγΐW</param>
	/// <param name="Tex">eNX`</param>
	/// <param name="NormalMap">@ό}bv</param>
	/// <param name="EmissiveMap">ϊΛ}bv(υΉήΏ)</param>
	/// <param name="SpriteDepth">AeθΙp·ιZl</param>
	/// <param name="Miror">½]tO</param>
	/// <param name="Diffuse">DiffuseeΏx</param>
	/// <param name="Specular">SpeculareΏx</param>
	/// <param name="Lim">LimeΏx</param>
	static void DrawExtendGraph2D(LightManager& LigManager,
		const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap = nullptr,
		const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr, const float& SpriteDepth = 0.0f, 
		const Vec2<bool>& Miror = { false,false },
		const float& Diffuse = 1.0f, const float& Specular = 1.0f, const float& Lim = 1.0f);

	/// <summary>
	/// QDζρ]`ζ(Ae)
	/// </summary>
	/// <param name="LigManager">Cgξρ</param>
	/// <param name="Center">SΐW</param>
	/// <param name="ExtRate">gε¦</param>
	/// <param name="Radian">ρ]px</param>
	/// <param name="Tex">eNX`</param>
	/// <param name="NormalMap">@ό}bv</param>
	/// <param name="EmissiveMap">ϊΛ}bv(υΉήΏ)</param>
	/// <param name="SpriteDepth">AeθΙp·ιZl</param>
	/// <param name="RotaCenterUV">ρ]SUV( 0.0 ~ 1.0 )</param>
	/// <param name="Miror">½]tO</param>
	/// <param name="Diffuse">DiffuseeΏx</param>
	/// <param name="Specular">SpeculareΏx</param>
	/// <param name="Lim">LimeΏx</param>
	static void DrawRotaGraph2D(LightManager& LigManager,
		const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap = nullptr,
		const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr, const float& SpriteDepth = 0.0f,
		const Vec2<float>& RotaCenterUV = { 0.5f,0.5f }, const Vec2<bool>& Miror = { false,false },
		const float& Diffuse = 1.0f, const float& Specular = 1.0f, const float& Lim = 1.0f);
};