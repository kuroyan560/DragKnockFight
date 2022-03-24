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
    v.movedVel += float2(v.forwardVec.x * v.speed, v.forwardVec.y * v.speed);

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
    output.alpha = input.alpha;
    output.isAlive = input.isAlive;
    output.drawScale = input.scale / 2.0f * zoom;
    output.texIdx = input.texIdx;
    return output;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
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
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //左上
    element.pos = input[0].pos;
    element.pos.x -= input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //右下
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y += input[0].drawScale;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //右上
    element.pos = input[0].pos;
    element.pos.x += input[0].drawScale;
    element.pos.y -= input[0].drawScale;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

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

float4 PSmain(GSOutput input) : SV_TARGET
{
    float4 color;
    if (input.texIdx == 0)
        color = tex0.Sample(smp, input.uv);
    if (input.texIdx == 1)
        color = tex1.Sample(smp, input.uv);
    if (input.texIdx == 2)
        color = tex2.Sample(smp, input.uv);
    if (input.texIdx == 3)
        color = tex3.Sample(smp, input.uv);
    if (input.texIdx == 4)
        color = tex4.Sample(smp, input.uv);
    if (input.texIdx == 5)
        color = tex5.Sample(smp, input.uv);
    if (input.texIdx == 6)
        color = tex6.Sample(smp, input.uv);
    if (input.texIdx == 7)
        color = tex7.Sample(smp, input.uv);
    if (input.texIdx == 8)
        color = tex8.Sample(smp, input.uv);
    return float4(color.xyz, input.alpha * color.w);
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}