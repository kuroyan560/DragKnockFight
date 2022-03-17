#include "ViewPortCollider.h"
#include "MapChipCollider.h"

ViewPortCollider::RECTANGLE_HIT_AREA ViewPortCollider::CheckHitRectanglePoint(const Vec2<float>& centerPos, const float& topSize, const float& rightSize, const float& bottomSize, const float& leftSize, const Vec2<float>& bulletPos, const float& bulletSize)
{

	/*===== ��`�Ɖ~�̓����蔻�� -�r���[�|�[�g�ƒe�p- =====*/

	// �e���_���v�Z����B
	Vec2<float> rectangleTopLeft = Vec2<float>(centerPos.x - leftSize, centerPos.y - topSize);
	Vec2<float> rectangleTopRight = Vec2<float>(centerPos.x + rightSize, centerPos.y - topSize);
	Vec2<float> rectangleBottomRight = Vec2<float>(centerPos.x + rightSize, centerPos.y + bottomSize);
	Vec2<float> rectangleBottomLeft = Vec2<float>(centerPos.x - leftSize, centerPos.y + bottomSize);

	// ��`�̊O�ɏo�Ă���̂����`�F�b�N����B
	if (!(bulletPos.x >= rectangleTopLeft.x + bulletSize && bulletPos.x <= rectangleTopRight.x - bulletSize &&
		bulletPos.y >= rectangleTopRight.y + bulletSize && bulletPos.y <= rectangleBottomRight.y - bulletSize)) {

		// ��`�̊O�ɏo�Ă���Ƃ������Ƃ́A�ǂ�������ɓ��������Ƃ������ƁB


		// �㉺���E���ꂼ��̋��������߂�B
		array<pair<float, ViewPortCollider::RECTANGLE_HIT_AREA>, ViewPortCollider::HIT_AREA_NONE> distanceBuff;

		// ���������߂�B
		distanceBuff[ViewPortCollider::HIT_AREA_TOP] = { fabs(rectangleTopRight.y) - fabs(bulletPos.y), ViewPortCollider::HIT_AREA_TOP };
		distanceBuff[ViewPortCollider::HIT_AREA_RIGHT] = { fabs(bulletPos.x) - fabs(rectangleTopRight.x), ViewPortCollider::HIT_AREA_RIGHT };
		distanceBuff[ViewPortCollider::HIT_AREA_BOTTOM] = { fabs(bulletPos.y) - fabs(rectangleBottomRight.y), ViewPortCollider::HIT_AREA_BOTTOM };
		distanceBuff[ViewPortCollider::HIT_AREA_LEFT] = { fabs(rectangleTopLeft.x) - fabs(bulletPos.x), ViewPortCollider::HIT_AREA_LEFT };

		// ��ԑ傫���v�f����������B
		pair<float, ViewPortCollider::RECTANGLE_HIT_AREA> mostBigAmount = { -100000, ViewPortCollider::HIT_AREA_NONE };

		for (int index = 0; index < ViewPortCollider::HIT_AREA_NONE; ++index) {

			// �v�f�������������玟��
			if (mostBigAmount.first >= distanceBuff[index].first) continue;

			// �v�f��ۑ��B
			mostBigAmount = distanceBuff[index];

		}

		// ��ԑ傫���v�f��Ԃ��B
		return mostBigAmount.second;

	}

	// ��`�̓����ɂ���Ƃ������Ƃ́A�܂��ǂ��ɂ��������Ă��Ȃ��Ƃ������ƁB

	return ViewPortCollider::HIT_AREA_NONE;
}

