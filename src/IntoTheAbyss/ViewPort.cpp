#include "ViewPort.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"

#include"KuroEngine.h"

void ViewPort::Init(Vec2<float> PLAYER_POS, Vec2<float> WINDOW_SIZE)
{
	//座標の設定-----------------------
	windowSize = WINDOW_SIZE;
	Vec2<float> windowHalfSize = windowSize / Vec2<float>(2.0f, 2.0f);
	Vec2<float> playerPos = PLAYER_POS;
	honraiCentralPos = playerPos;
	centralPos = playerPos;
	this->playerPos = playerPos;
	prevPlayerPos = playerPos;

	pointPos[LEFT_UP] =
	{
		centralPos.x - windowHalfSize.x,
		centralPos.y - windowHalfSize.y
	};
	pointPos[LEFT_DOWN] =
	{
		centralPos.x - windowHalfSize.x,
		centralPos.y + windowHalfSize.y
	};
	pointPos[RIGHT_UP] =
	{
		centralPos.x + windowHalfSize.x,
		centralPos.y - windowHalfSize.y
	};
	pointPos[RIGHT_DOWN] =
	{
		centralPos.x + windowHalfSize.x,
		centralPos.y + windowHalfSize.y
	};
	//座標の設定-----------------------


	for (int i = 0; i < pointPos.size(); ++i)
	{
		initPointPos[i] = pointPos[i];
		hitLineFlag[i] = false;
		addLineValue[i] = 0.0f;
		honraiAddLineValue[i] = 0.0f;
		countHitLine[i] = 0;
		increLineVel[i] = 3.0f;
		enableAuraFlags[i] = false;
		noHitFlags[i] = false;
	}
	zoomFlag = false;
	zoomRate = 1.0f;
	zoomVel = 0.0f;



	decreVel = 0.1f;
}

