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
	serachFlag = false;
	lineFlag = true;

	checkingHandle = { -1,-1 };
	checkTimer = 0;
}

void NavigationAI::Update(const Vec2<float> &POS)
{
#ifdef _DEBUG

	resetSearchFlag = false;
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
		resetSearchFlag = true;
	}
	oldStartPoint = startPoint;
	oldEndPoint = endPoint;

	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			debugColor[y][x] = Color(255, 255, 255, 255);
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

#endif // DEBUG

	//�X�^�[�g�ƃS�[����ݒ肵����A�X�^�[�̒T�����J�n����
	if (startPoint.handle != Vec2<int>(-1, -1) &&
		endPoint.handle != Vec2<int>(-1, -1))
	{
		AStart(startPoint, endPoint);
	}
}

void NavigationAI::Draw()
{
	//�f�o�b�N�̎��̂ݕ\��
#ifdef _DEBUG

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

					//���̏ꏊ���}�E�X�J�[�\���ƍ�������m�F���̏ꏊ���ƔF������
					isCheckingFlag = checkingHandle.x == wayPoints[y][x].handle.x && checkingHandle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}

					//�ŒZ���[�g�̕`��
					for (int i = 0; i < queue.size(); ++i)
					{
						if (queue[i].handle == wayPoints[y][x].handle)
						{
							color = Color(255, 0, 255, 255);
							break;
						}
					}

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

					//�E�F�C�|�C���g�̕`��
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, color);
				}
				//�T���͈͂̕`��
				if (serachFlag)
				{
					for (int i = 0; i < 8; ++i)
					{
						int angle = i * (360.0f / 8.0f);
						float distance = 250.0f;

						int adj = 15;
						float upDown = 250.0f;
						float leftRight = 400.0f;
						float naname = 400.0f;

						if (angle == 0)
						{
							distance = leftRight;
						}
						else if (angle == 45)
						{
							angle += adj;
							distance = naname;
						}
						else if (angle == 90)
						{
							distance = upDown;
						}
						else if (angle == 135)
						{
							angle += adj;
							distance = naname;
						}
						else if (angle == 180)
						{
							distance = leftRight;
						}
						else if (angle == 225)
						{
							angle -= adj;
							distance = naname;
						}
						else if (angle == 270)
						{
							distance = upDown;
						}
						else if (angle == 315)
						{
							angle += adj;
							distance = naname;
						}
						else
						{
							distance = leftRight;
						}

						float dir = Angle::ConvertToRadian(angle);




						Vec2<float>lineEndPos(cosf(dir), sinf(dir));
						DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), ScrollMgr::Instance()->Affect(wayPoints[y][x].pos + lineEndPos * distance), Color(255, 255, 255, 255));
					}
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


	ImGui::Begin("Queue");
	for (int i = 0; i < queue.size(); ++i)
	{
		Vec2<int>handle = queue[i].handle;
		std::string name = "Handle:" + std::to_string(i) + ",X:" +
			std::to_string(handle.x) + ",Y:" +
			std::to_string(handle.y) + ",Distance+Pass:" + std::to_string(queue[i].sum);
		ImGui::Text(name.c_str());
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
		ImGui::Text("PassNum:%d", wayPoints[handle.y][handle.x].passNum);
		ImGui::End();
	}
}

std::vector<WayPointData> NavigationAI::GetShortestRoute()
{
	std::vector<WayPointData>result;
	for (int i = queue.size() - 1; 0 <= i; --i)
	{
		int x = queue[i].handle.x;
		int y = queue[i].handle.y;
		result.push_back(wayPoints[y][x]);
	}

	return result;
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
	return DATA.handle != Vec2<int>(-1, -1);
}

