#pragma once

// �����蔻��ɂĂǂ̕ӂƓ����������̎��ʗp
enum INTERSECTED_LINE {
	INTERSECTED_NONE,	// �ǂ��̕ǂɂ��������Ă��Ȃ��B
	INTERSECTED_TOP,	// ��ɓ������Ă���B
	INTERSECTED_RIGHT,	// �E�ɓ������Ă���B
	INTERSECTED_LEFT,	// ���ɓ������Ă���B
	INTERSECTED_BOTTOM,	// ���ɓ������Ă���B
	INTERSECTED_RISE,	// �̂��オ��ʒu�ɓ������Ă���B
};