void ViewPort::Update(const Vec2<float>& playerPos)
{
	if (zoomFlag)
	{
		int sum = 0;
		for (int i = 0; i < addLineValue.size(); ++i)
		{
			sum += addLineValue[i];
		}
		zoomRate = 1.0f - (sum / 1500.0f);
	}

	if (1.0f <= zoomRate)
	{
		zoomRate = 1.0f;
		zoomFlag = false;
	}

	zoomFlag = true;

	for (int i = 0; i < addLineValue.size(); ++i)
	{
		if (hitLineFlag[i])
		{
			//左辺を動かす
			honraiAddLineValue[i] += increLineVel[i];
			noHitFlags[i] = true;

			//移動量を減らす処理
			increLineVel[i] += -decreVel;
			if (increLineVel[i] <= 0.0f)
			{
				increLineVel[i] = 0.0f;
				countHitLine[i] = 0;
				++intervalTimer[i];
				noHitFlags[i] = false;
			}
			//一定時間後に
			//徐々に戻る処理
			if (30 <= intervalTimer[i] && !holdFlags[i])
			{
				honraiAddLineValue[i] -= 1.5f;
			}
		}

		//当たり判定は取っていないのにhonraiAddLineValueが0.1以上あったら0に近づける
		if (!hitLineFlag[i] && 0.1f <= honraiAddLineValue[i])
		{
			honraiAddLineValue[i] -= 1.5f;
		}

		//float num = honraiAddLineValue[i] - addLineValue[i];
		//毎フレーム代入する
		addLineValue[i] = honraiAddLineValue[i];
	}



#pragma region サイズを超えないようにする
	for (int i = 0; i < honraiAddLineValue.size(); ++i)
	{
		if (honraiAddLineValue[i] <= 0.0f)
		{
			honraiAddLineValue[i] = 0.0f;
			hitLineFlag[i] = false;
		}
	}
#pragma endregion


	//距離算出
	//Vec2<float> distance(pointPos[RIGHT_UP].x - pointPos[LEFT_UP].x, pointPos[RIGHT_DOWN].y - pointPos[RIGHT_UP].y);
	//Vec2<float> playerDistance(pointPos[RIGHT_UP].x - playerPos.x, pointPos[RIGHT_DOWN].y - playerPos.y);

	////割合算出
	//Vec2<float> rate = playerDistance / distance;

	//if (hitAuraFlags[RIGHT] && 0.4f <= rate.x)
	//{
	//	enableAuraFlags[RIGHT] = true;
	//	SavePrevFlamePos();
	//}
	//else if (0.5f <= rate.x)
	//{
	//	enableAuraFlags[RIGHT] = false;
	//}

	// 左側の引っ掛かり判定があったら、左側の当たり判定を行わないフラグを折る。
	//if (holdFlags[LEFT] && noHitFlags[LEFT]) noHitFlags[LEFT] = false;


	// 右側での当たり判定がある場合、中心座標より左側に行っていたらフラグを折る。
	if (isHitRight && playerPos.x <= centralPos.x) {

		isHitRight = false;

	}
	// 左側での当たり判定がある場合、中心座標より右側に行っていたらフラグを折る。
	if (!holdFlags[LEFT] && isHitLeft && playerPos.x >= centralPos.x) {

		isHitLeft = false;

	}
	// 左側で引っかかり判定がある場合、プレイヤーが中心座標より左側に言っていたらフラグを折る。
	if (holdFlags[LEFT] && playerPos.x <= centralPos.x) {

		isHitLeft = false;
		holdFlags[LEFT] = false;

	}
	// 下側での当たり判定がある場合、中心座標より上側に行っていたらフラグを折る。
	if (isHitBottom && playerPos.y <= centralPos.y) {

		isHitBottom = false;

	}
	// 上側での当たり判定がある場合、中心座標より下側に行っていたらフラグを折る。
	if (isHitTop && playerPos.y >= centralPos.y) {

		isHitTop = false;

	}


	//プレイヤーの補間
	Vec2<float> distance = honraiCentralPos - centralPos;
	centralPos = honraiCentralPos;


	//何処の辺と当たっているか-----------------------
	//float leftAngle = Input::GetJoyLeftStickAngle();
	float leftAngle = KuroFunc::GetAngle(UsersInput::Instance()->GetLeftStickVec());
	//float rightAngle = Input::GetJoyRightStickAngle();
	float rightAngle = KuroFunc::GetAngle(UsersInput::Instance()->GetRightStickVec());
	float add = 1.0f;
	Vec2<float> left(playerPos.x + cosf(leftAngle) * 10000.0f, playerPos.y + sinf(leftAngle) * 10000.0f);
	Vec2<float> right(playerPos.x + cosf(rightAngle) * 10000.0f, playerPos.y + sinf(rightAngle) * 10000.0f);
	if (RayHit(pointPos[LEFT_UP], pointPos[LEFT_DOWN], playerPos, left) ||
		RayHit(pointPos[LEFT_UP], pointPos[LEFT_DOWN], playerPos, right))
	{
		addVel[LEFT] = add;
	}
	if (RayHit(pointPos[RIGHT_UP], pointPos[RIGHT_DOWN], playerPos, left) ||
		RayHit(pointPos[RIGHT_UP], pointPos[RIGHT_DOWN], playerPos, right))
	{
		addVel[RIGHT] = add;
	}
	if (RayHit(pointPos[RIGHT_UP], pointPos[LEFT_UP], playerPos, left) ||
		RayHit(pointPos[RIGHT_UP], pointPos[LEFT_UP], playerPos, right))
	{
		addVel[UP] = add;
	}
	if (RayHit(pointPos[LEFT_DOWN], pointPos[RIGHT_DOWN], playerPos, left) ||
		RayHit(pointPos[LEFT_DOWN], pointPos[RIGHT_DOWN], playerPos, right))
	{
		addVel[DOWN] = add;
	}
	//何処の辺と当たっているか-----------------------


	//DrawFormatString(100, 150, GetColor(255, 255, 255), "X%f,Y:%f", rate.x, rate.y);

}

