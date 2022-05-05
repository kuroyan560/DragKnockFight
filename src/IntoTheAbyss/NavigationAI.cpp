#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"../Engine/UsersInput.h"
#include"../IntoTheAbyss/DebugKeyManager.h"
#include<queue>
#include "StaminaItemMgr.h"

const float NavigationAI::SERACH_RADIUS = 180.0f;
const float NavigationAI::WAYPOINT_RADIUS = 20.0f;
const float NavigationAI::SERACH_LAY_UPDOWN_DISTANCE = 300.0f;
const float NavigationAI::SERACH_LAY_LEFTRIGHT_DISTANCE = 350.0f;
const float NavigationAI::SERACH_LAY_NANAME_DISTANCE = 400.0f;

void NavigationAI::Init(const RoomMapChipArray& MAP_DATA)
{
	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//���W�̐ݒ�--------------------------
	int xNum = MAP_DATA[0].size();
	int yNum = MAP_DATA.size();

	// �}�b�v�`�b�v�̐����E�F�C�|�C���g�𐶐��B
	wayPoints.resize(yNum);
	debugColor.resize(yNum);
	for (int index = 0; index < yNum; ++index) {
		wayPoints[index].resize(xNum);
		debugColor[index].resize(xNum);
	}

	// �E�F�C�|�C���g�̐���ۑ��B
	wayPointXCount = wayPoints[0].size() - 1;
	wayPointYCount = wayPoints.size() - 1;

	for (int y = 0; y < yNum; ++y)
	{
		for (int x = 0; x < xNum; ++x)
		{
			debugColor[y][x] = Color(255, 255, 255, 255);

			//��x���[���h���W�ɕϊ����Ă���}�b�v�`�b�v���W�ɕϊ�����
			const Vec2<float> worldPos(MAP_CHIP_SIZE * x, MAP_CHIP_SIZE * y);
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

			wayPoints[y][x] = std::make_shared<WayPointData>();
			//���Ԋu�ŊJ����
			wayPoints[y][x]->pos = worldPos;
			wayPoints[y][x]->radius = WAYPOINT_RADIUS;
			wayPoints[y][x]->handle = { x,y };
			wayPoints[y][x]->isWall = wallFlag;
			wayPoints[y][x]->numberOfItemHeld = 0;
		}
	}
	//���W�̐ݒ�--------------------------


	//�߂��ɂ���E�F�C�|�C���g�ւ̌q��--------------------------
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (!DontUse(wayPoints[y][x]->handle)) continue;
			if (wayPoints[y][x]->isWall) continue;		// �ǂ̏ꍇ�͌q���Ȃ��B
			RegistHandle(wayPoints[y][x]);
		}
	}
	//�߂��ɂ���E�F�C�|�C���g�ւ̌q��--------------------------

	wayPointFlag = true;
	serachFlag = false;
	lineFlag = true;

	checkingHandle = { -1,-1 };
	checkTimer = 0;
	startFlag = false;
}

