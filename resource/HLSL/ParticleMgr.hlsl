struct Vertex
{
    float2 pos;
    float2 forwardVec;
    float2 movedVel;
    float alpha;
    float speed;
    min16int isAlive;
    float scale;
    uint texIdx;
    float4 slow;
};

RWStructuredBuffer<Vertex> vertices : register(u0);

static const float SCALE = 6.0f;   //CPU側と合わせる必要がある
//static const float SUB_SPEED = 0.3f;
static const float SUB_SPEED = 0.1f;

[numthreads(10, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID)
{
    int i = DTid.x;
    
    Vertex v = vertices[i];
    
    if(!v.isAlive)
    {
        return;
    }
    
	// 移動させる。
    v.movedVel += float2(v.forwardVec.x * v.speed * v.slow.x, v.forwardVec.y * v.speed * v.slow.x);

	// 移動した量がサイズを上回ったらサイズを動かす。
    if (abs(v.movedVel.x) >= v.scale)
    {

		// 符号を取得。
        float signX = sign(v.movedVel.x);

		// 座標を動かす。
        v.pos.x += v.scale * signX;

		// 移動した量を減らす。
        v.movedVel.x -= v.scale * signX;

    }
	// 移動した量がサイズを上回ったらサイズを動かす。Yバージョン
    if (abs(v.movedVel.y) >= v.scale)
    {

		// 符号を取得。
        float signY = sign(v.movedVel.y);

		// 座標を動かす。
        v.pos.y += v.scale * signY;

		// 移動した量を減らす。
        v.movedVel.y -= SCALE * signY;

    }

	// スピードを下げる。
    if (v.speed > 0)
        v.speed -= SUB_SPEED;

	// スピードが規定値以下になったらアルファ値を下げ始める。
    if (v.speed <= 0.5f)
    {
        v.alpha -= v.alpha / 3.0f;
    }

	// アルファ値が一定以下になったら初期化する。
    if (v.alpha <= 10.0f)
        v.isAlive = 0;

    if (v.alpha <= 123)
        v.scale = SCALE / 2.0f;
    
    vertices[i] = v;
}

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

cbuffer cbuff1 : register(b1)
{
    float zoom;
    float2 scroll;
};

struct VSInput
{
    float4 pos : POSITION;
    float2 forwardVec : FORWARD_VEC;
    float2 movedVel : MOVED_VEL;
    float alpha : ALPHA;
    float speed : SPEED;
    min16int isAlive : ALIVE;
    float scale : SCALE;
    uint texIdx : TEX_IDX;
};

struct VSOutput
{
    float4 pos : POSITION;
    float alpha : ALPHA;
    min16int isAlive : ALIVE;
    float drawScale : DRAW_SCALE;
    uint texIdx : TEX_IDX;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output;
    output.pos = input.pos;
    output.pos *= zoom;
    output.pos.xy -= scroll;
    output.alpha = input.alpha / 255.0f;
    output.isAlive = input.isAlive;
    output.drawScale = input.scale / 2.0f * zoom;
    output.texIdx = input.texIdx;
    return output;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD_POSITION;
    float2 uv : TEXCOORD;
    float alpha : ALPHA;
    uint texIdx : TEX_IDX;
};

[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    if (!input[0].isAlive)
        return;
    
    GSOutput element;
    element.alpha = input[0].alpha;
    element.texIdx = input[0].texIdx;
        
    //左下
    element.pos = input[0].pos;
    element.pos.x -= input[0].drawScale;
    element.pos.y += input[0].drawScale;
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //左上
    element.pos = input[0].pos;
    element.pos.x -= input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //右下
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y += input[0].drawScale;
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //右上
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

#include"Engine/LightInfo.hlsli"
cbuffer cbuff2 : register(b2)
{
    LightInfo ligNum; //アクティブ中のライトの数の情報
}

StructuredBuffer<DirectionLight> dirLight : register(t0);
StructuredBuffer<PointLight> pointLight : register(t1);
StructuredBuffer<SpotLight> spotLight : register(t2);
StructuredBuffer<HemiSphereLight> hemiSphereLight : register(t3);

Texture2D<float4> tex0 : register(t4);
Texture2D<float4> tex1 : register(t5);
Texture2D<float4> tex2 : register(t6);
Texture2D<float4> tex3 : register(t7);
Texture2D<float4> tex4 : register(t8);
Texture2D<float4> tex5 : register(t9);
Texture2D<float4> tex6 : register(t10);
Texture2D<float4> tex7 : register(t11);
Texture2D<float4> tex8 : register(t12);
SamplerState smp : register(s0);

struct PSOutput
{
    float4 color : SV_Target0;
    float4 emissive : SV_Target1;
};

static const float3 EYE_POS = float3(640, 360, -5.0f);
static const float DEPTH = 1.0f;
static const float DIFFUSE = 1.0f;
static const float SPECULAR = 1.0f;
static const float LIM = 1.0f;

PSOutput PSmain(GSOutput input) : SV_TARGET
{
    //ポジション
    float3 pos = float3(input.worldPos.xy, DEPTH);
    float3 normal = float3(0, 0, -1.0f);
    float3 vnormal = mul(parallelProjMat, float4(normal, 1.0f)).xyz;
    
      //ライトの影響
    //float3 ligEffect = { 0.3f, 0.3f, 0.3f };
    float3 ligEffect = { 0.0f, 0.0f, 0.0f };
    
    //ディレクションライト
    for (int i = 0; i < ligNum.dirLigNum; ++i)
    {
        float3 dir = dirLight[i].direction;
        float3 ligCol = dirLight[i].color.xyz * dirLight[i].color.w;
        ligEffect += CalcLambertDiffuse(dir, ligCol, normal) * DIFFUSE;
        ligEffect += CalcPhongSpecular(dir, ligCol, normal, pos, EYE_POS) * SPECULAR;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * LIM;
    }
    //ポイントライト
    for (int i = 0; i < ligNum.ptLigNum; ++i)
    {
        float3 dir = pos - pointLight[i].pos;
        dir = normalize(dir);
        float3 ligCol = pointLight[i].color.xyz * pointLight[i].color.w;
        
        //減衰なし状態
        float3 diffPoint = CalcLambertDiffuse(dir, ligCol, normal);
        float3 specPoint = CalcPhongSpecular(dir, ligCol, normal, pos, EYE_POS);
        
        //距離による減衰
        float3 distance = length(pos - pointLight[i].pos);
		//影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / pointLight[i].influenceRange * distance;
		//影響力がマイナスにならないように補正をかける
        if (affect < 0.0f)
            affect = 0.0f;
		//影響を指数関数的にする
        affect = pow(affect, 3.0f);
        diffPoint *= affect;
        specPoint *= affect;
        
        ligEffect += diffPoint * DIFFUSE;
        ligEffect += specPoint * SPECULAR;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * LIM;
    }
    //スポットライト
    for (int i = 0; i < ligNum.spotLigNum; ++i)
    {
        float3 ligDir = pos - spotLight[i].pos;
        ligDir = normalize(ligDir);
        float3 ligCol = spotLight[i].color.xyz * spotLight[i].color.w;
        
        //減衰なし状態
        float3 diffSpotLight = CalcLambertDiffuse(ligDir, ligCol, normal);
        float3 specSpotLight = CalcPhongSpecular(ligDir, ligCol, normal, pos, EYE_POS);
        
        //スポットライトとの距離を計算
        float3 distance = length(pos - spotLight[i].pos);
       	//影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / spotLight[i].influenceRange * distance;
        //影響力がマイナスにならないように補正をかける
        if (affect < 0.0f)
            affect = 0.0f;
    //影響を指数関数的にする
        affect = pow(affect, 3.0f);
        diffSpotLight *= affect;
        specSpotLight *= affect;
    
        float3 spotLIM = CalcLimLight(ligDir, ligCol, normal, vnormal) * affect;
        
        float3 dir = normalize(spotLight[i].target - spotLight[i].pos);
        float angle = dot(ligDir, dir);
        angle = abs(acos(angle));
        affect = 1.0f - 1.0f / spotLight[i].angle * angle;
        if (affect < 0.0f)
            affect = 0.0f;
        affect = pow(affect, 0.5f);
        
        ligEffect += diffSpotLight * affect * DIFFUSE;
        ligEffect += specSpotLight * affect * SPECULAR;
        ligEffect += spotLIM * affect * LIM;
    }
    //天球
    for (int i = 0; i < ligNum.hemiSphereNum; ++i)
    {
        float t = dot(normal.xyz, hemiSphereLight[i].groundNormal);
        t = (t + 1.0f) / 2.0f;
        float3 hemiLight = lerp(hemiSphereLight[i].groundColor, hemiSphereLight[i].skyColor, t);
        ligEffect += hemiLight;
    }
    
    float4 result;
    if (input.texIdx == 0)
        result = tex0.Sample(smp, input.uv);
    if (input.texIdx == 1)
        result = tex1.Sample(smp, input.uv);
    if (input.texIdx == 2)
        result = tex2.Sample(smp, input.uv);
    if (input.texIdx == 3)
        result = tex3.Sample(smp, input.uv);
    if (input.texIdx == 4)
        result = tex4.Sample(smp, input.uv);
    if (input.texIdx == 5)
        result = tex5.Sample(smp, input.uv);
    if (input.texIdx == 6)
        result = tex6.Sample(smp, input.uv);
    if (input.texIdx == 7)
        result = tex7.Sample(smp, input.uv);
    if (input.texIdx == 8)
        result = tex8.Sample(smp, input.uv);
    
    result.xyz *= ligEffect;
    result.w *= input.alpha;
    
    PSOutput output;
    output.color = result;
    output.emissive = output.color;
    
    //明るさ計算
    //float bright = dot(result.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    //if (1.0f < bright)
    //    output.emissive += result;
    //output.emissive.w = result.w;
    
    return output;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}