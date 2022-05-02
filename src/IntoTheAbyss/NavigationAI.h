#pragma once
#include"../KuroEngine.h"
#include<vector>
#include<array>
#include"StageMgr.h"
#include"BulletCollision.h"
#include"SelectStage.h"
#include"Collider.h"

static const int GOAL_MAX_NUM = 5;

/// <summary>
/// �E�F�C�|�C���g�������
/// </summary>
struct WayPointData
{
	Vec2<int> handle;						//�n���h��
	Vec2<float> pos;						//���W
	float radius;							//�傫��
	std::vector<Vec2<int>> wayPointHandles;	//�ǂ̕����ɍs���邩�n���h�����������
	int passNum;							//�ڕW�n�_�܂ł̃p�X��
	int branchHandle;						//�T�����̕���̃n���h��
	int branchReferenceCount;				//���񂻂̕�����Q�Ƃ�����

	WayPointData() :handle(Vec2<int>(-1, -1)), passNum(0), branchHandle(-1), branchReferenceCount(0), radius(0.0f)
	{
	}

	void RegistHandle(const Vec2<int> &HANDLE)
	{
		for (int i = 0; i < wayPointHandles.size(); ++i)
		{
			bool sameFlag = wayPointHandles[i] == HANDLE;
			if (sameFlag)
			{
				return;
			}
		}

		bool mineFlag = handle == HANDLE;
		if (mineFlag)
		{
			return;
		}

		wayPointHandles.push_back(HANDLE);
	}
};

class NavigationAI
{
public:
	/// <summary>
	/// �|�C���g�̐���
	/// </summary>
	/// <param name="MAP_DATA">�X�e�[�W��CSV</param>
	void Init(const RoomMapChipArray &MAP_DATA);

	void Update(const Vec2<float> &POS);

	void Draw();

	void ImGuiDraw();

	/// <summary>
	/// �ŒZ���[�g���󂯎��
	/// </summary>
	/// <returns></returns>
	std::vector<WayPointData> GetShortestRoute();

	/// <summary>
	/// �T������ۂɃL���[�ɋl�ߍ��ނׂ����
	/// </summary>
	struct QueueData
	{
		Vec2<int>handle;
		float sum;
		QueueData(const Vec2<int> &HANDLE, float SUM) :handle(HANDLE), sum(SUM)
		{
		};
	};

	bool resetSearchFlag;
private:

	static const int WAYPOINT_MAX_X = 10;	 //X���̃E�F�C�|�C���g�̐�
	static const int WAYPOINT_MAX_Y = 10;	 //Y���̃E�F�C�|�C���g�̐�
	static const float SERACH_RADIUS;//�E�F�C�|�C���g���m�̃����N�t������͈�

	static const float WAYPOINT_RADIUS;//�E�F�C�|�C���g�̃f�o�b�N�`��̑傫��

	static const int MAP_CHIP_SIZE = 50;
	static const int MAP_CHIP_HALF_SIZE = MAP_CHIP_SIZE / 2;



	std::array<std::array<std::shared_ptr<WayPointData>, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> wayPoints;//�E�F�C�|�C���g�̔z��

	/// <summary>
	/// �}�b�v�`�b�v���W�ɕϊ���float����int�ɐ؂�ւ���ۂɐ؂藎�Ƃ����؂�グ�邩�𔻒f����
	/// </summary>
	/// <param name="WORLD_POS">���[���h���W</param>
	/// <returns>�}�b�v�`�b�v���W</returns>
	inline const Vec2<int> &GetMapChipNum(const Vec2<float> &WORLD_POS);

	/// <summary>
	/// �����ɓn�����E�F�C�|�C���g������̃E�F�C�|�C���g�ƌq����
	/// </summary>
	/// <param name="HANDLE">�E�F�C�|�C���g���q����ׂ̔���</param>
	/// <param name="DATA">�����N�t������Ώ�</param>
	inline void RegistHandle(const SphereCollision &HANDLE, std::shared_ptr<WayPointData> DATA);

	/// <summary>
	/// �g�p���Ă���E�F�C�|�C���g���ǂ���
	/// </summary>
	/// <param name="DATA">�E�F�C�|�C���g�̃f�[�^</param>
	/// <returns>true...�g���Ă���,false...�g���Ă��Ȃ�</returns>
	inline bool DontUse(const Vec2<int> &HANDLE);

