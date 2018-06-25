/**
 * @file fpe.h
 *
 * Copyright(C), 2017 - 2017, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief Half precision float
 *
 * @version 1.0
 *
 */
#ifndef _FPE_H_
#define _FPE_H_


#define BIAS16                         (15 - 2)
#define BIAS32                         (127 - 2)

/**
 *@ingroup fp basic parameter
 *@brief   Alternative half-precision
 */
#define FP_AHP                         (0x04000000U)	
/**
 *@ingroup fp basic parameter
 *@brief   Default NaN mode
 */
#define FP_DN	                       (0x02000000U)
/**
 *@ingroup fp basic parameter
 *@brief   Flush-to-zero mode
 */
#define FP_FZ	                       (0x01000000U)
/**
 *@ingroup fp basic parameter
 *@brief   fp16 Flush-to-zero mode
 */
#define FP_FZ16	                       (0x00080000U)
/**
 *@ingroup fp basic parameter
 *@brief   Input Denormal trap Enable
 */
#define FP_IDE	                       (0x00008000U)
/**
 *@ingroup fp basic parameter
 *@brief   IneXact Enable
 */
#define FP_IXE	                       (0x00001000U)
/**
 *@ingroup fp basic parameter
 *@brief   Input Denormal cumulative
 */
#define FP_IDC	                       (0x00000080U)
/**
 *@ingroup fp basic parameter
 *@brief   IneXact Cumulative
 */
#define FP_IXC	                       (0x00000010U)
/**
 *@ingroup fp basic parameter
 *@brief   Invalid Operand Cumulative
 */
#define FP_IOC	                       (0x00000001U)
/**
 *@ingroup fp basic parameter
 *@brief   UnderFlow Enable
 */
#define FP_UFE	                       (0x00000800U)
/**
 *@ingroup fp basic parameter
 *@brief   OverFlow Enable
 */
#define FP_OFE	                       (0x00000400U)
/**
 *@ingroup fp basic parameter
 *@brief   Divide-by-Zero Enable
 */
#define FP_DZE	                       (0x00000200U)
/**
 *@ingroup basic parameter
 *@brief   Invalid Operand Enable
 */
#define FP_IOE	                       (0x00000100U)
/**
 *@ingroup fp basic parameter
 *@brief   UnderFlow Cumulative
 */
#define FP_UFC	                       (0x00000008U)
/**
 *@ingroup fp basic parameter
 *@brief   OverFlow Cumulative
*/
#define FP_OFC	                       (0x00000004U)
/**
 *@ingroup fp basic parameter
 *@brief   Divide-by-Zero Cumulative
 */
#define FP_DZC	                       (0x00000002U)
/**
 *@ingroup fp basic operator
 *@brief   taishan FP trap enabled
 */
#define FP_TFP_EN                      (1u << 31) 

/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is normal number
 */
#define FP16_IS_NORMAL(X)              ((uint16_t) (((X) & (FP16_SIGN_MASK - 1)) - 1) >= FP16_MAN_HIDE_BIT - 1)
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16
 */
#define FP16_IS_QNAN(X)                (((X) & 0x0200) != 0)
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16
 */
#define FP16_IS_SNAN(X)                (((X) & 0x0200) == 0)
/**
 *@ingroup fp16 basic operator
 *@brief
 */
#define FP16_QNAN(C, X)                (((X) & -(((C) & FP_DN) == 0)) | FP16_INFINITY | 0x0200)

#define ISQNAN32(X)                    (((X) & 0x00400000U) != 0)

#define ISSNAN32(X)                    (((X) & 0x00400000U) == 0)

#define ISNORM32(X)                    ((uint32_t) (((X) & (0x80000000U - 1)) - 1) >= 0x00800000U - 1)

#define QNAN32(C, X)                   (((X) & -(((C) & FP_DN) == 0)) | 0x7F800000U | 0x00400000U)

#define RMINUS(C)                      ((int32_t) (((C) ^ 0x800000) & 0xC00000) - 1)

#ifdef __GNUC__
#define likely(X) __builtin_expect((X), 1)
#define unlikely(X) __builtin_expect((X), 0)
#else
#define likely(X) (X)
#define unlikely(X) (X)
#define inline __inline	//add by zc
#endif

/*
 * IEEE-754 floating point status returned from instruction execution.
 * This is not the cumulative status with sticky bits required by the
 * standard, but just the status of one individual operation.  Callers
 * using these routines to emulate an architecture are expected to know
 * how to merge status of individual operations into the architecture's
 * cumulative status with sticky exception bits.
 *
 * XFLOAT_STATUS_*_EXCEPT(XFS) returns the bitmap of exceptions (both
 * masked and unmasked) detected by execution.
 *
 * If (~XFLOAT_CONTROL_*_MASKED(XFC) & XFLOAT_STATUS_*_EXCEPT(XFS)) != 0
 * then execution is said to have signalled an exception.  IEEE specifies
 * results of operations even in the case that an exception is signalled,
 * and in particular specifies results for overflows and underflows that
 * allow the "true" result of the operation to be recovered.  Not all
 * hardware supports this feature; many architectures simply abort an
 * operation without writing a destination if an exception is signalled.
 * For architectures that deliver results even in the case  of signalled
 * exceptions, these routines deliver the same result.  For architectures
 * that deliver no results, these routines return a quiet NaN.
 */
