#pragma once

#define CLAMP(x, minVal, maxVal) ((x) < (minVal) ? (minVal) : ((x) > (maxVal) ? (maxVal) : (x)))

#define LERP(a, b, time) (a * (1.0f - time)) + (b * time)

class DMMath
{
public:
    inline static float smoothstep(float _edge0, float _edge1, float _x)
    {
        _x = CLAMP((_x - _edge0) / (_edge1 - _edge0), 0.0f, 1.0f);
        return _x * _x * (3.0f - 2.0f * _x);
    }
};