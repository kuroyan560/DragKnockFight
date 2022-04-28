#include "Vec.h"
#include <array>
#include <vector>
#include"CharacterInfo.h"

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
	void Draw(const WHICH_TEAM& Team);

	// �����C�� �X�C���O���͐����̍X�V���~�߂邪�A�X�C���O���ɕR�̒������ς���Ă��܂��ꍇ������̂ł���ɑΉ������邽�߂ɒǉ��B
	void ResetDistance(const Vec2<float>& Pos, const float& Distance);

	inline void SetAlpha(const int& Alpha) { alpha = Alpha; }
	inline void Deactivate() { isActive = false; }
	inline const Vec2<float>& GetStart() { return start; }
	inline const Vec2<float>& GetEnd() { return end; }
	inline const bool& GetIsActive() { return isActive; }
	inline const SEGMENT_ID& GetID() { return id; }
	inline void SetEnd(const Vec2<float>& End) { end = End; }
	inline void SetID(const SEGMENT_ID& ID) { id = ID; }
	inline void SetHandle(const int& Handle) { graphHandle = Handle; }

};


// �������Ǘ�����N���X
class SwingLineSegmentMgr {

private:

	/*===== �����o�ϐ� =====*/

	static const int LINE_COUNT = 9;	// �����̐�
	std::array<SwingLineSegment, LINE_COUNT> lineSegments;	// ����
	Vec2<float> reticlePos;	// �Ə����W

	int UIHandle;		// �{�^����UI�̉摜
	int arrowHandle;	// ���摜
	int lineHandle;		// ���̉摜
	int reticleHandle;	// �Ə��摜

	bool isClockWise;	// ���v��肩�H

	const float ANGLE_DIFFERENCE = 0.349066f;	// 20��

public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting(const bool& IsClockWise, const int& HandleUI, const int& HandleArrow, const int& HandleLine, const int& ReticleHandle);

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
	void Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const std::vector<std::vector<int>>& MapData);

	// �`�揈��
	void Draw(const WHICH_TEAM& Team);

private:

	// �����ƃ}�b�v�`�b�v�̌���
	const Vec2<float>& SwingLineSegmentMgr::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos);

};