void NavigationAI::Update(const Vec2<float>& POS)
{

	// �E�F�C�|�C���g�̃A�C�e���ێ������v�Z����B
	CheckNumberOfItemHeldCount();

#ifdef _DEBUG

	resetSearchFlag = false;
	Vec2<float>pos = UsersInput::Instance()->GetMousePos();

	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				SphereCollision data1, data2;

				//�X�N���[�����W�Ɣ�������ׂɃX�N���[�����W�ɕϊ�����
				data1.center = &ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos);
				data1.radius = wayPoints[y][x]->radius;
				data2.center = &pos;
				data2.radius = WAYPOINT_RADIUS;

				bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(data1, data2);

				//�}�E�X�J�[�\���ƍ�������E�F�C�|�C���g�̏����Q�Ƃ���
				if (hitFlag)
				{
					checkingHandle = wayPoints[y][x]->handle;
					checkTimer = 0;
				}
				//�E�N���b�N������X�^�[�g�n�_�ɐݒ肷��
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_H, "SetStart", "DIK_H"))
				{
					startPoint = *wayPoints[y][x];
				}
				//���N���b�N������S�[���n�_�ɐݒ肷��
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_N, "SetGoal", "DIK_N"))
				{
					endPoint = *wayPoints[y][x];
				}
			}
		}
	}

	//�ܕb�ȏ�ǂ̃E�F�C�|�C���g���Q�Ƃ��Ȃ�������E�F�C�|�C���g����`�悵�Ȃ�--------------------------
	++checkTimer;
	if (60 * 5 <= checkTimer)
	{
		checkingHandle = { -1,-1 };
	}
	//�ܕb�ȏ�ǂ̃E�F�C�|�C���g���Q�Ƃ��Ȃ�������E�F�C�|�C���g����`�悵�Ȃ�--------------------------


	//�X�^�[�g�n�_���S�[���n�_��ς���ۂ͔����ꂷ��
	if (startPoint.handle != prevStartPoint.handle || endPoint.handle != prevEndPoint.handle)
	{
		resetSearchFlag = true;
	}
	prevStartPoint = startPoint;
	prevEndPoint = endPoint;

	//���t���[���������̕K�v�̂��镨������������
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			debugColor[y][x] = Color(255, 255, 255, 255);
			wayPoints[y][x]->branchReferenceCount = 0;
			wayPoints[y][x]->branchHandle = -1;
		}
	}

	//�i�K�ɕ����ĒT������F���o�͂���
	for (int layer = 0; layer < layerNum; ++layer)
	{
		for (int num = 0; num < searchMap[layer].size(); ++num)
		{
			Vec2<int>handle = searchMap[layer][num].handle;
			debugColor[handle.y][handle.x] = searchMap[layer][num].color;
		}
	}

#endif // DEBUG

	if (startPoint.handle != Vec2<int>(-1, -1)&&
		endPoint.handle != Vec2<int>(-1, -1))
	{
		startFlag = true;
	}


	//�X�^�[�g�ƃS�[����ݒ肵����A�X�^�[�̒T�����J�n����
	if (startFlag)
	{
		AStart(startPoint, endPoint);
	}
}

