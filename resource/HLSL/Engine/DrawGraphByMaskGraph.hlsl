cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

struct VSOutput
{
    float4 center : CENTER;
    float4 maskCenter : MASK_CENTER;
    int2 mirror : MIRROR;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 maskUv : MASK_TEXCOORD;
    float2 texUv : TEXCOORD;
};

Texture2D<float4> tex : register(t0);
Texture2D<float4> maskTex : register(t1);
SamplerState smp : register(s0);

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
    uint2 maskTexSize;
    maskTex.GetDimensions(maskTexSize.x, maskTexSize.y);
    
    float width_h = maskTexSize.x / 2.0f;
    float height_h = maskTexSize.y / 2.0f;
    float2 uvOffset = -maskTexSize / (input[0].center.xy - input[0].maskCenter.xy);
    
    GSOutput element;
    
    //左下
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos = mul(parallelProjMat, element.pos);
    float2 leftBottomUV = float2(0.0f + input[0].mirror.x, 1.0f - input[0].mirror.y);
    element.maskUv = leftBottomUV - uvOffset;
    element.texUv = leftBottomUV + uvOffset;
    output.Append(element);
    
    //左上
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos = mul(parallelProjMat, element.pos);
    float2 leftUpUV = float2(0.0f + input[0].mirror.x, 0.0f + input[0].mirror.y);
    element.maskUv = leftUpUV - uvOffset;
    element.texUv = leftUpUV + uvOffset;
    output.Append(element);
    
     //右下
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos = mul(parallelProjMat, element.pos);
    float2 rightBottomUV = float2(1.0f - input[0].mirror.x, 1.0f - input[0].mirror.y);
    element.maskUv = rightBottomUV - uvOffset;
    element.texUv = rightBottomUV + uvOffset;
    output.Append(element);
    
    //右上
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos = mul(parallelProjMat, element.pos);
    float2 rightUpUV = float2(1.0f - input[0].mirror.x, 0.0f + input[0].mirror.y);
    element.maskUv = rightUpUV - uvOffset;
    element.texUv = rightUpUV + uvOffset;
    output.Append(element);
}

float4 PSmain(GSOutput input) : SV_TARGET
{
    clip(step(0.0f, input.pos.x));
    clip(step(input.pos.x, 1.0f));
    clip(step(0.0f, input.pos.y));
    clip(step(input.pos.y, 1.0f));
    
    float4 maskCol = maskTex.Sample(smp, input.maskUv);
    float4 texCol = tex.Sample(smp, input.texUv);

    return maskCol * texCol;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}