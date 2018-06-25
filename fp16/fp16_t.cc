/**
 * @file fp16_t.cc
 *
 * Copyright(C), 2017 - 2018, Lxzh Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief Half precision float
 *
 * @version 1.0
 *
 */
#include "fp16_t.h"

/**
 *@ingroup fp16_t global filed
 *@brief   round mode of last valid digital
 */
fp16RoundMode_t g_RoundMode = ROUND_TO_NEAREST;

void ExtractFP16(const uint16_t &val, uint16_t *s, int16_t *e, uint16_t *m){
    //1.Extract
    *s = FP16_EXTRAC_SIGN(val);
    *e = FP16_EXTRAC_EXP(val);
    *m = FP16_EXTRAC_MAN(val);

    //Denormal
    if (0 == (*e)){
        *e = 1;
    }
}
/**
 *@ingroup fp16_t static method
 *@param [in] man       truncated mantissa
 *@param [in] shift_out left shift bits based on ten bits
 *@brief   judge whether to add one to the result while converting fp16_t to other datatype
 *@return  Return true if add one, otherwise false
 */
static bool IsRoundOne(uint64_t man, uint16_t trunc_len){
    uint64_t mask0 = 0x4;
    uint64_t mask1 = 0x2;
    uint64_t mask2 = 0x1;
    uint16_t shift_out = trunc_len - 2;
    mask0 = mask0 << shift_out;
    mask1 = mask1 << shift_out;
    mask2 = mask1 - 1;

    bool last_bit = ((man & mask0) > 0);
    bool trunc_high = 0;
    bool trunc_left = 0;
    if (ROUND_TO_NEAREST == g_RoundMode){
        trunc_high = ((man & mask1) > 0);
        trunc_left = ((man & mask2) > 0);
    }
    return (trunc_high && (trunc_left || last_bit));
}
/**
 *@ingroup fp16_t static method
 *@param [in] sign      sign of the orginal value
 *@param [in] man       truncated mantissa
 *@param [in] trunc_len truncated bit length based on ten bits
 *@param [in] roundMode round mode
 *@brief   judge whether to add one to the result while converting fp16_t to other datatype
 *@return  Return true if add one, otherwise false
 */
static bool IsRoundOne(uint32_t sign, uint64_t man, uint16_t trunc_len, fp16RoundMode_t roundMode){
    uint64_t mask0 = 0x4;
    uint64_t mask1 = 0x2;
    uint64_t mask2 = 0x1;
    uint16_t shift_out = trunc_len - 2;
    mask0 = mask0 << shift_out;
    mask1 = mask1 << shift_out;
    mask2 = mask1 - 1;
    uint64_t mask12 = mask0 - 1;

    bool last_bit = ((man & mask0) > 0);
    bool trunc_high = 0;
    bool trunc_left = 0;
    switch (roundMode)
    {
    case ROUND_TO_NEAREST:
        trunc_high = ((man & mask1) > 0);
        trunc_left = ((man & mask2) > 0);
        return (trunc_high && (trunc_left || last_bit));
    case ROUND_TO_CEILING:
        return ((sign == 0) && ((man&mask12) > 0));//+ and (0,1)
    case ROUND_TO_FLOOR:
        return ((sign == 1) && ((man&mask12) > 0));//- and (0,1)
    case ROUND_BY_TRUNCATED:
        return false;
    }
    return false;
}
/**
 *@ingroup fp16_t static method
 *@param [in] fpVal uint16_t value of fp16_t object
 *@brief   Get float mantissa of uint16_t value of fp16_t object
 *@return  Return float mantissa of fpVal which should be in [ 0, 1 )
 */
static float fp16GetFMantissa(uint16_t fpVal){
    float ret = 0.0f, weight;
    uint16_t mantissa, bitmask;

    mantissa = (fpVal & FP16_MAN_MASK);
    for (bitmask = 0x200, weight = 0.5f; bitmask > 0; bitmask >>= 1, weight *= 0.5) {
        if (!(mantissa & bitmask))
            continue;
        ret += weight;
    }
    //assert(ret >= 0 && ret < 1);
    return ret;
}

float    fp16ToFloat(const uint16_t &fpVal){
    float ret;

    uint16_t hf_sign,hf_man;
    int16_t hf_exp;
    ExtractFP16(fpVal, &hf_sign, &hf_exp, &hf_man);

    while (hf_man&&!(hf_man&FP16_MAN_HIDE_BIT)){
        hf_man <<= 1;
        hf_exp--;
    }

    uint32_t s_ret, e_ret, m_ret, fVal;

    s_ret = hf_sign;
    if (!hf_man){
        e_ret = 0;
        m_ret = 0;
    }
    else{
        e_ret = hf_exp - FP16_EXP_BIAS + FP32_EXP_BIAS;
        m_ret = hf_man&FP16_MAN_MASK;
        m_ret = m_ret << (FP32_MAN_LEN - FP16_MAN_LEN);
    }
    fVal = FP32_CONSTRUCTOR(s_ret, e_ret, m_ret);
    ret = *((float *)&fVal);

    return ret;
}

double   fp16ToDouble(const uint16_t &fpVal){
    double ret;

    uint16_t hf_sign, hf_man;
    int16_t hf_exp;
    ExtractFP16(fpVal, &hf_sign, &hf_exp, &hf_man);

    while (hf_man&&!(hf_man&FP16_MAN_HIDE_BIT)){
        hf_man <<= 1;
        hf_exp--;
    }

    uint64_t s_ret, e_ret, m_ret, fVal;

    s_ret = hf_sign;
    if (!hf_man){
        e_ret = 0;
        m_ret = 0;
    }
    else{
        e_ret = hf_exp - FP16_EXP_BIAS + FP64_EXP_BIAS;
        m_ret = hf_man&FP16_MAN_MASK;
        m_ret = m_ret << (FP64_MAN_LEN - FP16_MAN_LEN);
    }
    fVal = (s_ret << FP64_SIGN_INDEX) | (e_ret << FP64_MAN_LEN) | (m_ret);
    ret = *((double *)&fVal);

    return ret;
}

