#pragma once
#include <assert.h>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>
#include <random>
// Asserts
#define ASSERT(x) assert((x))
#define CHECK(x) assert((x) != nullptr); (x)

// Helpers
#define NO_DISCARD [[nodiscard]]
#define NO_RETURN [[noreturn]]
#define FALLTHROUGH [[fallthrough]]
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]


// typedef
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using usize = std::size_t;

#define endl '\n'

template<class T>
using vec = std::vector<T>;

template<class T>
using fn = std::function<T>;


// macros
#define EXPAND(x) x
#define CAT(a, b) a##b
#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) __VA_ARGS__

#define REPEAT(n, macro) EVAL(REPEAT_INDIRECT()(n, macro))
#define REPEAT_INDIRECT() REPEAT_
#define REPEAT_(n, macro) \
IF(n)(OBSTRUCT(REPEAT_INDIRECT)()(DEC(n), macro) OBSTRUCT(macro)(n), )

// --- integer helpers ---
#define IF(c) CAT(IF_, c)
#define IF_0(t, f)
#define IF_1(t, f) t
#define DEC(x) CAT(DEC_, x)
#define DEC_0 0
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DEC_10 9
#define DEC_11 10
#define DEC_12 11
#define DEC_13 12
#define DEC_14 13
#define DEC_15 14
#define DEC_16 15
#define DEC_17 16
#define DEC_18 17
#define DEC_19 18
#define DEC_20 19
#define DEC_21 20
#define DEC_22 21
#define DEC_23 22
#define DEC_24 23
#define DEC_25 24
#define DEC_26 25
#define DEC_27 26
#define DEC_28 27
#define DEC_29 28
#define DEC_30 29
#define DEC_31 30
#define DEC_32 31

inline f32 RandomValue(f32 min, f32 max) {
    if (min > max) std::swap(min, max);
    static thread_local std::mt19937 rng{ std::random_device{}() };

    std::uniform_real_distribution<f32> dist(min, max);
    return dist(rng);
}