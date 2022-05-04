cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 leftUpPos : POSITION_L_U;
    float4 rightBottomPos : POSITION_R_B;
    float4 emitColor : EMIT_COLOR;
    int2 miror : MIROR;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 emitColor : EMIT_COLOR;
};


[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    float width = input[0].rightBottomPos.x - input[0].leftUpPos.x;
    
    GSOutput element;
    element.emitColor = input[0].emitColor;
        
    //����
    element.pos = input[0].rightBottomPos;
    element.pos.x -= width;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //����
    element.pos = input[0].leftUpPos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
    
     //�E��
    element.pos = input[0].rightBottomPos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //�E��
    element.pos = input[0].leftUpPos;
    element.pos.x += width;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(GSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * input.emitColor;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}