#include"DrawFunc.h"
void ViewPort::Draw()
{
	Vec2<float> scrollVel =
	{
		ScrollMgr::Instance()->scrollAmount.x + ShakeMgr::Instance()->shakeAmount.x,
		ScrollMgr::Instance()->scrollAmount.y + ShakeMgr::Instance()->shakeAmount.y
	};

	//int white = GetColor(255, 255, 255);
	Color white = Color(1.0f, 1.0f, 1.0f, 1.0f);


	Vec2<float> startPos, endPos;
	//上辺
	startPos =
	{
		(pointPos[LEFT_UP].x - 0.0f) - scrollVel.x,
		(pointPos[LEFT_UP].y - 0.0f) - scrollVel.y
	};
	endPos =
	{
		(pointPos[RIGHT_UP].x + 0.0f) - scrollVel.x,
		(pointPos[RIGHT_UP].y - 0.0f) - scrollVel.y
	};
	//DrawLine(startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom, endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom, white);
	DrawFunc::DrawLine2D({ startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom }, { endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom }, white);

	//下辺
	startPos =
	{
		(pointPos[LEFT_DOWN].x - 0.0f) - scrollVel.x,
		(pointPos[LEFT_DOWN].y + 0.0f) - scrollVel.y
	};
	endPos =
	{
		(pointPos[RIGHT_DOWN].x + 0.0f) - scrollVel.x,
		(pointPos[RIGHT_DOWN].y + 0.0f) - scrollVel.y
	};
	//DrawLine(startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom, endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom, white);
	DrawFunc::DrawLine2D({ startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom }, { endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom }, white);

	//左辺
	startPos =
	{
		(pointPos[LEFT_UP].x - 0.0f) - scrollVel.x,
		(pointPos[LEFT_UP].y - 0.0f) - scrollVel.y
	};
	endPos =
	{
		(pointPos[LEFT_DOWN].x - 0.0f) - scrollVel.x,
		(pointPos[LEFT_DOWN].y + 0.0f) - scrollVel.y
	};
	//DrawLine(startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom, endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom, white);
	DrawFunc::DrawLine2D({ startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom }, { endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom }, white);

	//前フレーム上辺
	startPos =
	{
		(pointData[LEFT_UP].prevFramePos.x - 0.0f) - scrollVel.x,
		(pointData[LEFT_UP].prevFramePos.y - 0.0f) - scrollVel.y
	};
	endPos =
	{
		(pointData[RIGHT_UP].prevFramePos.x - 0.0f) - scrollVel.x,
		(pointData[RIGHT_UP].prevFramePos.y + 0.0f) - scrollVel.y
	};
	//DrawLine(startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom, endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom, GetColor(255, 0, 0));
	DrawFunc::DrawLine2D({ startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom }, { endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom }, Color(255, 0, 0, 255));

	// 前フレームの左片が今フレームより右にあるか左にあるかをチェックする。
	//if (pointData[LEFT_UP].prevFramePos.y < pointData[LEFT_UP].nowFramePos.y) {
	//	DrawFormatString(200, 200, GetColor(255, 255, 255), "上");
	//}
	//else if (pointData[LEFT_UP].prevFramePos.y > pointData[LEFT_UP].nowFramePos.y) {
	//	DrawFormatString(200, 200, GetColor(255, 255, 255), "下");
	//}
	//else {
	//	DrawFormatString(200, 200, GetColor(255, 255, 255), "真ん中");
	//}


	//右辺
	startPos =
	{
		(pointPos[RIGHT_UP].x + 0.0f) - scrollVel.x,
		(pointPos[RIGHT_UP].y - 0.0f) - scrollVel.y
	};
	endPos =
	{
		(pointPos[RIGHT_DOWN].x + 0.0f) - scrollVel.x,
		(pointPos[RIGHT_DOWN].y + 0.0f) - scrollVel.y
	};
	//DrawLine(startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom, endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom, white);
	DrawFunc::DrawLine2D({ startPos.x * ScrollMgr::Instance()->zoom, startPos.y * ScrollMgr::Instance()->zoom }, { endPos.x * ScrollMgr::Instance()->zoom, endPos.y * ScrollMgr::Instance()->zoom }, white);


	//float leftAngle = Input::GetJoyLeftStickAngle();
	//float rightAngle = Input::GetJoyRightStickAngle();

	//Dir leftLook, rightLook;

	//DrawFormatString(100, 50, GetColor(255, 255, 255), "L:%f,R:%f", leftAngle * 180 / DX_PI_F, rightAngle * 180 / DX_PI_F);


	//DrawFormatString(100, 450, GetColor(255, 255, 255), "上 : %f", addLineValue[0]);
	//DrawFormatString(100, 500, GetColor(255, 255, 255), "下 : %f", addLineValue[1]);
	//DrawFormatString(100, 550, GetColor(255, 255, 255), "左 : %f", addLineValue[2]);
	//DrawFormatString(100, 600, GetColor(255, 255, 255), "右 : %f", addLineValue[3]);

}

