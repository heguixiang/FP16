/** 
 * @file fp16_unit.h
 *
 * Copyright(C), 2017 - 2018, Lxzh Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief mathematics method of Half precision float 
 *
 * @version 1.0 
 *
*/
#ifndef _FP16_UNIT_H_
#define _FP16_UNIT_H_

#include "fp16_t.h"

#define MATRIX_LENGTH                  (16)
/**
 *@ingroup fp16_t public method
 *@param [in]     sa sign of one fp16_t/float number
 *@param [in]     ea exponent of one fp16_t/float number
 *@param [in|out] ma mantissa of one fp16_t/float number
 *@param [in]     sb sign of another fp16_t/float number
 *@param [in]     eb exponent of another fp16_t/float number
 *@param [in|out] mb mantissa of another fp16_t/float number
 *@brief   Pre-calculate sign of sum from two fp16_t/float number
 *@return  Return sign of sum
 */
template <typename T> uint32_t CalcAdditionSign(uint32_t sa, int16_t ea, T *ma, uint16_t sb, int16_t eb, T *mb){
    uint32_t sign;
    if (sa != sb){
        ReverseMan(sa > 0, ma);
        ReverseMan(sb > 0, mb);
        T sum = GetManSum(ea, *ma, eb, *mb);
        int bits = sizeof(T)* 8;
        T mask = (((T)1u) << (bits - 1));
        sign = (uint32_t)((sum&mask) >> (bits - 1));
        ReverseMan(sign > 0, ma);
        ReverseMan(sign > 0, mb);
    }
    else{
        sign = sa;
    }
    return sign;
}
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate the value of fp16_t use activation function
 *@return  Returns the value of fp16_t use activation function to calculate on fp
 */
fp16_t hf_relu(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp1 fp16_t multiplicator to be calculate
 *@param [in] fp2 fp16_t multiplicand to be calculate
 *@param [in] fp3 fp16_t augend to be calculate
 *@brief   Calculate fp16_t multiplication and addition of fp1, fp2 and fp3
 *@return  Returns the value of fp1*fp2+fp3
 */
fp16_t hf_mla(fp16_t fp1, fp16_t fp2, fp16_t fp3);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp1 fp16_t multiplicator to be calculate
 *@param [in] fp2 fp16_t multiplicand to be calculate
 *@param [in] fc float augend to be calculate
 *@brief   Calculate fp16_t multiplication and addition of fp1, fp2 and fp3
 *@return  Returns the value of fp1*fp2+fp3
 */
float hf_mla(fp16_t fp1, fp16_t fp2, float fc);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp1 addition operator one
 *@param [in] fp2 addition operator two 
 *@brief   Do addition operate of two fp16_t and output a float/fp32 result
 *            this method is more precise than doing additon operate by '+' 
 *            operator and then convert to float/fp32
 *@return  Returns float addition result(the sign bit is 0) of of fp1 and fp2
 */
float hf_fadd(fp16_t fp1, fp16_t fp2);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fpas array A of type fp16_t
 *@param [in] fpbs array B of type fp16_t
 *@param [in] c float number c
 *@param [in] len array length (just support 16)
 *@brief multiplication and addition
 *    fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+
 *    fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16
 *@return  Returns fp16_t result of multiplication and addition
 */
fp16_t hf_mma(fp16_t fpas[], fp16_t fpbs[], fp16_t c, int len);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fpas array A of type fp16_t
 *@param [in] fpbs array B of type fp16_t
 *@param [in] c float number c
 *@param [in] len array length (just support 16)
 *@brief multiplication and addition
 *    fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+      
 *    fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp16a*fp16b+fp32  
 *@return  Returns float result of multiplication and addition
 */
float hf_mma(fp16_t fpas[], fp16_t fpbs[], float c, int len);

float d_mma(fp16_t fpas[], fp16_t fpbs[], float c, int len);

/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate the half value of the give value
 *@return  Returns fp16_t half value of of fp
 */
fp16_t hf_half(fp16_t fp);

fp16_t hf_recip(fp16_t fp);

fp16_t hf_recipsqrt(fp16_t fp);

#endif /*_FP16_UNIT_H_*/
