//�X�v���C�g�̃C���X�^���V���O�`��

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

struct InstanceData
{
    matrix mat;
    float4 color;
};

StructuredBuffer<InstanceData> instanceDatas : register(t1);

//���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
    float4 color : COLOR;
};

//���_�V�F�[�_�[
VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    //�C���X�^���X�̃f�[�^�擾
    InstanceData data = instanceDatas[instanceID];
    
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(data.mat, pos);
    output.svpos = mul(parallelProjMat, output.svpos);
    pos.z = 0.0f;
    output.uv = uv;
    output.color = data.color;
    return output;
}

//�s�N�Z���V�F�[�_�[
float4 PSmain(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * input.color;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}