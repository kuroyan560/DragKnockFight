#include "MapChipCollider.h"
#include "DrawFunc.h"
#include "StageMgr.h"

bool MapChipCollider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
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

Vec2<float> MapChipCollider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//��_�����߂� ���̎��͎������̂܂�
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheVel(Vec2<float>& pos, const Vec2<float>& prevFramePos, const Vec2<float>& vel, const Vec2<float>& size, const vector<vector<int>>& mapChipData, Vec2<int>& hitChipIndex, const bool& isPlayer)
{
	/*===== �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� =====*/


	// �v���C���[�̈ړ��ʂ��擾
	Vec2<float> lineStartPos = prevFramePos;
	Vec2<float> lineEndPos = pos + vel;

	const int HEIGHT = mapChipData.size();
	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�̉�
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �}�b�vID��0�������珈�����΂��B
			if (!(mapChipSizeData.min <= mapChipData[height][width] && mapChipData[height][width] <= mapChipSizeData.max)) continue;

			// ���̃}�b�v�̒��S���W�����߂�B
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// �v���C���[�Ƃ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
			if (MAP_CHIP_SIZE * 10.0f <= Vec2<float>(centerX - pos.x, centerY - pos.y).Length()) {
				continue;
			}

			// �}�b�v�`�b�v���v���C���[���g������4���_�����߂�B
			const Vec2<float> rightTop = { centerX + MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> rightBottom = { centerX + MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftTop = { centerX - MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftBottom = { centerX - MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };

			// ��_�ۑ��p
			vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;

			// �S�Ă̐����Ƃ̓����蔻����s���B

			// ���
			if (IsIntersected(rightTop, leftTop, lineStartPos, lineEndPos)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, lineStartPos, lineEndPos);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height - 1 >= 0) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height - 1][width] && mapChipSizeData.max <= mapChipData[height - 1][width])) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					//}

				}

			}

			// �E��
			bool isHitRight = false;
			isHitRight = IsIntersected(rightTop, rightBottom, lineStartPos, lineEndPos);
			if (isHitRight) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, lineStartPos, lineEndPos);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width + 1 < mapChipData[height].size()) {

					// ��_�̉E�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height][width + 1] && mapChipSizeData.max <= mapChipData[height][width + 1])) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ����
			if (IsIntersected(leftBottom, rightBottom, lineStartPos, lineEndPos)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, lineStartPos, lineEndPos);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height + 1 < mapChipData.size()) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height + 1][width] && mapChipSizeData.max <= mapChipData[height + 1][width])) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ����
			bool isIntersectedLeft = false;
			isIntersectedLeft = IsIntersected(leftBottom, leftTop, lineStartPos, lineEndPos);
			if (isIntersectedLeft) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, lineStartPos, lineEndPos);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width - 1 >= 0) {

					// ��_�̍��̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height][width - 1] && mapChipSizeData.max <= mapChipData[height][width - 1])) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ��_��0�������玟��
			if (intersectPos.size() <= 0) continue;

			// �S�Ă̌�_�̒�����v���C���[�Ƃ̋������ŏ��̂��̂����߂�B
			pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
			miniIntersectedPoint.first = { 100000,100000 };
			for (int index = 0; index < intersectPos.size(); ++index) {

				// ��_�Ԃ̋������ۑ�����Ă�����̂���������������A���̍��W��ۑ�����B
				if (Vec2<float>(intersectPos[index].first - prevFramePos).Length() < Vec2<float>(miniIntersectedPoint.first - prevFramePos).Length()) {

					miniIntersectedPoint = intersectPos[index];

				}

			}

			// �����߂��Ă��瓖�������ӂ�Ԃ��B


			float offset = 1.0f;

			// �ŏ��̌�_�̎�ނɂ���ď����𕪂���B
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y - size.y - offset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// �����߂��B
				pos.x = miniIntersectedPoint.first.x + size.x + offset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y + size.y + offset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// �����߂��B
				pos.x = miniIntersectedPoint.first.x - size.x - offset;

			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheScale(Vec2<float>& pos, const Vec2<float>& size, const vector<vector<int>>& mapChipData, const INTERSECTED_LINE& direction, Vec2<int>& hitChipIndex, const bool& onGimmick, const bool& isPlayer)
{
	/*===== �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� =====*/


	// �����蔻��Ɏg�p��������𐶐��B
	Vec2<float> checkHitDirection = {};
	Vec2<float> checkHitPos = {};

	switch (direction) {

	case INTERSECTED_TOP:

		// ������̐����𐶐��B
		checkHitDirection = { pos.x, pos.y - size.y };
		checkHitPos = { pos.x, pos.y + size.y };

		break;

	case INTERSECTED_BOTTOM:

		// �������̐����𐶐��B
		checkHitDirection = { pos.x, pos.y + size.y };
		checkHitPos = { pos.x, pos.y - size.y };

		break;

	case INTERSECTED_LEFT:

		// �������̐����𐶐��B
		checkHitDirection = { pos.x - size.x, pos.y };
		checkHitPos = { pos.x + size.x, pos.y };

		break;

	case INTERSECTED_RIGHT:

		// �E�����̐����𐶐��B
		checkHitDirection = { pos.x + size.x, pos.y };
		checkHitPos = { pos.x - size.x, pos.y };

		break;

	default:

		break;

	}


	const int HEIGHT = mapChipData.size();
	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�̉�
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �}�b�vID��0�������珈�����΂��B
			if (!(mapChipSizeData.min <= mapChipData[height][width] && mapChipData[height][width] <= mapChipSizeData.max)) continue;

			// ���̃}�b�v�̒��S���W�����߂�B
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// �v���C���[�Ƃ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
			if (size.x * 3.0f <= fabs(centerX - pos.x)) {
				continue;
			}
			if (size.y * 3.0f <= fabs(centerY - pos.y)) {
				continue;
			}

			// �}�b�v�`�b�v���v���C���[���g������4���_�����߂�B
			const Vec2<float> rightTop = { centerX + MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> rightBottom = { centerX + MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftTop = { centerX - MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftBottom = { centerX - MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };

			// ��_�ۑ��p
			vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;


			// �S�Ă̐����Ƃ̓����蔻����s���B

			// ���
			if (IsIntersected(rightTop, leftTop, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, checkHitPos, checkHitDirection);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height - 1 >= 0) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height - 1][width] && mapChipSizeData.max <= mapChipData[height - 1][width])) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					//}

				}

			}

			// �E��
			if (IsIntersected(rightTop, rightBottom, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, checkHitPos, checkHitDirection);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width + 1 < mapChipData[height].size()) {

					// ��_�̉E�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height][width + 1] && mapChipSizeData.max <= mapChipData[height][width + 1])) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ����
			if (IsIntersected(leftBottom, rightBottom, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, checkHitPos, checkHitDirection);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height + 1 < mapChipData.size()) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height + 1][width] && mapChipSizeData.max <= mapChipData[height + 1][width])) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ����
			if (IsIntersected(leftBottom, leftTop, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, checkHitPos, checkHitDirection);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width - 1 >= 0) {

					// ��_�̍��̃}�b�v�`�b�v���ǂ������疳��������B
					//if (!(mapChipSizeData.min <= mapChipData[height][width - 1] && mapChipSizeData.max <= mapChipData[height][width - 1])) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					//}

				}

			}

			// ��_��0�������玟��
			if (intersectPos.size() <= 0) continue;

			// �S�Ă̌�_�̒�����v���C���[�Ƃ̋������ŏ��̂��̂����߂�B
			pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
			miniIntersectedPoint.first = { 100000,100000 };
			for (int index = 0; index < intersectPos.size(); ++index) {

				// ��_�Ԃ̋������ۑ�����Ă�����̂���������������A���̍��W��ۑ�����B
				if (Vec2<float>(intersectPos[index].first - pos).Length() < Vec2<float>(miniIntersectedPoint.first - pos).Length()) {

					miniIntersectedPoint = intersectPos[index];

				}

			}

			// �s�b�^�������߂�����s�s���Ȃ��Ƃ��N����̂ŁA�����I�t�Z�b�g�l��݂���B
			float pushBackOffset = 0.0f;

			// �ŏ��̌�_�̎�ނɂ���ď����𕪂���B
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y - size.y - pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// �����߂��B
				pos.x = miniIntersectedPoint.first.x + size.x + pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y + size.y + pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// �����߂��B
				pos.x = miniIntersectedPoint.first.x - size.x - pushBackOffset;

			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}
