#include "SwingLineSegmentMgr.h"
#include "DrawFunc.h"
#include "DrawFunc_Color.h"
#include "ScrollMgr.h"
#include "TexHandleMgr.h"
#include "MapChipCollider.h"

void SwingLineSegment::Init()
{

	/*===== ���������� =====*/

	isActive = false;

}

void SwingLineSegment::Update(const Vec2<float>& Start, const Vec2<float>& End, const Vec2<float>& StartDir, const Vec2<float>& EndDir, const int& Alpha, const SEGMENT_ID& ID, const int& Handle)
{

	/*===== �X�V���� =====*/

	start = Start;
	end = End;
	startDir = StartDir;
	endDir = EndDir;
	alpha = Alpha;
	id = ID;
	isActive = true;
	graphHandle = Handle;

}

void SwingLineSegment::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> drawPos = {};
	Vec2<float> size = {};

	switch (id)
	{
	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_LINE:

		// ���ŃA���t�@�ŉ摜��ς���B
		if (alpha == 50) {
			DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(255, 255, 255, alpha), AlphaBlendMode_Trans);
		}
		else if (alpha == 100) {
			DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(0, 0, 0, alpha), AlphaBlendMode_Trans);
		}
		else {
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle), 64, AlphaBlendMode_Trans);
		}

		break;

	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_UI:

		drawPos = Vec2<float>(start - end) / 2.0f;
		size = { 64.0f,64.0f };
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle), 64, AlphaBlendMode_Trans);

		break;

	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW:

		// ���ŃA���t�@�ŉ摜��ς���B
		if (alpha == 50 || alpha == 100) {
			DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(255, 255, 255, alpha), AlphaBlendMode_Trans);
		}
		else if (alpha == 100) {
			DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(0, 0, 0, alpha), AlphaBlendMode_Trans);
		}
		else {
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle), 64, AlphaBlendMode_Trans);
		}

		break;

	default:
		break;
	}

	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(255, 255, 255, alpha), AlphaBlendMode_Trans);

}

void SwingLineSegment::ResetDistance(const Vec2<float>& Pos, const float& Distance)
{

	/*===== �����C�� =====*/

	start = Pos + startDir * Distance;
	end = Pos + endDir * Distance;

}

void SwingLineSegmentMgr::Setting(const bool& IsClockWise, const int& HandleUI, const int& HandleArrow, const int& HandleLine, const int& ReticleHandle)
{

	/*===== �O���� =====*/

	isClockWise = IsClockWise;
	UIHandle = HandleUI;
	arrowHandle = HandleArrow;
	lineHandle = HandleLine;
	reticleHandle = ReticleHandle;

}

void SwingLineSegmentMgr::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < LINE_COUNT; ++index) {

		lineSegments[index].Init();

	}

}

