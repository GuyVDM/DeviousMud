#include <cstdint>

#include <string>

#include <memory>

using I8 = char;

using I16 = short;

using I32 = int;

using U16 = short;

using U32 = int;

using U8 = unsigned char;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;
