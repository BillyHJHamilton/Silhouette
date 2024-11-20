#pragma once

#if _DEBUG
	#define UNIT_TESTS true
	#define DEBUG_NAME_HASHING true
	#define DEBUG_MEMORY true
	#define PERF_TEST true
#else
	#define UNIT_TESTS true // For now, I'd like to test release, too.
	#define DEBUG_NAME_HASHING false
	#define DEBUG_MEMORY false
	#define PERF_TEST true
#endif

// Include most essential library headers.
// This probably hurts compile times, but it's just so convenient.
#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "BasicTypes.h"
