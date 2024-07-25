#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <optional>
#include <algorithm>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/binary.hpp>

#include "Shared/Utilities/vec2.hpp"

using I8  = char;

using I16 = short;

using I32 = int;

using I64 = long long;

using U8  = unsigned char;

using U16 = unsigned short;

using U32 = unsigned int;

using U64 = unsigned long long;

template<typename T>
using Optional = std::optional<T>;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;

namespace cereal
{
	template<class Archive>
	void serialize(Archive& _ar, Utilities::ivec2& _ivec)
	{
		_ar(_ivec.x, _ivec.y);
	}
}

inline bool U32IsBitSet(const U32& _value, const U32 _bitPosition)
{
    const U32 mask = 1U << _bitPosition;
    return (_value & mask) != 0;
}

inline void U32FlipBit(U32& _value, const U32 _bitPosition) 
{
    const U32 mask = 1U << _bitPosition;
    _value ^= mask;
}
