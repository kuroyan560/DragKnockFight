cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 center : CENTER;
    float extRate : EXT_RATE;
    float radian : RADIAN;
    int mirorX : MIROR_X;
    float2 texSize : TEX_SIZE;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float2 RotateFloat2(float2 Pos,float Radian)
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
    float width_h = input[0].texSize.x * input[0].extRate / 2.0f;
    float height_h = input[0].texSize.x * input[0].extRate / 2.0f;
    
    GSOutput element;
        
    //����
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos.xy = input[0].center.xy + RotateFloat2(element.pos.xy - input[0].center.xy, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].mirorX, 1.0f);
    output.Append(element);
    
    //����
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos.xy = input[0].center.xy + RotateFloat2(element.pos.xy - input[0].center.xy, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].mirorX, 0.0f);
    output.Append(element);
    
     //�E��
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos.xy = input[0].center.xy + RotateFloat2(element.pos.xy - input[0].center.xy, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].mirorX, 1.0f);
    output.Append(element);
    
    //�E��
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos.xy = input[0].center.xy + RotateFloat2(element.pos.xy - input[0].center.xy, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].mirorX, 0.0f);
    output.Append(element);
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(GSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}