#include "BossPatternNormalMove.h"
#include "Camera.h"
#include"../KuroEngine.h"
#include"../IntoTheAbyss/DebugParameter.h"
#include"SelectStage.h"
#include"StageMgr.h"
#include"Collider.h"

BossPatternNormalMove::BossPatternNormalMove()
{
}

void BossPatternNormalMove::Init()
{
	pullTimer = 0;
}

void BossPatternNormalMove::Update(BossPatternData *DATA)
{
	if (Camera::Instance()->Active())
	{
		*DATA->moveVel = { 0,0 };
		return;
	}

	bool allDidntHitFlg = true;
	//�ߋ������C�ƕǂƂ̔���
	for (int i = 0; i < DATA->nearLimmitLine.size(); ++i)
	{
		DATA->nearLimmitLine[i].hitFlag = CheckMapChipWallAndRay(DATA->nearLimmitLine[i].startPos, DATA->nearLimmitLine[i].endPos);
		
		if (DATA->nearLimmitLine[i].hitFlag)
		{
			allDidntHitFlg = false;
		}
	}


	//���������C�ƕǂƂ̔���
	for (int i = 0; i < DATA->farLimmitLine.size(); ++i)
	{
		//�ߋ����̔��肪�S���Ȃ��Ȃ�܂ŗL���ɂ�������
		if (CheckMapChipWallAndRay(DATA->farLimmitLine[i].startPos, DATA->farLimmitLine[i].endPos))
		{
			DATA->farLimmitLine[i].hitFlag = true;
		}
		if (allDidntHitFlg)
		{
			DATA->farLimmitLine[i].hitFlag = false;
		}
	}







	int PULL_SPAN_MIN = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MIN;
	int PULL_SPAN_MAX = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MAX;
	int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);

	Vec2<float>ACCEL = { 0.0f,0.0f };	//�����x
	float PULL_POWER_MIN = DebugParameter::Instance()->bossDebugData.PULL_POWER_MIN;
	float PULL_POWER_MAX = DebugParameter::Instance()->bossDebugData.PULL_POWER_MAX;

	if (pullTimer < PULL_SPAN)
	{
		++pullTimer;
		if (PULL_SPAN <= pullTimer)
		{
			PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
			pullTimer = 0;

			//�ǂ̕����ɐi��ł����̂�
			//auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
			float rad = GetDir(DATA->nearLimmitLine, DATA->farLimmitLine);
			auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
			ACCEL.x = cos(rad) * power * DebugParameter::Instance()->bossDebugData.PULL_ADD_X_POWER;
			ACCEL.y = sin(rad) * power;
		}
	}
	DATA->moveVel->x = KuroMath::Lerp(DATA->moveVel->x, DebugParameter::Instance()->bossDebugData.moveX, 0.1f);
	DATA->moveVel->y = KuroMath::Lerp(DATA->moveVel->y, 0.0f, 0.1f);
	*DATA->moveVel += ACCEL;

	ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

	if (UsersInput::Instance()->Input(DIK_0)) {
		//	vel.x = OFFSET_VEL * 5.0f;
		bool debug = false;
	}
}

bool BossPatternNormalMove::CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
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

float BossPatternNormalMove::GetDir(const std::array<BossLimitMoveData, 8> &DATA, const std::array<BossLimitMoveData, 4> &FAR_LINE_DATA)
{
	//�ǂ̊p�x�Ɍ������Đi��ł������z��ɓZ�߂���
	std::vector<float>allowToUseThisAngleArray;
	std::vector<float>moveToPlayerAreaAngleArray;

	//���������C���ߋ����̏��������킹��ׂɕʂ̔z��ɑg�ݍ���
	std::array<BossLimitMoveData, 8>tmpFarLineData;
	tmpFarLineData[0] = FAR_LINE_DATA[0];
	tmpFarLineData[2] = FAR_LINE_DATA[1];
	tmpFarLineData[4] = FAR_LINE_DATA[2];
	tmpFarLineData[6] = FAR_LINE_DATA[3];


	//�ǂ̕����Ɍ������Đi��ł�����
	for (int i = 0; i < DATA.size(); ++i)
	{
		//�v���C���[�w�n�̕����ɐi��
		if (!DATA[i].hitFlag && 3 <= i && i <= 5)
		{
			//�����Ă����������㉺���ꂼ��45�x�͈͓̔��ŗ��������
			int maxAngle = (i + 1) * 45;
			int minAngle = maxAngle - 90;
			float rad = Angle::ConvertToRadian(KuroFunc::GetRand(minAngle, maxAngle));
			moveToPlayerAreaAngleArray.push_back(rad);
		}
		//�v���C���[�w�n�̕����ȊO�ɐi��
		else if (!DATA[i].hitFlag && !tmpFarLineData[i].hitFlag)
		{
			//�����Ă����������㉺���ꂼ��45�x�͈͓̔��ŗ��������
			int maxAngle = (i + 1) * 45;
			int minAngle = maxAngle - 90;
			float rad = Angle::ConvertToRadian(KuroFunc::GetRand(minAngle, maxAngle));
			allowToUseThisAngleArray.push_back(rad);
		}
	}

	int getDirNum = -1;
	//�v���C���[�w�n�Ɍ�����Ȃ�����
	if (allowToUseThisAngleArray.size() != 0)
	{
		getDirNum = KuroFunc::GetRand(0, allowToUseThisAngleArray.size() - 1);
		return 	allowToUseThisAngleArray[getDirNum];
	}
	else
	{
		getDirNum = KuroFunc::GetRand(0, moveToPlayerAreaAngleArray.size() - 1);
		return 	moveToPlayerAreaAngleArray[getDirNum];
	}
}
