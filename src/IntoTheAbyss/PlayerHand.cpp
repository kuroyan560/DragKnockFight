#include "PlayerHand.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "SightCollisionStorage.h"
#include "MapChipCollider.h"
#include "StageMgr.h"
#include "SelectStage.h"

#include"KuroEngine.h"
#include"TexHandleMgr.h"
#include "WinApp.h"

PlayerHand::PlayerHand(const int& HandGraph, const int& AimGraphHandle) : handGraphHandle(HandGraph), aimGraphHandle(AimGraphHandle)
{

	/*-- �R���X�g���N�^ --*/

	// ���W��������
	handPos = {};

	// �����̍��W��������
	sightPos = {};

	// �v���C���[�̒��S����̋�����������
	armDistance = 0;

	// ���͂��ꂽ�p�x��������
	inputAngle = 0;

	// ��̉摜�����[�h
	//playerHandGraph = LoadGraph("Resource/PlayerHand.png");
	//playerHandGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/PlayerHand.png");

	ptLight.SetInfluenceRange(64.0f);
}

void PlayerHand::Init(const float& armDistance)
{

	/*-- �R���X�g���N�^ --*/

	// ���W��������
	handPos = {};

	// �����̍��W��������
	sightPos = {};

	// �v���C���[�̒��S����̋�����������
	this->armDistance = armDistance;

	// ���͂��ꂽ�p�x��������
	inputAngle = 0;

	offsetRadiusTimer = OFFSET_RADIUS_TIME;

}

void PlayerHand::Update(const Vec2<float>& playerCenterPos)
{

	/*-- �X�V���� --*/

	// �v���C���[�̒��S���W�����ɁA�r�̈ʒu���Z�b�g�B
	Vec2<float> honraiHandPos = playerCenterPos;
	static const float RADIUS = 48.0f;
	honraiHandPos.x += cos(inputAngle) * RADIUS;
	honraiHandPos.y += sin(inputAngle) * RADIUS;

	// �{������ׂ��v���C���[�̎�̍��W�ɋߕt����B
	handPos = KuroMath::Lerp(handPos, honraiHandPos, 1.0f);

	/*static const float OFFSET_RADIUS_MAX = 18.0f;
	if (offsetRadiusTimer < OFFSET_RADIUS_TIME)offsetRadiusTimer++;
	float offsetRadius = KuroMath::Ease(Out, Back, offsetRadiusTimer, OFFSET_RADIUS_TIME, OFFSET_RADIUS_MAX, 0.0f);
	Vec2<float>offset = { cos(inputAngle) * offsetRadius,sin(inputAngle) * offsetRadius };
	drawPos = handPos + offset;*/

	//isFirstShot = false;

}

#include"DrawFunc.h"
#include"DrawFunc_Shadow.h"
void PlayerHand::Draw(const float& ExtRate, const float& InitAngle, const Vec2<float>& RotaCenterUV, const bool& DRAW_CURSOR)
{
	/*-- �`�揈�� --*/
	afterImg.Draw();

	Vec2<float>ext = { ScrollMgr::Instance()->zoom * ExtRate ,ScrollMgr::Instance()->zoom * ExtRate };
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(drawPos), ext, inputAngle - InitAngle, TexHandleMgr::GetTexBuffer(handGraphHandle));

	//�Ə���`��
	if (DRAW_CURSOR)
	{
		const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(sightPos);
		//DrawFunc::DrawBox2D(leftUp, rightBottom, Color(179, 255, 239, 255), D3D12App::Instance()->GetBackBuffFormat(), true);
		//DrawFunc::DrawRotaGraph2D(drawPos, ext, 0.0f, TexHandleMgr::GetTexBuffer(aimGraphHandle));
		ptLight.SetPos(Vec3<float>(drawPos, -1.0f));
	}

}

void PlayerHand::Shot(const Vec2<float>& forwardVec, const bool& isFirstShot)
{

	/*===== �e��ł������̏��� =====*/
	offsetRadiusTimer = 0;
}

void PlayerHand::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== �����蔻��֐� =====*/

	// �Ə���`�悷��ŒZ���������߂�B
	CheckShortestPoint(mapData);

}

