#pragma once
#include"../KuroEngine.h"
#include<vector>
#include<array>
#include"StageMgr.h"

static const int GOAL_MAX_NUM = 5;

/// <summary>
/// �E�F�C�|�C���g�������
/// </summary>
struct WayPointData
{
	Vec2<int> handle;						//�n���h��
	Vec2<float> pos;						//���W
	float radius;							//�傫��
	std::vector<Vec2<int>> wayPointHandles;		//�ǂ̕����ɍs���邩�n���h�����������
	std::array<int, GOAL_MAX_NUM> passNum;	//�ڕW�n�_�܂ł̃p�X��

	WayPointData() :handle(Vec2<int>(-1, -1))
	{
	}

	void RegistHandle(const Vec2<int> &HANDLE)
	{
		for (int i = 0; i < wayPointHandles.size(); ++i)
		{
			if (wayPointHandles[i].x == HANDLE.x && wayPointHandles[i].x == HANDLE.y)
			{
				return;
			}
		}
		wayPointHandles.push_back(HANDLE);
	}
};

class NavigationAI
{
public:
	/// <summary>
	/// �|�C���g�̐���
	/// </summary>
	/// <param name="MAP_DATA">�X�e�[�W��CSV</param>
	void Init(const RoomMapChipArray &MAP_DATA);

	void Update(const Vec2<float> &POS);

	void Draw();
private:

	static const int WAYPOINT_MAX_X = 10;
	static const int WAYPOINT_MAX_Y = 10;

	std::array<std::array<WayPointData, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> wayPoints;//�E�F�C�|�C���g�̔z��

	inline const Vec2<int> &GetMapChipNum(const Vec2<float> &WORLD_POS);

};