int8_t   fp16ToInt8(const uint16_t &fpVal){
    int8_t ret;
    uint8_t ret_v;
    uint8_t s_ret, m_ret = 0;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal)){//Denormalized number
        ret_v = 0;
        ret = *((uint8_t *)&ret_v);
        return ret;
    }

    uint64_t long_int_m = hf_m;
    uint8_t overflow_flag = 0;
    uint16_t shift_out = 0;

    if (FP16_IS_INF(fpVal)){//Inf or NaN
        overflow_flag = 1;
    }
    else{
        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
                if (s_ret == 1 && long_int_m >= 0x20000u){//sign=1,negative number(<0)
                    long_int_m = 0x20000u;//10 0000 0000 0000 0000  10(fp16_t-man)+7(int8)=17bit
                    overflow_flag = 1;
                    break;
                }
                else if (s_ret != 1 && long_int_m >= 0x1FFFFu){//sign=0,positive number(>0)
                    long_int_m = 0x1FFFFu;//01 1111 1111 1111 1111  10(fp16_t-man)+7(int8)=17bit
                    overflow_flag = 1;
                    break;
                }
            }
            else{
                hf_e++;
                shift_out++;
            }
        }
    }
    if (overflow_flag){
        ret_v = INT8_T_MAX + s_ret;
    }
    else{
        bool needRound = IsRoundOne(long_int_m, shift_out + FP16_MAN_LEN);
        m_ret = (uint8_t)((long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN8_MAX);
        if (needRound && ((s_ret == 0 && m_ret < INT8_T_MAX) || (s_ret == 1 && m_ret <= INT8_T_MAX))){
            m_ret++;
        }
        if (s_ret){
            m_ret = (~m_ret) + 1;
        }
        if (m_ret == 0){
            s_ret = 0;
        }
        //Generate final result
        ret_v = (s_ret << 7) | (m_ret);
    }

    ret = *((uint8_t *)&ret_v);
    return ret;
}

uint8_t  fp16ToUInt8(const uint16_t &fpVal){
    uint8_t ret;
    uint8_t s_ret, m_ret = 0;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal)){//Denormalized number
        return 0;
    }

    if (FP16_IS_INF(fpVal)){
        m_ret = ~0;
    }
    else{
        uint64_t long_int_m = hf_m;
        uint8_t overflow_flag = 0;

        uint16_t shift_out = 0;

        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
                if (long_int_m >= 0x40000Lu){//overflow 0100 0000 0000 0000 0000
                    long_int_m = 0x3FFFFLu;//             11 1111 1111 1111 1111   10(fp16_t-man)+8(uint8)=18bit
                    overflow_flag = 1;
                    m_ret = ~0;
                    break;
                }
            }
            else{
                hf_e++;
                shift_out++;
            }
        }
        if (!overflow_flag){
            bool needRound = IsRoundOne(long_int_m, shift_out + FP16_MAN_LEN);
            m_ret = (uint8_t)((long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN8_MAX);
            if (needRound && m_ret != BIT_LEN8_MAX){
                m_ret++;
            }
        }
    }

    if (s_ret == 1){//Negative number
        m_ret = 0;
    }
    //Generate final result
    ret = m_ret;

    return ret;
}

int16_t  fp16ToInt16(const uint16_t &fpVal){
    int16_t ret;
    uint16_t ret_v;
    uint16_t s_ret, m_ret = 0;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal)){//Denormalized number
        ret_v = 0;
        ret = *((uint8_t *)&ret_v);
        return ret;
    }

    uint64_t long_int_m = hf_m;
    uint8_t overflow_flag = 0;
    uint16_t shift_out = 0;

    if (FP16_IS_INF(fpVal)){//Inf or NaN
        overflow_flag = 1;
    }
    else{
        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
                if (s_ret == 1 && long_int_m > 0x2000000Lu){//sign=1,negative number(<0)
                    long_int_m = 0x2000000Lu;//10 0000 0000 0000 0000 0000 0000  10(fp16_t-man)+15(int16)=25bit
                    overflow_flag = 1;
                    break;
                }
                else if (s_ret != 1 && long_int_m >= 0x1FFFFFFLu){//sign=0,positive number(>0) Overflow 
                    long_int_m = 0x1FFFFFFLu;//01 1111 1111 1111 1111 1111 1111   10(fp16_t-man)+15(int16)=25bit
                    overflow_flag = 1;
                    break;
                }
            }
            else{
                hf_e++;
                shift_out++;
            }
        }

    }
    if (overflow_flag){
        ret_v = INT16_T_MAX + s_ret;
    }
    else {
        bool needRound = IsRoundOne(long_int_m, shift_out + FP16_MAN_LEN);
        m_ret = (uint16_t)((long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN16_MAX);
        if (needRound && m_ret < INT16_T_MAX){
            m_ret++;
        }
        if (s_ret){
            m_ret = (~m_ret) + 1;
        }
        if (m_ret == 0){
            s_ret = 0;
        }
        //Generate final result
        ret_v = (s_ret << 15) | (m_ret);
    }

    ret = *((int16_t *)&ret_v);
    return ret;
}

