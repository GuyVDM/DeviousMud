#pragma once
#include <iostream> 
#include <stdexcept> 
#include <cmath>

namespace Utilities
{

#pragma region SIGNED_32BIT_INT_VEC2
    typedef union _ivec2
    {
        struct 
        {
            int32_t x;
            int32_t y;
        };  int32_t contents[2];


        inline _ivec2(const _ivec2& other) : x(other.x), y(other.y) 
        {
        }

        inline _ivec2() 
        {
            x = 0;
            y = 0;
        }

        inline _ivec2(const int32_t& _x, const int32_t& _y) 
        {
            x = _x;
            y = _y;
        }

        inline _ivec2(const int32_t& v) 
        {
            x = v;
            y = v;
        }

        // Copy assignment operator
        inline _ivec2& operator=(const _ivec2& _other)
        {
            x = _other.x;
            y = _other.y;
            return *this;
        }

        friend inline _ivec2 operator+(const _ivec2& _lhs, const _ivec2& _rhs)
        {
            return _ivec2(_lhs.x + _rhs.x,_lhs.y + _rhs.y);
        }

        inline _ivec2 operator+=(const _ivec2& _other) const
        {
            return _ivec2(x + _other.x, y + _other.y);
        }

        inline _ivec2 operator-(const _ivec2& _other) const
        {
            return _ivec2(x - _other.x, y - _other.y);
        }

        inline _ivec2 operator*(const _ivec2& _other) const
        {
            return _ivec2(x * _other.x, y * _other.y);
        }

        inline _ivec2 operator*(const float& _scale) const
        {
            float newX = static_cast<float>(x) * _scale;
            float newY = static_cast<float>(y) * _scale;

            return _ivec2
            (
                static_cast<int32_t>(newX),
                static_cast<int32_t>(newY)
            );
        }

        inline _ivec2 operator*(const int32_t& _scale) const
        {
            return _ivec2
            (
                x * _scale,
                y * _scale
            );
        }

        inline _ivec2 operator/(const int32_t& divisor) const 
        {
            if (divisor == 0)
            {
                throw std::runtime_error("Division by zero");
            }

            return { x / divisor, y / divisor };
        }

        inline bool operator==(const _ivec2& a) const
        {
            return (x == a.x) && (y == a.y);
        }

        inline bool operator!=(const _ivec2& _other) const
        {
            return !(*this == _other);
        }

        inline bool operator<(const _ivec2& _other) const
        {
            return (x < _other.x) || (x == _other.x && y < _other.y);
        }

        inline static int32_t get_distance(const _ivec2& _a, const _ivec2& _b)
        {
            const Utilities::ivec2 delta = _b - _a;
            return std::max<int32_t>(std::abs(delta.x), std::abs(delta.y));
        }

    } ivec2;

#pragma endregion

#pragma region FLOATPOINT_VEC2
    typedef union _vec2
    {
        struct
        {
            float x;
            float y;
        };  float contents[2];

        inline _vec2() 
        {
            x = 0;
            y = 0;
        }

        inline _vec2(const float& _x, const float& _y)
        {
            x = _x;
            y = _y;
        }

        inline _vec2(const float& v)
        {
            x = v;
            y = v;
        }

        inline _vec2& operator=(const _vec2& _a)
        {
            if (this != &_a)
            {
                x = _a.x;
                y = _a.y;
            }

            return *this;
        }

        friend inline _vec2 operator+(const _vec2& _lhs, const _vec2& _rhs) //Possibly broken???
        {
            return _vec2(_lhs.x + _rhs.x, _lhs.y + _rhs.y);
        }

        inline _vec2 operator+=(const _vec2& _other) const
        {
            return _vec2(x + _other.x, y + _other.y);
        }

        inline _vec2 operator-(const _vec2& _other) const
        {
            return _vec2(x - _other.x, y - _other.y);
        }

        inline _vec2 operator*(const _vec2& _other) const
        {
            return _vec2(x * _other.x, y * _other.y);
        }

        inline _vec2 operator/(const _vec2& a) const //TODO: Possibly broken, needs unit testing.
        {
            try
            {
                return Utilities::vec2(x / a.x, y / a.y);
            }
            // catch block catches exception if any 
            // of type Exception 
            catch (std::invalid_argument& e)
            {
                // prints that exception has occurred 
                // calls the what function using object of 
                // the user defined class called Exception 
                std::cout << "Exception occurred" << std::endl << e.what();
            }

            return Utilities::vec2(0.0f);
        }

        inline bool operator==(const _vec2& a) const
        {
            return (x == a.x) && (y == a.y);
        }

        inline bool operator!=(const _vec2& a) const
        {
            return !(*this == a);
        }

        inline static _vec2 lerp(const _vec2& a, const _vec2& b, const float& timeline) 
        {
            return _vec2
            {
                a.x + (b.x - a.x) * timeline,
                a.y + (b.y - a.y) * timeline 
            };
        }

        inline const float get_distance(const _vec2& _a, const _vec2& _b) const
        {
            const Utilities::_vec2 delta = _b - _a;
            return std::max<float>(std::abs(delta.x), std::abs(delta.y));
        }

    } vec2;

    inline const vec2 to_vec2(const Utilities::_ivec2& _ivec)
    {
        return Utilities::vec2
        {
            (float)_ivec.x,
            (float)_ivec.y
        };
    }

    inline static ivec2 to_ivec2(const Utilities::vec2 _vec)
    {
        return Utilities::ivec2
        {
            (int32_t)roundf(_vec.x),
            (int32_t)roundf(_vec.y)
        };
    }
}


#pragma endregion

/// <summary>
/// Hashing of the vectors.
/// </summary>
namespace std
{
    template <>
    struct hash<Utilities::_ivec2> 
    {
        std::size_t operator()(const Utilities::_ivec2& s) const 
        {
            // Combine hash values of x and y using XOR (^)
            return hash<int32_t>()(s.x) ^ (hash<int32_t>()(s.y) << 1);
        }
    };
}