bool PlayerHand::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
{
	/*--�����̊O�ς��v�Z���Č���������s��--*/
	//���� ����A���猩��B�̌�������
	Vec2<float> buffA = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffA.Normalize();
	Vec2<float> buffB = Vec2<float>(posB1.x - posA1.x, posB1.y - posA1.y);
	buffB.Normalize();
	Vec2<float> buffC = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffC.Normalize();
	Vec2<float> buffD = Vec2<float>(posB2.x - posA1.x, posB2.y - posA1.y);
	buffD.Normalize();
	float buffE = buffA.Cross(buffB);
	float buffF = buffC.Cross(buffD);
	float result1 = buffE * buffF;
	bool zero1 = false;
	if (buffE * buffF == 0) zero1 = true;
	//���� ����B���猩��A�̌�������
	buffA = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffA.Normalize();
	buffB = Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y);
	buffB.Normalize();
	buffC = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffC.Normalize();
	buffD = Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y);
	buffD.Normalize();
	buffE = buffA.Cross(buffB);
	buffF = buffC.Cross(buffD);
	float result2 = buffE * buffF;
	bool zero2 = false;
	if (buffE * buffF == 0) zero2 = true;

	//�������������Ă��鎞�́A�������猩�Č������肵���������̒[�_2�������ɂ��鎞�B
	//�O�ςō��E���������ƁA�������Ă��鎞�͒l�̌��ʂ�+��-�ɂȂ�B
	//�܂藼���̊O�ς��|���Č��ʂ�-�ɂȂ����ꍇ�̂݌������Ă���B
	//����A����݂�B�𔻒�A����B����݂�A�𔻒�Ɠ�ʂ蔻����s���B
	//����2�̔��茋�ʂ��|��������-�A�Ⴕ����0�̎�2�̐����͌������Ă���B
	if ((result1 <= 0 && result2 <= 0) ||
		(result1 <= 0 && zero2) ||
		(zero1 && result2 <= 0)) {
		return true;
	}
	return false;
}

Vec2<float> PlayerHand::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//��_�����߂� ���̎��͎������̂܂�
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

void PlayerHand::EmitAfterImg(const Vec2<float>& TeleAmount, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror)
{
	afterImg.EmitArray(drawPos, drawPos + TeleAmount, GraphHandle, GraphSize, Miror);
}

void PlayerHand::CheckShortestPoint(const vector<vector<int>>& mapData)
{

	/*===== �Ə���`�悷��ŒZ���������߂� =====*/

	// �ŒZ������ۑ�����悤�̔z��B
	vector<pair<Vec2<float>, float>> shortestPoints;

	// �g�p����������쐬����B
	Vec2<float> handSegmentStart = handPos;
	Vec2<float> handSegmentEnd = Vec2<float>(handPos.x + cosf(inputAngle) * 1000.0f, handPos.y + sinf(inputAngle) * 1000.0f);
	Vec2<float> handSegmentDir = { handSegmentEnd - handSegmentStart };
	handSegmentDir.Normalize();

	// �Ə��̃��C�̕����ɂ���ē����蔻��𖳌�������ׂ̃t���O���Z�b�g����B
	bool isTop = handSegmentDir.y < 0;
	bool isLeft = handSegmentDir.x < 0;

	// �ۑ�����Ă���f�[�^�̔z�񐔂��擾����B
	const int STORAGE_COUNT = SightCollisionStorage::Instance()->data.size();
	for (int index = 0; index < STORAGE_COUNT; ++index) {

		// ���̃C���f�b�N�X�̃u���b�N�̃T�C�Y���擾�B
		const Vec2<float> BLOCK_SIZE = SightCollisionStorage::Instance()->data[index]->scale;
		// ���̃C���f�b�N�X�̃u���b�N�̍��W���擾�B
		const Vec2<float> BLOCK_POS = SightCollisionStorage::Instance()->data[index]->pos;

		//// �r���[�|�[�g�O�ɂ������珈�����s��Ȃ��B
		//if (ViewPort::Instance()->pointPos[ViewPort::LEFT_UP].x - MAP_CHIP_HALF_SIZE > BLOCK_POS->x) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].x + MAP_CHIP_HALF_SIZE < BLOCK_POS->x) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].y - MAP_CHIP_HALF_SIZE > BLOCK_POS->y) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_DOWN].y + MAP_CHIP_HALF_SIZE < BLOCK_POS->y) continue;

		// �l�ӕ���_�����߂�B

		// ��_�ۑ��p
		vector<Vec2<float>> intersectedPos;

		// �����
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y))) {

			// ��_�����߂ĕۑ�����B
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y)));

		}
		// �E����
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y))) {

			// ��_�����߂ĕۑ�����B
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y)));

		}
		// ������
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y))) {

			// ��_�����߂ĕۑ�����B
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x + BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y)));

		}
		// ������
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y))) {

			// ��_�����߂ĕۑ�����B
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS.x - BLOCK_SIZE.x, BLOCK_POS.y + BLOCK_SIZE.y)));

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

			// �r���[�|�[�g�O�ɂ������珈�����s��Ȃ��B
			//if (ViewPort::Instance()->pointPos[ViewPort::LEFT_UP].x - MAP_CHIP_HALF_SIZE > BLOCK_POS.x) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].x + MAP_CHIP_HALF_SIZE < BLOCK_POS.x) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].y - MAP_CHIP_HALF_SIZE > BLOCK_POS.y) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_DOWN].y + MAP_CHIP_HALF_SIZE < BLOCK_POS.y) continue;

			// �l�ӕ���_�����߂�B

			// ��_�ۑ��p
			vector<Vec2<float>> intersectedPos;

			// �����
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

			}
			// �E����
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// ������
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// ������
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

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
		return;

	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
		if (shortestPoints[index].second >= shortestLength) continue;

		// �f�[�^��ۑ�����B
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;

	}

	// ���W���A���C�������g����B
	//sightPos.x = RoundUp(sightPos.x, SIGHT_SIZE * 2.0f);
	//sightPos.y = RoundUp(sightPos.y, SIGHT_SIZE * 2.0f);

}