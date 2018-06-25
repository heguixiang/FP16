/**
 * @file fplib.h
 *
 * Copyright(C), 2017 - 2017, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief float point library
 *
 * @version 1.0
 *
 */
#ifndef _FPLIB_H_
#define _FPLIB_H_

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <iostream>

/**
 *@ingroup fp16 basic parameter 
 *@brief   fp16 exponent bias
 */
#define FP16_EXP_BIAS                  (15)
/**
 *@ingroup fp16 basic parameter 
 *@brief   the exponent bit length of fp16 is 5
 */
#define FP16_EXP_LEN                   (5)            
/**
 *@ingroup fp16 basic parameter 
 *@brief   the mantissa bit length of fp16 is 10
 */
#define FP16_MAN_LEN                   (10)
/**
 *@ingroup fp16 basic parameter 
 *@brief   bit index of sign in fp16
 */
#define FP16_SIGN_INDEX                (15)    
/**
 *@ingroup fp16 basic parameter 
 *@brief   sign mask of fp16         (1 00000 00000 00000)
 */
#define FP16_SIGN_MASK                 (0x8000)
/**
 *@ingroup fp16 basic parameter 
 *@brief   exponent mask of fp16     (  11111 00000 00000)
 */
#define FP16_EXP_MASK                  (0x7C00)
/**
 *@ingroup fp16 basic parameter 
 *@brief   mantissa mask of fp16     (        11111 11111)
 */
#define FP16_MAN_MASK                  (0x03FF)
/**
 *@ingroup fp16 basic parameter 
 *@brief   hide bit of mantissa of fp16(   1 00000 00000)
 */
#define FP16_MAN_HIDE_BIT              (0x0400)
/**
 *@ingroup fp16 basic parameter
 *@brief   maximum value            (0111 1011 1111 1111)
 */
#define FP16_MAX                       (0x7BFF)
/**
 *@ingroup fp16 basic parameter
 *@brief   minimum value            (1111 1011 1111 1111)
 */
#define FP16_MIN                       (0xFBFF)
/**
 *@ingroup fp16 basic parameter
 *@brief   absolute maximum value   (0111 1111 1111 1111)
 */
#define FP16_ABS_MAX                   (0x7FFF)
/**
 *@ingroup fp16 basic parameter
 *@brief   maximum exponent value of fp16 is 31(11111)
 */
#define FP16_MAX_EXP                   (0x001F)
/**
 *@ingroup fp16 basic parameter
 *@brief   maximum valid exponent value of fp16 is 14(11110)
 */
#define FP16_MAX_VALID_EXP             (0x001E)
/**
 *@ingroup fp16 basic parameter
 *@brief   maximum mantissa value of fp16(11111 11111)
 */
#define FP16_MAX_MAN                   (0x03FF)
/**
 *@ingroup fp16 basic parameter
 *@brief   absolute minimum normal value of fp16
 *         (E=1,M=0 D=2^(-14)=0.00006103515625)
 */
#define FP16_MIN_NORMAL                ((1.0f/(1<<14)))
/**
 *@ingroup fp16 basic parameter
 *@brief   infinity of fp16
 */
#define FP16_INFINITY                  (0x7C00)
/**
 *@ingroup fp16 basic operator
 *@brief   get sign of fp16
 */
#define FP16_EXTRAC_SIGN(x)            (((x)>>15)&1)
/**
 *@ingroup fp16 basic operator
 *@brief   get exponent of fp16
 */
#define FP16_EXTRAC_EXP(x)             (((x)>>10)&FP16_MAX_EXP)
/**
 *@ingroup fp16 basic operator
 *@brief   get mantissa of fp16
 */
#define FP16_EXTRAC_MAN(x)             ((((x)>>0)&0x3FF)|(((((x)>>10)&0x1F)>0?1:0)*0x400))    
/**
 *@ingroup fp16 basic operator
 *@brief   constructor of fp16 from sign exponent and mantissa
 */
#define FP16_CONSTRUCTOR(s,e,m)        (((s) << FP16_SIGN_INDEX) | ((e) << FP16_MAN_LEN) | ((m)&FP16_MAX_MAN))

/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is zero
 */