vector<ViewPortAuraReturnData> ViewPortCollider::CheckHitRectangleAura(VertexData vertex1, VertexData vertex2, const CHECK_HIT_LINE& checkHitLine, vector<unique_ptr<AuraBlock>>& auraData)
{

	/*===== �r���[�|�[�g�̎w��̐����ƃI�[���̓����蔻�� =====*/

	// �߂�l�p�z��B
	vector<ViewPortAuraReturnData> returnData;

	// �����Ώۂ̐����̑O�t���[���̍��W�Ȃǂ��g���ċ�`�𐶐�����B
	Vec2<float> viewportLeftUpPos = {};

	// �I�[���f�[�^�̃T�C�Y�����߂�B
	const int MAX_AURA_BLOCK = auraData.size();

	// �T�C�Y���z����񂵂āA�S�Ẵu���b�N�Ƃ̓����蔻����s���B
	for (int auraIndex = 0; auraIndex < MAX_AURA_BLOCK; ++auraIndex) {

		float offset = 0.0f;

		// �킩��₷�����邽�߁A�I�[���̊e���_����U�����ɏ����o���B
		Vec2<float> leftUpPos = auraData[auraIndex]->leftUpPos + Vec2<float>(offset, offset);
		Vec2<float> leftDownPos = Vec2<float>(auraData[auraIndex]->leftUpPos.x, auraData[auraIndex]->rightDownPos.y) + Vec2<float>(offset, -offset);
		Vec2<float> rightUpPos = Vec2<float>(auraData[auraIndex]->rightDownPos.x, auraData[auraIndex]->leftUpPos.y) + Vec2<float>(-offset, offset);
		Vec2<float> rightDownPos = auraData[auraIndex]->rightDownPos + Vec2<float>(-offset, -offset);

		/*-- ��`���m�őS�Ă̐����Ɛ����̓����蔻����s���B --*/

		// �܂��͍��t���[���Ƃ̃r���[�|�[�g�̐����Ƃ̔�����s���B
		{

			// �I�[���̏�̕ӂ��瓖���蔻����s���B
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, leftUpPos, rightUpPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_TOP);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̉E�̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, rightUpPos, rightDownPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_RIGHT);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̉��̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, leftDownPos, rightDownPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_BOTTOM);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̍��̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, Vec2<float>(leftUpPos.x + 0.0f, leftUpPos.y), leftDownPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_LEFT);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}

		}

		// ���ɑO�t���[���Ƃ̃r���[�|�[�g�̐����Ƃ̔�����s���B
		{

			//�I�[���̏�̕ӂ��瓖���蔻����s���B
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, leftUpPos, Vec2<float>(rightUpPos.x + 0.0f, rightUpPos.y))) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_TOP, true);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̉E�̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, rightUpPos, rightDownPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_RIGHT, true);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̉��̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, leftDownPos, rightDownPos)) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_BOTTOM, true);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}
			// �I�[���̍��̕ӂ̓����蔻����s���B
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, Vec2<float>(leftUpPos.x + 0.0f, leftUpPos.y), Vec2<float>(leftDownPos.x - 0.0f, leftDownPos.y))) {

				// ������ID�A���t���[���̍��W�ƑO�t���[���̍��W�̊֌W���牟���߂������ƈ���������t���O�̗L���𒲂ׂ�B
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_LEFT, true);

				// �f�[�^��ۑ��B
				returnData.push_back(buff);

			}

		}

	}

	return returnData;

}

bool ViewPortCollider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
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
	// T���̎���true��Ԃ����Ȃ��ׂɎ����B
	//if (buffE == 0 && buffF != 0) return false;
	//if (buffE != 0 && buffF == 0) return false;
	if (buffE == 0 && buffF == 0) zero1 = true;
	if (result1 > -0.001f && result1 < 0) zero1 = true;
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
	//if (buffE == 0 && buffF != 0) return false;
	//if (buffE != 0 && buffF == 0) return false;
	if (buffE * buffF == 0) zero2 = true;
	if (fabs(result2) < 0.001f) zero2 = true;

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

Vec2<float> ViewPortCollider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//��_�����߂� ���̎��͎������̂܂�
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