void NavigationAI::AStart(const WayPointData &START_POINT, const WayPointData &END_POINT)
{
	std::vector<WayPointData>startPoint;//�E�F�C�|�C���g�̒T���J�n�ʒu
	std::vector<WayPointData>nextPoint; //���̃E�F�C�|�C���g�̒T���J�n�ʒu
	std::vector<WayPointData>failPoint; //�T�����O�̃E�F�C�|�C���g
	nextPoint.push_back(START_POINT);
	searchMap.clear();
	queue.clear();
	bool failFlag = false;

	//���Ɍ������ׂ��ꏊ��T������
	while (1)
	{
		//���ɒT������E�F�C�|�C���g�̃f�[�^���X�^�[�g�n�_�Ƃ���
		startPoint = nextPoint;
		//���̃E�F�C�|�C���g�̏���~����ׂɋ󂫂�p�ӂ���
		nextPoint.clear();

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
				//�X�^�[�g�n�_����L���[�ɓ���Ȃ�
				if (nowHeuristicValue <= 0.0f || wayPoints[handle.y][handle.x].handle == START_POINT.handle)
				{
					continue;
				}

				int layerArrayNum = searchMap.size() - 1;
				int nowHandleArrayNum = searchMap[layerArrayNum].size();
				//�T�����Ă��Ȃ�������f�o�b�N����ǉ�����
				if (!CheckQueue(handle))
				{
					searchMap[layerArrayNum].push_back(SearchMapData(handle, Color(0, 255, 0, 255)));

					//���ݒn���q���[���X�e�B�b�N����l�����������ĂȂ��Ȃ�X�^�b�N����
					if (nextHeuristicValue <= nowHeuristicValue)
					{
						//�T�����s���ォ��n�߂Ă���ۂ́A�Q�Ƃ����E�F�C�|�C���g�����������ہA�O�̃E�F�C�|�C���g�����ɉ�����
						if (failFlag)
						{
							//�L���[�ɂ̓n���h���ƌ��ݒn����S�[���܂ł̋���(�q���[���X�e�B�b�N����l)���X�^�b�N����
							queue.push_back(QueueData(startPoint[startPointIndex].handle, nowHeuristicValue));
						}


						searchMap[layerArrayNum][nowHandleArrayNum].color = Color(223, 144, 53, 255);
						//�p�X���̋L�^
						wayPoints[handle.y][handle.x].passNum = startPoint[startPointIndex].passNum + 1;
						//�L���[�ɂ̓n���h���ƌ��ݒn����S�[���܂ł̋���(�q���[���X�e�B�b�N����l)���X�^�b�N����
						queue.push_back(QueueData(handle, nextHeuristicValue + wayPoints[handle.y][handle.x].passNum));
						//���ɒT������n�_���L�^����
						nextPoint.push_back(wayPoints[handle.y][handle.x]);
					}
					else
					{
						failPoint.push_back(wayPoints[handle.y][handle.x]);
					}
				}

			}
		}

		failFlag = false;

		//���ɒT���ł���ꏊ��������΃S�[���܂ł��ǂ蒅���Ă��邩�m�F����
		if (nextPoint.size() == 0)
		{
			bool goalFlag = false;
			for (int i = 0; i < queue.size(); ++i)
			{
				if (queue[i].handle == END_POINT.handle)
				{
					goalFlag = true;
					break;
				}
			}

			//�ړI�n�܂ł��ǂ蒅���Ă�����T�����I����
			if (goalFlag)
			{
				break;
			}
			//�����łȂ���Ύ��ɑ傫�����v�l��nextPoint�ɓ���čĒT������
			else
			{
				nextPoint = failPoint;
				failFlag = true;

				//�\�[�g����ۂɏ��Ԃ�����Ȃ��悤���܂ł̃L���[�ɒ���������
				for (int i = 0; i < queue.size(); ++i)
				{
					queue[i].sum += 15.0f;
				}
			}
		}
	}


	queue = SortQueue(queue);

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

