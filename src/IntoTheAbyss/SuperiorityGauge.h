#pragma once
#include"../Common/Singleton.h"
#include<memory>
#include"../Common/Vec.h"

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
	/// �v���C���[�ɃQ�[�W�ʂ�ǉ����A�ǉ��������G�l�~�[�̃Q�[�W�ʂ����炵�܂�
	/// </summary>
	/// <param name="VALUE">�ǉ�����l</param>
	void AddPlayerGauge(const float &VALUE);

	/// <summary>
	/// �G�l�~�[�ɃQ�[�W�ʂ�ǉ����A�ǉ��������v���C���[�̃Q�[�W�ʂ����炵�܂�
	/// </summary>
	/// <param name="VALUE"></param>
	void AddEnemyGauge(const float &VALUE);

	/// <summary>
	/// �v���C���[�Ɋւ���Q�[�W�̃f�[�^����肵�܂�
	/// </summary>
	/// <returns>�v���C���[�̃Q�[�W�f�[�^</returns>
	const std::unique_ptr<GaugeData> &GetPlayerGaugeData();

	/// <summary>
	/// �G�l�~�[�Ɋւ���Q�[�W�̃f�[�^����肵�܂�
	/// </summary>
	/// <returns>�G�l�~�[�̃Q�[�W�f�[�^</returns>
	const std::unique_ptr<GaugeData> &GetEnemyGaugeData();

	const bool &IsStacking();

	void Init();
	void Update();
	void Draw();

	void DebugValue(float *ADD_VALUE);
private:
	static float GAUGE_MAX_VALUE;			//�Q�[�W�̍ő��
	static int STACK_MAX_TIMER;			//���b�ԝh�R������h�R��Ԃ��Ɣ��f���邩

	std::unique_ptr<GaugeData> playerGaugeData;	//�v���C���[�̃Q�[�W�f�[�^
	std::unique_ptr<GaugeData> enemyGaugeData;	//�G�̃Q�[�W�f�[�^

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