uint16_t fp16ToUInt16(const uint16_t &fpVal){
    uint16_t ret;
    uint16_t s_ret, m_ret = 0;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal)){//Denormalized number
        return 0;
    }

    if (FP16_IS_INF(fpVal)){//Inf or NaN
        m_ret = ~0;
    }
    else{
        uint64_t long_int_m = hf_m;
        uint8_t overflow_flag = 0;
        uint16_t shift_out = 0;

        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
                if (long_int_m >= 0x4000000Lu){//overflow 100 0000 0000 0000 0000 0000 0000
                    long_int_m = 0x3FFFFFFLu;//               11 1111 1111 1111 1111 1111 1111  10(fp-16-man)+16(uint16)=26bit
                    overflow_flag = 1;
                    m_ret = BIT_LEN16_MAX;
                    break;
                }
            }
            else{
                hf_e++;
                shift_out++;
            }
        }
        if (!overflow_flag){
            bool needRound = IsRoundOne(long_int_m, shift_out + FP16_MAN_LEN);
            m_ret = (uint16_t)((long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN16_MAX);
            if (needRound && m_ret != BIT_LEN16_MAX){
                m_ret++;
            }
        }
    }

    if (s_ret == 1){//Negative number
        m_ret = 0;
    }
    //Generate final result
    ret = m_ret;

    return ret;
}

int32_t  fp16ToInt32(const uint16_t &fpVal, fp16RoundMode_t roundMode){
    int32_t ret;
    uint32_t ret_v;
    uint32_t s_ret, m_ret;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_INF(fpVal)){//Inf or NaN
        ret_v = INT32_T_MAX + s_ret;
    }
    else{
        uint64_t long_int_m = hf_m;
        uint16_t shift_out = 0;

        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
            }
            else{
                hf_e++;
                shift_out++;
            }
        }
        bool needRound = IsRoundOne(s_ret, long_int_m, shift_out + FP16_MAN_LEN, roundMode);
        m_ret = (uint32_t)((long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN32_MAX);
        if (needRound&& m_ret < INT32_T_MAX){
            m_ret++;
        }
        if (s_ret == 1){
            m_ret = (~m_ret) + 1;
        }
        if (m_ret == 0){
            s_ret = 0;
        }
        //Generate final result
        ret_v = (s_ret << 31) | (m_ret);
    }

    ret = *((int32_t *)&ret_v);
    return ret;
}

uint32_t fp16ToUInt32(const uint16_t &fpVal){
    uint32_t ret;
    uint32_t s_ret, m_ret;
    uint16_t hf_e, hf_m;

    //1.get s_ret and shift it to bit0.
    s_ret = FP16_EXTRAC_SIGN(fpVal);
    //2.get hf_e and hf_m
    hf_e = FP16_EXTRAC_EXP(fpVal);
    hf_m = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal)){//Denormalized number
        return 0u;
    }

    if (FP16_IS_INF(fpVal)){//Inf or NaN
        m_ret = ~0u;
    }
    else{
        uint64_t long_int_m = hf_m;
        uint16_t shift_out = 0;

        while (hf_e != FP16_EXP_BIAS){
            if (hf_e > FP16_EXP_BIAS){
                hf_e--;
                long_int_m = long_int_m << 1;
            }
            else{
                hf_e++;
                shift_out++;
            }
        }
        bool needRound = IsRoundOne(long_int_m, shift_out + FP16_MAN_LEN);
        m_ret = (uint32_t)(long_int_m >> (FP16_MAN_LEN + shift_out))&BIT_LEN32_MAX;
        if (needRound && m_ret != BIT_LEN32_MAX){
            m_ret++;
        }
    }

    if (s_ret == 1){//Negative number
        m_ret = 0;
    }
    //Generate final result
    ret = m_ret;

    return ret;
}

fp16_t deq(const uint32_t &fixVal, fp16_t fpScale){
    uint16_t s_x;
    uint32_t inte, dcml;
    /***************signed fixed-point number**************/
    /******bit[31]   :sign*********************************/
    /******bit[30:17]:integer******************************/
    /******bit[16:0] :decimal******************************/
    /******************************************************/
    s_x = (uint16_t)((fixVal&FP32_SIGN_MASK) >> FP32_SIGN_INDEX);
    inte = (fixVal & 0x7FFE0000) >> 17;
    dcml = (fixVal & 0x1FFFF) << 15;

    int16_t e_x = FP16_EXP_BIAS;
    uint32_t bit_mask = 0x80000000;
    while (inte > 1){
        dcml >>= 1;
        if (inte & 1){
            dcml |= bit_mask;
        }
        inte >>= 1;
        e_x++;
    }
    while ((!inte) && (e_x > 0)){
        if (bit_mask&dcml){
            inte |= 1;
        }
        dcml <<= 1;
        e_x--;
    }

    uint16_t m_x = (dcml >> 22 | (FP16_MAN_HIDE_BIT*(e_x > 0)));//32-10
    uint32_t m_trunc = dcml & 0x3FFFFF;//22 bits 1
    bool needRound = IsRoundOne(dcml, 22);
    if (needRound){
        m_x++;
    }

    fp16_t fpX(FP16_CONSTRUCTOR(s_x, e_x, m_x));

    fp16_t fpMul = fpX*fpScale;
    uint16_t s_m, m_m;
    int16_t e_m;
    ExtractFP16(fpMul.val, &s_m, &e_m, &m_m);
    e_m += 17;
    if (e_m >= FP16_MAX_EXP){
        e_m = FP16_MAX_EXP - 1;
        m_m = FP16_MAX_MAN;
    }
    fp16_t ret(FP16_CONSTRUCTOR(s_m, e_m, m_m));
    return ret;
}

