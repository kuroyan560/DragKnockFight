struct Vertex
{
    float2 pos;
    float2 uv;
    float2 vel;
    float2 force;
};

cbuffer cbuff0 : register(b0)
{
    float2 startPoint;
    float2 oldPoint;
};

cbuffer cbuff1 : register(b1)
{
    int2 vertexNum;
    float2 mufflerSize;
}

RWStructuredBuffer<Vertex> vertices : register(u0);

uint GetIndex(uint x,uint y)
{
    return y * vertexNum.x + x;
}

static const float DENSITY = 8.0f; //単位面積あたりの質量密度
static const float GRAVITY = 0.05f; //重力
static const float DAMPING = 32.0f;  //ダンピング
 //バネ定数（バネの硬さ、反発力の強さ）
static const float SPRING_FACTOR = 28.0f;   //上下左右
static const float SPRING_FACTOR_SKEW = SPRING_FACTOR * 0.9f; //斜め
static const float POS_SCALE = 6.0f;
static const float2 SPRING_LENGTH = { 16.0f, 1.0f };

//static const float TENSION = 15.0f; //張力
//static const float FRICTION = 3.0f; //減衰力係数
//static const float DT = 0.01f; //シミュレーションの時間刻み

//１個の隣接質点から受ける力を求める
float2 GetForce(const float2 MyPos, const float2 OtherPos, const float SpringLength, const float SpringFactor)
{
    float2 d = MyPos - OtherPos;
    float f = (length(d) - SpringLength) * SpringFactor;
    return normalize(OtherPos - MyPos) * f;
}


//質点に働く力
//[numthreads(8, 8, 1)]
[numthreads(1, 1, 1)]
void CSmain( uint2 DTid : SV_DispatchThreadID )
{
    uint myIdx = GetIndex(DTid.y, DTid.x);
    uint2 idx = uint2(DTid.y, DTid.x);
    if(idx.y == 0)
    {
        vertices[myIdx].pos = startPoint;
        const float offset = mufflerSize.x / vertexNum.x * 0.1f;
        vertices[myIdx].pos.x += -mufflerSize.x / 2.0f * 0.1f + offset;
        return;
    }
    
    const float m = DENSITY * mufflerSize.x * mufflerSize.y / ((vertexNum.x + 1) * (vertexNum.y + 1));
    //const float2 springLength = mufflerSize / vertexNum;
    const float2 springLength = SPRING_LENGTH;
    Vertex vert = vertices[myIdx];
    
    //力をゼロにする
    vert.force = float2(0.0f, 0.0f);
    
    //重力
    vert.force.y += m * GRAVITY;
    
    //ダンピング
    vert.force -= vert.vel * DAMPING;
    
    
    //上のバネ
    if (0 < idx.y)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x, idx.y - 1)].pos, springLength.y, SPRING_FACTOR);
    }
        
     //下のバネ
    if(idx.y < vertexNum.y - 1)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x, idx.y + 1)].pos, springLength.y, SPRING_FACTOR);
    }
    
    //左のバネ
    if (0 < idx.x)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x - 1, idx.y)].pos, springLength.x, SPRING_FACTOR);
    }
    
    //右のバネ
    if (idx.x < vertexNum.x - 1)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x + 1, idx.y)].pos, springLength.x, SPRING_FACTOR);
    }
    
    const float springLengthSkew = length(springLength);
    
    //左上のばね
    if (0 < idx.x && 0 < idx.y)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x - 1, idx.y - 1)].pos, springLengthSkew, SPRING_FACTOR_SKEW);
    }
    
     //右上のばね
    if (idx.x < vertexNum.x - 1 && 0 < idx.y)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x + 1, idx.y - 1)].pos, springLengthSkew, SPRING_FACTOR_SKEW);
    }
    
    //左下のばね
    if (0 < idx.x && idx.y < vertexNum.y - 1)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x - 1, idx.y + 1)].pos, springLengthSkew, SPRING_FACTOR_SKEW);
    }
    
    //右下のばね
    if (idx.x < vertexNum.x - 1 && idx.y < vertexNum.y - 1)
    {
        vert.force += GetForce(vert.pos, vertices[GetIndex(idx.x + 1, idx.y + 1)].pos, springLengthSkew, SPRING_FACTOR_SKEW);
    }
    
    vert.vel += vert.force / float2(m, m);
    vert.pos += vert.vel;

    vertices[myIdx] = vert;
}