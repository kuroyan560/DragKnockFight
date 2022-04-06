#include "MapChipCollider.h"
#include "DrawFunc.h"

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

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheVel(Vec2<float>& pos, const Vec2<float>& prevFramePos, const Vec2<float>& vel, const Vec2<float>& size, bool& onGround, const vector<vector<int>>& mapChipData)
{
	/*===== �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� =====*/


	// �v���C���[�̈ړ��ʂ��擾
	Vec2<float> prevFramePosBuff = prevFramePos;
	Vec2<float> velBuff = vel;
	Vec2<float> playerVel = (pos + velBuff) - prevFramePosBuff;

	// �v���C���[�̈ړ��ʂ̒������v���C���[�̃T�C�Y����������������A�v���C���[�̃T�C�Y�܂ŐL�΂��B
	if (fabs(playerVel.Length()) < size.x) {

		playerVel.Normalize();
		playerVel = playerVel * size;

	}

	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�̉�
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �}�b�vID��0�������珈�����΂��B
			if (!(mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9)) continue;

			// ���̃}�b�v�̒��S���W�����߂�B
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// �z��O�������珈�����΂��B
			//if (centerX < 0) return INTERSECTED_NONE;
			//if (centerX >= WIDTH) return INTERSECTED_NONE;
			//if (centerY < 0) return INTERSECTED_NONE;
			//if (centerY >= HEIGHT) return INTERSECTED_NONE;

			// �v���C���[�Ƃ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
			if (Vec2<float>(centerX - pos.x, centerY - pos.y).Length() >= MAP_CHIP_SIZE * 2.0f) {
				onGround = false;
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
			if (IsIntersected(rightTop, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height - 1 >= 0) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height - 1][width] >= 1 && mapChipData[height - 1][width] <= 9)) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					}

				}

			}

			// �E��
			if (IsIntersected(rightTop, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width + 1 < mapChipData[height].size()) {

					// ��_�̉E�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height][width + 1] >= 1 && mapChipData[height][width + 1] <= 9)) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					}

				}

			}

			// ����
			if (IsIntersected(leftBottom, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

				// ��_�Ƃ̋������v���C���[�̔��a+�}�b�v���a��菬����������ۑ�����B
				//if (fabs(Vec2<float>(buff.first - player.centerPos).x) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE &&
				//	fabs(Vec2<float>(buff.first - player.centerPos).y - MAP_CHIP_HALF_SIZE) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE) {

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height + 1 < mapChipData.size()) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height + 1][width] >= 1 && mapChipData[height + 1][width] <= 9)) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					}

				}

			}

			// ����
			if (IsIntersected(leftBottom, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width - 1 >= 0) {

					// ��_�̍��̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height][width - 1] >= 1 && mapChipData[height][width - 1] <= 9)) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					}

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

			// �����߂��Ă��瓖�������ӂ�Ԃ��B

			// �ŏ��̌�_�̎�ނɂ���ď����𕪂���E
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y - size.y;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// �ЂƂ�̃}�b�v�`�b�v���󔒂������牟���߂��Ȃ��ŏ�ɔ�΂��Ƃ������������ށB
				int mapX = (miniIntersectedPoint.first.x - MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// ��Ɉړ�������B

					// ��Ɉړ�������ׂ��ʂ𒲂ׂ�B
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// �ړ�������B
					pos.y -= risingAmount + offset;

					// �ǂɂ�����������������Ȃ����߂ɖ߂�l�p��ID�𖳌�������B
					miniIntersectedPoint.second = INTERSECTED_RISE;

					if (0 < vel.x) {

						pos.x += offset;

					}
					else if (vel.x < 0) {

						pos.x -= offset;

					}

				}
				else {

					// �����߂��B
					pos.x = miniIntersectedPoint.first.x + size.x;

				}

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// �����߂��B
				float offset = 1.0f;
				pos.y = miniIntersectedPoint.first.y + size.y + offset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// �ЂƂ�̃}�b�v�`�b�v���󔒂������牟���߂��Ȃ��ŏ�ɔ�΂��Ƃ������������ށB
				int mapX = (miniIntersectedPoint.first.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// ��Ɉړ�������B

					// ��Ɉړ�������ׂ��ʂ𒲂ׂ�B
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// �ړ�������B
					pos.y -= risingAmount + offset;

					// �ǂɂ�����������������Ȃ����߂ɖ߂�l�p��ID�𖳌�������B
					miniIntersectedPoint.second = INTERSECTED_RISE;

					if (0 < vel.x) {

						pos.x += offset;

					}
					else if (vel.x < 0) {

						pos.x -= offset;

					}

				}
				else {

					// �����߂��B
					pos.x = miniIntersectedPoint.first.x - size.x;

				}


			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheScale(Vec2<float>& pos, const Vec2<float>& size, const vector<vector<int>>& mapChipData, const INTERSECTED_LINE& direction, const bool& onGimmick)
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
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�̉�
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �}�b�vID��0�������珈�����΂��B
			if (!(mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9)) continue;

			// ���̃}�b�v�̒��S���W�����߂�B
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// �z��O�������珈�����΂��B
			//if (centerX < 0) return INTERSECTED_NONE;
			//if (centerX >= WIDTH) return INTERSECTED_NONE;
			//if (centerY < 0) return INTERSECTED_NONE;
			//if (centerY >= HEIGHT) return INTERSECTED_NONE;

			// �v���C���[�Ƃ̋��������ȏ㗣��Ă����珈�����s��Ȃ��B
			if (size.x * 2.0f <= fabs(centerX - pos.x)) {
				//player.onGround = false;
				continue;
			}
			if (size.y * 2.0f <= fabs(centerY - pos.y)) {
				//player.onGround = false;
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
			//if (direction == INTERSECTED_BOTTOM) {
			if (IsIntersected(rightTop, leftTop, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, checkHitPos, checkHitDirection);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height - 1 >= 0) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height - 1][width] >= 1 && mapChipData[height - 1][width] <= 9)) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					}

				}

			}
			//		}

			// �E��
			//if (direction == INTERSECTED_LEFT) {
			if (IsIntersected(rightTop, rightBottom, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, checkHitPos, checkHitDirection);

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width + 1 < mapChipData[height].size()) {

					// ��_�̉E�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height][width + 1] >= 1 && mapChipData[height][width + 1] <= 9)) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					}

				}

			}
			//		}

			// ����
			//if (direction == INTERSECTED_TOP) {
			if (IsIntersected(leftBottom, rightBottom, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, checkHitPos, checkHitDirection);

				// ��_�Ƃ̋������v���C���[�̔��a+�}�b�v���a��菬����������ۑ�����B
				//if (fabs(Vec2<float>(buff.first - player.centerPos).x) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE &&
				//	fabs(Vec2<float>(buff.first - player.centerPos).y - MAP_CHIP_HALF_SIZE) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE) {

				// ����������E�̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (height + 1 < mapChipData.size()) {

					// ��_�̏�̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height + 1][width] >= 1 && mapChipData[height + 1][width] <= 9)) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					}

				}

			}
			//	}

			// ����
			//if (direction == INTERSECTED_RIGHT) {
			if (IsIntersected(leftBottom, leftTop, checkHitPos, checkHitDirection)) {

				// �������Ă������_���v�Z���ĕۑ�
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, checkHitPos, checkHitDirection);

				// �����������̃C���f�b�N�X������̂��ǂ������`�F�b�N����B
				if (width - 1 >= 0) {

					// ��_�̍��̃}�b�v�`�b�v���ǂ������疳��������B
					if (!(mapChipData[height][width - 1] >= 1 && mapChipData[height][width - 1] <= 9)) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					}

				}

			}
			//	}

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
			float pushBackOffset = -1.0f;

			// �ŏ��̌�_�̎�ނɂ���ď����𕪂���B
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y - size.y - pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// �ЂƂ�̃}�b�v�`�b�v���󔒂������牟���߂��Ȃ��ŏ�ɔ�΂��Ƃ������������ށB
				int mapX = (miniIntersectedPoint.first.x - MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (!onGimmick && mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// ��Ɉړ�������B

					// ��Ɉړ�������ׂ��ʂ𒲂ׂ�B
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// �ړ�������B
					pos.y -= risingAmount + offset;

					// �ǂɂ�����������������Ȃ����߂ɖ߂�l�p��ID�𖳌�������B
					miniIntersectedPoint.second = INTERSECTED_RISE;

					pos.x -= offset;

				}
				else {

					// �����߂��B
					pos.x = miniIntersectedPoint.first.x + size.x + pushBackOffset;

				}

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// �����߂��B
				pos.y = miniIntersectedPoint.first.y + size.y + pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// �ЂƂ�̃}�b�v�`�b�v���󔒂������牟���߂��Ȃ��ŏ�ɔ�΂��Ƃ������������ށB
				int mapX = (miniIntersectedPoint.first.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (!onGimmick && mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// ��Ɉړ�������B

					// ��Ɉړ�������ׂ��ʂ𒲂ׂ�B
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// �ړ�������B
					pos.y -= risingAmount + offset;

					// �ǂɂ�����������������Ȃ����߂ɖ߂�l�p��ID�𖳌�������B
					miniIntersectedPoint.second = INTERSECTED_RISE;

					pos.x += offset;

				}
				else {

					// �����߂��B
					pos.x = miniIntersectedPoint.first.x - size.x - pushBackOffset;

				}

			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}
