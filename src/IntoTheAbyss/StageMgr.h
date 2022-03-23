#pragma once
#include<string>
#include"CSVLoader.h"
#include"Singleton.h"
#include<array>
#include"Vec.h"
#include<assert.h>
#include"GimmickLoader.h"

#include"WinApp.h"
#include"Angle.h"
#include"KuroFunc.h"

/// <summary>
/// 小部屋同士の部屋のリンク付けする際に必要な情報
/// </summary>
struct RelationData
{
	int doorNumber;
	int nextRoomNumber;

	RelationData() :doorNumber(-1), nextRoomNumber(-1)
	{
	}
};

/// <summary>
/// マップチップ領域の最小値と最大値
/// </summary>
struct SizeData
{
	int min;
	int max;
};

/// <summary>
/// マップチップの種類
/// </summary>
enum MapChipData
{
	MAPCHIP_TYPE_STATIC_BLOCK,
	MAPCHIP_TYPE_MOVING_BLOCK,
	MAPCHIP_TYPE_TOCH,
	MAPCHIP_TYPE_DOOR,
	MAPCHIP_TYPE_THOWNP,
	MAPCHIP_TYPE_BUBBLE,
	MAPCHIP_TYPE_MAX
};
enum MapChipBlockData
{
	MAPCHIP_BLOCK_START = 30,
	MAPCHIP_BLOCK_GOAL = 31,
	MAPCHIP_BLOCK_DEBUG_START = 32,
	MAPCHIP_BLOCK_AURABLOCK = 40
};

struct MapChipDrawData
{
	int handle = -1;	//画像ハンドル
	float radian;//画像の角度
	bool animationFlag;//アニメーションを有効にするフラグ
	int interval;		//アニメーションの間隔
	int animationNum;//現在参照しているアニメーションの画像
	Vec2<float> offset;	//32*32の画像じゃない場合の画像位置調整
	Vec2<float> offsetInterval;//上下左右の画像の間隔を空ける

	//MapChipDrawData() : handle(-1), radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	MapChipDrawData() : radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	{
	};
};

struct MapChipAnimationData
{
	vector<int> handle;	//再生するアニメーションの総数
	int maxInterval;		//アニメーションの間隔

	MapChipAnimationData(const vector<int> &HANDLE, int INTERVAL = -1) :handle(HANDLE), maxInterval(INTERVAL)
	{
	};
};

typedef std::vector<std::vector<int>> RoomMapChipArray;
typedef std::vector<RoomMapChipArray> StageMapChipData;

typedef std::vector<RelationData> RoomRelationData;
typedef std::vector<RoomRelationData> StageRelationData;

typedef std::vector<std::vector<MapChipDrawData>> RoomMapChipDrawArray;
typedef std::vector<RoomMapChipDrawArray> StageMapChipDrawData;

//全てのステージのマップチップデータを管理するクラス
class StageMgr :public Singleton<StageMgr>
{
public:
	StageMgr();

	/// <summary>
	/// 小部屋のマップチップデータを受け取ります
	/// </summary>
	/// <param name="STAGE_NUMBER">ステージ番号</param>
	/// <param name="ROOM_NUMBER">小部屋番号</param>
	/// <returns>vector<vector<int>>のマップチップ配列</returns>
	const RoomMapChipArray &GetMapChipData(const int &STAGE_NUMBER, const int &ROOM_NUMBER);

	/// <summary>
	/// 扉からどこの部屋に向かうかを返します
	/// </summary>
	/// <param name="STAGE_NUMBER">ステージ番号</param>
	/// <param name="ROOM_NUMBER">小部屋番号</param>
	/// <param name="DOOR_NUMBER">ドア番号</param>
	/// <returns>次に向かう小部屋番号</returns>
	const int &GetRelationData(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER);

	/// <summary>
	/// マップチップ領域の情報を返します
	/// </summary>
	/// <param name="TYPE">マップチップの種類</param>
	/// <returns>マップチップ領域の情報</returns>
	const SizeData GetMapChipSizeData(MapChipData TYPE);

	/// <summary>
	/// XとYのブロック座標の情報から小部屋のチップ番号を返します
	/// </summary>
	/// <param name="STAGE_NUMBER">ステージ番号</param>
	/// <param name="ROOM_NUMBER">小部屋番号</param>
	/// <param name="MAPCHIP_POS">マップチップ座標</param>
	/// <returns>チップ番号</returns>
	inline const int &GetMapChipBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const Vec2<float> &MAPCHIP_POS)
	{
		RoomMapChipArray tmp = allMapChipData[STAGE_NUMBER][ROOM_NUMBER];

		Vec2<int>mapChipPos;
		mapChipPos.x = MAPCHIP_POS.x;
		mapChipPos.y = MAPCHIP_POS.y;

		//Y軸のマップチップが範囲外なら-1を返す
		if (tmp.size() <= mapChipPos.y || mapChipPos.y < 0)
		{
			return -1;
		}
		//X軸のマップチップが範囲外なら-1を返す
		if (tmp[mapChipPos.y].size() <= mapChipPos.x || mapChipPos.x < 0)
		{
			return -1;
		}
		int result = tmp[mapChipPos.y][mapChipPos.x];
		//指定した場所のマップチップ番号を返す
		return result;
	};

	RoomMapChipDrawArray GetMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER);


	const bool &CheckStageNum(const int &STAGE_NUMBER);
	const bool &CheckRoomNum(const int &STAGE_NUMBER, const int &ROOM_NUMBER);


	/// <summary>
	/// 指定したステージ番号の小部屋の数を返します
	/// </summary>
	/// <param name="STAGE_NUMBER">ステージ番号</param>
	/// <returns>小部屋の数</returns>
	inline const int &GetMaxRoomNumber(const int &STAGE_NUMBER)
	{
		return allMapChipData[STAGE_NUMBER].size();
	}

	/// <summary>
	/// 今保管されているステージの数を返します
	/// </summary>
	/// <returns></returns>
	inline const int &GetMaxStageNumber()
	{
		return allMapChipData.size();
	}

	std::vector<MapChipAnimationData *> animationData;//マップチップのアニメーション情報の一覧