void SwingLineSegmentMgr::Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const bool& IsSwing, const bool& NoMove, const std::vector<std::vector<int>>& mapData)
{

	/*===== �X�V���� =====*/

	// ���݂̊p�x
	float nowAngle = atan2f(TargetVec.y, TargetVec.x);

	// ���݂̊p�x��-�������ꍇ�͐������p�x�ɒ����B
	if (nowAngle < 0) {

		float angleBuff = 3.14f - fabs(nowAngle);
		nowAngle = 3.14f + angleBuff;

	}

	// �S�Ă̐����𐶐�����B
	for (int index = 0; index < LINE_COUNT; ++index) {

		// �������Ȃ��t���O�������Ă�����for���𔲂���B
		if (IsSwing) {

			lineSegments[index].SetAlpha(200);
			lineSegments[index].ResetDistance(Pos, Distance);
			continue;

		}
		else if (NoMove) {

			lineSegments[index].SetAlpha(50);
			lineSegments[index].ResetDistance(Pos, Distance);
			continue;

		}

		// �J�n���p�x�ƏI�����p�x�����߂�B
		float startAngle = nowAngle;
		float endAngle = nowAngle + (isClockWise ? ANGLE_DIFFERENCE : -ANGLE_DIFFERENCE);

		// �I�����p�x�� - or 6.28f �𒴂��Ă����狸��
		if (endAngle < 0 || 6.28f < endAngle) {

			// ���v��肾������B
			if (isClockWise) {

				endAngle -= 6.28f;

			}
			// �����v��肾������B
			else {

				endAngle += 6.28f;

			}

		}

		// �g�p����摜�����߂�B
		int handle = 0;
		SwingLineSegment::SEGMENT_ID id;
		// �ŏ��͑���{�^���������Ă���UI��`�悷��B
		if (index == 0) {
			handle = UIHandle;
			id = SwingLineSegment::SEGMENT_ID::SEGMENT_ID_UI;
		}
		// �Ō�̐����͖���`�悷��B
		else if (index == LINE_COUNT - 1) {
			handle = arrowHandle;
			id = SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW;
		}
		// ����ȊO�͕��ʂ̐�����`�悷��B
		else {
			handle = lineHandle;
			id = SwingLineSegment::SEGMENT_ID::SEGMENT_ID_LINE;
		}


		Vec2<float> startPos = Pos + Vec2<float>(cosf(startAngle), sinf(startAngle)) * Distance;
		Vec2<float> endPos = Pos + Vec2<float>(cosf(endAngle), sinf(endAngle)) * Distance;

		// �����𐶐��B
		lineSegments[index].Update(startPos, endPos, Vec2<float>(cosf(startAngle), sinf(startAngle)), Vec2<float>(cosf(endAngle), sinf(endAngle)), 50, id, handle);

		// �p�x��ۑ��B
		nowAngle = endAngle;

	}

	/*===== �e�����ƃ}�b�v�`�b�v�̓����蔻����s�� =====*/

	// �Ə��摜�𐁂���΂��B(���t���[���Փ˓_���v�Z���邽��)�B
	reticlePos = { -1000,-1000 };

	// ��x�}�b�v�`�b�v�ɓ��������炻��ȍ~�͕`�悵�Ȃ�and�����蔻����s��Ȃ��悤�ɂ��邽�߂Ɏg�p�B
	int isHitMapChip = false;
	for (int index = 0; index < LINE_COUNT; ++index) {

		// ���Ƀ}�b�v�`�b�v�Ɠ������Ă����珈�����΂� and ����������B
		if (isHitMapChip) {

			lineSegments[index].Deactivate();
			continue;

		}

		// �}�b�v�`�b�v�Ɠ����蔻����s���B
		Vec2<float> resultPos = CheckHitMapChip(lineSegments[index].GetStart(), lineSegments[index].GetEnd());

		// �l��-1�������瓖�����Ă��Ȃ��Ƃ������ƂȂ̂ŁA�������΂��B
		if (resultPos.x == -1) continue;

		// �}�b�v�`�b�v�Ɠ�����������ɂ���B
		lineSegments[index].SetEnd(resultPos);

		// �X�C���O�����������_�̂Ƃ���̐�������ɂ���B
		if (IsSwing) {
			lineSegments[index].SetID(SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW);
			lineSegments[index].SetHandle(arrowHandle);
		}

		isHitMapChip = true;

		// �Ə��̈ʒu��ۑ��B
		reticlePos = resultPos;

	}

	// �}�b�v�`�b�v�Ɠ������Ă�����A�S�Ă̐����̐F��������Ƃ����Z������B
	if (isHitMapChip) {

		for (int index = 0; index < LINE_COUNT; ++index) {

			// �����͔Z�����Ȃ��B	���{���񂩂�Ⴄ�摜������ۂ͂��̏����͂���Ȃ��Ȃ�B
			if(lineSegments[index].GetID() == SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW) continue;

			lineSegments[index].SetAlpha(100);

		}

	}

}

void SwingLineSegmentMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < LINE_COUNT; ++index) {

		if (!lineSegments[index].GetIsActive())continue;

		lineSegments[index].Draw();

	}

	// �Ə���`��B
	Vec2<float> size = { 32,32 };
	DrawFunc::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(reticlePos - size), ScrollMgr::Instance()->Affect(reticlePos + size), TexHandleMgr::GetTexBuffer(reticleHandle), AlphaBlendMode_Trans);

}

#include "WinApp.h"
#include <IntoTheAbyss/StageMgr.h>
#include "SelectStage.h"
#include "Collider.h"
const Vec2<float>& SwingLineSegmentMgr::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos)
{
	//�ǂ�����Ďg����
	Vec2<float>handSegmentStart(StartPos), handSegmentEnd(EndPos);//����
	Vec2<float>handSegmentDir(EndPos - StartPos);					//�����̕���
	Vec2<float>handPos(StartPos);									//�����̎n�_
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
		return Vec2<float>(-1, -1);
	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
		if (shortestPoints[index].second >= shortestLength) continue;

		// �f�[�^��ۑ�����B
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;
	}


	//�ŒZ��������ł��Z�o���ꂽ�瓖���蔻����o��
	return sightPos;

}