#define FP16_IS_ZERO(x)                (((x)&FP16_ABS_MAX)==0)
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is a denormalized value
 */
#define FP16_IS_DENORM(x)              ((((x)&FP16_EXP_MASK)==0))
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is invalid
 */
#define FP16_IS_INVALID(x)             ((x&FP16_EXP_MASK)==FP16_EXP_MASK)
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is infinite
 */
#define FP16_IS_INF(x)                 ((uint16_t(x)==0xFC00)||(uint16_t(x)==0x7C00))
/**
 *@ingroup fp16 special value judgment
 *@brief   whether a fp16 is NaN
 */
#define FP16_IS_NAN(x)                 (((x&FP16_EXP_MASK)==FP16_EXP_MASK)&&(x&FP16_MAN_MASK))
/**
 *@ingroup fp32 special value
 *@brief   maximum float number convert to maximum valid positive fp16 (65504)
 */
#define FP32_FP16_PMAX                 (65520.0f)
/**
 *@ingroup fp32 special value
 *@brief   minimun positive float number convert to minimun positive fp16(1.0f/(1<<24))
 */
#define FP32_FP16_PMIN                 (1.0f/(1<<25))

/**
 *@ingroup fp32 basic parameter
 *@brief   fp32 exponent bias
 */
#define FP32_EXP_BIAS                  (127)
/**
 *@ingroup fp32 basic parameter
 *@brief   the exponent bit length of float/fp32 is 8
 */
#define FP32_EXP_LEN                   (8)
/**
 *@ingroup fp32 basic parameter
 *@brief   the mantissa bit length of float/fp32 is 23
 */
#define FP32_MAN_LEN                   (23)
/**
 *@ingroup fp32 basic parameter
 *@brief   bit index of sign in float/fp32
 */
#define FP32_SIGN_INDEX                (31)
/**
 *@ingroup fp32 basic parameter
 *@brief   sign mask of fp32         (1 0000 0000  0000 0000 0000 0000 000)
 */
#define FP32_SIGN_MASK                 (0x80000000u)
/**
 *@ingroup fp32 basic parameter
 *@brief   exponent mask of fp32     (  1111 1111  0000 0000 0000 0000 000)
 */
#define FP32_EXP_MASK                  (0x7F800000u)
/**
 *@ingroup fp32 basic parameter
 *@brief   mantissa mask of fp32     (             1111 1111 1111 1111 111)
 */
#define FP32_MAN_MASK                  (0x007FFFFFu)
/**
 *@ingroup fp32 basic parameter
 *@brief   hide bit of mantissa of fp32      (  1  0000 0000 0000 0000 000)
 */
#define FP32_MAN_HIDE_BIT              (0x00800000u)
/**
 *@ingroup fp32 basic parameter
 *@brief   absolute maximum value    (0 1111 1111  1111 1111 1111 1111 111)
 */
#define FP32_ABS_MAX                   (0x7FFFFFFFu)
/**
 *@ingroup fp32 basic parameter
 *@brief   maximum exponent value of fp32 is 255(1111 1111)
 */
#define FP32_MAX_EXP                   (0xFF)
/**
 *@ingroup fp32 basic parameter
 *@brief   maximum mantissa value of fp32    (1111 1111 1111 1111 1111 111)
 */
#define FP32_MAX_MAN                   (0x7FFFFF)
/**
 *@ingroup fp32 basic parameter
 *@brief   maximun valid fp32
 */
#define FP32_MAX                       (0x7F7FFFFFu)
/**
 *@ingroup fp32 basic parameter
 *@brief   minimun valid fp32
 */
#define FP32_MIN                       (0xFF7FFFFFu)
/**
 *@ingroup fp32 special value judgment
 *@brief   whether a fp32 is NaN
 */
#define FP32_IS_NAN(x)                 (((x&FP32_EXP_MASK)==FP32_EXP_MASK)&&(x&FP32_MAN_MASK))
/**
 *@ingroup fp32 special value judgment
 *@brief   whether a fp32 is infinite
 */
#define FP32_IS_INF(x)                 (((x&FP32_EXP_MASK)==FP32_EXP_MASK)&&(!(x&FP32_MAN_MASK)))
/**
 *@ingroup fp32 special value judgment
 *@brief   whether a fp32 is a denormalized value
 */
