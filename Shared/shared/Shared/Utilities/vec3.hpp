#pragma once
#include <iostream> 
#include <stdexcept> 
#include <cmath>

namespace Utilities
{

#pragma region SIGNED_32BIT_INT_VEC2
    typedef union _ivec3
    {
        struct
        {
            int32_t x, y, z;
        };  int32_t contents[2];


        inline _ivec3(const _ivec3& other) : x(other.x), y(other.y), z(other.z)
        {
        }

        inline _ivec3()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        inline _ivec3(const int32_t& _x, const int32_t& _y, const int32_t& _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        inline _ivec3(const int32_t& v)
        {
            x = v;
            y = v;
            z = v;
        }

        // Copy assignment operator
        inline _ivec3& operator=(const _ivec3& _other)
        {
            x = _other.x;
            y = _other.y;
            z = _other.z;
            return *this;
        }

        inline _ivec3 operator+(const _ivec3& _other)
        {
            x += _other.x;
            y += _other.y;
            z += _other.z;
            return *this;
        }

        inline _ivec3 operator-(const _ivec3& _other)
        {
            x -= _other.x;
            y -= _other.y;
            z -= _other.z;
            return *this;
        }

        inline _ivec3 operator*(const _ivec3& _other)
        {
            x *= _other.x;
            y *= _other.y;
            z *= _other.z;
            return *this;
        }

        inline _ivec3 operator/(const _ivec3& _other) //TODO: Possibly broken, needs unit test
        {
            try
            {
                x /= _other.x;
                y /= _other.y;
                z /= _other.z;
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


            return *this;
        }

        inline bool operator==(const _ivec3& a) const
        {
            return (x == a.x) && (y == a.y) && (z == a.z);
        }

        inline bool operator!=(const _ivec3& _other) const
        {
            return !(*this == _other);
        }

        inline bool operator<(const _ivec3& _other) const
        {
            return (x < _other.x) || (x == _other.x && y < _other.y);
        }

    } ivec3;

#pragma endregion

#pragma region FLOATPOINT_VEC2
    typedef union _vec3
    {
        struct
        {
            float x, y, z;
        };  float contents[3];

        inline _vec3()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        inline _vec3(const float& _x, const float& _y, const float& _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        inline _vec3(const float& v)
        {
            x = v;
            y = v;
            z = v;
        }

        inline _vec3 operator=(const _vec3& a)
        {
            x = a.x;
            y = a.y;
            z = a.z;
            return *this;
        }

        inline _vec3 operator+(const _vec3& a)
        {
            x += a.x;
            y += a.y;
            z += a.z;
            return *this;
        }

        inline _vec3 operator-(const _vec3& a)
        {
            x -= a.x;
            y -= a.y;
            z -= a.z;
            return *this;
        }

        inline _vec3 operator*(const _vec3& a)
        {
            x *= a.x;
            y *= a.y;
            z *= a.z;
            return *this;
        }

        inline _vec3 operator/(const _vec3& a) //TODO: Possibly broken, needs unit testing.
        {
            try
            {
                x /= a.x;
                y /= a.y;
                z /= a.z;
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


            return *this;
        }

        inline bool operator==(const _vec3& a) const
        {
            return (x == a.x) && (y == a.y) && (z == a.z);
        }

        inline bool operator!=(const _vec3& a) const
        {
            return !(*this == a);
        }

    } vec3;
}
#pragma endregion

/// <summary>
/// Hashing of the vectors.
/// </summary>
namespace std
{
    template <>
    struct hash<Utilities::_ivec3>
    {
        std::size_t operator()(const Utilities::_ivec3& s) const
        {
            // Combine hash values of x and y using XOR (^)
            return hash<int32_t>()(s.x) ^ (hash<int32_t>()(s.y) << 1);
        }
    };
}