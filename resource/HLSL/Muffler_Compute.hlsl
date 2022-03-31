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
    float2 vertexOffset;  //�e���_�̃I�t�Z�b�g
    float2 mufflerSize;
}

RWStructuredBuffer<Vertex> vertices : register(u0);

int GetIndex(int x,int y)
{
    return y * vertexNum.x + x;
}

//�P�̗אڎ��_����󂯂�͂����߂�
float GetForce(float ZSide, float ZCenter, float Tension, float Delta)
{
    return ((ZSide - ZCenter) * Tension / Delta);
}

static const float DENSITY = 10.0f; //�P�ʖʐς�����̎��ʖ��x
static const float GRAVITY = 0.1f;  //�d��

static const float TENSION = 15.0f; //����
static const float FRICTION = 3.0f; //�����͌W��
static const float DT = 0.01f; //�V�~�����[�V�����̎��ԍ���

//���_�ɓ�����
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
    
    //�o�l���炩�����
    float force = 0.0f;

    {
	    //���g��Z(����Y)
        float h = vertices[myIdx].pos.y;

	    //X�����ɗאڂ��鎿�_�Ƃ̃o�l���炩����
        if (idxX != 0)
        {
            force += GetForce(vertices[GetIndex(idxX - 1, idxY)].pos.y, h, tension, vertexOffset.x);
        }
        else if (idxX != vertexNum.x - 1)
        {
            force += GetForce(vertices[GetIndex(idxX + 1, idxY)].pos.y, h, tension, vertexOffset.x);
        }

		//Y�����ɗאڂ��鎿�_�Ƃ̃o�l���炩����
        if (idxY != 0)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY - 1)].pos.y, h, tension, vertexOffset.y);
        }
        else if (idxY != vertexNum.y - 1)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY + 1)].pos.y, h, tension, vertexOffset.y);
        }

		//�o�l���炩����� + ������ = ���_�ɂ������
        vertices[myIdx].force.y = force - friction * vertices[myIdx].vel.y;
    }

    force = 0.0f;
    {
	    //���g��Z(����Y)
        float h = vertices[myIdx].pos.x;

	    //X�����ɗאڂ��鎿�_�Ƃ̃o�l���炩����
        if (idxX != 0)
        {
            force += GetForce(vertices[GetIndex(idxX - 1, idxY)].pos.x, h, tension, vertexOffset.x);
        }
        else if (idxX != vertexNum.x - 1)
        {
            force += GetForce(vertices[GetIndex(idxX + 1, idxY)].pos.x, h, tension, vertexOffset.x);
        }

		//Y�����ɗאڂ��鎿�_�Ƃ̃o�l���炩����
        if (idxY != 0)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY - 1)].pos.x, h, tension, vertexOffset.y);
        }
        else if (idxY != vertexNum.y - 1)
        {
            force += GetForce(vertices[GetIndex(idxX, idxY + 1)].pos.x, h, tension, vertexOffset.y);
        }

		//�o�l���炩����� + ������ = ���_�ɂ������
        vertices[myIdx].force.x = force - friction * vertices[myIdx].vel.x;
    }
    
    //���x�ω��̌v�Z
    vertices[myIdx].vel.x += vertices[myIdx].force.x / m * dt;
    vertices[myIdx].vel.y += vertices[myIdx].force.y / m * dt;
    //���W�ω��̌v�Z
    vertices[myIdx].pos.x += vertices[myIdx].vel.x * dt;
    vertices[myIdx].pos.y += vertices[myIdx].vel.y * dt;
}