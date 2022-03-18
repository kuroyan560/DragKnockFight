//スプライトのインスタンシング描画

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

struct InstanceData
{
    matrix mat;
    float4 color;
};

StructuredBuffer<InstanceData> instanceDatas : register(t1);

//頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
    float4 color : COLOR;
};

//頂点シェーダー
VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    //インスタンスのデータ取得
    InstanceData data = instanceDatas[instanceID];
    
    VSOutput output; //ピクセルシェーダに渡す値
    output.svpos = mul(data.mat, pos);
    output.svpos = mul(parallelProjMat, output.svpos);
    pos.z = 0.0f;
    output.uv = uv;
    output.color = data.color;
    return output;
}

//ピクセルシェーダー
float4 PSmain(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * input.color;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}