/**
 *@ingroup fp16_t math operator
 *@param [in] v1 left operator value of fp16_t object
 *@param [in] v2 right operator value of fp16_t object
 *@brief   Performing fp16_t addition
 *@return  Return fp16_t result of adding this and fp
 */
 static uint16_t fp16Add(uint16_t v1, uint16_t v2){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(v1) || FP16_IS_INVALID(v2)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    uint16_t ret;
    uint16_t s_a, s_b;
    int16_t e_a, e_b;
    uint32_t m_a, m_b;
    uint16_t s_ret, m_ret;
    int16_t e_ret;
    uint16_t ma_tmp, mb_tmp;

    uint16_t shift_out = 0;
    //1.Extract
    ExtractFP16(v1, &s_a, &e_a, &ma_tmp);
    ExtractFP16(v2, &s_b, &e_b, &mb_tmp);
    m_a = ma_tmp;
    m_b = mb_tmp;

    uint16_t sum;
    if (s_a != s_b){
        ReverseMan(s_a > 0, &m_a);
        ReverseMan(s_b > 0, &m_b);
        sum = (uint16_t)GetManSum(e_a, m_a, e_b, m_b);
        s_ret = (sum&FP16_SIGN_MASK) >> FP16_SIGN_INDEX;
        ReverseMan(s_ret > 0, &m_a);
        ReverseMan(s_ret > 0, &m_b);
    }
    else{
        sum = (uint16_t)GetManSum(e_a, m_a, e_b, m_b);
        s_ret = s_a;
    }

    if (sum == 0){
        shift_out = 3;
        m_a = m_a << shift_out;
        m_b = m_b << shift_out;
    }

    uint32_t m_trunc = 0;
    int16_t e_tmp = 0;

    e_ret = std::max(e_a, e_b);
    e_tmp = std::abs(e_a - e_b);
    if (e_a > e_b){
        m_trunc = (m_b << (32 - e_tmp));
        m_b = RightShift(m_b, e_tmp);
    }
    else if (e_a < e_b){
        m_trunc = (m_a << (32 - e_tmp));
        m_a = RightShift(m_a, e_tmp);
    }
    //calculate mantissa
    m_ret = (uint16_t)(m_a + m_b);

    //if (m_ret){
    uint16_t m_min = FP16_MAN_HIDE_BIT << shift_out;
    uint16_t m_max = m_min << 1;
    //Denormal
    while (m_ret<m_min && e_ret>0){//the value of m_ret should not be smaller than 2^23
        m_ret = m_ret << 1;
        m_ret += (FP32_SIGN_MASK&m_trunc) >> FP32_SIGN_INDEX;
        m_trunc = m_trunc << 1;
        e_ret = e_ret - 1;
    }
    while (m_ret >= m_max){//the value of m_ret should be smaller than 2^24
        m_trunc = m_trunc >> 1;
        m_trunc = m_trunc | (FP32_SIGN_MASK*(m_ret & 1));
        m_ret = m_ret >> 1;
        e_ret = e_ret + 1;
    }

    bool b_last_bit = ((m_ret & 1) > 0);
    bool b_trunc_high = 0;
    bool b_trunc_left = 0;
    b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP32_SIGN_MASK) > 0);
    b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP32_ABS_MAX) > 0);
    m_ret = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_ret, shift_out);
    while (m_ret >= m_max){
        m_ret = m_ret >> 1;
        e_ret = e_ret + 1;
    }
    if (e_ret >= FP16_MAX_EXP){
        e_ret = FP16_MAX_EXP-1;
        m_ret = FP16_MAX_MAN;
    } else if (e_ret == 0 && m_ret <= m_max){
        m_ret = m_ret >> 1;
    }
    ret = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    return ret;
}
/**
    *@ingroup fp16_t math operator
    *@param [in] v1 left operator value of fp16_t object
    *@param [in] v2 right operator value of fp16_t object
    *@brief   Performing fp16_t subtraction
    *@return  Return fp16_t result of subtraction fp from this
    */
static uint16_t fp16Sub(uint16_t v1, uint16_t v2){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(v1) || FP16_IS_INVALID(v2)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    uint16_t ret;
    uint16_t tmp;

    //Reverse
    tmp = ((~(v2))&FP16_SIGN_MASK) | (v2&FP16_ABS_MAX);

    ret = fp16Add(v1, tmp);

    return ret;
}
/**
    *@ingroup fp16_t math operator
    *@param [in] v1 left operator value of fp16_t object
    *@param [in] v2 right operator value of fp16_t object
    *@brief   Performing fp16_t multiplication
    *@return  Return fp16_t result of multiplying this and fp
    */
static uint16_t fp16Mul(uint16_t v1, uint16_t v2){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(v1) || FP16_IS_INVALID(v2)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    uint16_t s_a, s_b;
    int16_t e_a, e_b;
    uint32_t m_a, m_b;
    uint16_t s_ret, m_ret;
    int16_t e_ret;
    uint32_t mul_m;
    uint16_t right_shift = 0;
    uint16_t ma_tmp, mb_tmp;
    //1.Extract
    ExtractFP16(v1, &s_a, &e_a, &ma_tmp);
    ExtractFP16(v2, &s_b, &e_b, &mb_tmp);
    m_a = ma_tmp;
    m_b = mb_tmp;

    e_ret = e_a + e_b - FP16_EXP_BIAS - 10;
    mul_m = m_a*m_b;
    s_ret = s_a^s_b;

    uint32_t m_min = FP16_MAN_HIDE_BIT;
    uint32_t m_max = m_min << 1;
    uint32_t m_trunc = 0;
    //the value of m_ret should not be smaller than 2^23
    while (mul_m<m_min&&e_ret>1){
        mul_m = mul_m << 1;
        e_ret = e_ret - 1;
    }
    while (mul_m >= m_max || e_ret < 1){
        m_trunc = m_trunc >> 1;
        m_trunc = m_trunc | (FP32_SIGN_MASK*(mul_m & 1));
        mul_m = mul_m >> 1;
        e_ret = e_ret + 1;
    }
    bool b_last_bit = ((mul_m & 1) > 0);
    bool b_trunc_high = 0;
    bool b_trunc_left = 0;
    b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP32_SIGN_MASK) > 0);
    b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP32_ABS_MAX) > 0);
    mul_m = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, mul_m);

    while (mul_m >= m_max || e_ret < 0){
        mul_m = mul_m >> 1;
        e_ret = e_ret + 1;
    }
    if (e_ret >= FP16_MAX_EXP){
        e_ret = FP16_MAX_EXP - 1;
        mul_m = FP16_MAX_MAN;
    }
    if (e_ret == 1 && mul_m < FP16_MAN_HIDE_BIT){
        e_ret = 0;
    }
    m_ret = (uint16_t)mul_m;
    uint16_t ret = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    return ret;
}
/**
    *@ingroup fp16_t math operator divided
    *@param [in] v1 left operator value of fp16_t object
    *@param [in] v2 right operator value of fp16_t object
    *@brief   Performing fp16_t division
    *@return  Return fp16_t result of division this by fp
    */
