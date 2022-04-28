#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

void NavigationAI::Init(const RoomMapChipArray &MAP_DATA)
{
	const Vec2<float> MAPCHIP_SIZE = { 50.0f,50.0f };

	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//���W�̐ݒ�--------------------------
	int xNum = (MAP_DATA[0].size() * MAPCHIP_SIZE.x) / WAYPOINT_MAX_Y;
	int yNum = (MAP_DATA.size() * MAPCHIP_SIZE.y) / WAYPOINT_MAX_Y;

	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//��x���[���h���W�ɕϊ����Ă���}�b�v�`�b�v���W�ɕϊ�����
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAPCHIP_SIZE));
			if (MAP_DATA[0].size() <= mapChipPos.x || MAP_DATA.size() <= mapChipPos.y)
			{
				continue;
			}

			float radisu = 3.0f;
			//32���^��
			if (mapChipPos.y == 25 && mapChipPos.x == 32)
			{
				radisu = 10.0f;
			}


			bool wallFlag = false;
			//�ǂ̂���ꏊ�Ƀ|�C���g��ݒu���Ȃ�
			for (int wallIndex = wallMemorySize.min; wallIndex < wallMemorySize.max; ++wallIndex)
			{
				if (MAP_DATA[mapChipPos.y][mapChipPos.x] == wallIndex)
				{
					wallFlag = true;
					break;
				}
			}



			if (!wallFlag)
			{
				//���Ԋu�ŊJ����
				wayPoints[y][x].pos = worldPos;
				wayPoints[y][x].radius = radisu;
				wayPoints[y][x].handle = { x,y };
			}
		}
	}
	//���W�̐ݒ�--------------------------


	//�߂��ɂ���E�F�C�|�C���g�ւ̌q��--------------------------
	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			
		}
	}
	//�߂��ɂ���E�F�C�|�C���g�ւ̌q��--------------------------

	//�S�[���n�_�ւ̒T��--------------------------
	//�S�[���n�_�ւ̒T��--------------------------
}

void NavigationAI::Update(const Vec2<float> &POS)
{

}

void NavigationAI::Draw()
{
	//�f�o�b�N�̎��̂ݕ\��
#ifdef  _DEBUG
	//�E�F�C�|�C���g�̕`��
	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, Color(255, 255, 255, 255));
		}
	}
#endif
}

inline const Vec2<int> &NavigationAI::GetMapChipNum(const Vec2<float> &WORLD_POS)
{
	//���������_��؂艺���鏈��
	Vec2<float> num =
	{
		floor(WORLD_POS.x),
		floor(WORLD_POS.y)
	};
	//�����̒l�Ɛ؂艺�����l���̍��������߁A�����_�𓾂�
	Vec2<float>checkNumebr = WORLD_POS - num;

	//�����_��0.5�����Ȃ�؂艺���A����ȊO�͐؂�グ��
	Vec2<int> result;
	if (checkNumebr.x < 0.5f)
	{
		result =
		{
			static_cast<int>(floor(WORLD_POS.x)),
			static_cast<int>(floor(WORLD_POS.y))
		};
	}
	else
	{
		result =
		{
			static_cast<int>(ceil(WORLD_POS.x)),
			static_cast<int>(ceil(WORLD_POS.y))
		};
	}

	return result;
}