private:
	CSVLoader loder;	//CSVデータを読み込む為のクラス

	//四次元配列
	/*
	[0]...ステージ番号
	[1]...小部屋番号
	[2]...Y軸
	[3]...X軸
	*/
	std::vector<StageMapChipData> allMapChipData;	//ゲーム内にある全てのステージのマップチップデータ
	std::vector<StageRelationData> relationRoomData;//ゲーム内にある全てのステージの小部屋同士のリンク付け
	std::array<SizeData, MAPCHIP_TYPE_MAX> mapChipMemoryData;//マップチップ番号の領域

	std::vector<StageMapChipDrawData> allMapChipDrawData;//マップチップの描画情報

	array<int, 30> mapChipGraphHandle;
	enum
	{
		MAPCHIP_DRAW_WALL_LEFT_UP,
		MAPCHIP_DRAW_FLOOR,
		MAPCHIP_DRAW_WALL_RIGHT_UP,
		MAPCHIP_DRAW_WALL_TOP,
		MAPCHIP_DRAW_WALL_IN_RIGHT_DOWN,
		MAPCHIP_DRAW_WALL_IN_LEFT_DOWN,
		MAPCHIP_DRAW_WALL_LEFT,
		MAPCHIP_DRAW_WALL_IN,
		MAPCHIP_DRAW_WALL_RIGHT,
		MAPCHIP_DRAW_WALL_CENTRAL,
		MAPCHIP_DRAW_WALL_IN_RIGHT_UP,
		MAPCHIP_DRAW_WALL_IN_LEFT_UP,
		MAPCHIP_DRAW_WALL_LEFT_DOWN,
		MAPCHIP_DRAW_WALL_DOWN,
		MAPCHIP_DRAW_WALL_RIGHT_DOWN,
		MAPCHIP_DRAW_WALL_BUTTOM,
		MAPCHIP_DRAW_WALL_SPACE1,
		MAPCHIP_DRAW_WALL_SCPACE2,
		MAPCHIP_DRAW_WALL_CORNER,
		MAPCHIP_DRAW_WALL_TBLOCK,
		MAPCHIP_DRAW_WALL_TBLOCK2,
		MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL,
		MAPCHIP_DRAW_WALL_RIGHTDOWN_1PIXEL,
		MAPCHIP_DRAW_ERROR,
		MAPCHIP_DRAW_DOOR
	};


	float Radian(float ANGLE)
	{
		return ANGLE * Angle::PI() / 180.0f;
	}
	;

	bool CheckDoor(vector<Vec2<float>> *DATA, int STAGE_NUM, int ROOM_NUM, Vec2<float> MAPCHIP, int DOOR_NUM);

	inline void DoorError(int STAGE_NUM, int ROOM_NUM, int DOOR_NUM)
	{
		string text("ステージ");
		string text2("部屋番号");
		string text3("ドア番号");
		string text4("は同じ部屋に二つ以上使われています");

		string result =
			text + std::to_string(STAGE_NUM) +
			text2 + std::to_string(ROOM_NUM) +
			text3 + std::to_string(DOOR_NUM) +
			text4 + "\nOKを押した後強制終了しますので、もう一度設定し直して下さい";

		//MessageBox(NULL, result.c_str(), TEXT("ドア番号被り"), MB_OK);
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("ドア番号被り"), MB_OK);
		assert(0);
	}

	inline void StartError(int STAGE_NUM, int ROOM_NUM, int DOOR_NUM)
	{
		string text("ステージ");
		string text2("部屋番号");
		string text3("ドア番号");
		string text4("は同じ部屋に二つ以上使われています");

		string result =
			text + std::to_string(STAGE_NUM) +
			text2 + std::to_string(ROOM_NUM) +
			text3 + std::to_string(DOOR_NUM) +
			text4 + "\nOKを押した後強制終了しますので、もう一度設定し直して下さい";

		//MessageBox(NULL, result.c_str(), TEXT("ドア番号被り"), MB_OK);
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("ドア番号被り"), MB_OK);
		assert(0);
	}

	//指定の番号が壁かどうか判断する
	inline void AlimentWallNumber(int *CHIP_NUMBER)
	{
		bool outSideFlag = *CHIP_NUMBER == -1;
		bool isWallFlag = false;

		//チップ番号が1から9以下なら壁判定を出す
		if (1 <= *CHIP_NUMBER && *CHIP_NUMBER <= 9)
		{
			isWallFlag = true;
		}

		if (isWallFlag || outSideFlag)
		{
			*CHIP_NUMBER = 1;
		}
	};

	//指定の番号が空白かどうか判断する
	inline void AlimentSpaceNumber(int *CHIP_NUMBER)
	{
		bool isSpaceFlag = false;

		//チップ番号が0もしくは10以上なら空白判定を出す
		if (*CHIP_NUMBER == 0 || 10 <= *CHIP_NUMBER)
		{
			isSpaceFlag = true;
		}

		if (isSpaceFlag)
		{
			*CHIP_NUMBER = 0;
		}
	};

};