	/// <summary>
	/// �}�b�v�`�b�v�ƃ��C�̔���
	/// </summary>
	/// <param name="START_POS">�n�_</param>
	/// <param name="END_POS">�I�_</param>
	/// <returns>true...���������Afalse...�������Ă��Ȃ�</returns>
	bool CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
	{
		//�ǂ�����Ďg����
		Vec2<float>handSegmentStart(START_POS), handSegmentEnd(END_POS);//����
		Vec2<float>handSegmentDir(END_POS - START_POS);					//�����̕���
		Vec2<float>handPos(START_POS);									//�����̎n�_
		Vec2<float>sightPos;						//���߂�ꂽ��_�̒��̍ŒZ����
		RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//�}�b�v
		//�ǂ�����Ďg����


		// �ŒZ������ۑ�����悤�̔z��B
		std::vector<std::pair<Vec2<float>, float>> shortestPoints;

		// �Ə��̃��C�̕����ɂ���ē����蔻��𖳌�������ׂ̃t���O���Z�b�g����B
		bool isTop = handSegmentDir.y < 0;
		bool isLeft = handSegmentDir.x < 0;

		// ���Ƀ}�b�v�`�b�v�Ƃ̍ŒZ���������߂�B
		const int MAP_Y = mapData.size();
		for (int height = 0; height < MAP_Y; ++height) {

			const int MAP_X = mapData[height].size();
			for (int width = 0; width < MAP_X; ++width) {

				// ���̃}�b�v�`�b�v��1~9�ȊO�������画����΂��B
				if (mapData[height][width] < 1 || mapData[height][width] > 9) continue;

				// ���̃C���f�b�N�X�̃u���b�N�̍��W���擾�B
				const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

				Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

				// ���͈͈ȊO�������珈�����΂��B
				bool checkInsideTop = BLOCK_POS.y < handPos.y - windowSize.y;
				bool checkInsideBottom = handPos.y + windowSize.y > BLOCK_POS.y;
				bool checkInsideLeft = BLOCK_POS.x < handPos.x + windowSize.x;
				bool checkInsideRight = handPos.x + windowSize.x > BLOCK_POS.x;
				if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
					//player.onGround = false;
					continue;
				}

				// ���̃u���b�N�������Ă���u���b�N�������珈�����΂��B
				Vec2<int> mapChipIndex = { width, height };
				if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {

					continue;

				}

				// ���C�̕����ƃu���b�N�̈ʒu�֌W�ŏ������΂��B�E�B���h�E��4����������
				float offsetHandPos = MAP_CHIP_SIZE;
				if (isLeft && handPos.x + offsetHandPos < BLOCK_POS.x) continue;
				if (!isLeft && BLOCK_POS.x < handPos.x - offsetHandPos) continue;
				if (isTop && handPos.y + offsetHandPos < BLOCK_POS.y) continue;
				if (!isTop && BLOCK_POS.y < handPos.y - offsetHandPos) continue;


				// �l�ӕ���_�����߂�B

				// ��_�ۑ��p
				vector<Vec2<float>> intersectedPos;

				// �����
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

					// ��_�����߂ĕۑ�����B
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

				}
				// �E����
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ��_�����߂ĕۑ�����B
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}
				// ������
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ��_�����߂ĕۑ�����B
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}
				// ������
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ��_�����߂ĕۑ�����B
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}

				// �ŒZ���������߂�B
				Vec2<float> shortestPos = {};
				float shoterstLength = 1000000.0f;

				// �T�C�Y��0�������珈�����΂��B
				const int INTERSECTED_COUNT = intersectedPos.size();
				if (INTERSECTED_COUNT <= 0) continue;

				// �ŒZ���������߂�B
				for (int index = 0; index < INTERSECTED_COUNT; ++index) {

					// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
					float lengthBuff = Vec2<float>(intersectedPos[index] - handPos).Length();
					if (lengthBuff >= shoterstLength) continue;

					// �f�[�^��ۑ�����B
					shoterstLength = lengthBuff;
					shortestPos = intersectedPos[index];

				}

				// �ŒZ�̋�����ۑ�����B
				pair<Vec2<float>, float> buff = { shortestPos, shoterstLength };
				shortestPoints.push_back(buff);
			}
		}


		/*-- �����܂ł̉ߒ��ŗl�X�ȍŒZ�����߂邱�Ƃ��ł����B --*/

		// �ŒZ�̍��W��ۑ�����p�ϐ��B
		float shortestLength = 100000.0f;

		// �S�Ă̍ŒZ�̒�����ł��Z�����̂����߂�B
		const int SHORTEST_COUNT = shortestPoints.size();

		// �T�C�Y��0��������Ə����ǂ����ɔ�΂��ă��^�[���B
		if (SHORTEST_COUNT <= 0) {

			sightPos = { -100,-100 };
			return false;
		}

		for (int index = 0; index < SHORTEST_COUNT; ++index) {

			// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
			if (shortestPoints[index].second >= shortestLength) continue;

			// �f�[�^��ۑ�����B
			shortestLength = shortestPoints[index].second;
			sightPos = shortestPoints[index].first;
		}


		//�ŒZ��������ł��Z�o���ꂽ�瓖���蔻����o��
		return 0 < shortestPoints.size();
	}


	//A*-------------------------------
	std::vector<std::shared_ptr<QueueData>>queue;	//�ŒZ���[�g�̌���Z�߂��z��

	/// <summary>
	/// A�X�^�[�ɂ��T�����s���֐�
	/// </summary>
	/// <param name="START_POINT">�X�^�[�g�n�_</param>
	/// <param name="END_POINT">�S�[���n�_</param>
	void AStart(const WayPointData &START_POINT, const WayPointData &END_POINT);

	/// <summary>
	/// �L���[�ɓ����n���h�����X�^�b�N����Ă��邩�ǂ���
	/// �S�[���n�_�͕����X�^�b�N���Ă��ǂ�
	/// </summary>
	/// <param name="HANDLE">�E�F�C�|�C���g�̃n���h��</param>
	inline bool CheckQueue(const Vec2<int> &HANDLE);

	/// <summary>
	/// �T�������ۂ̍ŒZ���[�g��₩��ŒZ���[�g�ɍi��
	/// </summary>
	/// <param name="QUEUE">�ŒZ���[�g���</param>
	/// <returns>�X�^�[�g����S�[���܂ł̍ŒZ���[�g</returns>
	std::vector<std::shared_ptr<QueueData>> ConvertToShortestRoute(const std::vector<std::shared_ptr<QueueData>> &QUEUE);


	void RegistBranch(const WayPointData &DATA);

	WayPointData startPoint;	//�T������ۂ̃X�^�[�g�n�_
	WayPointData endPoint;		//�T������ۂ̃S�[���n�_
	WayPointData prevStartPoint;//�T������ۂ̑O�t���[���̃X�^�[�g�n�_
	WayPointData prevEndPoint;	//�T������ۂ̑O�t���[���̃S�[���n�_

	std::vector<std::vector<std::shared_ptr<WayPointData>>> branchQueue;//�T�����̃��[�g��ۑ�����



	//�f�o�b�N--------------------------
	Vec2<int> checkingHandle;	//�}�E�X�J�[�\���ŎQ�Ƃ��Ă���E�F�C�|�C���g
	int checkTimer;				//�E�F�C�|�C���g���Q�Ƃ��Ă��鎞��

	bool serachFlag;			//�E�F�C�|�C���g���m�̃����N�t������ׂ̔���̕`��
	bool lineFlag;				//�E�F�C�|�C���g���m�̃����N���Ă��邩�̊m�F
	bool wayPointFlag;			//�E�F�C�|�C���g�̕`��

	std::array<std::array<Color, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> debugColor;	//�E�F�C�|�C���g�̐F

	struct SearchMapData
	{
		Vec2<int>handle;
		Color color;
		SearchMapData(const Vec2<int> &HANDLE, const Color &COLOR) :handle(HANDLE), color(COLOR)
		{
		}
	};
	std::vector<std::vector<SearchMapData>>searchMap;	//
	int layerNum;										//�T�������w�肷��p�̕ϐ�


};

