#include"Engine/Math.hlsli"
struct Vertex
{
    float2 pos;
    float2 emitPos;
    float2 emitVec;
    float speed;
    float emitSpeed;
    float scale;
    float emitScale;
    float radian;
    float emitRadian;
    float alpha;
    int life;
    int lifeSpan;
    min16int isAlive;
    uint texIdx;
    uint type;
};

RWStructuredBuffer<Vertex> vertices : register(u0);
cbuffer cbuff1 : register(b1)
{
    float zoom;
    float2 scroll;
    float gameSpeed;
};

[numthreads(10, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID)
{
    int i = DTid.x;
    
    Vertex v = vertices[i];
    
    if(!v.isAlive)
    {
        return;
    }
    
    //NORMAL_SMOKE
    if(v.type == 0)
    {
        float2 toPos = v.emitPos + v.emitVec * v.speed;
        v.pos = Easing_Circ_Out(v.life, v.lifeSpan, v.emitPos, toPos);
        v.radian = Easing_Quart_Out(v.life, v.lifeSpan, 0.0f, v.emitRadian);
        
        //éıñΩÇ™îºï™à»â∫Ç»ÇÁägèkÇµÇƒè¡Ç¶ÇÈ
        if(v.lifeSpan / 2.0f <= v.life)
        {
            float t = v.life - v.lifeSpan / 2.0f;
            v.scale = Easing_Circ_In(t, v.lifeSpan / 2.0f, v.emitScale, 0.0f);
            v.alpha = Easing_Circ_In(t, v.lifeSpan / 2.0f, 1.0f, 0.0f);
        }
    }
    //FAST_SMOKE
    else if(v.type == 1)
    {
        float2 toPos = v.emitPos + v.emitVec * v.speed;
        v.pos = Easing_Circ_Out(v.life, v.lifeSpan, v.emitPos, toPos);
        v.radian = Easing_Quart_Out(v.life, v.lifeSpan, 0.0f, v.emitRadian);
        
        //éıñΩÇ™îºï™à»â∫Ç»ÇÁägèkÇµÇƒè¡Ç¶ÇÈ
        if (v.lifeSpan / 2.0f <= v.life)
        {
            float t = v.life - v.lifeSpan / 2.0f;
            v.scale = Easing_Circ_In(t, v.lifeSpan / 2.0f, v.emitScale, 0.0f);
            v.alpha = Easing_Circ_In(t, v.lifeSpan / 2.0f, 1.0f, 0.0f);
        }
    }
    //EMIT_STAR
    else if (v.type == 2)
    {
        float2 toPos = v.emitPos + v.emitVec * v.speed;
        v.pos = Easing_Circ_Out(v.life, v.lifeSpan, v.emitPos, toPos);
        v.radian = Easing_Quart_Out(v.life, v.lifeSpan, 0.0f, v.emitRadian);
        
        //éıñΩÇ™îºï™à»â∫Ç»ÇÁägèkÇµÇƒè¡Ç¶ÇÈ
        if (v.lifeSpan / 2.0f <= v.life)
        {
            float t = v.life - v.lifeSpan / 2.0f;
            v.scale = Easing_Circ_In(t, v.lifeSpan / 2.0f, v.emitScale, 0.0f);
            //v.alpha = Easing_Circ_In(t, v.lifeSpan / 2.0f, 1.0f, 0.0f);
        }
    }
    else if(v.type == 3)
    {
        
    }
    
    v.life++;
    if (v.lifeSpan <= v.life)
    {
        v.isAlive = 0;
    }
    
    vertices[i] = v;
}

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //ïΩçsìäâeçsóÒ
};

struct VSInput
{
    float4 pos : POSITION;
    float2 emitVec : EMIT_VEC;
    float speed : SPEED;
    float scale : SCALE;
    float radian : RADIAN;
    float alpha : ALPHA;
    int life : LIFE;
    int lifeSpan : LIFE_SPAN;
    min16int isAlive : ALIVE;
    uint texIdx : TEX_IDX;
    uint type : TYPE;
};

struct VSOutput
{
    float4 pos : POSITION;
    float radian : RADIAN;
    float alpha : ALPHA;
    min16int isAlive : ALIVE;
    float drawScale : DRAW_SCALE;
    uint texIdx : TEX_IDX;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output;
    output.pos = input.pos;
    output.pos.xy *= zoom;
    output.pos.xy += scroll;
    output.radian = input.radian;
    output.alpha = input.alpha;
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
    if (!input[0].isAlive)
        return;
    
    GSOutput element;
    element.alpha = input[0].alpha;
    element.texIdx = input[0].texIdx;
        
    //ç∂â∫
    element.pos = input[0].pos;
    element.pos.x -= input[0].drawScale;
    element.pos.y += input[0].drawScale;
    element.pos.xy = input[0].pos.xy + RotateFloat2(element.pos.xy - input[0].pos.xy, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //ç∂è„
    element.pos = input[0].pos;
    element.pos.x -= input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.pos.xy = input[0].pos.xy + RotateFloat2(element.pos.xy - input[0].pos.xy, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //âEâ∫
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y += input[0].drawScale;
    element.pos.xy = input[0].pos.xy + RotateFloat2(element.pos.xy - input[0].pos.xy, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //âEè„
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.pos.xy = input[0].pos.xy + RotateFloat2(element.pos.xy - input[0].pos.xy, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

#include"Engine/LightInfo.hlsli"
Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);
Texture2D<float4> tex3 : register(t3);
Texture2D<float4> tex4 : register(t4);
Texture2D<float4> tex5 : register(t5);
Texture2D<float4> tex6 : register(t6);
Texture2D<float4> tex7 : register(t7);
Texture2D<float4> tex8 : register(t8);
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
    //É|ÉWÉVÉáÉì
    float3 pos = float3(input.worldPos.xy, DEPTH);
    
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
    
    //result.xyz *= ligEffect;
    result.w *= input.alpha;
    
    PSOutput output;
    output.color = result;
    //output.emissive = output.color;
    
    //ñæÇÈÇ≥åvéZ
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