/**
 * @file fp16_math.cc
 *
 * Copyright(C), 2017 - 2018, Lxzh Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief Half precision float
 *
 * @version 1.0
 *
 */

#include "fp16_math.h"

fp16_t hf_rcp(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //Convert half precision float number to double 
    double dVal = fp;
    // Calculate double number reciprocal
    double dRcp = 1.0 / dVal;
    // calculate result
    ret = dRcp;/*lint !e524 */
    return ret;
}

fp16_t hf_sqrt(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //half precision float number to double 
    double dVal = fp;
    // Calculate double number square root
    double dSqrt = std::sqrt(dVal);
    // calculate result
    ret = dSqrt;/*lint !e524 */
    return ret;
}

fp16_t hf_rsqrt(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //Convert half precision float number to double 
    double dVal = fp;
    // Calculate double number square root and reciprocal
    double drSqrt = 1.0 / std::sqrt(dVal);
    // calculate result
    ret = drSqrt;/*lint !e524 */
    return ret;
}

fp16_t hf_pow2(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //Convert half precision float number to double 
    double dVal = fp;
    // Calculate double number binary exponential
    double dExp2 = std::pow(2, dVal);
    // calculate result
    ret = dExp2;/*lint !e524 */

    return ret;
}

fp16_t hf_pow10(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //Convert half precision float number to double 
    double dVal = fp;
    // Calculate double number decimal exponential
    double dExp10 = std::pow(10, dVal);
    // calculate result
    ret = dExp10;/*lint !e524 */

    return ret;
}

fp16_t hf_log2(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    // Convert half precision float number to double
    double dVal = fp;
    // Calculate double number binary logarithm
    double dLog2 = std::log2(dVal);
    // calculate result
    ret = dLog2; /*lint !e524 */

    return ret;
}

fp16_t hf_log10(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    // Convert half precision float number to double
    double dVal = fp;
    // Calculate double number binary logarithm
    double dLog10 = std::log10(dVal);
    // calculate result
    ret = dLog10;/*lint !e524 */

    return ret;
}

fp16_t hf_cos(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    // Convert half precision float number to double
    double dVal = fp;
    // Calculate double number cos
    double dCos = std::cos(dVal);
    // calculate result
    ret = dCos;/*lint !e524 */

    return ret;
}

fp16_t hf_sin(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    // Convert half precision float number to double
    double dVal = fp;
    // Calculate double number sine
    double dSin = std::sin(dVal);
    // calculate result
    ret = (float)dSin;/*lint !e524 */

    return ret;
}

fp16_t hf_abs(fp16_t fp){
    fp16_t ret;
    ret.val = (fp.val&FP16_ABS_MAX);
    return ret;
}

fp16_t hf_max(fp16_t fp1, fp16_t fp2){
    if (fp1 >= fp2){
        return fp1;
    }
    else{
        return fp2;
    }
}

fp16_t hf_min(fp16_t fp1, fp16_t fp2){
    if (fp1 <= fp2){
        return fp1;
    }
    else{
        return fp2;
    }
}

fp16_t hf_exp(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    //Convert half precision float number to double 
    double d_val = fp;
    // Calculate double number exponential
    double d_exp = exp(d_val);
    // calculate result
    ret = d_exp;/*lint !e524 */

    return ret;
}

fp16_t hf_ln(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;
    // Convert half precision float number to double
    double d_val = fp;
    // Calculate double number exponential
    double d_ln = std::log(d_val);
    // calculate result
    ret = d_ln;/*lint !e524 */

    return ret;
}
