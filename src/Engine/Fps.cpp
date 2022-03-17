#include"Fps.h"
#include <profileapi.h>

static const float MILL_SECOND = 1.0 / 1000;

Fps::Fps(const int& FrameRate) : frameRate(FrameRate), minFrameTime(1.0f / (float)frameRate)
{
	// ���C�����[�v�ɓ���O�ɐ��x���擾���Ă���
	if (QueryPerformanceFrequency(&timeFreq) == FALSE) { // ���̊֐���0(FALSE)���A�鎞�͖��Ή�
		// ��������QueryPerformanceFrequency���g���Ȃ��l��(�Â�)PC�ł͂ǂ����F�X�L�c�C���낤��
	}
	// 1�x�擾���Ă���(����v�Z�p)
	QueryPerformanceCounter(&timeStart);
}

void Fps::Update()
{
	Sleep(MILL_SECOND * 20);	//20ms�҂�

	// ���̎��Ԃ��擾
	QueryPerformanceCounter(&timeEnd);
	// (���̎��� - �O�t���[���̎���) / ���g�� = �o�ߎ���(�b�P��)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < minFrameTime) { // ���Ԃɗ]�T������
		// �~���b�ɕϊ�
		DWORD sleepTime = static_cast<DWORD>((minFrameTime - frameTime) * 1000);

		timeBeginPeriod(1); // ����\���グ��(�������Ȃ���Sleep�̐��x�̓K�^�K�^)
		Sleep(sleepTime);   // �Q��
		timeEndPeriod(1);   // �߂�

		// ���T�Ɏ����z��(�������Ȃ���fps���ςɂȂ�?)
	}

	if (frameTime > 0.0) { // �o�ߎ��Ԃ�0���傫��(�������Ȃ��Ɖ��̌v�Z�Ń[�����Z�ɂȂ�Ǝv���)
		fps = (fps * 0.99f) + (0.01f / frameTime); // ����fps���v�Z
		timeStart = timeEnd; // ����ւ�
	}
}