void ViewPort::MoveLine(int LINE, float ZOOM_RATE)
{
	Dir dir;
	switch (LINE)
	{
	case 0:
		dir = UP;
		break;
	case 1:
		dir = RIGHT;
		break;
	case 2:
		dir = DOWN;
		break;
	case 3:
		dir = LEFT;
		break;
	default:
		break;
	}


	hitLineFlag[dir] = true;
	increLineVel[dir] = 1.0 + addVel[dir] + static_cast<float>(countHitLine[dir]) / 10.0f;
	++countHitLine[dir];

	intervalTimer[dir] = 0;

	zoomFlag = true;
	zoomVel = ZOOM_RATE;
}

void ViewPort::SetPlayerPosX(float PLAYER_POS)
{
	Vec2<float> localWindowSize = windowSize / Vec2<float>(2.0f, 2.0f);

	Vec2<float> start = ScrollMgr::Instance()->CaluStartScrollLine(localWindowSize);
	Vec2<float> end = ScrollMgr::Instance()->CaluEndScrollLine(localWindowSize);

	prevPlayerPos.x = playerPos.x;
	playerPos.x = PLAYER_POS;

	honraiCentralPos.x += playerPos.x - prevPlayerPos.x;
	if (playerPos.x <= start.x)
	{
		honraiCentralPos.x = start.x - 25.0f;
	}
	if (end.x <= playerPos.x)
	{
		honraiCentralPos.x = end.x + 25.0f;
	}
}

void ViewPort::SetPlayerPosY(float PLAYER_POS)
{
	Vec2<float> localWindowSize = windowSize / Vec2<float>(2.0f, 2.0f);
	Vec2<float> start = ScrollMgr::Instance()->CaluStartScrollLine(localWindowSize);
	Vec2<float> end = ScrollMgr::Instance()->CaluEndScrollLine(localWindowSize);

	prevPlayerPos.y = playerPos.y;
	playerPos.y = PLAYER_POS;

	honraiCentralPos.y += playerPos.y - prevPlayerPos.y;
	if (playerPos.y <= start.y)
	{
		honraiCentralPos.y = start.y - 25.0f;
	}
	if (end.y <= playerPos.y)
	{
		honraiCentralPos.y = end.y + 25.0f;
	}
}

void ViewPort::PushBackAura(const Vec2<float>& pushBackAmount)
{

	/*===== ビューポートの四点の押し戻しの処理 =====*/

	// 描画用の四点をずらす。
	ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_UP] += pushBackAmount;
	ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_UP] += pushBackAmount;
	ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_DOWN] += pushBackAmount;
	ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_DOWN] += pushBackAmount;

	// 当たり判定用の四点も押し戻す。
	ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_UP].nowFramePos += pushBackAmount;
	ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos += pushBackAmount;
	ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_DOWN].nowFramePos += pushBackAmount;
	ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos += pushBackAmount;

	//ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_UP].prevFramePos += pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_UP].prevFramePos += pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_DOWN].prevFramePos += pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_DOWN].prevFramePos += pushBackAmount;

	//centralPos -= pushBackAmount;

}

