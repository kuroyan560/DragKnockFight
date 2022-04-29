#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"../Engine/UsersInput.h"
#include"../IntoTheAbyss/DebugKeyManager.h"
#include<queue>

const float NavigationAI::SERACH_RADIUS = 180.0f;
const float NavigationAI::WAYPOINT_RADIUS = 20.0f;

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
			debugColor[y][x] = Color(255, 255, 255, 255);

			//��x���[���h���W�ɕϊ����Ă���}�b�v�`�b�v���W�ɕϊ�����
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAP_CHIP_SIZE));
			if (MAP_DATA[0].size() <= mapChipPos.x || MAP_DATA.size() <= mapChipPos.y)
			{
				continue;
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
				wayPoints[y][x].radius = WAYPOINT_RADIUS;
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

	wayPointFlag = true;
	serachFlag = true;
	lineFlag = true;

	checkingHandle = { -1,-1 };
	checkTimer = 0;
}

void NavigationAI::Update(const Vec2<float> &POS)
{
	Vec2<float>pos = UsersInput::Instance()->GetMousePos();

	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data1, data2;

				//�X�N���[�����W�Ɣ�������ׂɃX�N���[�����W�ɕϊ�����
				data1.center = &ScrollMgr::Instance()->Affect(wayPoints[y][x].pos);
				data1.radius = wayPoints[y][x].radius;
				data2.center = &pos;
				data2.radius = WAYPOINT_RADIUS;

				bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(data1, data2);

				//�}�E�X�J�[�\���ƍ�������E�F�C�|�C���g�̏����Q�Ƃ���
				if (hitFlag)
				{
					checkingHandle = wayPoints[y][x].handle;
					checkTimer = 0;
				}
				//�E�N���b�N������X�^�[�g�n�_�ɐݒ肷��
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_H, "SetStart", "DIK_H"))
				{
					startPoint = wayPoints[y][x];
				}
				//���N���b�N������S�[���n�_�ɐݒ肷��
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_N, "SetGoal", "DIK_N"))
				{
					endPoint = wayPoints[y][x];
				}
			}
		}
	}

	++checkTimer;
	if (60 * 5 <= checkTimer)
	{
		checkingHandle = { -1,-1 };
	}

	//�X�^�[�g�n�_���S�[���n�_��ς���ۂ͔����ꂷ��
	if (startPoint.handle != oldStartPoint.handle || endPoint.handle != oldEndPoint.handle)
	{
		for (int y = 0; y < wayPoints.size(); ++y)
		{
			for (int x = 0; x < wayPoints[y].size(); ++x)
			{
				debugColor[y][x] = Color(255, 255, 255, 255);
			}
		}
	}

	//�i�K�ɕ����ĒT������F���o��
	for (int layer = 0; layer < layerNum; ++layer)
	{
		for (int num = 0; num < searchMap[layer].size(); ++num)
		{
			Vec2<int>handle = searchMap[layer][num].handle;
			debugColor[handle.y][handle.x] = searchMap[layer][num].color;
		}
	}

	//�X�^�[�g�ƃS�[����ݒ肵����A�X�^�[�̒T�����J�n����
	if (startPoint.handle.x != -1 && startPoint.handle.y != -1 &&
		endPoint.handle.x != -1 && endPoint.handle.y != -1)
	{
		AStart(startPoint, endPoint);
	}



	//�D��x�t���L���[
	std::priority_queue<std::tuple<int, int, float>> hoge;
	std::tuple<int, int, float> data1(0, 0, 10.0f);
	std::tuple<int, int, float> data2(0, 1, 10.0f);
	std::tuple<int, int, float> data3(1, 1, 10.0f);
	hoge.push(data1);
	hoge.push(data2);
	hoge.push(data3);
	std::tuple<int, int, float> data = hoge.top();
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

					bool isCheckingFlag = false;
					Color color = debugColor[y][x];

					//���̏ꏊ���X�^�[�g�n�_�Ȃ�F��ς���
					isCheckingFlag = startPoint.handle.x == wayPoints[y][x].handle.x && startPoint.handle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(255, 255, 0, 255);
					}

					//���̏ꏊ���S�[���n�_�Ȃ�F��ς���
					isCheckingFlag = endPoint.handle.x == wayPoints[y][x].handle.x && endPoint.handle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(0, 255, 255, 255);
					}

					//���̏ꏊ���}�E�X�J�[�\���ƍ�������m�F���̏ꏊ���ƔF������
					isCheckingFlag = checkingHandle.x == wayPoints[y][x].handle.x && checkingHandle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}

					//�E�F�C�|�C���g�̕`��
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, color);
				}
				//�T���͈͂̕`��
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

	//�}�E�X�J�[�\��
	DrawFunc::DrawCircle2D(UsersInput::Instance()->GetMousePos(), WAYPOINT_RADIUS, Color(0, 0, 255, 255));



#endif
}

