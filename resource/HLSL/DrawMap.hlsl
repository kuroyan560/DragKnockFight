cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

cbuffer cbuff1 : register(b1)
{
    float extRate;
};

Texture2D<float4> tex : register(t0);

struct VSOutput
{
    float2 pos : POSITION;
    float radian : RADIAN;
};
SamplerState smp : register(s0);

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float2 RotateFloat2(float2 Pos, float Radian)
{
    float2 result;
    result.x = Pos.x * cos(Radian) - Pos.y * sin(Radian);
    result.y = Pos.y * cos(Radian) + Pos.x * sin(Radian);
    return result;
}

[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    uint2 texSize;
    tex.GetDimensions(texSize.x, texSize.y);
    
    float width_h = texSize.x * extRate / 2.0f;
    float height_h = texSize.y * extRate / 2.0f;
    
    GSOutput element;
    
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

float4 PSmain(GSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}