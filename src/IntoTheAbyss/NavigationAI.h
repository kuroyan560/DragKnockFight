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
	bool isWall;							//�ǂ��ǂ��� ���܂ł͕ǂɂ̓E�F�C�|�C���g��z�u���Ȃ��Ƃ��������ɂȂ��Ă������A�ڑ��̒T���̂��₷������ǂɂ��E�F�C�|�C���g��ݒu����悤�ɂ��܂����B

	int numberOfItemHeld;					// �ێ��A�C�e����

	float wallDistanceTop;					//������̕ǂ܂ł̋���
	float wallDistanceBottom;				//�������̕ǂ܂ł̋���
	float wallDistanceLeft;					//�������̕ǂ܂ł̋���
	float wallDistanceRight;				//�E�����̕ǂ܂ł̋���

	WayPointData() :handle(Vec2<int>(-1, -1)), passNum(0), branchHandle(-1), branchReferenceCount(0), radius(0.0f)
	{
	}

	void RegistHandle(const Vec2<int>& HANDLE)
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
	NavigationAI();

	/// <summary>
	/// �|�C���g�̐���
	/// </summary>
	/// <param name="MAP_DATA">�X�e�[�W��CSV</param>
	void Init(const RoomMapChipArray& MAP_DATA);

	void Update(const Vec2<float>& POS);

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
		QueueData(const Vec2<int>& HANDLE, float SUM) :handle(HANDLE), sum(SUM)
		{
		};
	};

	bool resetSearchFlag;


	std::vector<std::vector<WayPointData>> wayPoints;//�E�F�C�|�C���g�̔z��
	int wayPointXCount;
	int wayPointYCount;

	WayPointData startPoint;	//�T������ۂ̃X�^�[�g�n�_
	WayPointData endPoint;		//�T������ۂ̃S�[���n�_

	bool startFlag;
