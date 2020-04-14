#pragma once

#include "stdint.h"
#include <numeric>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// Floating point values are not garunteed alignment
using f32  = float;
using f64 = double;

using uptr = uintptr_t;
using iptr = intptr_t;

using b8 = i8;
using b16 = i16;
using b32 = i32;

#define U8_MIN UINT8_MIN
#define U8_MAX UINT8_MAX
#define I8_MIN INT8_MIN
#define I8_MAX INT8_MAX

#define U16_MIN UINT16_MIN
#define U16_MAX UINT16_MAX
#define I16_MIN INT16_MIN
#define I16_MAX INT16_MAX

#define U32_MIN UINT32_MIN
#define U32_MAX UINT32_MAX
#define I32_MIN INT32_MIN
#define I32_MAX INT32_MAX

#define U64_MIN UINT64_MIN
#define U64_MAX UINT64_MAX
#define I64_MIN INT64_MIN
#define I64_MAX INT64_MAX

#define F32_MIN std::numeric_limits<float>::min()
#define F32_MAX std::numeric_limits<float>::max()

#define F64_MIN std::numeric_limits<double>::min()
#define F64_MAX std::numeric_limits<double>::max()

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