ViewPortAuraReturnData ViewPortCollider::GetViewPortAuraReturnData(const CHECK_HIT_LINE& checkHitLine, VertexData vertex1, VertexData vertex2, const Vec2<float>& leftUpPos, const Vec2<float>& rightUpPos, const Vec2<float>& rightDownPos, const Vec2<float>& leftDownPos, const CHECK_HIT_LINE& auraLine, bool isPrevViewPort)
{
	// �q�b�g���ۑ��p�ϐ��B
	ViewPortAuraReturnData hitData{};

	const float offset = 0.0f;

	// �ゾ������B
	if (checkHitLine == CHECK_HIT_TOP) {

		// ���t���[���̂ق����ゾ������(�������ɓo���Ă���ꍇ)�B
		if (vertex1.prevFramePos.y > vertex1.nowFramePos.y) {

			// �O�t���[���̔��肾�����ꍇ�A���A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = leftDownPos.y - vertex1.nowFramePos.y + offset;

				// ���̏ꍇ�͈����������Ă��Ȃ��B
				hitData.isHold = false;

				// �������Ă���B
				hitData.isHit = true;

			}

		}
		// �O�t���[���̂ق����ゾ������(�ォ�牺�ɗ����Ă��Ă���ꍇ)�B
		else if (vertex1.prevFramePos.y < vertex1.nowFramePos.y) {

			// �O�t���[���̔��肾�����ꍇ�A��A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_BOTTOM || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = leftUpPos.y - vertex1.nowFramePos.y - offset;

				// ���̏ꍇ�͈����������Ă���B
				hitData.isHold = true;

				// �������Ă���B
				hitData.isHit = true;

			}

		}

	}
	// �E����������B
	else if (checkHitLine == CHECK_HIT_RIGHT) {

		// �O�t���[���̂ق����E��������(�E���獶�ɗ��Ă���ꍇ)�B
		if (vertex1.prevFramePos.x > vertex1.nowFramePos.x) {

			// �O�t���[���̔��肾�����ꍇ�A��A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_LEFT || auraLine == CHECK_HIT_BOTTOM))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = rightDownPos.x - vertex1.nowFramePos.x + 0.0f;

				// ���̏ꍇ�͈����������Ă���B
				hitData.isHold = true;

				// �������Ă���B
				hitData.isHit = true;

			}

		}
		// ���t���[���̂ق����E��������(������E�ɗ��Ă���ꍇ)�B
		else if (vertex1.prevFramePos.x < vertex1.nowFramePos.x) {

			// �O�t���[���̔��肾�����ꍇ�A��A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_BOTTOM))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = leftDownPos.x - vertex1.nowFramePos.x - 0.0f;

				// ���̏ꍇ�͈����������Ă��Ȃ��B
				hitData.isHold = false;

				// �������Ă���B
				hitData.isHit = true;

			}

		}

	}
	// ������������B
	else if (checkHitLine == CHECK_HIT_BOTTOM) {

		// �O�t���[���̂ق�������������(�������ɗ��Ă���ꍇ)�B
		if (vertex1.prevFramePos.y > vertex1.nowFramePos.y) {

			// �O�t���[���̔��肾�����ꍇ�A���A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = rightDownPos.y - vertex1.nowFramePos.y + offset;

				// ���̏ꍇ�͈����������Ă���B
				hitData.isHold = true;

				// �������Ă���B
				hitData.isHit = true;

			}

		}
		// ���t���[���̂ق�������������(�ォ�牺�ɗ��Ă���ꍇ)�B
		else if (vertex1.prevFramePos.y < vertex1.nowFramePos.y) {

			// �O�t���[���̔��肾�����ꍇ�A���A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_BOTTOM || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = rightUpPos.y - vertex1.nowFramePos.y - offset;

				// ���̏ꍇ�͈����������Ă��Ȃ��B
				hitData.isHold = false;

				// �������Ă���B
				hitData.isHit = true;

			}

		}

	}
	// ������������B
	else if (checkHitLine == CHECK_HIT_LEFT) {

		// �O�t���[���̂ق�������������(�E���獶�ɗ��Ă���ꍇ)�B
		if (vertex1.prevFramePos.x < vertex1.nowFramePos.x) {

			// �O�t���[���̔��肾�����ꍇ�A��A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_BOTTOM))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = leftDownPos.x - vertex1.nowFramePos.x - 0.0f;

				// ���̏ꍇ�͈����������Ă���B
				hitData.isHold = true;

				// �������Ă���B
				hitData.isHit = true;

			}

		}
		// �O�t���[���̂ق�������������(������E�ɗ��Ă���ꍇ)�B
		else if (vertex1.prevFramePos.x > vertex1.nowFramePos.x) {

			// �O�t���[���̔��肾�����ꍇ�A��A�E�A���̏ꍇ�͏������s��Ȃ��B
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_LEFT || auraLine == CHECK_HIT_BOTTOM))) {

				// ��_�Ƃ̍��������߂ĉ����߂��ʂ����߂�B
				hitData.pushBackAmount = rightDownPos.x - vertex1.nowFramePos.x + 0.0f;

				// ���̏ꍇ�͈����������Ă��Ȃ��B
				hitData.isHold = false;

				// �������Ă���B
				hitData.isHit = true;

			}

		}

	}

	return hitData;
}