static uint16_t fp16Div(uint16_t v1, uint16_t v2){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(v1) || FP16_IS_INVALID(v2)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    uint16_t ret;
    if (FP16_IS_ZERO(v2)){//result is inf
        //throw "fp16_t division by zero.";
        uint16_t s_a, s_b;
        uint16_t s_ret;
        s_a = FP16_EXTRAC_SIGN(v1);
        s_b = FP16_EXTRAC_SIGN(v2);
        s_ret = s_a^s_b;
        ret = FP16_CONSTRUCTOR(s_ret, FP16_MAX_EXP-1, FP16_MAX_MAN);
    }
    else if (FP16_IS_ZERO(v1)){
        ret = 0u;
    }
    else{
        uint16_t s_a, s_b;
        int16_t e_a, e_b;
        uint64_t m_a, m_b;
        float m_div;
        uint16_t ma_tmp, mb_tmp;
        //1.Extract
        ExtractFP16(v1, &s_a, &e_a, &ma_tmp);
        ExtractFP16(v2, &s_b, &e_b, &mb_tmp);
        m_a = ma_tmp;
        m_b = mb_tmp;

        uint64_t m_tmp;
        if (e_a > e_b){
            m_tmp = m_a;
            uint16_t tmp;
            tmp = e_a - e_b;
            for (int i = 0; i < tmp; i++){
                m_tmp = m_tmp << 1;
                //e_a = e_a - 1;
            }
            m_a = m_tmp;
        }
        else if (e_a < e_b){
            m_tmp = m_b;
            uint16_t tmp = e_b - e_a;
            for (int i = 0; i < tmp; i++){
                m_tmp = m_tmp << 1;
                //e_b = e_b - 1;
            }
            m_b = m_tmp;
        }
        m_div = (float)(m_a*1.0f / m_b);
        fp16_t fp_div;
        fp_div = m_div;
        ret = fp_div.val;
        if (s_a != s_b){
            ret |= FP16_SIGN_MASK;
        }
    }
    return ret;
}

//operate
fp16_t fp16_t::operator+(const fp16_t fp){
    uint16_t retVal = fp16Add(val, fp.val);
    fp16_t ret(retVal);
    return ret;
}
fp16_t fp16_t::operator-(const fp16_t fp){
    uint16_t retVal = fp16Sub(val, fp.val);
    fp16_t ret(retVal);
    return ret;
}
fp16_t fp16_t::operator*(const fp16_t fp){
    uint16_t retVal = fp16Mul(val, fp.val);
    fp16_t ret(retVal);
    return ret;
}
fp16_t fp16_t::operator/(const fp16_t fp){
    uint16_t retVal = fp16Div(val, fp.val);
    fp16_t ret(retVal);
    return ret;
}

fp16_t fp16_t::operator+=(const fp16_t fp){
    val = fp16Add(val, fp.val);
    return *this;
}
fp16_t fp16_t::operator-=(const fp16_t fp){
    val = fp16Sub(val, fp.val);
    return *this;
}
fp16_t fp16_t::operator*=(const fp16_t fp){
    val = fp16Mul(val, fp.val);
    return *this;
}
fp16_t fp16_t::operator/=(const fp16_t fp){
    val = fp16Div(val, fp.val);
    return *this;
}

//compare
bool fp16_t::operator==(const fp16_t &fp) const {
    bool result;
    if (FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val)){
        result = true;
    }
    else{
        result = ((val & BIT_LEN16_MAX) == (fp.val & BIT_LEN16_MAX));//bit compare
    }
    return result;
}
bool fp16_t::operator!=(const fp16_t &fp) const {
    bool result;
    if (FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val)){
        result = false;
    }
    else{
        result = ((val & BIT_LEN16_MAX) != (fp.val & BIT_LEN16_MAX));//bit compare
    }
    return result;
}
bool fp16_t::operator> (const fp16_t &fp) const {
    uint16_t s_a, s_b;
    uint16_t e_a, e_b;
    uint16_t m_a, m_b;
    bool result;

    //1.Extract
    s_a = FP16_EXTRAC_SIGN(val);
    s_b = FP16_EXTRAC_SIGN(fp.val);
    e_a = FP16_EXTRAC_EXP(val);
    e_b = FP16_EXTRAC_EXP(fp.val);
    m_a = FP16_EXTRAC_MAN(val);
    m_b = FP16_EXTRAC_MAN(fp.val);

    //Compare
    if ((s_a == 0) && (s_b > 0)){  //+  -
        //-0=0
        result = !(FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val));
    }
    else if ((s_a == 0) && (s_b == 0)){        // + +
        if (e_a > e_b){        //e_a - e_b >= 1; Va always larger than Vb
            result = true;
        }
        else if (e_a == e_b){
            result = m_a > m_b;
        }
        else{
            result = false;
        }
    }
    else if ((s_a > 0) && (s_b > 0)){        // - -    opposite to  + +
        if (e_a < e_b){
            result = true;
        }
        else if (e_a == e_b){
            result = m_a < m_b;
        }
        else{
            result = false;
        }
    }
    else{        //-  +
        result = false;
    }

    return result;
}
bool fp16_t::operator>=(const fp16_t &fp) const {
    bool result;
    if ((*this) > fp){
        result = true;
    }
    else if ((*this) == fp){
        result = true;
    }
    else{
        result = false;
    }

    return result;
}
bool fp16_t::operator< (const fp16_t &fp) const {
    bool result;
    if ((*this) >= fp){
        result = false;
    }
    else{
        result = true;
    }

    return result;
}
bool fp16_t::operator<=(const fp16_t &fp) const {
    bool result;
    if ((*this) > fp){
        result = false;
    }
    else{
        result = true;
    }

    return result;
}


