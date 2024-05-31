#define CLAMP(x, minVal, maxVal) ((x) < (minVal) ? (minVal) : ((x) > (maxVal) ? (maxVal) : (x)))

#define LERP(a, b, time) (a * (1.0f - time)) + (b * time)

//template<typename T>
//static T clamp(T value, T min, T max) 
//{
//	if (value > max)
//	{
//		value = max;
//	}
//	else if (value < min)
//	{
//		value = min;
//	}
//
//	return value;
//}