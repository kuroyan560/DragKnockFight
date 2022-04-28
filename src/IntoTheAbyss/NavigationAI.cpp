#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

const float NavigationAI::SERACH_RADIUS = 200.0f;

void NavigationAI::Init(const RoomMapChipArray &MAP_DATA)
{
	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//���W�̐ݒ�--------------------------
	int xNum = (MAP_DATA[0].size() * MAP_CHIP_SIZE) / WAYPOINT_MAX_Y;
	int yNum = (MAP_DATA.size() * MAP_CHIP_SIZE) / WAYPOINT_MAX_Y;

	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//��x���[���h���W�ɕϊ����Ă���}�b�v�`�b�v���W�ɕϊ�����
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAP_CHIP_SIZE));
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
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data;
				data.center = &wayPoints[y][x].pos;
				data.radius = SERACH_RADIUS;
				RegistHandle(data, &wayPoints[y][x]);
			}
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
			if (DontUse(wayPoints[y][x]))
			{
				if (wayPointFlag)
				{
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, Color(255, 255, 255, 255));
				}
				if (serachFlag)
				{
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), SERACH_RADIUS, Color(255, 255, 255, 255));
				}
			}
		}
	}

	//�q����̕`��
	if (lineFlag)
	{
		for (int y = 0; y < wayPoints.size(); ++y)
		{
			for (int x = 0; x < wayPoints[y].size(); ++x)
			{
				if (DontUse(wayPoints[y][x]))
				{
					//�o�^�����n���h����������q����
					for (int i = 0; i < wayPoints[y][x].wayPointHandles.size(); ++i)
					{
						Vec2<int> endHandle = wayPoints[y][x].wayPointHandles[i];
						DrawFunc::DrawLine2D
						(
							ScrollMgr::Instance()->Affect(wayPoints[y][x].pos),
							ScrollMgr::Instance()->Affect(wayPoints[endHandle.y][endHandle.x].pos),
							Color(0, 155, 0, 255)
						);
					}
				}
			}
		}
	}
#endif
}

void NavigationAI::ImGuiDraw()
{
	ImGui::Begin("Navigation");
	ImGui::Checkbox("SearchCircle", &serachFlag);
	ImGui::Checkbox("WayPoint", &wayPointFlag);
	ImGui::Checkbox("MoveLine", &lineFlag);
	ImGui::End();
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

inline bool NavigationAI::DontUse(const WayPointData &DATA)
{
	return DATA.handle.x != -1 && DATA.handle.y != -1;
}

inline void NavigationAI::RegistHandle(const SphereCollision &HANDLE, WayPointData *DATA)
{
	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//�g�p�ł���f�[�^���画����s��
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data;
				data.center = &wayPoints[y][x].pos;
				data.radius = SERACH_RADIUS;

				//�ړ��͈͓��ɃE�F�C�|�C���g�����邩
				bool serachFlag = BulletCollision::Instance()->CheckSphereAndSphere(HANDLE, data);
				//�����ǂɓ������Ă��邩�ǂ���
				bool canMoveFlag = false;
				if (serachFlag)
				{
					canMoveFlag = !CheckMapChipWallAndRay(*HANDLE.center, *data.center);
				}

				//�T���͈͓�&&���ڍs����ꏊ�Ȃ�����q����
				if (serachFlag && canMoveFlag)
				{
					wayPoints[y][x].RegistHandle({ x,y });
					DATA->RegistHandle({ x,y });
				}
			}
		}
	}
}
