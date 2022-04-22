#include "Vec.h"
#include <array>
#include <vector>

// �U��񂵗̈�����̂��߂̐����P��
class SwingLineSegment {

private:

	/*===== �����o�ϐ� =====*/

	Vec2<float> start;		// �����J�n�n�_
	Vec2<float> end;		// �����I���n�_
	Vec2<float> startDir;	// ���S����̃x�N�g��
	Vec2<float> endDir;		// ���S����̃x�N�g��
	int alpha;				// �A���t�@�l
	int graphHandle;		// �g�p����摜
	bool isActive;			// �L�����t���O


public:

	enum class SEGMENT_ID {

		SEGMENT_ID_LINE,	// �ʏ�̐�
		SEGMENT_ID_UI,		// ���������
		SEGMENT_ID_ARROW,	// ���

	};

private:

	SEGMENT_ID id;


public:

	// �R���X�g���N�^
	SwingLineSegment() :start({}), end({}), startDir({}), endDir({}), alpha(0), graphHandle(0), isActive(false) {};

	// ����������
	void Init();

	// �X�V����
	void Update(const Vec2<float>& Start, const Vec2<float>& End, const Vec2<float>& StartDir, const Vec2<float>& EndDir, const int& Alpha, const SEGMENT_ID& ID, const int& Handle);

	// �`�揈��
	void Draw();

	// �����C�� �X�C���O���͐����̍X�V���~�߂邪�A�X�C���O���ɕR�̒������ς���Ă��܂��ꍇ������̂ł���ɑΉ������邽�߂ɒǉ��B
	void ResetDistance(const Vec2<float>& Pos, const float& Distance);

	inline void SetAlpha(const int& Alpha) { alpha = Alpha; }
	inline void Deactivate() { isActive = false; }

};


// �������Ǘ�����N���X
class SwingLineSegmentMgr {

private:

	/*===== �����o�ϐ� =====*/

	static const int LINE_COUNT = 9;	// �����̐�
	std::array<SwingLineSegment, LINE_COUNT> lineSegments;	// ����

	int UIHandle;		// �{�^����UI�̉摜
	int arrowHandle;	// ���摜
	int lineHandle;		// ���̉摜

	bool isClockWise;	// ���v��肩�H

	const float ANGLE_DIFFERENCE = 0.349066f;	// 20��

public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting(const bool& IsClockWise, const int& HandleUI, const int& HandleArrow, const int& HandleLine);

	// ����������
	void Init();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="Pos"> �L�������W </param>
	/// <param name="TargetVec"> �U��񂵑Ώۂւ̃x�N�g�� </param>
	/// <param name="Distance"> �U��񂵑Ώۂ܂ł̋��� </param>
	/// <param name="IsSwing"> �U��񂵂����Ă��邩 </param>
	/// <param name="NoMove"> �����̍X�V�����Ȃ��� </param>
	void Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const bool& IsSwing, const bool& NoMove, const std::vector<std::vector<int>>& mapData);

	// �`�揈��
	void Draw();

private:

	// �����ƃ}�b�v�`�b�v�̌���
	bool CheckHitMapChip();

};