void ViewPort::PushBackAuraHoldUp(const float& pushBackAmount)
{

	/*===== 引っかかり時の上側の押し戻し処理 =====*/

	// 上側の辺を押し戻す。
	pointPos[ViewPort::Instance()->RIGHT_UP].y += pushBackAmount;
	pointPos[ViewPort::Instance()->LEFT_UP].y += pushBackAmount;
	pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos.y += pushBackAmount;
	pointData[ViewPort::Instance()->LEFT_UP].nowFramePos.y += pushBackAmount;

	// 上側の辺の長さを変える。
	honraiAddLineValue[UP] += fabs(pushBackAmount);
	addLineValue[UP] += fabs(pushBackAmount);

}

void ViewPort::PushBackAuraHoldDown(const float& pushBackAmount)
{

	/*===== 引っかかり時の上側の押し戻し処理 =====*/

	// 下側の辺を押し戻す。
	pointPos[ViewPort::Instance()->RIGHT_DOWN].y += pushBackAmount;
	pointPos[ViewPort::Instance()->LEFT_DOWN].y += pushBackAmount;
	pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos.y += pushBackAmount;
	pointData[ViewPort::Instance()->LEFT_DOWN].nowFramePos.y += pushBackAmount;

	// 下側の辺の長さを変える。
	honraiAddLineValue[DOWN] += fabs(pushBackAmount);
	addLineValue[DOWN] += fabs(pushBackAmount);

}

void ViewPort::PushBackAuraHoldRight(const float& pushBackAmount)
{

	/*===== 引っかかり時の右側の押し戻し処理 =====*/

	// 右側の辺を押し戻す。
	pointPos[ViewPort::Instance()->RIGHT_UP].x += pushBackAmount;
	pointPos[ViewPort::Instance()->RIGHT_DOWN].x += pushBackAmount;
	pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos.x += pushBackAmount;
	pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos.x += pushBackAmount;

	// 右側の辺の長さを変える。
	honraiAddLineValue[RIGHT] += pushBackAmount;
	addLineValue[RIGHT] += pushBackAmount;

}

void ViewPort::PushBackAuraHoldLeft(const float& pushBackAmount)
{

	/*===== 引っかかり時の左側の押し戻し処理 =====*/

	// 中心座標を動かす。
	//centralPos.x -= pushBackAmount;

	// 右側の辺を動かす。
	//pointPos[ViewPort::Instance()->RIGHT_UP].x -= pushBackAmount;
	//pointPos[ViewPort::Instance()->RIGHT_DOWN].x -= pushBackAmount;
	//pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos.x -= pushBackAmount;
	//pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos.x -= pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_UP].prevFramePos.x -= pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_DOWN].prevFramePos.x -= pushBackAmount;

	// 左側の辺を押し戻す。
	pointPos[ViewPort::Instance()->LEFT_UP].x += pushBackAmount;
	pointPos[ViewPort::Instance()->LEFT_DOWN].x += pushBackAmount;
	pointData[ViewPort::Instance()->LEFT_UP].nowFramePos.x += pushBackAmount;
	pointData[ViewPort::Instance()->LEFT_DOWN].nowFramePos.x += pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_UP].prevFramePos.x += pushBackAmount;
	//ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_DOWN].prevFramePos.x += pushBackAmount;

	// 左側の辺の長さを変える。
	honraiAddLineValue[LEFT] -= pushBackAmount;
	addLineValue[LEFT] -= pushBackAmount;

}