void NavigationAI::ImGuiDraw()
{
	ImGui::Begin("Navigation");
	ImGui::Checkbox("SearchCircle", &serachFlag);
	ImGui::Checkbox("WayPoint", &wayPointFlag);
	ImGui::Checkbox("MoveLine", &lineFlag);
	ImGui::InputInt("LayerNumber", &layerNum);
	ImGui::Text("A*Data");
	ImGui::Text("StartHandle,X:%d,Y:%d", startPoint.handle.x, startPoint.handle.y);
	ImGui::Text("EndHandle,X:%d,Y:%d", endPoint.handle.x, endPoint.handle.y);
	ImGui::Text("SearchHandle");
	if (searchMap.size() <= layerNum)
	{
		layerNum = searchMap.size() - 1;
	}
	if (0 <= layerNum && searchMap[layerNum].size() != 0)
	{
		for (int i = 0; i < searchMap[layerNum].size(); ++i)
		{
			Vec2<int>handle = searchMap[layerNum][i].handle;
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(handle.x) + ",Y:" +
				std::to_string(handle.y);
			ImGui::Text(name.c_str());
		}
	}
	ImGui::End();

	if (checkingHandle.x != -1 && checkingHandle.y != -1)
	{
		Vec2<int>handle = checkingHandle;
		ImGui::Begin("WayPointData");
		ImGui::Text("Handle:X%d,Y:%d", wayPoints[handle.y][handle.x].handle.x, wayPoints[handle.y][handle.x].handle.y);
		ImGui::Text("Pos:X%f,Y:%f", wayPoints[handle.y][handle.x].pos.x, wayPoints[handle.y][handle.x].pos.y);
		ImGui::Text("Radius:%f", wayPoints[handle.y][handle.x].radius);
		ImGui::Text("RelateHandles");
		for (int i = 0; i < wayPoints[handle.y][handle.x].wayPointHandles.size(); ++i)
		{
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(wayPoints[handle.y][handle.x].wayPointHandles[i].x) + ",Y:" +
				std::to_string(wayPoints[handle.y][handle.x].wayPointHandles[i].y);
			ImGui::Text(name.c_str());
		}
		ImGui::End();
	}
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

void NavigationAI::AStart(const WayPointData &START_POINT, const WayPointData &END_POINT)
{
	std::vector<WayPointData>startPoint;//�E�F�C�|�C���g�̒T���J�n�ʒu
	std::vector<WayPointData>nextPoint; //���̃E�F�C�|�C���g�̒T���J�n�ʒu
	nextPoint.push_back(START_POINT);
	searchMap.clear();
	queue.clear();

	//���Ɍ������ׂ��ꏊ��T������
	while (1)
	{
		//���ɒT������E�F�C�|�C���g�̃f�[�^���X�^�[�g�n�_�Ƃ���
		startPoint = nextPoint;
		//���̃E�F�C�|�C���g�̏���~����ׂɋ󂫂�p�ӂ���
		nextPoint.clear();

		bool goalFlag = false;

		searchMap.push_back({});
		//���ݒǐՒ��̃��[�g���T��
		for (int startPointIndex = 0; startPointIndex < startPoint.size(); ++startPointIndex)
		{
			for (int i = 0; i < startPoint[startPointIndex].wayPointHandles.size(); ++i)
			{
				//�Q�Ƃ���E�F�C�|�C���g�̎w��
				Vec2<int>handle(startPoint[startPointIndex].wayPointHandles[i]);
				//�q���[���X�e�B�b�N����l����T������ׂ����ǂ������f����
				float nowHeuristicValue = startPoint[startPointIndex].pos.Distance(END_POINT.pos);						//���ݒn����̃S�[���܂ł̃q���[���X�e�B�b�N����l
				float nextHeuristicValue = wayPoints[handle.y][handle.x].pos.Distance(END_POINT.pos);	//�Q�Ƃ��Ă���E�F�C�|�C���g����S�[���܂ł̃q���[���X�e�B�b�N����l

				//�q���[���X�e�B�b�N��0�Ȃ�S�[���ɂ��ǂ蒅�������ɂȂ�̂ŒT�����Ȃ�
				if (nowHeuristicValue <= 0.0f)
				{
					continue;
				}

				int layerArrayNum = searchMap.size() - 1;
				int nowHandleArrayNum = searchMap[layerArrayNum].size();
				//�T�����Ă��Ȃ�������f�o�b�N����ǉ�����
				if (!CheckQueue(handle))
				{
					searchMap[layerArrayNum].push_back(SearchMapData(handle, Color(0, 255, 0, 255)));
				}
				//���ݒn���q���[���X�e�B�b�N����l��������&&�L���[�ɃX�^�b�N���ĂȂ��Ȃ�X�^�b�N����
				if (nextHeuristicValue <= nowHeuristicValue && !CheckQueue(handle))
				{
					searchMap[layerArrayNum][nowHandleArrayNum].color = Color(223, 144, 53, 255);

					//�L���[�ɂ̓n���h���ƃq���[���X�e�B�b�N����l+�p�X���̍��v�l���X�^�b�N����
					queue.push_back(QueueData(handle, nextHeuristicValue));
					//���ɒT������n�_���L�^����
					nextPoint.push_back(wayPoints[handle.y][handle.x]);
				}
			}
		}

		//���ɒT���ł���ꏊ��������ΒT���I��
		if (nextPoint.size() == 0)
		{
			break;
		}
	}
}

inline bool NavigationAI::CheckQueue(const Vec2<int> &HANDLE)
{
	for (int i = 0; i < queue.size(); ++i)
	{
		if (queue[i].handle == HANDLE)
		{
			return true;
		}
	}
	return false;
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
					wayPoints[y][x].RegistHandle(DATA->handle);
					DATA->RegistHandle({ x,y });
				}
			}
		}
	}
}
