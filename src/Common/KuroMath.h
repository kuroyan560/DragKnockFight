#pragma once
#include<functional>
#include"Vec.h"
#include<vector>

#include<DirectXMath.h>
using namespace DirectX;
using Matrix = XMMATRIX;

bool operator!=(const Matrix& lhs, const Matrix& rhs);
bool operator==(const Matrix& lhs, const Matrix& rhs);

#include"Angle.h"

static const enum EASE_CHANGE_TYPE
{
    In, Out, InOut, EASE_CHANGE_TYPE_NUM
};
static const enum EASING_TYPE
{
    Quad, Cubic, Quart, Quint, Sine, Exp, Circ, Elastic, Back, Bounce, EASING_TYPE_NUM
};

static class KuroMath
{
private:
    static float QuadIn(float t, float totaltime, float min, float max);
    static float QuadOut(float t, float totaltime, float min, float max);
    static float QuadInOut(float t, float totaltime, float min, float max);

    static float CubicIn(float t, float totaltime, float min, float max);
    static float CubicOut(float t, float totaltime, float min, float max);
    static float CubicInOut(float t, float totaltime, float min, float max);

    static float QuartIn(float t, float totaltime, float min, float max);
    static float QuartOut(float t, float totaltime, float min, float max);
    static float QuartInOut(float t, float totaltime, float min, float max);

    static float QuintIn(float t, float totaltime, float min, float max);
    static float QuintOut(float t, float totaltime, float min, float max);
    static float QuintInOut(float t, float totaltime, float min, float max);

    static float SineIn(float t, float totaltime, float min, float max);
    static float SineOut(float t, float totaltime, float min, float max);
    static float SineInOut(float t, float totaltime, float min, float max);

    static float ExpIn(float t, float totaltime, float min, float max);
    static float ExpOut(float t, float totaltime, float min, float max);
    static float ExpInOut(float t, float totaltime, float min, float max);

    static float CircIn(float t, float totaltime, float min, float max);
    static float CircOut(float t, float totaltime, float min, float max);
    static float CircInOut(float t, float totaltime, float min, float max);

    static float ElasticIn(float t, float totaltime, float min, float max);
    static float ElasticOut(float t, float totaltime, float min, float max);
    static float ElasticInOut(float t, float totaltime, float min, float max);

    static float BackIn(float t, float totaltime, float min, float max);
    static float BackOut(float t, float totaltime, float min, float max);
    static float BackInOut(float t, float totaltime, float min, float max);

    static float BounceIn(float t, float totaltime, float min, float max);
    static float BounceOut(float t, float totaltime, float min, float max);
    static float BounceInOut(float t, float totaltime, float min, float max);

    //ラムダ式のための型
    using EasingFunction = std::function<float(float t, float totaltime, float min, float max)>;
    static EasingFunction easing[EASE_CHANGE_TYPE_NUM][EASING_TYPE_NUM];

    static float GetSplineSection(const float& p0, const float& p1, const float& p2, const float& p3, const float& t);
public:
    static float Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, float Min, float Max);
    static Vec2<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec2<float> Min, Vec2<float> Max);
    static Vec3<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec3<float> Min, Vec3<float> Max);
    static Vec4<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec4<float> Min, Vec4<float> Max);
    
    static float Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, float Min, float Max);
    static Vec2<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec2<float> Min, Vec2<float> Max);
    static Vec3<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec3<float> Min, Vec3<float> Max);
    static Vec4<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec4<float> Min, Vec4<float> Max);
    
    static float Lerp(float Min, float Max, float Rate);
    static Vec2<float> Lerp(Vec2<float> Min, Vec2<float> Max, float Rate);
    static Vec3<float> Lerp(Vec3<float> Min, Vec3<float> Max, float Rate);
    static Vec4<float> Lerp(Vec4<float> Min, Vec4<float> Max, float Rate);

    static float Lerp(float Min, float Max, float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Lerp(Min, Max, T / TotalTime);
    }
    static Vec2<float> Lerp(Vec2<float> Min, Vec2<float> Max, float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Lerp(Min, Max, T / TotalTime);
    }
    static Vec3<float> Lerp(Vec3<float> Min, Vec3<float> Max, float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Lerp(Min, Max, T / TotalTime);
    }
    static Vec4<float> Lerp(Vec4<float> Min, Vec4<float> Max, float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Lerp(Min, Max, T / TotalTime);
    }

    static float Liner(float Min, float Max, float Rate);
    static Vec2<float> Liner(Vec2<float> Min, Vec2<float> Max, float Rate);
    static Vec3<float> Liner(Vec3<float> Min, Vec3<float> Max, float Rate);
    static Vec4<float> Liner(Vec4<float> Min, Vec4<float> Max, float Rate);

    static float Liner(float Min, float Max, float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Liner(Min, Max, T / TotalTime);
    }
    static Vec2<float> Liner(Vec2<float> Min, Vec2<float> Max,  float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Liner(Min, Max, T / TotalTime);
    }
    static Vec3<float> Liner(Vec3<float> Min, Vec3<float> Max,  float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Liner(Min, Max, T / TotalTime);
    }
    static Vec4<float> Liner(Vec4<float> Min, Vec4<float> Max,  float T, float TotalTime)
    {
        if (T < 0)T = 0;
        if (TotalTime < T)T = TotalTime;
        return Liner(Min, Max, T / TotalTime);
    }

    static float GetSpline(const float& T, const int& P1Idx, const std::vector<float>& Array, bool Loop = false);
    static float GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<float>& Array, bool Loop = false);
    static Vec2<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop = false);
    static Vec2<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop = false);
    static Vec3<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop = false);
    static Vec3<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop = false);
    static Vec4<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop = false);
    static Vec4<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop = false);

    //x,y,zの角度から回転行列を生成
    static Matrix RotateMat(const Vec3<Angle>& Rotate);
    //軸と角度を指定して回転行列を生成
    static Matrix RotateMat(const Vec3<float>& Axis, const float& Radian);
    //aをbに向かせる回転行列を求める
    static Matrix RotateMat(const Vec3<float>& VecA, const Vec3<float>& VecB);

    //2Dベクトルを回転させる
    static Vec2<float>RotateVec2(const Vec2<float>& Vec, const float& Radian);
};