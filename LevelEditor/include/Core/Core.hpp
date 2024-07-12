#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <optional>
#include <algorithm>

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