//evaluation
fp16_t& fp16_t::operator=(const fp16_t   &fp){
    if (this == &fp){
        return *this;
    }
    val = fp.val;
    return *this;
}
fp16_t& fp16_t::operator=(const float    &fVal){
    uint16_t s_ret, e_ret, m_ret;
    uint32_t e_f, m_f;
    uint32_t ui32_v = *((uint32_t *)&fVal);//1:8:23bit sign:exp:man
    uint32_t m_len_delta;

    s_ret = (uint16_t)((ui32_v & FP32_SIGN_MASK) >> FP32_SIGN_INDEX);//4Byte->2Byte
    e_f = (ui32_v & FP32_EXP_MASK) >> FP32_MAN_LEN;//8 bit exponent
    m_f = (ui32_v & FP32_MAN_MASK);//23 bit mantissa dont't need to care about denormal
    m_len_delta = FP32_MAN_LEN - FP16_MAN_LEN;

    bool needRound = false;
    // Exponent overflow/NaN converts to signed inf/NaN
    if (e_f > 0x8Fu){//0x8Fu:142=127+15
        e_ret = FP16_MAX_EXP-1;
        m_ret = FP16_MAX_MAN;
    }
    else if (e_f <= 0x70u){//0x70u:112=127-15 Exponent underflow converts to denormalized half or signed zero
        e_ret = 0;
        if (e_f >= 0x67){//0x67:103=127-24 Denormal
            m_f = (m_f | FP32_MAN_HIDE_BIT);
            uint16_t shift_out = FP32_MAN_LEN;
            uint64_t m_tmp = ((uint64_t)m_f) << (e_f - 0x67);

            needRound = IsRoundOne(m_tmp, shift_out);
            m_ret = (uint16_t)(m_tmp >> shift_out);
            if (needRound){
                m_ret++;
            }
        }
        else if (e_f == 0x66 && m_f > 0){//0x66:102 Denormal 0<f_v<min(Denormal)
            m_ret = 1;
        }
        else{
            m_ret = 0;
        }
    }
    else{// Regular case with no overflow or underflow
        e_ret = (uint16_t)(e_f - 0x70u);

        needRound = IsRoundOne(m_f, m_len_delta);
        m_ret = (uint16_t)(m_f >> m_len_delta);
        if (needRound){
            m_ret++;
        }
        if (m_ret & FP16_MAN_HIDE_BIT){
            e_ret++;
        }
    }
    
    val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    if (FP16_IS_INVALID(val)){
        val = (s_ret << FP16_SIGN_INDEX) | FP16_MAX;
    }
    return *this;
}
fp16_t& fp16_t::operator=(const int8_t   &iVal){
    uint16_t s_ret, e_ret, m_ret;

    s_ret = (uint16_t)((iVal & 0x80) >> 7);
    m_ret = (uint16_t)((iVal&INT8_T_MAX));

    if (m_ret == 0){
        e_ret = 0;
    }
    else{
        if (s_ret){//negative number(<0)
            m_ret = (uint16_t)std::abs(iVal);//m_ret = ~(m_ret - 1);//complement 
        }

        e_ret = FP16_MAN_LEN;
        while ((m_ret&FP16_MAN_HIDE_BIT) == 0){
            m_ret = m_ret << 1;
            e_ret = e_ret - 1;
        }
        e_ret = e_ret + FP16_EXP_BIAS;
    }

    val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    return *this;
}
fp16_t& fp16_t::operator=(const uint8_t  &uiVal){
    uint16_t s_ret, e_ret, m_ret;
    s_ret = 0;
    e_ret = 0;
    m_ret = uiVal;
    if (m_ret){
        e_ret = FP16_MAN_LEN;
        while ((m_ret&FP16_MAN_HIDE_BIT) == 0){
            m_ret = m_ret << 1;
            e_ret = e_ret - 1;
        }
        e_ret = e_ret + FP16_EXP_BIAS;
    }

    val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    return *this;
}
fp16_t& fp16_t::operator=(const int16_t  &iVal){
    if (iVal == 0){
        val = 0;
    }
    else{
        uint16_t s_ret;
        uint16_t uiVal = *((uint16_t *)&iVal);
        s_ret = (uint16_t)(uiVal >> 15);
        if (s_ret){
            int16_t iVal_m = -iVal;
            uiVal = *((uint16_t *)&iVal_m);
        }
        uint32_t m_tmp = (uiVal&FP32_ABS_MAX);

        uint16_t m_min = FP16_MAN_HIDE_BIT;
        uint16_t m_max = m_min << 1;
        int16_t len = GetManBitLength(m_tmp);
        if (m_tmp){
            int16_t e_ret;

            if (len > 11){
                e_ret = FP16_EXP_BIAS + FP16_MAN_LEN;
                uint16_t e_tmp = len - 11;
                uint32_t trunc_mask = 1;
                for (int i = 1; i < e_tmp; i++){
                    trunc_mask = (trunc_mask << 1) + 1;
                }
                uint32_t m_trunc = (m_tmp&trunc_mask) << (32 - e_tmp);
                for (int i = 0; i < e_tmp; i++){
                    m_tmp = (m_tmp >> 1);
                    e_ret = e_ret + 1;
                }
                bool b_last_bit = ((m_tmp & 1) > 0);
                bool b_trunc_high = 0;
                bool b_trunc_left = 0;
                if (ROUND_TO_NEAREST == g_RoundMode){//trunc
                    b_trunc_high = ((m_trunc&FP32_SIGN_MASK) > 0);
                    b_trunc_left = ((m_trunc&FP32_ABS_MAX) > 0);
                }
                m_tmp = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_tmp);
                while (m_tmp >= m_max || e_ret < 0){
                    m_tmp = m_tmp >> 1;
                    e_ret = e_ret + 1;
                }
                /*if (e_ret>FP16_MAX_EXP){
                e_ret = FP16_MAX_EXP;
                m_tmp = FP16_MAX_MAN;
                }*/
            }
            else{
                e_ret = FP16_EXP_BIAS;
                m_tmp = m_tmp << (11 - len);
                e_ret = e_ret + (len - 1);
            }
            uint16_t m_ret = (uint16_t)m_tmp;
            val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
        }
        else{

        }
    }
    return *this;
}
fp16_t& fp16_t::operator=(const uint16_t &uiVal){
    if (uiVal == 0){
        val = 0;
    }
    else{
        int16_t e_ret;
        uint16_t m_ret = uiVal;

        uint16_t m_min = FP16_MAN_HIDE_BIT;
        uint16_t m_max = m_min << 1;
        int16_t len = GetManBitLength(m_ret);

        if (len > 11){
            e_ret = FP16_EXP_BIAS + FP16_MAN_LEN;
            uint32_t m_trunc;
            uint32_t trunc_mask = 1;
            uint16_t e_tmp = len - 11;
            for (int i = 1; i < e_tmp; i++){
                trunc_mask = (trunc_mask << 1) + 1;
            }
            m_trunc = (m_ret&trunc_mask) << (32 - e_tmp);
            for (int i = 0; i < e_tmp; i++){
                m_ret = (m_ret >> 1);
                e_ret = e_ret + 1;
            }
            bool b_last_bit = ((m_ret & 1) > 0);
            bool b_trunc_high = 0;
            bool b_trunc_left = 0;
            if (ROUND_TO_NEAREST == g_RoundMode){//trunc
                b_trunc_high = ((m_trunc&FP32_SIGN_MASK) > 0);
                b_trunc_left = ((m_trunc&FP32_ABS_MAX) > 0);
            }
            m_ret = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_ret);
            while (m_ret >= m_max || e_ret < 0){
                m_ret = m_ret >> 1;
                e_ret = e_ret + 1;
            }
            if (FP16_IS_INVALID(val)){
                val = FP16_MAX;
            }
        }
        else{
            e_ret = FP16_EXP_BIAS;
            m_ret = m_ret << (11 - len);
            e_ret = e_ret + (len - 1);
        }
        val = FP16_CONSTRUCTOR(0u, e_ret, m_ret);
    }
    return *this;
}
fp16_t& fp16_t::operator=(const int32_t  &iVal){
    if (iVal == 0){
        val = 0;
    }
    else{
        uint32_t uiVal = *((uint32_t *)&iVal);
        uint16_t s_ret = (uint16_t)(uiVal >> 31);
        if (s_ret){
            int32_t iVal_m = -iVal;
            uiVal = *((uint32_t *)&iVal_m);
        }
        int16_t e_ret;
        uint32_t m_tmp = (uiVal&FP32_ABS_MAX);

        uint32_t m_min = FP16_MAN_HIDE_BIT;
        uint32_t m_max = m_min << 1;
        int16_t len = GetManBitLength(m_tmp);

        if (len > 11){
            e_ret = FP16_EXP_BIAS + FP16_MAN_LEN;
            uint32_t m_trunc = 0;
            uint32_t trunc_mask = 1;
            uint16_t e_tmp = len - 11;
            for (int i = 1; i < e_tmp; i++){
                trunc_mask = (trunc_mask << 1) + 1;
            }
            m_trunc = (m_tmp&trunc_mask) << (32 - e_tmp);
            for (int i = 0; i < e_tmp; i++){
                m_tmp = (m_tmp >> 1);
                e_ret = e_ret + 1;
            }
            bool b_last_bit = ((m_tmp & 1) > 0);
            bool b_trunc_high = 0;
            bool b_trunc_left = 0;
            if (ROUND_TO_NEAREST == g_RoundMode){//trunc
                b_trunc_high = ((m_trunc&FP32_SIGN_MASK) > 0);
                b_trunc_left = ((m_trunc&FP32_ABS_MAX) > 0);
            }
            m_tmp = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_tmp);
            while (m_tmp >= m_max || e_ret < 0){
                m_tmp = m_tmp >> 1;
                e_ret = e_ret + 1;
            }
            if (e_ret>=FP16_MAX_EXP){
                e_ret = FP16_MAX_EXP-1;
                m_tmp = FP16_MAX_MAN;
            }
        }
        else{
            e_ret = FP16_EXP_BIAS;
            m_tmp = m_tmp << (11 - len);
            e_ret = e_ret + (len - 1);
        }
        uint16_t m_ret = (uint16_t)m_tmp;
        val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    }
    return *this;
}
fp16_t& fp16_t::operator=(const uint32_t &uiVal){
    if (uiVal == 0){
        val = 0;
    }
    else{
        int16_t e_ret;
        uint32_t m_tmp = uiVal;

        uint32_t m_min = FP16_MAN_HIDE_BIT;
        uint32_t m_max = m_min << 1;
        int16_t len = GetManBitLength(m_tmp);

        if (len > 11){
            e_ret = FP16_EXP_BIAS + FP16_MAN_LEN;
            uint32_t m_trunc = 0;
            uint32_t trunc_mask = 1;
            uint16_t e_tmp = len - 11;
            for (int i = 1; i < e_tmp; i++){
                trunc_mask = (trunc_mask << 1) + 1;
            }
            m_trunc = (m_tmp&trunc_mask) << (32 - e_tmp);
            for (int i = 0; i < e_tmp; i++){
                m_tmp = (m_tmp >> 1);
                e_ret = e_ret + 1;
            }
            bool b_last_bit = ((m_tmp & 1) > 0);
            bool b_trunc_high = 0;
            bool b_trunc_left = 0;
            if (ROUND_TO_NEAREST == g_RoundMode){//trunc
                b_trunc_high = ((m_trunc&FP32_SIGN_MASK) > 0);
                b_trunc_left = ((m_trunc&FP32_ABS_MAX) > 0);
            }
            m_tmp = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_tmp);
            while (m_tmp >= m_max || e_ret < 0){
                m_tmp = m_tmp >> 1;
                e_ret = e_ret + 1;
            }
            if (e_ret>=FP16_MAX_EXP){
                e_ret = FP16_MAX_EXP-1;
                m_tmp = FP16_MAX_MAN;
            }
        }
        else{
            e_ret = FP16_EXP_BIAS;
            m_tmp = m_tmp << (11 - len);
            e_ret = e_ret + (len - 1);
        }
        uint16_t m_ret = (uint16_t)m_tmp;
        val = FP16_CONSTRUCTOR(0u, e_ret, m_ret);
    }
    return *this;
}
fp16_t& fp16_t::operator=(const double   &dVal){
    uint16_t s_ret, e_ret, m_ret;
    uint64_t e_d, m_d;
    uint64_t ui64_v = *((uint64_t *)&dVal);//1:11:52bit sign:exp:man
    uint32_t m_len_delta;

    s_ret = (uint16_t)((ui64_v & FP64_SIGN_MASK) >> FP64_SIGN_INDEX);//4Byte
    e_d = (ui64_v & FP64_EXP_MASK) >> FP64_MAN_LEN;//10 bit exponent
    m_d = (ui64_v & FP64_MAN_MASK);//52 bit mantissa
    m_len_delta = FP64_MAN_LEN - FP16_MAN_LEN;

    bool needRound = false;
    // Exponent overflow/NaN converts to signed inf/NaN
    if (e_d >= 0x410u){//0x410:1040=1023+16
        e_ret = FP16_MAX_EXP - 1;
        m_ret = FP16_MAX_MAN;
        val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    }
    else if (e_d <= 0x3F0u){//Exponent underflow converts to denormalized half or signed zero
        //0x3F0:1008=1023-15
        /**
        * Signed zeros, denormalized floats, and floats with small
        * exponents all convert to signed zero half precision.
        */
        e_ret = 0;
        if (e_d >= 0x3E7u){//0x3E7u:999=1023-24 Denormal
            // Underflows to a denormalized value
            m_d = (FP64_MAN_HIDE_BIT | m_d);
            uint16_t shift_out = FP64_MAN_LEN;
            uint64_t m_tmp = ((uint64_t)m_d) << (e_d - 0x3E7u);

            needRound = IsRoundOne(m_tmp, shift_out);
            m_ret = (uint16_t)(m_tmp >> shift_out);
            if (needRound){
                m_ret++;
            }
        }
        else if (e_d == 0x3E6u && m_d > 0){
            m_ret = 1;
        }
        else{
            m_ret = 0;
        }
        //if (e_d < 0x3F0u){
        //    val = s_ret;
        //}
        //else{
        //    // Underflows to a denormalized value
        //    m_d = ((FP64_MAN_HIDE_BIT | m_d) >> 1);

        //    bool needRound = IsRoundOne(m_d, m_len_delta);
        //    m_ret = (uint16_t)(m_d >> m_len_delta);
        //    if (needRound){
        //        m_ret++;
        //    }
        //    val = (uint16_t)(s_ret + m_ret);
        //}
    }
    else{// Regular case with no overflow or underflow
        e_ret = (uint16_t)(e_d - 0x3F0u);

        needRound = IsRoundOne(m_d, m_len_delta);
        m_ret = (uint16_t)(m_d >> m_len_delta);
        if (needRound){
            m_ret++;
        }
        if (m_ret & FP16_MAN_HIDE_BIT){
            e_ret++;
        }

        //e_ret = (uint16_t)((e_d - 0x3F0u) << FP16_MAN_LEN);

        //bool needRound = IsRoundOne(m_d, m_len_delta);
        //m_ret = (uint16_t)(m_d >> m_len_delta);
        //if (needRound){
        //    m_ret++;
        //}
        //val = s_ret + e_ret + m_ret;
        //
    }

    val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    if (FP16_IS_INVALID(val)){
        val = (s_ret << FP16_SIGN_INDEX) | FP16_MAX;
    }
    return *this;
}