void ViewPort::PushBackLine(int LINE, const float& pushBackAmount)
{
	switch (LINE)
	{
	case ViewPort::UP:
		// 描画用の四点をずらす。
		ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_UP].y += pushBackAmount;
		ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_UP].y += pushBackAmount;
		// 当たり判定用の四点も押し戻す。
		ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_UP].nowFramePos.y += pushBackAmount;
		ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos.y += pushBackAmount;
		break;
	case ViewPort::DOWN:
		// 描画用の四点をずらす。
		ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_DOWN].y += pushBackAmount;
		ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_DOWN].y += pushBackAmount;

		// 当たり判定用の四点も押し戻す。
		ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_DOWN].nowFramePos.y += pushBackAmount;
		ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos.y += pushBackAmount;
		break;
	case ViewPort::LEFT:
		// 描画用の四点をずらす。
		ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_UP].x += pushBackAmount;
		ViewPort::Instance()->pointPos[ViewPort::Instance()->LEFT_DOWN].x += pushBackAmount;

		// 当たり判定用の四点も押し戻す。
		ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_UP].nowFramePos.x += pushBackAmount;
		ViewPort::Instance()->pointData[ViewPort::Instance()->LEFT_DOWN].nowFramePos.x += pushBackAmount;
		break;
	case ViewPort::RIGHT:
		// 描画用の四点をずらす。
		ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_UP].x += pushBackAmount;
		ViewPort::Instance()->pointPos[ViewPort::Instance()->RIGHT_DOWN].x += pushBackAmount;

		// 当たり判定用の四点も押し戻す。
		ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_UP].nowFramePos.x += pushBackAmount;
		ViewPort::Instance()->pointData[ViewPort::Instance()->RIGHT_DOWN].nowFramePos.x += pushBackAmount;
		break;
	default:
		break;
	}

}

void ViewPort::AddAddLineValue(const float& value, const int& ID)
{
	addLineValue[ID] += value;

}

void ViewPort::SavePrevFlamePos()
{
	// このフレームの座標を保存
	pointData[LEFT_UP].prevFramePos = pointPos[LEFT_UP];
	pointData[RIGHT_UP].prevFramePos = pointPos[RIGHT_UP];
	pointData[LEFT_DOWN].prevFramePos = pointPos[LEFT_DOWN];
	pointData[RIGHT_DOWN].prevFramePos = pointPos[RIGHT_DOWN];

	// 規定のウィンドウサイズの半分
	Vec2<float> tmp = windowSize / Vec2<float>(2.0f, 2.0f);

	// 中心座標を中心として、規定のウィンドウサイズ分移動させた位置に座標をセットする。
	pointPos[LEFT_UP] =
	{
		centralPos.x - tmp.x,
		centralPos.y - tmp.y
	};
	pointPos[LEFT_DOWN] =
	{
		centralPos.x - tmp.x,
		centralPos.y + tmp.y
	};
	pointPos[RIGHT_UP] =
	{
		centralPos.x + tmp.x,
		centralPos.y - tmp.y
	};
	pointPos[RIGHT_DOWN] =
	{
		centralPos.x + tmp.x,
		centralPos.y + tmp.y
	};


	// 規定のウィンドウサイズに、加算された分のウィンドウサイズを足す。左側。
	pointPos[LEFT_UP].x += -addLineValue[LEFT];
	pointPos[LEFT_DOWN].x += -addLineValue[LEFT];

	if (fabs(addLineValue[LEFT] < 0.1f)) holdFlags[LEFT] = false;

	// 規定のウィンドウサイズに、加算された分のウィンドウサイズを足す。右側。
	pointPos[RIGHT_UP].x += addLineValue[RIGHT];
	pointPos[RIGHT_DOWN].x += addLineValue[RIGHT];

	if (fabs(addLineValue[RIGHT] < 0.1f)) holdFlags[RIGHT] = false;

	// 規定のウィンドウサイズに、加算された分のウィンドウサイズを足す。上側。
	pointPos[LEFT_UP].y += -addLineValue[UP];
	pointPos[RIGHT_UP].y += -addLineValue[UP];

	// 規定のウィンドウサイズに、加算された分のウィンドウサイズを足す。下側。
	pointPos[LEFT_DOWN].y += addLineValue[DOWN];
	pointPos[RIGHT_DOWN].y += addLineValue[DOWN];


	// 現在フレームの座標を保存
	pointData[LEFT_UP].nowFramePos = pointPos[LEFT_UP];
	pointData[RIGHT_UP].nowFramePos = pointPos[RIGHT_UP];
	pointData[LEFT_DOWN].nowFramePos = pointPos[LEFT_DOWN];
	pointData[RIGHT_DOWN].nowFramePos = pointPos[RIGHT_DOWN];
}