typedef uint32_t XFStatus;
/*
 * IEEE-754 floating point control state, including exception mask bits,
 * precision control (if any), and rounding control.  The encoding is
 * different for each supported architecture.
 *
 * Each architecture provides an XFLOAT_CONTROL_*_MASKED(XFC) accessor
 * macro that returns a bitmask specifying which IEEE-754 exceptions
 * are masked according to that architecture's representation of the
 * control state.  The bits in this bitmask are aligned for use with
 * the XFLOAT_STATUS_*_EXCEPT(XFS) accessor described below.
 *
 * Architectures may have additional exception mask bits for exception
 * types not specified as part of IEEE-754.
 */
typedef uint32_t XFControl;

struct XFFP16 {
    uint16_t result;
    XFStatus status;
};

struct XFFP32 {
    uint32_t result;
    XFStatus status;
};

/*
 * Each entry in this array is the encoded rounding table for a particular mode.
 * A rounding table is a bit vector; certain bits from the unrounded result
 * are used to form an index into this bit vector, and the bit selected from
 * the vector is added at the least-significant-bit position in the result.
 * The encoding of the 6-bit index is as follows:
 *
 *	index bit 5: result least-significant bit
 *	index bit 4: result guard bit 0
 *	index bit 3: result guard bit 1
 *	index bit 2: result guard bit 2
 *	index bit 1: result guard bit 3/sticky bit
 *	index bit 0: result sign bit
 *
 * This encoding was chosen since it can be conveniently formed by rotating
 * the result 1 bit left so that the sign bit becomes the low-order bit.
 *
 * Although IEEE arithmetic conceptually requires only 3 guard bits (the
 * last of which must be a sticky bit), this table provides for 4 guard bits,
 * which allows the addition/subtraction algorithms to avoid a right shift,
 * and adds no cost to other algorithms.
 *
 * Rounding modes higher than 3 are only accessible to xfloat_rint*_arm()
 * and xfloat_f*to[su]*_arm().
 */
const uint64_t rtab[129] = {
    0xFFFF0000FFFC0000U,		/* round to nearest even */
    0x5555555455555554U,		/* round towards +infinity */
    0xAAAAAAA8AAAAAAA8U,		/* round towards -infinity */
    0x0000000000000000U,		/* round towards zero */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0xFFFF0000FFFF0000U,		/* round to nearest ties away */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,
    0x0000FFFC0000FFFCU,		/* round to odd (von Neumann) */
};


/*
 * Lookup table for results of recip_sqrt_estimate() function specified
 * in the ARM v8 architecture reference manual.
 */
static const uint16_t recip_sqrt_table[256] = {
    511, 509, 507, 505, 503, 501, 499, 498,
    496, 494, 492, 490, 489, 487, 485, 484,
    482, 480, 479, 477, 475, 474, 472, 471,
    469, 468, 466, 465, 463, 462, 460, 459,
    457, 456, 454, 453, 452, 450, 449, 448,
    446, 445, 444, 442, 441, 440, 439, 437,
    436, 435, 434, 432, 431, 430, 429, 428,
    426, 425, 424, 423, 422, 421, 420, 419,
    418, 416, 415, 414, 413, 412, 411, 410,
    409, 408, 407, 406, 405, 404, 403, 402,
    401, 400, 399, 398, 397, 396, 396, 395,
    394, 393, 392, 391, 390, 389, 388, 387,
    387, 386, 385, 384, 383, 382, 382, 381,
    380, 379, 378, 377, 377, 376, 375, 374,
    374, 373, 372, 371, 370, 370, 369, 368,
    367, 367, 366, 365, 365, 364, 363, 362,
    361, 360, 359, 357, 356, 355, 353, 352,
    351, 349, 348, 347, 346, 344, 343, 342,
    341, 340, 338, 337, 336, 335, 334, 333,
    332, 331, 330, 328, 327, 326, 325, 324,
    323, 322, 321, 320, 319, 318, 317, 316,
    316, 315, 314, 313, 312, 311, 310, 309,
    308, 307, 307, 306, 305, 304, 303, 302,
    302, 301, 300, 299, 298, 298, 297, 296,
    295, 294, 294, 293, 292, 291, 291, 290,
    289, 289, 288, 287, 286, 286, 285, 284,
    284, 283, 282, 282, 281, 280, 280, 279,
    278, 278, 277, 276, 276, 275, 275, 274,
    273, 273, 272, 272, 271, 270, 270, 269,
    269, 268, 267, 267, 266, 266, 265, 265,
    264, 264, 263, 262, 262, 261, 261, 260,
    260, 259, 259, 258, 258, 257, 257, 256,
};


