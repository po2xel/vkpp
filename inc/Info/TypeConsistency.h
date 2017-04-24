#ifndef __VKPP_INFO_TYPE_CONSISTENCY_H__
#define __VKPP_INFO_TYPE_CONSISTENCY_H__



#include <cstddef>



#define VA_ARGS_EXPAND(X) X
#define CONCATE_STATIC_OFFSET_CHECK_NAME(NAME, N) NAME##N

#define OFFSET_ERROR_MSG(S, a) #S " -> " #a": Struct Offset doesn't match."

#define VA_ARGS_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, N, ...) N
#define VA_ARGS_COUNT(...) VA_ARGS_EXPAND(VA_ARGS_COUNT_IMPL(__VA_ARGS__, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

#define STATIC_OFFSET_CHECK_1(S, a) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a))

#define STATIC_OFFSET_CHECK_2(S, a, b) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_1(S, b)

#define STATIC_OFFSET_CHECK_3(S, a, b, c) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_2(S, b, c)

#define STATIC_OFFSET_CHECK_4(S, a, b, c, d) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_3(S, b, c, d)

#define STATIC_OFFSET_CHECK_5(S, a, b, c, d, e) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_4(S, b, c, d, e)

#define STATIC_OFFSET_CHECK_6(S, a, b, c, d, e, f) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_5(S, b, c, d, e, f)

#define STATIC_OFFSET_CHECK_7(S, a, b, c, d, e, f, g) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_6(S, b, c, d, e, f, g)

#define STATIC_OFFSET_CHECK_8(S, a, b, c, d, e, f, g, h) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_7(S, b, c, d, e, f, g, h)

#define STATIC_OFFSET_CHECK_9(S, a, b, c, d, e, f, g, h, i) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_8(S, b, c, d, e, f, g, h, i)

#define STATIC_OFFSET_CHECK_10(S, a, b, c, d, e, f, g, h, i, j) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_9(S, b, c, d, e, f, g, h, i, j)

#define STATIC_OFFSET_CHECK_11(S, a, b, c, d, e, f, g, h, i, j, k) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_10(S, b, c, d, e, f, g, h, i, j, k)

#define STATIC_OFFSET_CHECK_12(S, a, b, c, d, e, f, g, h, i, j, k, l) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_11(S, b, c, d, e, f, g, h, i, j, k, l)

#define STATIC_OFFSET_CHECK_13(S, a, b, c, d, e, f, g, h, i, j, k, l, m) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_12(S, b, c, d, e, f, g, h, i, j, k, l, m)

#define STATIC_OFFSET_CHECK_14(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_13(S, b, c, d, e, f, g, h, i, j, k, l, m, n)

#define STATIC_OFFSET_CHECK_15(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_14(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o)

#define STATIC_OFFSET_CHECK_16(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_15(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)

#define STATIC_OFFSET_CHECK_17(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_16(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)

#define STATIC_OFFSET_CHECK_18(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_17(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)

#define STATIC_OFFSET_CHECK_19(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_18(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s)

#define STATIC_OFFSET_CHECK_20(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_19(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)

#define STATIC_OFFSET_CHECK_21(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_20(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u)

#define STATIC_OFFSET_CHECK_22(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_21(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v)

#define STATIC_OFFSET_CHECK_23(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_23(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w)

#define STATIC_OFFSET_CHECK_24(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_24(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x)

#define STATIC_OFFSET_CHECK_25(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_24(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y)

#define STATIC_OFFSET_CHECK_26(S, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_25(S, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z)



#define STATIC_OFFSET_CHECK_IMPL(N, S, ...) VA_ARGS_EXPAND(CONCATE_STATIC_OFFSET_CHECK_NAME(STATIC_OFFSET_CHECK_, N)(S, __VA_ARGS__))

#define STATIC_OFFSET_CHECK_N(N, S, a, ...) \
    static_assert(offsetof(S, a) == offsetof(S::VkType, a), OFFSET_ERROR_MSG(S, a)); \
    STATIC_OFFSET_CHECK_IMPL(VA_ARGS_COUNT(__VA_ARGS__), S, __VA_ARGS__)

#define StaticOffsetCheck(S, ...) STATIC_OFFSET_CHECK_IMPL(VA_ARGS_COUNT(__VA_ARGS__), S, __VA_ARGS__)



#define SIZE_ERROR_MSG(Class) #Class ": Struct size doesn't match."
#define StaticSizeCheck(Class) static_assert(sizeof(Class) == sizeof(Class::VkType), SIZE_ERROR_MSG(Class));



#define ConsistencyCheck(Class, ...) \
    StaticSizeCheck(Class); \
    StaticOffsetCheck(Class, __VA_ARGS__);




#endif              // __VKPP_INFO_TYPE_CONSISTENCY_H__
