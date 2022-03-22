#include"Engine/LightInfo.hlsli"

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

cbuffer cbuff1 : register(b1)
{
    float extRate;
};

cbuffer cbuff2 : register(b2)
{
    LightInfo ligNum; //�A�N�e�B�u���̃��C�g�̐��̏��
}

StructuredBuffer<DirectionLight> dirLight : register(t0);
StructuredBuffer<PointLight> pointLight : register(t1);
StructuredBuffer<SpotLight> spotLight : register(t2);
StructuredBuffer<HemiSphereLight> hemiSphereLight : register(t3);

Texture2D<float4> tex : register(t4); //�e�N�X�`��
Texture2D<float4> normalMap : register(t5); //�m�[�}���}�b�v
Texture2D<float4> emissiveMap : register(t6); //�G�~�b�V�u�}�b�v

SamplerState smp : register(s0);

struct VSOutput
{
    float2 pos : POSITION;
    float radian : RADIAN;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : WORLD_POS;
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
    uint2 texSize;
    tex.GetDimensions(texSize.x, texSize.y);
    
    float width_h = texSize.x * extRate / 2.0f;
    float height_h = texSize.y * extRate / 2.0f;
    
    GSOutput element;
    
    float2 center = input[0].pos;
        
    //����
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //����
    element.pos = float4(center, 0, 1);
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //�E��
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //�E��
    element.pos = float4(center, 0, 1);
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos.xy = center + RotateFloat2(element.pos.xy - center, input[0].radian);
    element.worldPos = element.pos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
}

struct PSOutput
{
    float4 color : SV_Target0;
    float4 emissive : SV_Target1;
};

static const float3 EYE_POS = float3(640, 360, -5.0f);
static const float DEPTH = 0.0f;
static const float DIFFUSE = 1.0f;
static const float SPECULAR = 1.0f;
static const float LIM = 1.0f;

PSOutput PSmain(GSOutput input)
{
 //�|�W�V����
    float3 pos = float3(input.worldPos.xy, DEPTH);
    
    //�m�[�}���}�b�v����@�����擾
    float3 normal = normalMap.Sample(smp, input.uv).xyz;
    normal = (normal - 0.5f) * 2.0f;
    normal.y *= -1.0f;
    normal.z *= -1.0f;
    
    float3 vnormal = mul(parallelProjMat, float4(normal, 1.0f)).xyz;
    
    //���C�g�̉e��
    //float3 ligEffect = { 0.3f, 0.3f, 0.3f };
    float3 ligEffect = { 0.0f, 0.0f, 0.0f };
    
    //�f�B���N�V�������C�g
    for (int i = 0; i < ligNum.dirLigNum; ++i)
    {
        float3 dir = dirLight[i].direction;
        float3 ligCol = dirLight[i].color.xyz * dirLight[i].color.w;
        ligEffect += CalcLambertDiffuse(dir, ligCol, normal) * DIFFUSE;
        ligEffect += CalcPhongSpecular(dir, ligCol, normal, pos, EYE_POS) * SPECULAR;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * LIM;
    }
    //�|�C���g���C�g
    for (int i = 0; i < ligNum.ptLigNum; ++i)
    {
        float3 dir = pos - pointLight[i].pos;
        dir = normalize(dir);
        float3 ligCol = pointLight[i].color.xyz * pointLight[i].color.w;
        
        //�����Ȃ����
        float3 diffPoint = CalcLambertDiffuse(dir, ligCol, normal);
        float3 specPoint = CalcPhongSpecular(dir, ligCol, normal, pos, EYE_POS);
        
        //�����ɂ�錸��
        float3 distance = length(pos - pointLight[i].pos);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / pointLight[i].influenceRange * distance;
		//�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
		//�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
        diffPoint *= affect;
        specPoint *= affect;
        
        ligEffect += diffPoint * DIFFUSE;
        ligEffect += specPoint * SPECULAR;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * LIM;
    }
    //�X�|�b�g���C�g
    for (int i = 0; i < ligNum.spotLigNum; ++i)
    {
        float3 ligDir = pos - spotLight[i].pos;
        ligDir = normalize(ligDir);
        float3 ligCol = spotLight[i].color.xyz * spotLight[i].color.w;
        
        //�����Ȃ����
        float3 diffSpotLight = CalcLambertDiffuse(ligDir, ligCol, normal);
        float3 specSpotLight = CalcPhongSpecular(ligDir, ligCol, normal, pos, EYE_POS);
        
        //�X�|�b�g���C�g�Ƃ̋������v�Z
        float3 distance = length(pos - spotLight[i].pos);
       	//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / spotLight[i].influenceRange * distance;
        //�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
    //�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
        diffSpotLight *= affect;
        specSpotLight *= affect;
    
        float3 spotLIM = CalcLimLight(ligDir, ligCol, normal, vnormal) * affect;
        
        float3 dir = normalize(spotLight[i].target - spotLight[i].pos);
        float angle = dot(ligDir, dir);
        angle = abs(acos(angle));
        affect = 1.0f - 1.0f / spotLight[i].angle * angle;
        if (affect < 0.0f)
            affect = 0.0f;
        affect = pow(affect, 0.5f);
        
        ligEffect += diffSpotLight * affect * DIFFUSE;
        ligEffect += specSpotLight * affect * SPECULAR;
        ligEffect += spotLIM * affect * LIM;
    }
    //�V��
    for (int i = 0; i < ligNum.hemiSphereNum; ++i)
    {
        float t = dot(normal.xyz, hemiSphereLight[i].groundNormal);
        t = (t + 1.0f) / 2.0f;
        float3 hemiLight = lerp(hemiSphereLight[i].groundColor, hemiSphereLight[i].skyColor, t);
        ligEffect += hemiLight;
    }
    
    float4 result = tex.Sample(smp, input.uv);
    result.xyz *= ligEffect;
    
    PSOutput output;
    output.color = result;
    
    output.emissive = emissiveMap.Sample(smp, input.uv);
    
    //���邳�v�Z
    float bright = dot(result.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    if (1.0f < bright)
        output.emissive += result;
    output.emissive.w = result.w;
    
    return output;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}