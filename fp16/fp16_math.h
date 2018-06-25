/**
 * @file fp16_math.h
 *
 * Copyright(C), 2017 - 2018, Lxzh Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief mathematics method of Half precision float 
 *
 * @version 1.0 
 *
 */
#ifndef _FP16_MATH_H_
#define _FP16_MATH_H_

#include "fp16_t.h"

/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculates fp16_t reciprocal function of input fp
 *@return  Returns fp16_t reciprocal of fp
 */
fp16_t hf_rcp(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculates fp16_t square root function of input fp
 *@return  Returns fp16_t square root of fp
 */
fp16_t hf_sqrt(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculates fp16_t reciprocal square root function of input fp
 *@return  Returns fp16_t reciprocal square root of fp
 */
fp16_t hf_rsqrt(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculates fp16_t natural exponential function of input fp
 *@return  Returns fp16_t natural exponential function of fp
 */
fp16_t hf_exp(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculates fp16_t binary exponential function of input fp
 *@return  Returns fp16_t binary exponential function of fp
 */
fp16_t hf_pow2(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp_v fp16_t object to be calculate
 *@brief   Calculates fp16_t decimal exponential function of input fp
 *@return  Returns fp16_t decimal exponential function of fp
 */
fp16_t hf_pow10(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp_v fp16_t object to be calculate
 *@brief   Calculate fp16_t natural logarithm of fp16_t
 *@return  Returns fp16_t natural logarithm of fp
 */
fp16_t hf_ln(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate fp16_t binary logarithm of fp16_t
 *@return  Returns fp16_t binary logarithm of fp
 */
fp16_t hf_log2(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate fp16_t decimal logarithm of fp16_t
 *@return  Returns fp16_t decimal logarithm of fp
 */
fp16_t hf_log10(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate fp16_t cosine of fp16_t
 *@return  Returns fp16_t cosine of fp
 */
fp16_t hf_cos(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate fp16_t sine of fp16_t
 *@return  Returns fp16_t sine of fp
 */
fp16_t hf_sin(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp fp16_t object to be calculate
 *@brief   Calculate the absolute value(the sign bit is 0) of the give value
 *@return  Returns fp16_t absolute value(the sign bit is 0) of of fp 
 */
fp16_t hf_abs(fp16_t fp);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp1 fp16_t object to be compare
 *@param [in] fp2 fp16_t object to be compare
 *@brief   Calculate the maximum fp16_t of fp1 and fp2
 *@return  Returns maximum fp16_t of fp1 and fp2
 */
fp16_t hf_max(fp16_t fp1, fp16_t fp2);
/**
 *@ingroup fp16_t mathematics method
 *@param [in] fp1 fp16_t object to be compare
 *@param [in] fp2 fp16_t object to be compare
 *@brief   Calculate the minimum fp16_t of fp1 and fp2
 *@return  Returns minimum fp16_t of fp1 and fp2
 */
fp16_t hf_min(fp16_t fp1, fp16_t fp2);


#endif /*_FP16_MATH_H_*/