#define FP32_IS_DENORM(x)              ((((x)&FP32_EXP_MASK)==0))
/**
 *@ingroup fp32 special value judgment
 *@brief   whether a fp32 is invalid
 */
#define FP32_IS_INVALID(x)             ((x&FP32_EXP_MASK)==FP32_EXP_MASK)
/**
 *@ingroup fp32 special value judgment
 *@brief   whether a fp32 is zero
 */
#define FP32_IS_ZERO(x)                (((x)&FP32_ABS_MAX)==0)
/**
 *@ingroup fp32 basic operator
 *@brief   get sign of fp32
 */
#define FP32_EXTRAC_SIGN(x)            (((x)>> FP32_SIGN_INDEX)&1)
/**
 *@ingroup fp32 basic operator
 *@brief   get exponent of fp16
 */
#define FP32_EXTRAC_EXP(x)             (((x)&FP32_EXP_MASK)>>FP32_MAN_LEN)
/**
 *@ingroup fp32 basic operator
 *@brief   get mantissa of fp16
 */
#define FP32_EXTRAC_MAN(x)             (((x)&FP32_MAN_MASK) | (((((x) >> FP32_MAN_LEN) & FP32_MAX_EXP) > 0 ? 1 : 0)*FP32_MAN_HIDE_BIT))
/**
 *@ingroup fp32 basic operator
 *@brief   constructor of fp32 from sign exponent and mantissa
 */
#define FP32_CONSTRUCTOR(s,e,m)        (((s)<< FP32_SIGN_INDEX)|((e)<< FP32_MAN_LEN)| ((m)&FP32_MAX_MAN))

/**
 *@ingroup fp64 special value
 *@brief   maximum double number convert to maximum valid positive fp32 (1<<128-1<<103)
 */
#define FP64_FP32_PMAX                 (340282356779733661637539395458142568448.00000)
/**
 *@ingroup fp64 special value
 *@brief   minimun positive float number convert to minimun positive fp32(1.0f/(1<<149))
 *         0.000000000000000000000000000000000000000000000700649232162408535461864791644958065640130970938257885878534141944895541342930300743319094181060791015625
 */
#define FP64_FP32_PMIN                 (0.00000000000000000000000000000000000000000000070064923216240854)
/**
 *@ingroup fp64 basic parameter
 *@brief   fp64 exponent bias
 */
#define FP64_EXP_BIAS                  (1023)
/**
 *@ingroup fp64 basic parameter
 *@brief   the exponent bit length of double/fp64 is 11
 */
#define FP64_EXP_LEN                   (11)
/**
 *@ingroup fp64 basic parameter
 *@brief   the mantissa bit length of double/fp64 is 52
 */
#define FP64_MAN_LEN                   (52)
/**
 *@ingroup fp64 basic parameter
 *@brief   bit index of sign in double/fp64 is 63
 */
#define FP64_SIGN_INDEX                (63)
/**
 *@ingroup fp64 basic parameter
 *@brief   sign mask of fp64                 (1 000                   (total 63bits 0))
 */
#define FP64_SIGN_MASK                 (0x8000000000000000LLu)
/**
 *@ingroup fp64 basic parameter
 *@brief   exponent mask of fp64            (0 1 11111 11111  0000?-?-(total 52bits 0))
 */
#define FP64_EXP_MASK                  (0x7FF0000000000000LLu)
/**
 *@ingroup fp64 basic parameter
 *@brief   mantissa mask of fp64            (                 1111?-?-(total 52bits 1))
 */
#define FP64_MAN_MASK                  (0x000FFFFFFFFFFFFFLLu)
/**
 *@ingroup fp64 basic parameter
 *@brief   hide bit of mantissa of fp64     (               1 0000?-?-(total 52bits 0))
 */
#define FP64_MAN_HIDE_BIT              (0x0010000000000000LLu)
/**
 *@ingroup fp64 basic parameter
 *@brief   absolute maximum value           (0 111?-?-(total 63bits 1))
 */
#define FP64_ABS_MAX                   (0x7FFFFFFFFFFFFFFFLLu)
/**
 *@ingroup fp64 basic parameter
 *@brief   maximum exponent value of fp64 is 2047(1 11111 11111)
 */