std::vector<NavigationAI::QueueData> NavigationAI::SortQueue(const std::vector<QueueData> &QUEUE)
{
	std::vector<float>sumArray;
	for (int i = 0; i < QUEUE.size(); ++i)
	{
		sumArray.push_back(QUEUE[i].sum);
	}
	std::sort(sumArray.begin(), sumArray.end());

	//�\�[�g�������ɕ��ёւ�
	std::vector<QueueData>sortData;
	for (int sumIndex = 0; sumIndex < sumArray.size(); ++sumIndex)
	{
		for (int queueIndex = 0; queueIndex < QUEUE.size(); ++queueIndex)
		{
			bool sameSumFlag = QUEUE[queueIndex].sum == sumArray[sumIndex];
			if (sameSumFlag)
			{
				sortData.push_back(QUEUE[queueIndex]);
			}
		}
	}


	int resultHandle = 0;
	std::vector<QueueData> result;
	std::vector<QueueData> fail;

	while (1)
	{
		for (int sortIndex = 0; sortIndex < sortData.size(); ++sortIndex)
		{
			Vec2<int>handle(sortData[sortIndex].handle);

			//�O�̃E�F�C�|�C���g�ƌq�����Ă��邩�ǂ���
			if (result.size() != 0)
			{
				for (int linkHandle = 0; linkHandle < wayPoints[handle.y][handle.x].wayPointHandles.size(); ++linkHandle)
				{
					//�E�F�C�|�C���g���q�����Ă��邩�ǂ���
					bool linkFlag = wayPoints[handle.y][handle.x].wayPointHandles[linkHandle] == result[resultHandle].handle;

					//����
					if (linkFlag)
					{
						result.push_back(sortData[sortIndex]);
						++resultHandle;
					}
					//���s
					else if (linkFlag)
					{
						fail.push_back(sortData[sortIndex]);
					}
				}
			}
			else
			{
				result.push_back(sortData[sortIndex]);
			}
		}




		////�\�[�g���ɕ��ׂ�&&�O�̃E�F�C�|�C���g�ƌq�����Ă���ӏ����q���Ă���
		//for (int sumIndex = 0; sumIndex < sumArray.size(); ++sumIndex)
		//{
		//	for (int queueIndex = 0; queueIndex < QUEUE.size(); ++queueIndex)
		//	{
		//		//�\�[�g���ɕ��ׂ鍇�}
		//		bool sameSumFlag = QUEUE[queueIndex].sum == sumArray[sumIndex];
		//		if (sameSumFlag)
		//		{
		//			//�O�̃E�F�C�|�C���g�ƌq�����Ă��邩�ǂ���
		//			if (result.size() != 0)
		//			{
		//				Vec2<int>handle(QUEUE[queueIndex].handle);
		//				for (int linkHandle = 0; linkHandle < wayPoints[handle.y][handle.x].wayPointHandles.size(); ++linkHandle)
		//				{
		//					//�E�F�C�|�C���g���q�����Ă��邩�ǂ���
		//					bool linkFlag = wayPoints[handle.y][handle.x].wayPointHandles[linkHandle] == result[resultHandle].handle;

		//					//�q���[���X�e�B�b�N�͌��ݒn��菬������
		//					bool distanceFlag = true;

		//					//����
		//					if (linkFlag && distanceFlag)
		//					{
		//						result.push_back(QUEUE[queueIndex]);
		//						++resultHandle;
		//					}
		//					//���s
		//					else if (linkFlag)
		//					{
		//						fail.push_back(QUEUE[queueIndex]);
		//					}
		//				}
		//			}
		//			else
		//			{
		//				result.push_back(QUEUE[queueIndex]);
		//			}
		//		}
		//	}
		//}

		bool startFlag = false;
		bool goalFlag = false;
		//�X�^�[�g�ƃS�[�����q�����Ă��Ȃ���ΒT��������
		for (int i = 0; i < result.size(); ++i)
		{
			if (result[i].handle == Vec2<int>())
			{

			}
			if (result[i].handle == Vec2<int>())
			{

			}
		}

		//�X�^�[�g����S�[���܂Ōq�������琬���A�ŒZ���[�g���Ĉ���
		if (startFlag && goalFlag)
		{

		}
		//���s������fail�������T��
		else
		{

		}

		break;
	}

	QueueData startPoint(startPoint.handle, 0.0f);
	result.push_back(startPoint);

	return result;
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
				//�ړ��͈͓��ɃE�F�C�|�C���g�����邩
				Vec2<float>lineStartPos = DATA->pos;
				Vec2<float>circlePos = wayPoints[y][x].pos;
				float r = wayPoints[y][x].radius;

				bool serachFlag = false;
				//���S���甪�����ɐL�т����ƃE�F�C�|�C���g�̔���
				for (int i = 0; i < 8; ++i)
				{
					int angle = i * (360.0f / 8.0f);
					float distance = 0.0f;

					int adj = 15;

					float upDown = 250.0f;
					float leftRight = 400.0f;
					float naname = 400.0f;

					if (angle == 0)
					{
						distance = leftRight;
					}
					else if (angle == 45)
					{
						angle += adj;
						distance = naname;
					}
					else if (angle == 90)
					{
						distance = upDown;
					}
					else if (angle == 135)
					{
						angle += adj;
						distance = naname;
					}
					else if (angle == 180)
					{
						distance = leftRight;
					}
					else if (angle == 225)
					{
						angle -= adj;
						distance = naname;
					}
					else if (angle == 270)
					{
						distance = upDown;
					}
					else if (angle == 315)
					{
						angle += adj;
						distance = naname;
					}
					else
					{
						distance = leftRight;
					}
					float dir = Angle::ConvertToRadian(angle);
					Vec2<float>lineEndPos(cosf(dir), sinf(dir));
					if (CheckHitLineCircle(lineStartPos, lineStartPos + lineEndPos * distance, circlePos, r))
					{
						serachFlag = true;
						break;
					}
				}

				//�����ǂɓ������Ă��邩�ǂ���
				bool canMoveFlag = false;
				if (serachFlag)
				{
					canMoveFlag = !CheckMapChipWallAndRay(*HANDLE.center, circlePos);
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
