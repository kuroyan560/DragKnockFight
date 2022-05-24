#include"Engine/Math.hlsli"

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

cbuffer cbuff1 : register(b1)
{
    float extRate;
};

Texture2D<float4> tex : register(t0); //テクスチャ

SamplerState smp : register(s0);

struct VSOutput
{
    float2 pos : POSITION;
    float radian : RADIAN;
    float shocked : SHOCKED;
    float expEaseRate : EXP_EASE_RATE;
    float alpha : ALPHA;
};

VSOutput VSmain_Base(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float shocked : SHOCKED;
    float alpha : ALPHA;
};

float2 RotateFloat2(float2 Pos, float Radian)
{
    float2 result;
    result.x = Pos.x * cos(Radian) - Pos.y * sin(Radian);
    result.y = Pos.y * cos(Radian) + Pos.x * sin(Radian);
    return result;
}

float CalcExpEaseRate(float ExpEaseRate)
{
    return Easing_Exp_Out(ExpEaseRate, 1.0f, 5.0f, 1.0f);
}

struct PSOutput
{
    float4 color : SV_Target0;
    //float4 emissive : SV_Target1;
};

[maxvertexcount(4)]
void GSmain_Base(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    uint2 texSize;
    tex.GetDimensions(texSize.x, texSize.y);
    
    float width_h = texSize.x * extRate / 2.0f * CalcExpEaseRate(input[0].expEaseRate);
    float height_h = texSize.y * extRate / 2.0f * CalcExpEaseRate(input[0].expEaseRate);
    
    GSOutput element;
    element.shocked = input[0].shocked;
    element.alpha = input[0].alpha;
    
    float2 center = input[0].pos;
        
    //左下
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //左上
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //右下
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //右上
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

PSOutput PSmain_Base(GSOutput input)
{
    PSOutput output;
    output.color = tex.Sample(smp, input.uv);
    output.color.w *= input.alpha;
    return output;
}

[maxvertexcount(4)]
void GSmain_Shocked(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    uint2 texSize;
    tex.GetDimensions(texSize.x, texSize.y);
    
    float width_h = texSize.x * extRate / 2.0f * Easing_Exp_Out(1.0f - input[0].shocked, 1.0f, 1.5f, 1.0f) * CalcExpEaseRate(input[0].expEaseRate);
    float height_h = texSize.y * extRate / 2.0f * Easing_Exp_Out(1.0f - input[0].shocked, 1.0f, 1.5f, 1.0f) * CalcExpEaseRate(input[0].expEaseRate);
    
    GSOutput element;
    element.shocked = input[0].shocked;
    element.alpha = input[0].alpha;
    
    float2 center = input[0].pos;
        
    //左下
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //左上
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //右下
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //右上
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

PSOutput PSmain_Shocked(GSOutput input)
{
    PSOutput output;
    output.color = tex.Sample(smp, input.uv);
    output.color.xyz *= input.shocked * output.color.w * 0.8f;
    output.color.w *= input.alpha;
    return output;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}