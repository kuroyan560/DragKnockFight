cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

struct VSOutput
{
    float4 leftUpPos : POSITION_L_U;
    float4 rightBottomPos : POSITION_R_B;
    int2 miror : MIROR;
    int srvIdx : SRV_IDX;
    float alpha : ALPHA;
    float scale : SCALE;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    int srvIdx : SRV_IDX;
    float alpha : ALPHA;
};

[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    float width_h = input[0].rightBottomPos.x - input[0].leftUpPos.x;
    width_h *= input[0].scale * 0.5f;
    float height_h = input[0].rightBottomPos.y - input[0].leftUpPos.y;
    height_h *= input[0].scale * 0.5f;
    float4 center = input[0].rightBottomPos - float4(width_h, height_h, 0, 0);
    
    GSOutput element;
    element.srvIdx = input[0].srvIdx;
    element.alpha = input[0].alpha;
        
    //左下
    element.pos = center;
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //左上
    element.pos = center;
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
    
     //右下
    element.pos = center;
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //右上
    element.pos = center;
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
}

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);
Texture2D<float4> tex3 : register(t3);
Texture2D<float4> tex4 : register(t4);
Texture2D<float4> tex5 : register(t5);
SamplerState smp : register(s0);

static const float3 COLOR = float3(0.3137, 0.3137, 1.0);
float4 PSmain(GSOutput input) : SV_TARGET
{
    float4 color;
    if (input.srvIdx == 0)
        color = tex0.Sample(smp, input.uv);
    if (input.srvIdx == 1)
        color = tex1.Sample(smp, input.uv);
    if (input.srvIdx == 2)
        color = tex2.Sample(smp, input.uv);
    if (input.srvIdx == 3)
        color = tex3.Sample(smp, input.uv);
    if (input.srvIdx == 4)
        color = tex4.Sample(smp, input.uv);
    if (input.srvIdx == 5)
        color = tex5.Sample(smp, input.uv);
    
    //return float4(COLOR.xyz * input.alpha * color.w, 1.0f);
    return float4(COLOR.xyz, input.alpha * color.w);

}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}