#define FP64_MAX_EXP                   (0x07FF)
/**
 *@ingroup fp64 basic parameter
 *@brief   maximum mantissa value of fp64  (111?-?-(total 52bits 1))
 */
#define FP64_MAX_MAN                   (0xFFFFFFFFFFFLLu)
/**
 *@ingroup fp64 special value judgment
 *@brief   whether a fp64 is NaN
 */
#define FP64_IS_NAN(x)                 (((x&FP64_EXP_MASK)==FP64_EXP_MASK)&&(x&FP64_MAN_MASK))
/**
 *@ingroup fp64 special value judgment
 *@brief   whether a fp64 is infinite
 */
#define FP64_IS_INF(x)                 (((x&FP64_EXP_MASK)==FP64_EXP_MASK)&&(!(x&FP64_MAN_MASK)))

/**
 *@ingroup integer special value judgment
 *@brief   maximum positive value of int8_t            (0111 1111)
 */
#define INT8_T_MAX                     (0x7F)
/**
 *@ingroup integer special value judgment
 *@brief   maximum value of a data with 8 bits length  (1111 111)
 */
#define BIT_LEN8_MAX                   (0xFF)
/**
 *@ingroup integer special value judgment
 *@brief   maximum positive value of int16_t           (0111 1111 1111 1111)
 */
#define INT16_T_MAX                    (0x7FFF)
/**
 *@ingroup integer special value judgment
 *@brief   maximum value of a data with 16 bits length (1111 1111 1111 1111)
 */
#define BIT_LEN16_MAX                  (0xFFFF)
/**
 *@ingroup integer special value judgment
 *@brief   maximum positive value of int32_t           (0111 1111 1111 1111 1111 1111 1111 1111)
 */
#define INT32_T_MAX                    (0x7FFFFFFFu)
/**
 *@ingroup integer special value judgment
 *@brief   maximum value of a data with 32 bits length (1111 1111 1111 1111 1111 1111 1111 1111)
 */
#define BIT_LEN32_MAX                  (0xFFFFFFFFu)

/**
 *@ingroup print switch 
 *@brief   print an error if input fp16 is overflow
 */
//#define PRINT_INPUT_OVERFLOW_ERROR

/**
 *@ingroup fp enum
 *@brief   round mode of last valid digital
 */
typedef enum tagFPRoundMode
{
    ROUND_TO_NEAREST = 0,    /**< round to nearest even */
    ROUND_TO_CEILING,        /**< round to ceiling      */
    ROUND_TO_FLOOR,          /**< round by floor        */
    ROUND_BY_TRUNCATED,      /**< round by truncated    */
    ROUND_MODE_RESERVED,
} fpRoundMode_t;

typedef enum tagFPMathMethodType{
    MATH_RCP = 0,
    MATH_SQRT,
    MATH_RSQRT,
    MATH_ABS,
    MATH_EXP,
    MATH_LN,
    MATH_LOG2,
    MATH_LOG10,
    MATH_POW2,
    MATH_POW10,
    MATH_SIN,
    MATH_COS
} fpMathMethodType;

typedef enum tagFPCompareType{
    EQUAL = 0,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    LESS_THAN,
    LESS_EQUAL
} fpCompareType;

typedef struct tagFPOpStatus{
    int ove;    //overflow
    int und;    //underflow
    int nan;    //not a number
    int inf;    //infinity
    int neg;    //negtive input (sqrt/rsqrt/ln)
    tagFPOpStatus(int ov = 0, int un = 0, int na = 0, int in = 0, int ne = 0)
        :ove(ov), und(un), nan(na), inf(in), neg(ne){};
    //获取算子运算status值
    int getStatus(){
        return ((neg << 4) | (inf << 3) | (nan << 2) | (und << 1) | (ove));
    }
} fpOpStatus;

