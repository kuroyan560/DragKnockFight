#pragma once
#include"../Common/Singleton.h"
#include<memory>
#include"../Common/Vec.h"
#include"CharacterInterFace.h"

/// <summary>
/// �D���Q�[�W
/// </summary>
class SuperiorityGauge:public Singleton<SuperiorityGauge>
{
private:
	//�Q�[�W�Ɋւ���f�[�^
	struct GaugeData
	{
		float gaugeValue;		//�Q�[�W�̒l
		float gaugeDivValue;	//�Q�[�W�̊���
		bool prevOverGaugeFlag;	//1F�O�̐U��؂�������
		bool overGaugeFlag;		//�U��؂�������

		GaugeData() {}
		void Init(const float& GaugeMaxValue)
		{
			gaugeValue = GaugeMaxValue / 2.0f;
			gaugeDivValue = 0.5f;
			overGaugeFlag = false;
			prevOverGaugeFlag = false;
		}
	};

public:
	SuperiorityGauge();

	/// <summary>
	/// �Q�[�W�ϓ�
	/// </summary>
	/// <param name="Team">���E�ǂ���̃`�[����</param>
	/// <param name="Value">�ϓ������
	/// </param>
	void AddGauge(const WHICH_TEAM& Team, const float& Value);

	/// <summary>
	/// �v���C���[�Ɋւ���Q�[�W�̃f�[�^����肵�܂�
	/// </summary>
	/// <returns>�v���C���[�̃Q�[�W�f�[�^</returns>
	const GaugeData& GetLeftGaugeData();

	/// <summary>
	/// �G�l�~�[�Ɋւ���Q�[�W�̃f�[�^����肵�܂�
	/// </summary>
	/// <returns>�G�l�~�[�̃Q�[�W�f�[�^</returns>
	const GaugeData& GetRightGaugeData();

	const bool &IsStacking();

	void Init();
	void Update();
	void Draw();

	void DebugValue(float *ADD_VALUE);
private:
	static float GAUGE_MAX_VALUE;			//�Q�[�W�̍ő��
	static int STACK_MAX_TIMER;			//���b�ԝh�R������h�R��Ԃ��Ɣ��f���邩

	GaugeData gaugeData[TEAM_NUM];

	bool isStackingFlag;	//�h�R���Ă��邩�ǂ���
	int stackTimer;			//���b�h�R���Ă��邩

	//�`��p
	int gaugeGraphHandle;
	int gaugeVarGraphPlayer;
	int gaugeVarGraphEnemy;

	void LimitGauge();
	//�f�o�b�N�p
	float oldMaxGauge;

	int imguiHandle;
};
