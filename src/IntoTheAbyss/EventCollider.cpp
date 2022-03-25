#include "EventCollider.h"
#include <vector>

using namespace std;

INTERSECTED_LINE EventCpllider::CheckHitVel(Vec2<float> pushBackPosA, const Vec2<float> prevFramePosA, const Vec2<float>& velA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB)
{
	/*===== ��`�Ƌ�`�̈ړ��ʂ��g���������߂��t�������蔻�� =====*/


	// �����߂��Ώۂ̈ړ��ʂ��擾
	Vec2<float> prevFramePosBuff = prevFramePosA;
	Vec2<float> velBuff = velA;
	Vec2<float> playerVel = (pushBackPosA + velBuff) - prevFramePosBuff;

	// �����߂��Ώۂ̈ړ��ʂ̒����������߂��Ώۂ̃T�C�Y����������������A�����߂��Ώۂ̃T�C�Y�܂ŐL�΂��B
	if (fabs(playerVel.Length()) < sizeA.x) {

		playerVel.Normalize();
		playerVel = playerVel * sizeA;

	}

	// �����߂��ΏۂƂ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
	if (Vec2<float>(posB.x - pushBackPosA.x, posB.y - pushBackPosA.y).Length() >= 100.0f) {
		return INTERSECTED_NONE;
	}

	// ����Ώۂ������߂��Ώە��g������4���_�����߂�B
	const Vec2<float> rightTop = { posB.x + sizeB.x , posB.y - sizeB.y };
	const Vec2<float> rightBottom = { posB.x + sizeB.x , posB.y + sizeB.y };
	const Vec2<float> leftTop = { posB.x - sizeB.x , posB.y - sizeB.y };
	const Vec2<float> leftBottom = { posB.x - sizeB.x , posB.y + sizeB.y };

	// ��_�ۑ��p
	vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;

	// �S�Ă̐����Ƃ̓����蔻����s���B

	// ���
	if (IsIntersected(rightTop, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// �������Ă������_���v�Z���ĕۑ�
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(rightTop, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

		// ��ɓ��������Ƃ����f�[�^��ۑ��B
		buff.second = INTERSECTED_TOP;

		// ��_�f�[�^��ۑ��B
		intersectPos.push_back(buff);

	}


	// �E��
	if (IsIntersected(rightTop, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// �������Ă������_���v�Z���ĕۑ�
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(rightTop, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

		// �E�ɓ��������Ƃ����f�[�^��ۑ��B
		buff.second = INTERSECTED_RIGHT;

		// ��_�f�[�^��ۑ��B
		intersectPos.push_back(buff);

	}

	// ����
	if (IsIntersected(leftBottom, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// �������Ă������_���v�Z���ĕۑ�
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(leftBottom, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

		// ���ɓ��������Ƃ����f�[�^��ۑ��B
		buff.second = INTERSECTED_BOTTOM;

		// ��_�f�[�^��ۑ��B
		intersectPos.push_back(buff);

	}

	// ����
	if (IsIntersected(leftBottom, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// �������Ă������_���v�Z���ĕۑ�
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(leftBottom, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

		// �����ɓ��������Ƃ��������ۑ��B
		buff.second = INTERSECTED_LEFT;

		// ��_�f�[�^��ۑ��B
		intersectPos.push_back(buff);

	}

	// ��_��0��������I���B
	if (intersectPos.size() <= 0) return INTERSECTED_NONE;

	// �S�Ă̌�_�̒����牟���߂��ΏۂƂ̋������ŏ��̂��̂����߂�B
	pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
	miniIntersectedPoint.first = { 100000,100000 };
	for (int index = 0; index < intersectPos.size(); ++index) {

		// ��_�Ԃ̋������ۑ�����Ă�����̂���������������A���̍��W��ۑ�����B
		if (Vec2<float>(intersectPos[index].first - pushBackPosA).Length() < Vec2<float>(miniIntersectedPoint.first - pushBackPosA).Length()) {

			miniIntersectedPoint = intersectPos[index];

		}

	}

	// �����߂��Ă��瓖�������ӂ�Ԃ��B

	// �ŏ��̌�_�̎�ނɂ���ď����𕪂���B
	if (miniIntersectedPoint.second == INTERSECTED_TOP) {

		// �����߂��B
		pushBackPosA.y = miniIntersectedPoint.first.y - sizeA.y;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

		// �����߂��B
		pushBackPosA.x = miniIntersectedPoint.first.x + sizeA.x;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

		// �����߂��B
		float offset = 1.0f;
		pushBackPosA.y = miniIntersectedPoint.first.y + sizeA.y + offset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

		// �����߂��B
		pushBackPosA.x = miniIntersectedPoint.first.x - sizeA.x;

	}

	return miniIntersectedPoint.second;

}

INTERSECTED_LINE EventCpllider::CheckHitSize(Vec2<float> posA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB, const INTERSECTED_LINE& dir)
{
	/*===== �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� =====*/


	// �����蔻��Ɏg�p��������𐶐��B
	Vec2<float> checkHitDirection = {};

	switch (dir) {

	case INTERSECTED_TOP:

		// ������̐����𐶐��B
		checkHitDirection = { posA.x, posA.y - sizeA.y };

		break;

	case INTERSECTED_BOTTOM:

		// �������̐����𐶐��B
		checkHitDirection = { posA.x, posA.y + sizeA.y };

		break;

	case INTERSECTED_LEFT:

		// �������̐����𐶐��B
		checkHitDirection = { posA.x - sizeA.x, posA.y };

		break;

	case INTERSECTED_RIGHT:

		// �E�����̐����𐶐��B
		checkHitDirection = { posA.x + sizeA.x, posA.y };

		break;

	default:

		break;

	}

	// �v���C���[�Ƃ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
	if (Vec2<float>(posB.x - posA.x, posB.y - posA.y).Length() >= 1000.0f) {
		return INTERSECTED_NONE;
	}

	// �}�b�v�`�b�v���v���C���[���g������4���_�����߂�B
	const Vec2<float> rightTop = { posB.x + sizeB.x , posB.y - sizeB.y };
	const Vec2<float> rightBottom = { posB.x + sizeB.x , posB.y + sizeB.y };
	const Vec2<float> leftTop = { posB.x - sizeB.x , posB.y - sizeB.y };
	const Vec2<float> leftBottom = { posB.x - sizeB.x , posB.y + sizeB.y };

	// ��_�ۑ��p
	vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;


	// �S�Ă̐����Ƃ̓����蔻����s���B

	// ���
	if (dir == INTERSECTED_BOTTOM) {
		if (IsIntersected(rightTop, leftTop, posA, checkHitDirection)) {

			// �������Ă������_���v�Z���ĕۑ�
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(rightTop, leftTop, posA, checkHitDirection);

			// ������ɓ�����������ۑ��B
			buff.second = INTERSECTED_TOP;

			// ��_�f�[�^��ۑ��B
			intersectPos.push_back(buff);

		}
	}

	// �E��
	if (dir == INTERSECTED_LEFT) {
		if (IsIntersected(rightTop, rightBottom, posA, checkHitDirection)) {

			// �������Ă������_���v�Z���ĕۑ�
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(rightTop, rightBottom, posA, checkHitDirection);

			// �E���ɓ�����������ۑ��B
			buff.second = INTERSECTED_RIGHT;

			// ��_�f�[�^��ۑ��B
			intersectPos.push_back(buff);

		}
	}

	// ����
	if (dir == INTERSECTED_TOP) {
		if (IsIntersected(leftBottom, rightBottom, posA, checkHitDirection)) {

			// �������Ă������_���v�Z���ĕۑ�
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(leftBottom, rightBottom, posA, checkHitDirection);

			// �������ɓ�����������ۑ��B
			buff.second = INTERSECTED_BOTTOM;

			// ��_�f�[�^��ۑ��B
			intersectPos.push_back(buff);

		}
	}

	// ����
	if (dir == INTERSECTED_RIGHT) {
		if (IsIntersected(leftBottom, leftTop, posA, checkHitDirection)) {

			// �������Ă������_���v�Z���ĕۑ�
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(leftBottom, leftTop, posA, checkHitDirection);

			// �����ɓ�����������ۑ��B
			buff.second = INTERSECTED_LEFT;

			// ��_�f�[�^��ۑ��B
			intersectPos.push_back(buff);

		}
	}

	// ��_��0�������玟��
	if (intersectPos.size() <= 0) return INTERSECTED_NONE;

	// �S�Ă̌�_�̒�����v���C���[�Ƃ̋������ŏ��̂��̂����߂�B
	pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
	miniIntersectedPoint.first = { 100000,100000 };
	for (int index = 0; index < intersectPos.size(); ++index) {

		// ��_�Ԃ̋������ۑ�����Ă�����̂���������������A���̍��W��ۑ�����B
		if (Vec2<float>(intersectPos[index].first - posA).Length() < Vec2<float>(miniIntersectedPoint.first - posA).Length()) {

			miniIntersectedPoint = intersectPos[index];

		}

	}

	// �s�b�^�������߂�����s�s���Ȃ��Ƃ��N����̂ŁA�����I�t�Z�b�g�l��݂���B
	float pushBackOffset = -1.0f;

	// �ŏ��̌�_�̎�ނɂ���ď����𕪂���B
	if (miniIntersectedPoint.second == INTERSECTED_TOP) {

		// �����߂��B
		posA.y = miniIntersectedPoint.first.y - sizeA.y - pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

		// �����߂��B
		posA.x = miniIntersectedPoint.first.x + sizeA.x + pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

		// �����߂��B
		posA.y = miniIntersectedPoint.first.y + sizeA.y + pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

		// �����߂��B
		posA.x = miniIntersectedPoint.first.x - sizeA.x - pushBackOffset;

	}

	return miniIntersectedPoint.second;

}

bool EventCpllider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
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

Vec2<float> EventCpllider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//��_�����߂� ���̎��͎������̂܂�
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = fabs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = fabs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}