typedef enum tagFPOp{
    FP_OP_ADD,
    FP_OP_SUB,
    FP_OP_MUL,
    FP_OP_DIV,
    FP_OP_CMP_EQ,
    FP_OP_CMP_NE,
    FP_OP_CMP_GT,
    FP_OP_CMP_GE,
    FP_OP_CMP_LT,
    FP_OP_CMP_LE,
    FP_OP_ABS,
    FP_OP_MAX,
    FP_OP_MIN,
    FP_OP_REC,
    FP_OP_SQRT,
    FP_OP_RSQRT,
    FP_OP_LN,
    FP_OP_EXP,
    FP_OP_RELU,
    FP_OP_DEQ_MUL
} fpOp;

/**
 *@ingroup fp public method
 *@param [in]     negative sign is negative
 *@param [in|out] man      mantissa to be reverse
 *@brief   Calculate a mantissa's complement (add ont to it's radix-minus-one complement)
 *@return  Return complement of man
 */
template <typename T> void ReverseMan(bool negative, T *man){
    if (negative){
        *man = (~(*man)) + 1;
    }
}
/**
 *@ingroup fp16_t public method
 *@param [in] ea exponent of one fp16_t/float number
 *@param [in] ma mantissa of one fp16_t/float number
 *@param [in] eb exponent of another fp16_t/float number
 *@param [in] mb mantissa of another fp16_t/float number
 *@brief   choose mantissa to be shift right whoes exponent is less than another one
 *@return  Return mantissawhoes exponent is less than another one
 */
template <typename T> T* MinMan(const int16_t &ea, T *ma, const int16_t &eb, T *mb){
    return (ea > eb) ? mb : ma;
}
/**
 *@ingroup fp16_t public method
 *@param [in] man   mantissa to be operate
 *@param [in] shift right shift bits
 *@brief   right shift a mantissa
 *@return  Return right-shift mantissa
 */
template <typename T> T RightShift(T man, int16_t shift){
    int bits = sizeof(T)* 8;
    T mask = (((T)1u) << (bits - 1));
    for (int i = 0; i < shift; i++){
        man = ((man & mask) | (man >> 1));
    }
    return man;
}
/**
 *@ingroup fp16_t public method
 *@param [in] ea exponent of one temp fp16_t number
 *@param [in] ma mantissa of one temp fp16_t number
 *@param [in] eb exponent of another temp fp16_t number
 *@param [in] mb mantissa of another temp fp16_t number
 *@brief   Get mantissa sum of two temp fp16_t numbers, T support types: uint16_t/uint32_t/uint64_t
 *@return  Return mantissa sum
 */
template <typename T> T GetManSum(int16_t ea, const T &ma, int16_t eb, const T &mb){
    T sum = 0;
    if (ea != eb){
        T m_tmp = 0;
        int16_t e_tmp=std::abs(ea-eb);
        if (ea > eb){
            m_tmp = mb;
            m_tmp = RightShift(m_tmp, e_tmp);
            sum = ma + m_tmp;
        }
        else{
            m_tmp = ma;
            m_tmp = RightShift(m_tmp, e_tmp);
            sum = m_tmp + mb;
        }
    }
    else{
        sum = ma + mb;
    }
    return sum;
}
/**
 *@ingroup fp16_t public method
 *@param [in] bit0    whether the last preserved bit is 1 before round
 *@param [in] bit1    whether the abbreviation's highest bit is 1
 *@param [in] bitLeft whether the abbreviation's bits which not contain highest bit grater than 0
 *@param [in] man     mantissa of a fp16_t or float number, support types: uint16_t/uint32_t/uint64_t
 *@param [in] shift   abbreviation bits
 *@brief    Round fp16_t or float mantissa to nearest value
 *@return   Returns true if round 1,otherwise false;
 */
template <typename T> T ManRoundToNearest(bool bit0, bool bit1, bool bitLeft, T man, uint16_t shift = 0){
    man = (man >> shift) + ((bit1 && (bitLeft || bit0)) ? 1 : 0);
    return man;
}
/**
 *@ingroup fp16_t public method
 *@param [in] man    mantissa of a float number, support types: uint16_t/uint32_t/uint64_t
 *@brief   Get bit length of a uint32_t number
 *@return  Return bit length of man
 */
template <typename T> int16_t GetManBitLength(T man){
    int16_t len = 0;
    while (man){
        man >>= 1;
        len++;
    }
    return len;
}

#endif /*_FPLIB_H_*/