void NavigationAI::Draw()
{
	//�f�o�b�N�̎��̂ݕ\��
#ifdef _DEBUG

	//�E�F�C�|�C���g�̕`��
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				if (wayPointFlag)
				{
					bool isCheckingFlag = false;
					Color color = debugColor[y][x];

					//���̏ꏊ���}�E�X�J�[�\���ƍ�������m�F���̏ꏊ���ƔF������
					isCheckingFlag = checkingHandle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}

					//�ŒZ���[�g�̕`��
					for (int i = 0; i < shortestRoute.size(); ++i)
					{
						if (shortestRoute[i]->handle == wayPoints[y][x]->handle)
						{
							color = Color(255, 0, 255, 255);
							break;
						}
					}

					//���̏ꏊ���X�^�[�g�n�_�Ȃ�F��ς���
					isCheckingFlag = startPoint.handle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(255, 255, 0, 255);
					}

					//���̏ꏊ���S�[���n�_�Ȃ�F��ς���
					isCheckingFlag = endPoint.handle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(0, 255, 255, 255);
					}

					//�E�F�C�|�C���g�̕`��
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos), wayPoints[y][x]->radius, color);
				}
				//�T���͈͂̕`��
				if (serachFlag)
				{
					for (int i = 0; i < 8; ++i)
					{
						Vec2<float>endPos = CaluLine(wayPoints[y][x]->pos, i * (360.0f / 8.0f));

						DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos), ScrollMgr::Instance()->Affect(endPos), Color(255, 255, 255, 255));
					}
				}
			}
		}
	}

	////�q����̕`��
	//if (lineFlag)
	//{
	//	for (int y = 0; y < wayPointYCount; ++y)
	//	{
	//		for (int x = 0; x < wayPointXCount; ++x)
	//		{
	//			if (wayPoints[y][x].get() == nullptr) continue;
	//			if (DontUse(wayPoints[y][x]->handle))
	//			{
	//				//�o�^�����n���h����������q����
	//				for (int i = 0; i < wayPoints[y][x]->wayPointHandles.size(); ++i)
	//				{
	//					Vec2<int> endHandle = wayPoints[y][x]->wayPointHandles[i];
	//					DrawFunc::DrawLine2D
	//					(
	//						ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos),
	//						ScrollMgr::Instance()->Affect(wayPoints[endHandle.y][endHandle.x]->pos),
	//						Color(0, 155, 0, 255)
	//					);
	//				}
	//			}
	//		}
	//	}
	//}

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
		Vec2<int>handle = queue[i]->handle;
		std::string name = "Handle:" + std::to_string(i) + ",X:" +
			std::to_string(handle.x) + ",Y:" +
			std::to_string(handle.y) + ",Distance+Pass:" + std::to_string(queue[i]->sum);
		ImGui::Text(name.c_str());
	}
	ImGui::End();


	ImGui::Begin("ShortestRoute");
	ImGui::Text("ShortestRoute");
	for (int branch = 0; branch < shortestRoute.size(); ++branch)
	{
		ImGui::Text("Handle:%d,X:%d,Y:%d", branch, shortestRoute[branch]->handle.x, shortestRoute[branch]->handle.y);
	}
	ImGui::Text("");
	for (int branchNum = 0; branchNum < branchQueue.size(); ++branchNum)
	{
		ImGui::Text("Route:%d", branchNum);
		if (reachToGoalFlag[branchNum])
		{
			ImGui::SameLine();
			ImGui::Text("Reach!!!");
		}
		for (int route = 0; route < branchQueue[branchNum].size(); ++route)
		{
			ImGui::Text("Handle:%d,X:%d,Y:%d",
				route,
				branchQueue[branchNum][route]->handle.x,
				branchQueue[branchNum][route]->handle.y
			);
		}
		ImGui::Text("");
	}
	ImGui::End();




	if (checkingHandle.x != -1 && checkingHandle.y != -1)
	{
		Vec2<int>handle = checkingHandle;
		ImGui::Begin("WayPointData");
		ImGui::Text("Handle:X%d,Y:%d", wayPoints[handle.y][handle.x]->handle.x, wayPoints[handle.y][handle.x]->handle.y);
		ImGui::Text("Pos:X%f,Y:%f", wayPoints[handle.y][handle.x]->pos.x, wayPoints[handle.y][handle.x]->pos.y);
		ImGui::Text("Radius:%f", wayPoints[handle.y][handle.x]->radius);
		ImGui::Text("RelateHandles");
		for (int i = 0; i < wayPoints[handle.y][handle.x]->wayPointHandles.size(); ++i)
		{
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(wayPoints[handle.y][handle.x]->wayPointHandles[i].x) + ",Y:" +
				std::to_string(wayPoints[handle.y][handle.x]->wayPointHandles[i].y);
			ImGui::Text(name.c_str());
		}
		ImGui::Text("PassNum:%d", wayPoints[handle.y][handle.x]->passNum);
		ImGui::End();
	}
}

std::vector<WayPointData> NavigationAI::GetShortestRoute()
{
	//�L���[����E�F�C�|�C���g�̔z��ɕϊ�����
	std::vector<WayPointData>result;
	for (int i = 0; i < shortestRoute.size(); ++i)
	{
		int x = shortestRoute[i]->handle.x;
		int y = shortestRoute[i]->handle.y;
		result.push_back(*wayPoints[y][x]);
	}
	return result;

}

inline const Vec2<int>& NavigationAI::GetMapChipNum(const Vec2<float>& WORLD_POS)
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

inline bool NavigationAI::DontUse(const Vec2<int>& HANDLE)
{
	return HANDLE != Vec2<int>(-1, -1);
}

