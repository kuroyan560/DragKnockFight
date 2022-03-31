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
};

cbuffer cbuff1 : register(b1)
{
    int2 vertexNum;
    float2 vertexOffset;  //各頂点のオフセット
    float2 mufflerSize;
}

RWStructuredBuffer<Vertex> vertices : register(u0);

int GetIndex(int x,int y)
{
    return y * vertexNum.x + x;
}

//１個の隣接質点から受ける力を求める
float GetForce(float ZSide, float ZCenter, float Tension, float Delta)
{
    return ((ZSide - ZCenter) * Tension / Delta);
}

static const float DENSITY = 10.0f; //単位面積あたりの質量密度
static const float GRAVITY = 0.1f;  //重力

static const float TENSION = 15.0f; //張力
static const float FRICTION = 3.0f; //減衰力係数
static const float DT = 0.01f; //シミュレーションの時間刻み

//質点に働く力
//[numthreads(8, 8, 1)]
[numthreads(1, 1, 1)]
void CSmain( uint2 DTid : SV_DispatchThreadID )
{
    const float m = DENSITY * mufflerSize.x * mufflerSize.y / ((vertexNum.x + 1) * (vertexNum.y + 1));
    
    int idxX = DTid.y;
    int idxY = DTid.x;
    int myIdx = GetIndex(idxX, idxY);
    
    if (idxY == 0)
    {
        vertices[myIdx].pos = startPoint;
        return;
    }
    
    //バネからかかる力
    float force = 0.0f;

    {
	    //自身のZ(高さY)
        float h = vertices[myIdx].pos.y;

	    //X方向に隣接する質点とのバネからからる力
        if (idxX != 0)
        {
            force += GetForce(vertices[GetIndex(idxX - 1, idxY)].pos.y, h, tension, vertexOffset.x);
        }
        else if (idxX != vertexNum.x - 1)
        {
            force += GetForce(vertices[GetIndex(idxX + 1, idxY)].pos.y, h, tension, vertexOffset.x);
        }

		//Y方向に隣接する質点とのバネからからる力
        if (idxY != 0)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY - 1)].pos.y, h, tension, vertexOffset.y);
        }
        else if (idxY != vertexNum.y - 1)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY + 1)].pos.y, h, tension, vertexOffset.y);
        }

		//バネからかかる力 + 減衰力 = 質点にかかる力
        vertices[myIdx].force.y = force - friction * vertices[myIdx].vel.y;
    }

    force = 0.0f;
    {
	    //自身のZ(高さY)
        float h = vertices[myIdx].pos.x;

	    //X方向に隣接する質点とのバネからからる力
        if (idxX != 0)
        {
            force += GetForce(vertices[GetIndex(idxX - 1, idxY)].pos.x, h, tension, vertexOffset.x);
        }
        else if (idxX != vertexNum.x - 1)
        {
            force += GetForce(vertices[GetIndex(idxX + 1, idxY)].pos.x, h, tension, vertexOffset.x);
        }

		//Y方向に隣接する質点とのバネからからる力
        if (idxY != 0)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY - 1)].pos.x, h, tension, vertexOffset.y);
        }
        else if (idxY != vertexNum.y - 1)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY + 1)].pos.x, h, tension, vertexOffset.y);
        }

		//バネからかかる力 + 減衰力 = 質点にかかる力
        vertices[myIdx].force.x = force - friction * vertices[myIdx].vel.x;
    }
    
    //速度変化の計算
    vertices[myIdx].vel.x += vertices[myIdx].force.x / m * dt;
    vertices[myIdx].vel.y += vertices[myIdx].force.y / m * dt;
    //座標変化の計算
    vertices[myIdx].pos.x += vertices[myIdx].vel.x * dt;
    vertices[myIdx].pos.y += vertices[myIdx].vel.y * dt;
}