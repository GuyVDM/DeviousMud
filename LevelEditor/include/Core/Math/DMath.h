namespace DMath
{
    template<typename T>
    static const T Occilate(float _from, float _to, float _frequency, float _time)
    {
        const float range = (_to - _from);
        const float alpha = (_from + range * 0.5f * (1.0f + std::sinf(_frequency * _time)));

        return static_cast<T>(alpha);
    }
}