//convert
fp16_t::operator float() const{
    return fp16ToFloat(val);
}
fp16_t::operator double() const{
    return fp16ToDouble(val);
}
fp16_t::operator int8_t() const{
    return fp16ToInt8(val);
}
fp16_t::operator uint8_t() const{
    return fp16ToUInt8(val);
}
fp16_t::operator int16_t() const{
    return fp16ToInt16(val);
}
fp16_t::operator uint16_t() const{
    return fp16ToUInt16(val);
}
fp16_t::operator int32_t() const{
    return fp16ToInt32(val);
}
fp16_t::operator uint32_t() const{
    return fp16ToUInt32(val);
}
int fp16_t::IsInf(){
    if ((val & FP16_ABS_MAX) == FP16_EXP_MASK){
        if (val & FP16_SIGN_MASK){
            return -1;
        }
        else{
            return 1;
        }
    }
    else return 0;
}

float    fp16_t::toFloat(){
    return fp16ToFloat(val);
}
double   fp16_t::toDouble(){
    return fp16ToDouble(val);
}
int8_t   fp16_t::toInt8(){
    return fp16ToInt8(val);
}
uint8_t  fp16_t::toUInt8(){
    return fp16ToUInt8(val);
}
int16_t  fp16_t::toInt16(){
    return fp16ToInt16(val);
}
uint16_t fp16_t::toUInt16(){
    return fp16ToUInt16(val);
}
int32_t  fp16_t::toInt32(){
    return fp16ToInt32(val);
}
int32_t  fp16_t::toInt32C(){
    return fp16ToInt32(val,ROUND_TO_CEILING);
}
int32_t  fp16_t::toInt32F(){
    return fp16ToInt32(val,ROUND_TO_FLOOR);
}
uint32_t fp16_t::toUInt32(){
    return fp16ToUInt32(val);
}