private:

	static const float SERACH_RADIUS;//�E�F�C�|�C���g���m�̃����N�t������͈�
	static const float SERACH_LAY_UPDOWN_DISTANCE;//�E�F�C�|�C���g���m�̃����N�t������͈�
	static const float SERACH_LAY_LEFTRIGHT_DISTANCE;//�E�F�C�|�C���g���m�̃����N�t������͈�
	static const float SERACH_LAY_NANAME_DISTANCE;//�E�F�C�|�C���g���m�̃����N�t������͈�

	static const float WAYPOINT_RADIUS;//�E�F�C�|�C���g�̃f�o�b�N�`��̑傫��

	static const int MAP_CHIP_SIZE = 50;
	static const int MAP_CHIP_HALF_SIZE = MAP_CHIP_SIZE / 2;



	/// <summary>
	/// �}�b�v�`�b�v���W�ɕϊ���float����int�ɐ؂�ւ���ۂɐ؂藎�Ƃ����؂�グ�邩�𔻒f����
	/// </summary>
	/// <param name="WORLD_POS">���[���h���W</param>
	/// <returns>�}�b�v�`�b�v���W</returns>
	inline const Vec2<int>& GetMapChipNum(const Vec2<float>& WORLD_POS);

	/// <summary>
	/// �����ɓn�����E�F�C�|�C���g������̃E�F�C�|�C���g�ƌq����
	/// </summary>
	/// <param name="HANDLE">�E�F�C�|�C���g���q����ׂ̔���</param>
	/// <param name="DATA">�����N�t������Ώ�</param>
	inline void RegistHandle(WayPointData DATA);

	/// <summary>
	/// �g�p���Ă���E�F�C�|�C���g���ǂ���
	/// </summary>
	/// <param name="DATA">�E�F�C�|�C���g�̃f�[�^</param>
	/// <returns>true...�g���Ă���,false...�g���Ă��Ȃ�</returns>
	inline bool DontUse(const Vec2<int>& HANDLE);

	/// <summary>
	/// �}�b�v�`�b�v�ƃ��C�̔���
	/// </summary>
	/// <param name="START_POS">�n�_</param>
	/// <param name="END_POS">�I�_</param>
	/// <returns>true...���������Afalse...�������Ă��Ȃ�</returns>
	bool CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
	{

		// �}�b�v�`�b�v�̏��B
		RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//�}�b�v

		// �}�b�v�`�b�v�̎��ʗp�f�[�^
		SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

		// ���Ƀ}�b�v�`�b�v�Ƃ̍ŒZ���������߂�B
		const int MAP_Y = mapData.size();
		for (int height = 0; height < MAP_Y; ++height) {

			const int MAP_X = mapData[height].size();
			for (int width = 0; width < MAP_X; ++width) {

				// ���̃}�b�v�`�b�v��1~9�ȊO�������画����΂��B
				if (!(mapChipSizeData.min <= mapData[height][width] && mapData[height][width] <= mapChipSizeData.max)) continue;

				// ���̃C���f�b�N�X�̃u���b�N�̍��W���擾�B
				const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

				Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

				// ���͈͈ȊO�������珈�����΂��B
				bool checkInsideTop = BLOCK_POS.y < START_POS.y - windowSize.y;
				bool checkInsideBottom = START_POS.y + windowSize.y > BLOCK_POS.y;
				bool checkInsideLeft = BLOCK_POS.x < START_POS.x + windowSize.x;
				bool checkInsideRight = START_POS.x + windowSize.x > BLOCK_POS.x;
				if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
					continue;
				}

				// ���̃u���b�N�������Ă���u���b�N�������珈�����΂��B
				Vec2<int> mapChipIndex = { width, height };
				if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {
					continue;
				}

				// �l�ӕ���_�����߂�B

				// �����
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

					// ������������
					return true;

				}
				// �E����
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ������������
					return true;

				}
				// ������
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ������������
					return true;

				}
				// ������
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// ������������
					return true;

				}

			}

		}

		return false;

	};


	//A*-------------------------------
	std::vector<QueueData>queue;	//�ŒZ���[�g�̌���Z�߂��z��

	float routeRate;	//�����_���Ń��[�g��������ۂ̐���
	bool initRouteFlag;	//�S�[���n�_���ς�����ۂɃ��[�g��₩�烉���_���őI������

	/// <summary>
	/// A�X�^�[�ɂ��T�����s���֐�
	/// </summary>
	/// <param name="START_POINT">�X�^�[�g�n�_</param>
	/// <param name="END_POINT">�S�[���n�_</param>
	void AStart(const WayPointData& START_POINT, const WayPointData& END_POINT);

	/// <summary>
	/// �L���[�ɓ����n���h�����X�^�b�N����Ă��邩�ǂ���
	/// �S�[���n�_�͕����X�^�b�N���Ă��ǂ�
	/// </summary>
	/// <param name="HANDLE">�E�F�C�|�C���g�̃n���h��</param>
	inline bool CheckQueue(const Vec2<int>& HANDLE);

	/// <summary>
	/// �T�������ۂ̍ŒZ���[�g��₩��ŒZ���[�g�ɍi��
	/// </summary>
	/// <param name="QUEUE">�ŒZ���[�g���</param>
	/// <returns>�X�^�[�g����S�[���܂ł̍ŒZ���[�g</returns>
	std::vector<WayPointData> ConvertToShortestRoute2(const std::vector<std::vector<WayPointData>>& QUEUE);


	void RegistBranch(const WayPointData& DATA);


	WayPointData prevStartPoint;//�T������ۂ̑O�t���[���̃X�^�[�g�n�_
	WayPointData prevEndPoint;	//�T������ۂ̑O�t���[���̃S�[���n�_

	std::vector<std::vector<WayPointData>> branchQueue;//�T�����̃��[�g��ۑ�����
	std::vector<WayPointData> shortestRoute;
	std::vector<bool> reachToGoalFlag;

	//�f�o�b�N--------------------------
	Vec2<int> checkingHandle;	//�}�E�X�J�[�\���ŎQ�Ƃ��Ă���E�F�C�|�C���g
	int checkTimer;				//�E�F�C�|�C���g���Q�Ƃ��Ă��鎞��

	bool serachFlag;			//�E�F�C�|�C���g���m�̃����N�t������ׂ̔���̕`��
	bool lineFlag;				//�E�F�C�|�C���g���m�̃����N���Ă��邩�̊m�F
	bool wayPointFlag;			//�E�F�C�|�C���g�̕`��

	std::vector<std::vector<Color>> debugColor;	//�E�F�C�|�C���g�̐F

	struct SearchMapData
	{
		Vec2<int>handle;
		Color color;
		SearchMapData(const Vec2<int>& HANDLE, const Color& COLOR) :handle(HANDLE), color(COLOR)
		{
		}
	};
	std::vector<std::vector<SearchMapData>>searchMap;
	int layerNum;


	bool CheckHitLineCircle(const Vec2<float>& LineStartPos, const Vec2<float>& LineEndPos, const Vec2<float>& CircleCenterPos, const float& CircleRadius) {

		// �n�_����I�_�܂ł̃x�N�g���𐳋K������B
		Vec2<float>& startEndPos = LineEndPos - LineStartPos;
		startEndPos.Normalize();

		// [�n�_����I�_�܂ł̃x�N�g��]�ƁA[�n�_����~�̒��S�܂ł̃x�N�g��]�̌��ʂ̊O�ς��~�̔��a���������������画��̑��i�K�N���A�I
		if (fabs(startEndPos.Cross(CircleCenterPos - LineStartPos)) < CircleRadius) {

			// [�n�_����I�_�܂ł̃x�N�g���Ǝn�_����~�̒��S�܂ł̃x�N�g���̓���] �� [�n�_����I�_�܂ł̃x�N�g���ƏI�_����~�̒��S�܂ł̃x�N�g���̓���] �̂������l��-�������瓖�����Ă���I
			if ((LineEndPos - LineStartPos).Dot(CircleCenterPos - LineStartPos) * (LineEndPos - LineStartPos).Dot(CircleCenterPos - LineEndPos) < 0) {

				return true;

			}
			else {

				// ��̏������œ������Ă��Ȃ������ꍇ�ł��A�n�_�I�_����̋������~�̔��a���������������瓖�����Ă���B
				float startCenterDistance = (LineStartPos - CircleCenterPos).Length();
				float endCenterDistance = (LineEndPos - CircleCenterPos).Length();
				if (fabs(startCenterDistance) < CircleRadius || fabs(endCenterDistance) < CircleRadius) {

					return true;

				}

			}

		}
		return false;
	}


	Vec2<float>CaluLine(const Vec2<float>& CENTRAL_POS, int angle);

	/// <summary>
	/// �E�F�C�|�C���g���q���鏈��
	/// </summary>
	/// <param name="DATA"> ���������̃E�F�C�|�C���g </param>
	/// <param name="SEARCH_OFFSET"> ��������}�b�v�`�b�v�̈ʒu (DATA����Ƃ��Ăǂ����������邩�̒l) </param>
	/// <param name="CHIP_DATA"> �}�b�v�`�b�v�̃u���b�N��ID���擾���ĕǔ��肷�邽�߂̃f�[�^ </param>
	/// <returns> �q����ꂽ�����Ԃ����B �q���鏈���͂��̊֐��̓����ɂ���B </returns>
	bool ConnectWayPoint(WayPointData DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA);

	/// <summary>
	/// �E�F�C�|�C���g����ǂ܂ł̋������v�Z���鏈��
	/// </summary>
	/// <param name="DATA"> ���������̃E�F�C�|�C���g </param>
	/// <param name="SEARCH_DIR"> ����������� </param>
	/// <param name="CHIP_DATA"> �}�b�v�`�b�v�̃u���b�N��ID���擾���ĕǔ��肷�邽�߂̃f�[�^ </param>
	/// <returns> �������������ł̕ǂ܂ł̋�����Ԃ��B </returns>
	float SearchWall(WayPointData DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA);

	void CheckNumberOfItemHeldCount();
};