inline int16_t Max16(int16_t x, int16_t y){
    return x >= y ? x : y;
}
inline int16_t Min16(int16_t x, int16_t y){
    return x <= y ? x : y;
}
inline int32_t Max32(int32_t x, int32_t y)
{
    return x > y ? x : y;
}
inline int Min32(int x, int y)
{
    return x < y ? x : y;
}
#ifndef WIN32 
inline int max(int x, int y)
{
    return x > y ? x : y;
}

inline int min(int x, int y)
{
    return x < y ? x : y;
}
#endif

inline int32_t Clz32(uint32_t z)
{
#ifdef __GNUC__
#if defined(__amd64__) || defined(__i386__)
    /*
    * We use this because version because it optimizes better
    * than GCC's __builtin_clz() when computing clz(x) - CONST.
    */
    uint32_t n;
    asm("bsr %1, %0" : "=r" (n) : "r" (z), "0" ((uint32_t)-1));
    return 31 - n;
#else
    uint32_t m;
    m = -((z | -z) >> 31);
    return (m & __builtin_clz(z)) | (~m & 32);
#endif
#else
    int num = 0;
    z |= 1; //防止type为0时，出现无限循环infinite loop，type为0时的计算结果为31。
    while (!(z & 0x80000000U)) //检测最高位是不是1。
    {
        num += 1;
        z <<= 1;
    }
    return num;
    //return -858993460;
#endif
}
inline int Clz64(uint64_t z)
{
#ifdef __GNUC__
#if defined(__amd64__)
    /*
    * We use this because version because it optimizes better
    * than GCC's __builtin_clz() when computing clz(x) - CONST.
    */
    uint64_t n;
    asm("bsr %1, %0" : "=r" (n) : "r" (z), "0" ((uint64_t)-1));
    return 63 - n;
#else
    unsigned int m;
    m = -((z | -z) >> 63);
    //if (sizeof (unsigned int) == sizeof z)
    //    return (m & __builtin_clz(z)) | (~m & 64);
    //else if (sizeof (unsigned long int) == sizeof z)
    //    return (m & __builtin_clzl(z)) | (~m & 64);
    //else if (sizeof (unsigned long long int) == sizeof z)
    //    return (m & __builtin_clzll(z)) | (~m & 64);
    //else
    //    assert(0 && "can't find correct __builtin_clz* function");
    return (m & __builtin_clzll(z)) | (~m & 64);
#endif
#else
    int num = 0;
    z |= 1; 
    while (!(z & 0x8000000000000000LLU)) 
    {
        num += 1;
        z <<= 1;
    }
    return num;
#endif
}
inline int32_t Clz16(uint16_t z)
{
    return Clz32((uint32_t)z) - 16;
}
inline uint32_t Rol16(uint32_t x, uint32_t n)
{
    return (uint16_t)((x << n) | (x >> (16 - n)));
}
inline uint32_t Rol32(uint32_t x, unsigned int n)
{
    return (x << n) | (x >> (32 - n));
}
inline uint32_t Shrsticky32(uint32_t x, int n)
{
    n = Min32(n, 31);
    return (x >> n) | ((x & (((uint32_t)1 << n) - 1)) != 0);
}
inline uint64_t Sar64(uint64_t x, unsigned int n)
{
    if ((uint64_t)((int64_t)0x8765432187654321U >> 16) == 0xFFFF876543218765U) {
        return (uint64_t)((int64_t)x >> n);
    }
    else if (((int64_t)0x8765432187654321U >> 16) == (int64_t)0xFFFF876543218765U) {
        union { int64_t i; uint64_t u; } u;
        return u.u = x, u.i >>= n, u.u;
    }
    else {
        uint64_t m = -(uint64_t)(x >> 63);
        return ((x ^ m) >> n) ^ m;
    }
}
inline uint64_t Sarsticky64(uint64_t x, int n)
{
    n = Min32(n, 63);
    return Sar64(x, n) | ((x & (((uint64_t)1 << n) - 1)) != 0);
}
inline uint64_t Shrsticky64(uint64_t x, int n)
{
    n = Min32(n, 63);
    return (x >> n) | ((x & (((uint64_t)1 << n) - 1)) != 0);
}

inline uint64_t Smtc64(uint64_t x)
{
    uint64_t m = -(uint64_t)(x >> 63);
    return (x + m) ^ (m >> 1);
}
inline uint64_t Tcabs64(uint64_t x)
{
    uint64_t m = -(uint64_t)(x >> 63);
    return (x + m) ^ m;
}

#endif /*_FPLIB_H_*/