void NavigationAI::AStart(const WayPointData& START_POINT, const WayPointData& END_POINT)
{
	std::vector<std::shared_ptr<WayPointData>>startPoint;//�E�F�C�|�C���g�̒T���J�n�ʒu
	std::vector<std::shared_ptr<WayPointData>>nextPoint; //���̃E�F�C�|�C���g�̒T���J�n�ʒu
	std::vector<std::shared_ptr<WayPointData>>failPoint; //�T�����O�̃E�F�C�|�C���g

	Vec2<int>sHandle(START_POINT.handle);

	nextPoint.push_back(std::make_shared<WayPointData>());
	nextPoint[0] = wayPoints[sHandle.y][sHandle.x];
	searchMap.clear();
	queue.clear();
	branchQueue.clear();
	bool failFlag = false;

	//�ŏ��̃u�����`�����
	branchQueue.push_back({});
	branchQueue[0].push_back(std::make_shared<WayPointData>());
	branchQueue[0][0] = wayPoints[sHandle.y][sHandle.x];
	branchQueue[0][0]->branchHandle = 0;

	//���Ɍ������ׂ��ꏊ��T������
	while (1)
	{
		//���ɒT������E�F�C�|�C���g�̃f�[�^���X�^�[�g�n�_�Ƃ���
		startPoint = nextPoint;
		//���̃E�F�C�|�C���g�̏���~����ׂɋ󂫂�p�ӂ���
		for (int i = 0; i < nextPoint.size(); ++i)
		{
			nextPoint.pop_back();
		}


		searchMap.push_back({});
		//���ݒǐՒ��̃��[�g���T��
		for (int startPointIndex = 0; startPointIndex < startPoint.size(); ++startPointIndex)
		{
			for (int i = 0; i < startPoint[startPointIndex]->wayPointHandles.size(); ++i)
			{
				//�Q�Ƃ���E�F�C�|�C���g�̎w��
				Vec2<int>handle(startPoint[startPointIndex]->wayPointHandles[i]);
				//�q���[���X�e�B�b�N����l����T������ׂ����ǂ������f����
				float nowHeuristicValue = startPoint[startPointIndex]->pos.Distance(END_POINT.pos);						//���ݒn����̃S�[���܂ł̃q���[���X�e�B�b�N����l
				float nextHeuristicValue = wayPoints[handle.y][handle.x]->pos.Distance(END_POINT.pos);	//�Q�Ƃ��Ă���E�F�C�|�C���g����S�[���܂ł̃q���[���X�e�B�b�N����l

				//�q���[���X�e�B�b�N��0�Ȃ�S�[���ɂ��ǂ蒅�������ɂȂ�̂ŒT�����Ȃ�
				//�X�^�[�g�n�_����L���[�ɓ���Ȃ�
				if (wayPoints[handle.y][handle.x]->handle == START_POINT.handle)
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
					if (nextHeuristicValue < nowHeuristicValue)
					{
						//�T�����s���ォ��n�߂Ă���ۂ́A�Q�Ƃ����E�F�C�|�C���g�����������ہA�O�̃E�F�C�|�C���g�����ɉ�����
						if (failFlag)
						{
							//�L���[�ɂ̓n���h���ƌ��ݒn����S�[���܂ł̋���(�q���[���X�e�B�b�N����l)���X�^�b�N����
							queue.push_back(std::make_shared<QueueData>(startPoint[startPointIndex]->handle, nowHeuristicValue));
						}

						searchMap[layerArrayNum][nowHandleArrayNum].color = Color(223, 144, 53, 255);
						//�p�X���̋L�^
						wayPoints[handle.y][handle.x]->passNum = startPoint[startPointIndex]->passNum + 1;
						//�L���[�ɂ̓n���h���ƌ��ݒn����S�[���܂ł̋���(�q���[���X�e�B�b�N����l)���X�^�b�N����
						queue.push_back(std::make_shared<QueueData>(handle, nextHeuristicValue + wayPoints[handle.y][handle.x]->passNum));

						//���ɒT������n�_���L�^����,�S�[���̏ꍇ�͎��̒T���̌��ɂ��Ȃ�
						if (endPoint.handle != handle)
						{
							nextPoint.push_back(std::make_shared<WayPointData>());
							nextPoint[nextPoint.size() - 1] = wayPoints[handle.y][handle.x];
						}

						//���ݒn�̃E�F�C�|�C���g�̃u�����`�̃n���h��������
						Vec2<int>startHandle(startPoint[startPointIndex]->handle);
						//�T�����̃��[�g��ۑ�����
						if (wayPoints[handle.y][handle.x]->branchHandle == -1)
						{
							//���̃u�����`�̒T�����ꂽ�񐔂����Ȃ�ǉ�����
							if (wayPoints[startHandle.y][startHandle.x]->branchReferenceCount < 1)
							{
								//���ݒn�̃E�F�C�|�C���g�̃u�����`�n���h������Q�Ɛ�̃E�F�C�|�C���g�Ƀu�����`�n���h����n������A���[�g�ǉ�
								int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
								wayPoints[handle.y][handle.x]->branchHandle = branchHandle;
								++wayPoints[startHandle.y][startHandle.x]->branchReferenceCount;

								branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
								branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
							}
							//���ȏ�T�����ꂽ��ʂ̃u�����`���쐬���A���܂ł̃��[�g��n���ǉ�����
							else
							{
								//�u�����`��ǉ����A�n���h����n��
								branchQueue.push_back({});
								int newbranchHandle = branchQueue.size() - 1;
								wayPoints[handle.y][handle.x]->branchHandle = newbranchHandle;

								int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
								//���ɕʂ̃E�F�C�|�C���g���ǉ�����Ă���̂ōŌ�̏ꏊ�ɏ㏑������
								branchQueue[newbranchHandle] = branchQueue[branchHandle];
								branchQueue[newbranchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
							}
						}
						//���ݒn���S�[���ł��̃S�[�������̃u�����`�ɂ��ǉ��ς݂̏ꍇ�A���̃u�����`�ɂ��ǉ��ł���悤�ɂ���
						else if (wayPoints[handle.y][handle.x]->branchHandle != -1 && endPoint.handle == handle)
						{
							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
							branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
					}
					//�T���Ɏ��s�����E�F�C�|�C���g���L�^����
					else
					{
						failPoint.push_back(std::make_shared<WayPointData>());
						failPoint[failPoint.size() - 1] = wayPoints[handle.y][handle.x];
						//failPoint[failPoint.size() - 1]->branchHandle = startPoint[startPointIndex]->branchHandle;
						Vec2<int>startHandle(startPoint[startPointIndex]->handle);

						//���̃u�����`�̒T�����ꂽ�񐔂����Ȃ�ǉ�����
						if (wayPoints[startHandle.y][startHandle.x]->branchReferenceCount < 1)
						{
							//���ݒn�̃E�F�C�|�C���g�̃u�����`�n���h������Q�Ɛ�̃E�F�C�|�C���g�Ƀu�����`�n���h����n������A���[�g�ǉ�
							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							wayPoints[handle.y][handle.x]->branchHandle = branchHandle;
							++wayPoints[startHandle.y][startHandle.x]->branchReferenceCount;

							branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
							branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
						//���ȏ�T�����ꂽ��ʂ̃u�����`���쐬���A���܂ł̃��[�g��n���ǉ�����
						else
						{
							//�u�����`��ǉ����A�n���h����n��
							branchQueue.push_back({});
							int newbranchHandle = branchQueue.size() - 1;
							wayPoints[handle.y][handle.x]->branchHandle = newbranchHandle;

							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							//���ɕʂ̃E�F�C�|�C���g���ǉ�����Ă���̂ōŌ�̏ꏊ�ɏ㏑������
							branchQueue[newbranchHandle] = branchQueue[branchHandle];
							branchQueue[newbranchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
					}
				}

			}
		}
		//���̏ꏊ��T��������failFlag�������Ă����牺�낷
		failFlag = false;

		//���ɒT���ł���ꏊ��������΃S�[���܂ł��ǂ蒅���Ă��邩�m�F����
		if (nextPoint.size() == 0)
		{
			bool goalFlag = false;
			for (int i = 0; i < queue.size(); ++i)
			{
				if (queue[i]->handle == END_POINT.handle)
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
			//�����łȂ���ΒT���Ɏ��s�����E�F�C�|�C���g�B��nextPoint�ɓ���čĒT������
			else
			{
				nextPoint = failPoint;
				failFlag = true;
				//�\�[�g����ۂɏ��Ԃ�����Ȃ��悤���܂ł̃L���[�ɒ���������
				//���݂̃E�F�C�|�C���g�̊Ԋu����Z�o���Ă���
				for (int i = 0; i < queue.size(); ++i)
				{
					queue[i]->sum += 325.0f / 2.0f;
				}
			}
		}
	}

	//���܂ł̌�₩��ŒZ���[�g�����
	queue = ConvertToShortestRoute(queue);
	shortestRoute = ConvertToShortestRoute2(branchQueue);
}

void NavigationAI::RegistBranch(const WayPointData& DATA)
{
}

inline bool NavigationAI::CheckQueue(const Vec2<int>& HANDLE)
{
	for (int i = 0; i < queue.size(); ++i)
	{
		if (queue[i]->handle == HANDLE && endPoint.handle != HANDLE)
		{
			return true;
		}
	}
	return false;
}

std::vector<std::shared_ptr<NavigationAI::QueueData>> NavigationAI::ConvertToShortestRoute(const std::vector<std::shared_ptr<QueueData>>& QUEUE)
{
	std::vector<float>sumArray;
	for (int i = 0; i < QUEUE.size(); ++i)
	{
		sumArray.push_back(QUEUE[i]->sum);
	}
	std::sort(sumArray.begin(), sumArray.end());

	//�\�[�g�������ɕ��ёւ�
	std::vector<std::shared_ptr<QueueData>>sortData;
	for (int sumIndex = 0; sumIndex < sumArray.size(); ++sumIndex)
	{
		for (int queueIndex = 0; queueIndex < QUEUE.size(); ++queueIndex)
		{
			bool sameSumFlag = QUEUE[queueIndex]->sum == sumArray[sumIndex];
			if (sameSumFlag)
			{
				sortData.push_back(QUEUE[queueIndex]);
			}
		}
	}


	int resultHandle = 0;
	std::vector<std::shared_ptr<QueueData>> result;
	std::vector<std::shared_ptr<QueueData>> fail;

	while (1)
	{
		for (int sortIndex = 0; sortIndex < sortData.size(); ++sortIndex)
		{
			Vec2<int>handle(sortData[sortIndex]->handle);

			//�O�̃E�F�C�|�C���g�ƌq�����Ă��邩�ǂ���
			if (result.size() != 0)
			{
				for (int linkHandle = 0; linkHandle < wayPoints[handle.y][handle.x]->wayPointHandles.size(); ++linkHandle)
				{
					//�E�F�C�|�C���g���q�����Ă��邩�ǂ���
					bool linkFlag = wayPoints[handle.y][handle.x]->wayPointHandles[linkHandle] == result[resultHandle]->handle;

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
			if (result[i]->handle == Vec2<int>())
			{

			}
			if (result[i]->handle == Vec2<int>())
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

	std::shared_ptr<QueueData> startPoint(std::make_shared<QueueData>(startPoint.handle, 0.0f));
	result.push_back(startPoint);

	return result;
}

Vec2<float> NavigationAI::CaluLine(const Vec2<float>& CENTRAL_POS, int angle)
{
	float distance = 0.0f;

	int adj = 15;

	float upDown = SERACH_LAY_UPDOWN_DISTANCE;
	float leftRight = SERACH_LAY_LEFTRIGHT_DISTANCE;
	float naname = SERACH_LAY_NANAME_DISTANCE;

	if (angle == 0)
	{
		distance = leftRight;
	}
	else if (angle == 45)
	{
		angle -= adj;
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

	return CENTRAL_POS + lineEndPos * distance;
}

bool NavigationAI::ConnectWayPoint(std::shared_ptr<WayPointData> DATA, const Vec2<int>& SEARCH_OFFSET, const SizeData& CHIP_DATA)
{

	Vec2<float> searchHandle = { (float)DATA->handle.x + SEARCH_OFFSET.x, (float)DATA->handle.y + SEARCH_OFFSET.y };
	int chipData = StageMgr::Instance()->GetMapChipBlock(0, 0, searchHandle);

	// ����������W���ǂ���Ȃ�������ڑ�����B
	if (!(CHIP_DATA.min <= chipData && chipData <= CHIP_DATA.max)) {

		// �ڑ�����B
		wayPoints[(int)searchHandle.y][(int)searchHandle.x]->RegistHandle(DATA->handle);
		DATA->RegistHandle({ (int)searchHandle.x,(int)searchHandle.y });

		return true;

	}

	return false;

}

float NavigationAI::SearchWall(std::shared_ptr<WayPointData> DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA)
{
	int searchCounter = 0;
	Vec2<float> searchIndex = { (float)DATA->handle.x, (float)DATA->handle.y };

	// �}�b�v�̎l�����ǂň͂܂�Ă��邽�߁A�z��O�փI�[�o�[���邱�Ƃ��Ȃ��Ǝv���̂Ŕz��O�ւ̑Ώ��̏����͏����Ȃ��ł����܂��I��O�X���[�����珑���܂��c�B
	while (1) {

		// �����Ώۂ̃}�b�v�`�b�v�ԍ��̃u���b�N�ԍ�����������B
		int chipData = StageMgr::Instance()->GetMapChipBlock(0, 0, searchIndex);

		// �u���b�N����Ȃ�������Idnex�𓮂����Ď��ցB
		if (CHIP_DATA.min <= chipData && chipData <= CHIP_DATA.max) {

			// �u���b�N�������狗�����v�Z����B
			return MAP_CHIP_SIZE * searchCounter;

		}
		else {
			searchIndex += SEARCH_DIR;
		}

		++searchCounter;

		// �T�[�`�񐔂�100000��𒴂�����(�㉺���E100000�u���b�N�̊Ԃɕǂ��Ȃ��Ƃ�������)���炩�ɂ���̓o�O�Ȃ̂ŃA�T�[�g�B
		if (100000 < searchCounter) assert(0);

	}
}

// �E�F�C�|�C���g�̃A�C�e���ێ������v�Z����B
void NavigationAI::CheckNumberOfItemHeldCount()
{

	// ��x�A�C�e���ێ�����0�ɂ���B
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				wayPoints[y][x]->numberOfItemHeld = 0;
			}
		}
	}

	// �X�^�~�i�A�C�e���̂����ɉ�����WayPoint�̃A�C�e���ێ�����ύX����B
	std::array<StaminaItem, 100> staminaItem = StaminaItemMgr::Instance()->GetItemArray();
	const int ITEM_COUNT = staminaItem.size();
	for (int index = 0; index < ITEM_COUNT; ++index) {

		// ��������Ă��Ȃ� or �擾����Ă��� �������珈�����΂��B
		if (!staminaItem[index].GetIsActive() || staminaItem[index].GetIsAcquired()) continue;

		// �A�C�e���̃}�b�v�`�b�v�C���f�b�N�X�ԍ������߂�B
		Vec2<int> mapChipIndex = { (int)(staminaItem[index].GetPos().x / MAP_CHIP_SIZE), (int)(staminaItem[index].GetPos().y / MAP_CHIP_SIZE) };

		// �e�C���f�b�N�X������l�𒴂��ĂȂ������`�F�b�N�B
		if (mapChipIndex.x < 0 || wayPointXCount <= mapChipIndex.x) continue;
		if (mapChipIndex.y < 0 || wayPointYCount <= mapChipIndex.y) continue;

		// ��������E�F�C�|�C���g�ɃA�C�e������ǉ��B
		++wayPoints[mapChipIndex.y][mapChipIndex.x]->numberOfItemHeld;

	}

}

std::vector<std::shared_ptr<WayPointData>> NavigationAI::ConvertToShortestRoute2(const std::vector<std::vector<std::shared_ptr<WayPointData>>>& QUEUE)
{
	std::vector<std::vector<std::shared_ptr<WayPointData>>> route;

	reachToGoalFlag.clear();
	reachToGoalFlag.reserve(QUEUE.size());
	reachToGoalFlag.resize(QUEUE.size());
	//�X�^�[�g�n�_����S�[���n�_�܂Ōq�����Ă��郋�[�g��T��
	for (int branch = 0; branch < QUEUE.size(); ++branch)
	{
		if (QUEUE[branch][0]->handle == startPoint.handle &&
			QUEUE[branch][QUEUE[branch].size() - 1]->handle == endPoint.handle)
		{
			route.push_back(QUEUE[branch]);
			reachToGoalFlag[branch] = true;
		}
		else
		{
			reachToGoalFlag[branch] = false;
		}
	}


	int minSize = 100000;
	int shortestRoute = 0;
	//�S�[���܂ł̃p�X�����r���A��ԏ��Ȃ��p�X���̃��[�g���ŒZ�����Ƃ���
	for (int i = 0; i < route.size(); ++i)
	{
		if (route[i].size() < minSize)
		{
			shortestRoute = i;
			minSize = route[i].size();
		}
	}

	return route[shortestRoute];
}

inline void NavigationAI::RegistHandle(std::shared_ptr<WayPointData> DATA)
{

	// �E�F�C�|�C���g���q���鏈���B

	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);


	// �܂��͏㉺���E����q����B
	bool isLeft = false;
	isLeft = ConnectWayPoint(DATA, { -1,0 }, mapChipSizeData);
	bool isRight = false;
	isRight = ConnectWayPoint(DATA, { 1,0 }, mapChipSizeData);
	bool isTop = false;
	isTop = ConnectWayPoint(DATA, { 0,-1 }, mapChipSizeData);
	bool isBottom = false;
	isBottom = ConnectWayPoint(DATA, { 0,1 }, mapChipSizeData);

	// ���Ɏ΂߂Ɍq����B
	if (isLeft && isTop) {

		// ����Ɍq����B
		ConnectWayPoint(DATA, { -1,-1 }, mapChipSizeData);

	}
	if (isRight && isTop) {

		// �E��Ɍq����B
		ConnectWayPoint(DATA, { 1,-1 }, mapChipSizeData);

	}
	if (isLeft && isBottom) {

		// �����Ɍq����B
		ConnectWayPoint(DATA, { -1,1 }, mapChipSizeData);

	}
	if (isRight && isBottom) {

		// �E���Ɍq����B
		ConnectWayPoint(DATA, { 1,1 }, mapChipSizeData);

	}


	// ���Ɋe�����̕ǂ܂ł̋��������߂�B

	// ������Ɍ�������B
	DATA->wallDistanceTop = SearchWall(DATA, Vec2<float>(0, -1), mapChipSizeData);
	// �������Ɍ�������B
	DATA->wallDistanceBottom = SearchWall(DATA, Vec2<float>(0, 1), mapChipSizeData);
	// �E�����Ɍ�������B
	DATA->wallDistanceRight = SearchWall(DATA, Vec2<float>(1, 0), mapChipSizeData);
	// �������Ɍ�������B
	DATA->wallDistanceLeft = SearchWall(DATA, Vec2<float>(-1, 0), mapChipSizeData);


}
