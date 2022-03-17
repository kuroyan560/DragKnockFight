#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "AuraBlock.h"
#include <vector>
#include <memory>

using namespace std;

// �O�t���[���̒��_���W�ƍ��t���[���̒��_���W��ۑ����Ă������߂̍\����
struct VertexData {

	Vec2<float> prevFramePos;	// �O�t���[���̍��W
	Vec2<float> nowFramePos;	// ���t���[���̍��W

};

// �r���[�|�[�g�ƃI�[���̔���̖߂�l�Ɏg�p����p�̍\����
struct ViewPortAuraReturnData {

	float pushBackAmount;	// �����߂���
	bool isHit;				// �����������ǂ���
	bool isHold;			// �����������Ă��邩�ǂ���

};

// �r���[�|�[�g�̂ǂ̐����̓����蔻����s���������ʂ���p�̗񋓑�
enum CHECK_HIT_LINE {

	CHECK_HIT_TOP,		// ������̔�����s���B
	CHECK_HIT_RIGHT,	// �E�����̔�����s���B
	CHECK_HIT_BOTTOM,	// �������̔�����s���B
	CHECK_HIT_LEFT,		// �������̔�����s���B

};


// �}�b�v�`�b�v�ȊO�̓����蔻��N���X
class ViewPortCollider : public Singleton<ViewPortCollider> {

public:

	/*===== �����o�ϐ� =====*/

	// ��`�̂ǂ̕ӂɓ�����������Ԃ��p�B
	enum RECTANGLE_HIT_AREA {

		HIT_AREA_TOP,		// ��̕ӂɓ��������B
		HIT_AREA_RIGHT,		// �E�̕ӂɓ��������B
		HIT_AREA_BOTTOM,	// ���̕ӂɓ��������B
		HIT_AREA_LEFT,		// ���̕ӂɓ��������B
		HIT_AREA_NONE,		// �ǂ̕ӂɂ��������Ă��Ȃ��B

	};


public:

	/*===== �����o�֐� =====*/

	/// <summary>
	/// ��`�Ɖ~�̔��� �r���[�|�[�g�ƒe�̔���Ɏg�p���܂��B
	/// </summary>
	/// <param name="centerPos"> �r���[�|�[�g�̒��S���W </param>
	/// <param name="topSize"> �r���[�|�[�g�̏�����̃T�C�Y�̐�Βl </param>
	/// <param name="rightSize"> �r���[�|�[�g�̉E�����̃T�C�Y�̐�Βl </param>
	/// <param name="bottomSize"> �r���[�|�[�g�̉������̃T�C�Y�̐�Βl </param>
	/// <param name="leftSize"> �r���[�|�[�g�̍������̃T�C�Y�̐�Βl </param>
	/// <param name="bulletPos"> �e�̍��W </param>
	/// <param name="bulletSize"> �e�̃T�C�Y </param>
	/// <returns> �ǂ̕ӂɓ����������A�ǂ̂ւ�ɂ��������Ă��Ȃ��̂��B </returns>
	RECTANGLE_HIT_AREA CheckHitRectanglePoint(const Vec2<float>& centerPos, const float& topSize, const float& rightSize, const float& bottomSize, const float& leftSize, const Vec2<float>& bulletPos, const float& bulletSize);

	/// <summary>
	/// �r���[�|�[�g�̎w��̐����ƃI�[���̓����蔻��
	/// </summary>
	/// <param name="vertex1"> �r���[�|�[�g�̎w��̐����̒��_1 ������ʂ̂�͂����� </param>
	/// <param name="vertex2"> �r���[�|�[�g�̎w��̐����̒��_2 �E�����ʂ̂�͂����� </param>
	/// <param name="checkHitLine"> �r���[�|�[�g�̓����蔻����s�����������̎���ID </param>
	/// <param name="auraData">	�I�[���ۑ��f�[�^�̔z�� </param>
	/// <returns> �߂�l�\���� </returns>
	vector<ViewPortAuraReturnData> CheckHitRectangleAura(VertexData vertex1, VertexData vertex2, const CHECK_HIT_LINE& checkHitLine, vector<unique_ptr<AuraBlock>>& auraData);


private:

	// �����Ɛ����̓����蔻��
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// �����Ɛ����̌�_�����߂鏈��
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

	// ����������񂩂�ViewPortAuraReturnData��Ԃ������B
	ViewPortAuraReturnData GetViewPortAuraReturnData(const CHECK_HIT_LINE& checkHitLine, VertexData vertex1, VertexData vertex2, const Vec2<float>& leftUpPos, const Vec2<float>& rightUpPos, const Vec2<float>& rightDownPos, const Vec2<float>& leftDownPos, const CHECK_HIT_LINE& auraLine, bool isPrevViewPort = false);

};