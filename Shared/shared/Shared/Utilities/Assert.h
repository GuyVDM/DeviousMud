#pragma once

#ifdef _DM_DEBUG
#include <cassert>
#define DEVIOUS_ASSERT(cond) assert(cond);
#else
#define DEVIOUS_ASSERT(cond)
#endif