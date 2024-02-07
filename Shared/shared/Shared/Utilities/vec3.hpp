#pragma once
#include <iostream> 
#include <stdexcept> 

namespace Utilities
{
    typedef union _ivec3
    {
        struct
        {
            int32_t x, y, z;
        };  int32_t contents[3];

        inline _ivec3() 
        {
            x = 0;
            y = 0;
            z = 0;
        }

        inline _ivec3(int32_t _x, int32_t _y, int32_t _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        inline _ivec3(int32_t v)
        {
            x = v;
            y = v;
            z = v;
        }

        inline _ivec3 operator=(_ivec3 a)
        {
            x = a.x;
            y = a.y;
            z = a.z;
            return *this;
        }

        inline _ivec3 operator+(_ivec3 a)
        {
            x += a.x;
            y += a.y;
            z += a.z;
            return *this;
        }

        inline _ivec3 operator-(_ivec3 a)
        {
            x -= a.x;
            y -= a.y;
            z -= a.z;
            return *this;
        }

        inline _ivec3 operator*(_ivec3 a)
        {
            x *= a.x;
            y *= a.y;
            z *= a.z;
            return *this;
        }

        inline _ivec3 operator/(_ivec3 a)
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

        inline bool operator==(_ivec3 a)
        {
            if (x == a.x && y == a.y && z == a.z)
                return true;
            else
                return false;
        }

        inline bool operator!=(_ivec3 a)
        {
            if (x != a.x || y != a.y || x != a.z)
                return true;
            else
                return false;
        }

    } ivec3;
}