/**
 * @file fp16_unit.cc
 *
 * Copyright(C), 2017 - 2018, Lxzh Tech. Co., Ltd. ALL RIGHTS RESERVED.
 *
 * @brief Half precision float
 *
 * @version 1.0
 *
 */

#include "fp16_unit.h"

/**
 *@ingroup fp16_t global filed
 *@brief   round mode of last valid digital
 */
extern fp16RoundMode_t g_RoundMode;

/**
 *@ingroup fp16_math inner method
 *@param [in]     ea exponent of one fp16_t/float number
 *@param [in|out] ma mantissa of one fp16_t/float number
 *@param [in]     eb exponent of another fp16_t/float number
 *@param [in|out] mb mantissa of another fp16_t/float number
 *@brief   Align exponent by shift right mantissa whoes exponent is less than another one
 */
static void AlignManByExp(const int16_t &ea, uint64_t *ma, const int16_t &eb, uint64_t *mb){
    if (ea != eb){
        uint64_t* m_tmp;
        uint16_t e_tmp = std::abs(ea - eb);
        m_tmp = MinMan(ea, ma, eb, mb);
        *m_tmp = RightShift(*m_tmp, e_tmp);
    }
}

fp16_t hf_relu(fp16_t fp){
    fp16_t ret;
    uint16_t sign;
    sign = FP16_EXTRAC_SIGN(fp.val);
    if (sign){
        ret.val = 0;
    }
    else{
        ret.val = fp.val;
    }
    return ret;
}

fp16_t hf_mla(fp16_t fp1, fp16_t fp2, fp16_t fp3){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp1.val) || FP16_IS_INVALID(fp2.val) || FP16_IS_INVALID(fp3.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    fp16_t ret;

    uint16_t v1, v2, v3;
    v1 = fp1.val;
    v2 = fp2.val;
    v3 = fp3.val;
    if (FP16_IS_ZERO(v1) || FP16_IS_ZERO(v2)){
        ret = fp3;
    }
    else{
        uint16_t s1, s2, s3;
        int16_t e1, e2, e3;
        uint16_t m1, m2, m3_tmp;
        uint16_t s_mul, s_ret, m_ret;
        int16_t e_mul, e_ret;
        uint32_t m3, m_mul, m_add;
        //1.Extrac
        ExtractFP16(v1, &s1, &e1, &m1);
        ExtractFP16(v2, &s2, &e2, &m2);
        ExtractFP16(v3, &s3, &e3, &m3_tmp);
        m3 = m3_tmp;

        //2.Mul
        s_mul = s1^s2;
        e_mul = e1 + e2 - FP16_EXP_BIAS - FP16_MAN_LEN;
        m_mul = (uint32_t)m1*m2;

        //3.Adjust mantissa
        s_ret = (uint16_t)CalcAdditionSign(s_mul, e_mul, &m_mul, s3, e3, &m3);

        e_ret = std::max(e_mul, e3);
        uint16_t e_tmp = std::abs(e_mul - e3);
        uint64_t m_trunc = 0;
        if (e_mul != e3){
            uint32_t *m_tmp = MinMan(e_mul, &m_mul, e3, &m3);
            m_trunc = (((uint64_t)(*m_tmp)) << (64 - e_tmp));
            if (e_tmp <= 32){
                *m_tmp = RightShift(*m_tmp, e_tmp);
            }
            else{
                *m_tmp = 0;
            }
        }
        //4.Add
        //calculate mantissa
        m_add = m_mul + m3;

        uint32_t m_min = FP16_MAN_HIDE_BIT;
        uint32_t m_max = m_min << 1;
        //the value of m_ret should not be smaller than 2^23
        while (m_add<m_min && e_ret>1){
            m_add = m_add << 1;
            m_add += (uint32_t)(e_tmp > 0 && (FP64_SIGN_MASK&m_trunc));
            m_trunc = m_trunc << 1;
            e_ret = e_ret - 1;
        }
        while (m_add >= m_max){
            m_trunc = m_trunc >> 1;
            m_trunc = m_trunc | (FP64_SIGN_MASK*(m_add & 1));
            m_add = m_add >> 1;
            e_ret = e_ret + 1;
        }
        bool b_last_bit = ((m_add & 1) > 0);
        bool b_trunc_high = 0;
        bool b_trunc_left = 0;
        b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP64_SIGN_MASK) > 0);
        b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP64_ABS_MAX) > 0);
        m_add = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_add);

        while (m_add >= m_max){
            m_add = m_add >> 1;
            e_ret = e_ret + 1;
        }
        if (e_ret >= FP16_MAX_EXP){
            e_ret = FP16_MAX_EXP - 1;
            m_add = FP16_MAX_MAN;
        }
        if (e_ret == 1 && m_add < FP16_MAN_HIDE_BIT){
            e_ret = 0;
        }
        m_ret = (uint16_t)m_add;
        ret.val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
    }
    return ret;
}

float hf_mla(fp16_t fp1, fp16_t fp2, float fc){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp1.val) || FP16_IS_INVALID(fp2.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    float ret;
    uint16_t v1, v2;
    uint32_t v3;

    v1 = fp1.val;
    v2 = fp2.val;
    v3 = *((uint32_t *)&fc);
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP32_IS_NAN(v3) || FP32_IS_INF(v3)){
        std::cout << "input float value is overflow\n";
    }
#endif

    if (FP16_IS_ZERO(v1) || FP16_IS_ZERO(v2)){
        ret = fc;
    }
    else{
        uint16_t s1, s2;
        int16_t e1, e2, e3;
        uint16_t m1, m2;
        uint32_t s3;
        uint32_t s_mul, s_ret, m_ret;
        int16_t e_mul;
        int32_t e_ret;
        uint64_t m3, m_mul, m_add;

        //1.Extrac
        ExtractFP16(v1, &s1, &e1, &m1);
        ExtractFP16(v2, &s2, &e2, &m2);
        s3 = FP32_EXTRAC_SIGN(v3);//4Byte
        e3 = FP32_EXTRAC_EXP(v3); //8bits exponent
        m3 = (uint64_t)(FP32_EXTRAC_MAN(v3)); //23 bits mantissa  [Normal:1.m Denormal:0.m]

        e3 += (int16_t)(e3 == 0);//Denormal

        //2.Mul
        s_mul = s1^s2;
        e_mul = e1 + e2 - FP16_EXP_BIAS;
        m_mul = (uint64_t)m1*m2;

        //Align exponent to float
        e_mul = e_mul - FP16_MAN_LEN * 2 + FP32_MAN_LEN - FP16_EXP_BIAS + FP32_EXP_BIAS;

        //3.Add
        s_ret = (uint32_t)CalcAdditionSign(s_mul, e_mul, &m_mul, s3, e3, &m3);

        uint64_t m_trunc_r = 0;
        uint64_t m_trunc_l = 0;
        uint64_t trunc_bit_r = 1;
        uint64_t trunc_mask = 1;
        e_ret = std::max(e_mul, e3);
        uint16_t e_tmp = std::abs(e_mul - e3);
        if (e_mul != e3){
            uint64_t *m_tmp = MinMan(e_mul, &m_mul, e3, &m3);
            if (e_tmp <= 64){
                trunc_bit_r = 1LLu << (e_tmp - 1);
                trunc_mask = (trunc_bit_r << 1) - 1;
                m_trunc_r = (*m_tmp)&trunc_mask;
                m_trunc_l = ((*m_tmp) << (64 - e_tmp))&(FP64_SIGN_MASK | FP64_ABS_MAX);
            }
            else{
                trunc_bit_r = FP64_SIGN_MASK;
                m_trunc_r = *m_tmp;
                trunc_mask = FP64_SIGN_MASK | FP64_ABS_MAX;
                m_trunc_l = RightShift(*m_tmp, e_tmp - 64);
            }
            *m_tmp = RightShift(*m_tmp, e_tmp);
        }
        //calculate mantissa
        m_add = m_mul + m3;
        uint32_t m_min = FP32_MAN_HIDE_BIT;
        uint32_t m_max = m_min << 1;
        //Denormal
        while (m_add<m_min && e_ret>0){//the value of m_ret should not be smaller than 2^23
            m_add = m_add << 1;
            m_add += (uint64_t)(e_tmp > 64 && (FP64_SIGN_MASK&m_trunc_l));
            m_add += (uint64_t)(e_tmp > 0 && e_tmp <= 64 && (trunc_bit_r&m_trunc_r));
            m_trunc_l = m_trunc_l << 1;
            trunc_bit_r = trunc_bit_r >> 1;
            trunc_mask = trunc_mask >> 1;
            e_ret = e_ret - 1;
        }
        while (m_add >= m_max){
            trunc_bit_r = trunc_bit_r << 1;
            trunc_mask = (trunc_mask << 1) + 1;
            m_trunc_l = m_trunc_l >> 1;
            m_trunc_r = m_trunc_r | (trunc_bit_r*(m_add & 1));
            m_trunc_l = m_trunc_l | (FP64_SIGN_MASK*(m_add & 1));
            m_add = m_add >> 1;
            e_ret = e_ret + 1;
        }
        bool b_last_bit = ((m_add & 1) > 0);
        bool b_trunc_high = 0;
        bool b_trunc_left = 0;
        b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_SIGN_MASK) > 0);
        b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_ABS_MAX) > 0);
        b_trunc_left = (b_trunc_left || (e_tmp > 64 && (m_trunc_r&(trunc_mask^trunc_bit_r))));
        m_add = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_add);

        while (m_add >= m_max || e_ret < 0){
            m_add = m_add >> 1;
            e_ret = e_ret + 1;
        }
        uint16_t shift_out3 = (e_ret == 0 && m_add < (FP32_MAN_HIDE_BIT << 1));
        m_add = m_add >> shift_out3;

        m_ret = (uint32_t)m_add;
        uint32_t ret_tmp = FP32_CONSTRUCTOR(s_ret, e_ret, m_ret);
        ret = *((float *)&ret_tmp);
    }
    return ret;
}

float hf_fadd(fp16_t fp1, fp16_t fp2){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp1.val) || FP16_IS_INVALID(fp2.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif

    float ret = 0;
    uint16_t s_a, s_b;
    int16_t e_a, e_b;
    uint32_t m_a, m_b;
    uint32_t s_ret, e_ret, m_ret;

    uint16_t ma_tmp, mb_tmp;

    //1.Extract
    ExtractFP16(fp1.val, &s_a, &e_a, &ma_tmp);
    ExtractFP16(fp2.val, &s_b, &e_b, &mb_tmp);
    m_a = ma_tmp;
    m_b = mb_tmp;

    //fp16(m:10bit)->fp32(m:23bit)
    m_a = m_a << 13;
    m_b = m_b << 13;

    e_a = e_a - FP16_EXP_BIAS + FP32_EXP_BIAS;
    e_b = e_b - FP16_EXP_BIAS + FP32_EXP_BIAS;

    s_ret = CalcAdditionSign(s_a, e_a, &m_a, s_b, e_b, &m_b);

    uint64_t m_trunc = 0;
    int16_t e_tmp = 0;
    uint64_t m_tmp = 0;

    e_ret = std::max(e_a, e_b);
    e_tmp = std::abs(e_a - e_b);

    if (e_a > e_b){
        m_tmp = m_b;
        m_b = RightShift(m_b, e_tmp);
    }
    else if (e_a < e_b){//e_a<e_b
        m_tmp = m_a;
        m_a = RightShift(m_a, e_tmp);
    }
    if (e_tmp <= 64){
        m_trunc = m_tmp;
        for (int i = 0; i < 64 - e_tmp; i++){
            m_trunc = m_trunc << 1;
        }
    }

    //calculate mantissa
    m_ret = m_a + m_b;
    if (m_ret){
        uint64_t m_min = 1LLu << FP32_MAN_LEN;
        uint64_t m_max = m_min << 1;
        //Denormal
        while (m_ret<m_min && e_ret>0){//the value of m_ret should not be smaller than 2^23
            m_ret = m_ret << 1;
            if (FP64_SIGN_MASK&m_trunc){
                m_ret += 1;
            }
            m_trunc = m_trunc << 1;
            e_ret = e_ret - 1;
        }
        while (m_ret >= m_max){//the value of m_ret should be smaller than 2^24
            m_trunc = m_trunc >> 1;
            m_ret = m_ret >> 1;
            e_ret = e_ret + 1;
        }

        bool b_last_bit = ((m_ret & 1) > 0);
        bool b_trunc_high = 0;
        bool b_trunc_left = 0;
        b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP64_SIGN_MASK) > 0);
        b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc&FP64_ABS_MAX) > 0);
        m_ret = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_ret);

        while (m_ret >= m_max){
            m_ret = m_ret >> 1;
            e_ret = e_ret + 1;
        }
        uint32_t ui32 = FP32_CONSTRUCTOR(s_ret, e_ret, m_ret);
        ret = *((float *)&ui32);
    }
    //}
    return ret;
}
/********************************************************************************************/
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+      */
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16  */
/********************************************************************************************/
fp16_t hf_mma(fp16_t fpas[], fp16_t fpbs[], fp16_t c, int len){
    fp16_t ret;

    uint16_t s_ret = 0, s_c, m_pc;
    int16_t e_ret;
    int16_t e_c;
    uint64_t m_ret_l;

    uint16_t s_pa[MATRIX_LENGTH];
    int16_t e_pa[MATRIX_LENGTH];
    uint16_t m_pa[MATRIX_LENGTH];
    uint16_t s_pb[MATRIX_LENGTH];
    int16_t e_pb[MATRIX_LENGTH];
    uint16_t m_pb[MATRIX_LENGTH];

    uint16_t s_mul[MATRIX_LENGTH];
    int16_t  e_mul[MATRIX_LENGTH];
    uint64_t m_mul[MATRIX_LENGTH];

    //1.Extract
    for (int i = 0; i < MATRIX_LENGTH; i++){
        uint16_t va = fpas[i].val;
        uint16_t vb = fpbs[i].val;

        ExtractFP16(va, s_pa + i, e_pa + i, m_pa + i);
        ExtractFP16(vb, s_pb + i, e_pb + i, m_pb + i);
    }
    ExtractFP16(c.val, &s_c, &e_c, &m_pc);
    uint64_t m_c = m_pc;

    ReverseMan(s_c > 0, &m_c);

    //2.Mult
    int16_t shift_out1 = 20;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        s_mul[i] = s_pa[i] ^ s_pb[i];
        e_mul[i] = e_pa[i] + e_pb[i] - FP16_EXP_BIAS;
        m_mul[i] = (uint64_t)m_pa[i] * m_pb[i];//2.20

        ReverseMan(s_mul[i]>0, m_mul + i);
        m_mul[i] = m_mul[i] << shift_out1;//2.35
    }

    //3.Align exponent and add mantissa
    uint32_t s_add;
    uint64_t m_add = m_mul[0];
    int16_t e_add = e_mul[0];
    for (int i = 1; i < MATRIX_LENGTH; i++){
        AlignManByExp(e_add, &m_add, e_mul[i], m_mul + i);
        e_add = std::max(e_add, e_mul[i]);
        m_add = m_add + m_mul[i];
    }
    s_add = (m_add&FP64_SIGN_MASK) >> FP64_SIGN_INDEX;

    //4.Align exponent to float
    e_add = e_add - shift_out1 - FP16_MAN_LEN;
    if (!m_add){
        return c;
    }

    s_ret = s_add;
    if (s_add != s_c){
        uint64_t sum = GetManSum(e_add, m_add, e_c, m_c);
        s_ret = (sum&FP64_SIGN_MASK) >> FP64_SIGN_INDEX;
        ReverseMan(s_ret > 0, &m_add);
        ReverseMan(s_ret > 0, &m_c);
    }
    else{
        ReverseMan((m_add&FP64_SIGN_MASK) > 0, &m_add);
        ReverseMan((m_c&FP64_SIGN_MASK) > 0, &m_c);
    }

    uint64_t m_trunc_r = 0;
    uint64_t m_trunc_l = 0;
    uint64_t trunc_bit_r = 1;
    uint64_t trunc_mask = 1;
    int16_t e_tmp = 0;

    e_ret = std::max(e_add, e_c);
    e_tmp = std::abs(e_add - e_c);
    if (e_add != e_c){
        uint64_t* m_tmp = MinMan(e_add, &m_add, e_c, &m_c);
        if (e_tmp <= 64){
            trunc_bit_r = 1LLu << (e_tmp - 1);
            trunc_mask = (trunc_bit_r << 1) - 1;
            m_trunc_r = (*m_tmp)&trunc_mask;
            m_trunc_l = ((*m_tmp) << (64 - e_tmp))&(FP64_SIGN_MASK | FP64_ABS_MAX);
        }
        else{
            trunc_bit_r = FP64_SIGN_MASK;
            m_trunc_r = *m_tmp;
            trunc_mask = FP64_SIGN_MASK | FP64_ABS_MAX;
            m_trunc_l = RightShift(*m_tmp, e_tmp - 64);
        }
        *m_tmp = RightShift(*m_tmp, e_tmp);
    }
    //calculate mantissa
    m_ret_l = m_add + m_c;

    if (m_ret_l == 0){
        ret = 0;
        return ret;
    }
    uint64_t m_min = 1LLu << FP16_MAN_LEN;
    uint64_t m_max = m_min << 1;
    //Denormal
    while (m_ret_l<m_min && e_ret>0){//the value of m_ret should not be smaller than 2^10
        m_ret_l = m_ret_l << 1;
        m_ret_l += (uint64_t)(e_tmp > 0 && e_tmp <= 64 && (trunc_bit_r&m_trunc_r));
        m_ret_l += (uint64_t)(e_tmp > 64 && (FP64_SIGN_MASK&m_trunc_l));
        m_trunc_l = m_trunc_l << 1;
        trunc_bit_r >>= 1;
        e_ret = e_ret - 1;
    }
    while (m_ret_l >= m_max){//the value of m_ret should be smaller than 2^11
        trunc_bit_r <<= 1;
        trunc_mask = (trunc_mask << 1) + 1;
        m_trunc_l = m_trunc_l >> 1;
        m_trunc_r = m_trunc_r | (trunc_bit_r*(m_ret_l & 1));
        m_trunc_l = m_trunc_l | (FP64_SIGN_MASK*(m_ret_l & 1));
        m_ret_l = m_ret_l >> 1;
        e_ret = e_ret + 1;
    }

    bool b_last_bit = ((m_ret_l & 1) > 0);
    bool b_trunc_high = 0;
    bool b_trunc_left = 0;
    b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_SIGN_MASK) > 0);
    b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_ABS_MAX) > 0);
    b_trunc_left = (b_trunc_left || (e_tmp > 64 && (m_trunc_r&(trunc_mask^trunc_bit_r))));

    m_ret_l = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_ret_l);

    while (m_ret_l >= m_max || e_ret < 0){
        m_ret_l = m_ret_l >> 1;
        e_ret = e_ret + 1;
    }
    uint16_t shift_out3 = (e_ret == 0 && m_ret_l < (FP16_MAN_HIDE_BIT << 1));
    m_ret_l = m_ret_l >> shift_out3;

    uint16_t m_ret = (uint16_t)m_ret_l;
    if (e_ret >= FP16_MAX_EXP){
        e_ret = FP16_MAX_EXP - 1;
        m_ret = FP16_MAX_MAN;
    }
    ret.val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);

    return ret;
}

/********************************************************************************************/
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+      */
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp32  */
/********************************************************************************************/
float hf_mma(fp16_t fpas[], fp16_t fpbs[], float c, int len){//len=16
    //if (len != MATRIX_LENGTH){
    //    throw "matrix length must be 16, not support other length";
    //}
    float ret;

    uint32_t s_ret = 0, s_c;
    int32_t e_ret;
    int16_t e_c;
    uint64_t m_ret_l, m_c;

    uint16_t s_pa[MATRIX_LENGTH];
    int16_t e_pa[MATRIX_LENGTH];
    uint16_t m_pa[MATRIX_LENGTH];
    uint16_t s_pb[MATRIX_LENGTH];
    int16_t e_pb[MATRIX_LENGTH];
    uint16_t m_pb[MATRIX_LENGTH];

    uint16_t s_mul[MATRIX_LENGTH];
    int16_t  e_mul[MATRIX_LENGTH];
    uint64_t m_mul[MATRIX_LENGTH];

    //1.Extract
    for (int i = 0; i < MATRIX_LENGTH; i++){
        uint16_t va = fpas[i].val;
        uint16_t vb = fpbs[i].val;

        ExtractFP16(va, s_pa + i, e_pa + i, m_pa + i);
        ExtractFP16(vb, s_pb + i, e_pb + i, m_pb + i);
    }
    uint32_t vc = *((uint32_t *)&c);
    s_c = FP32_EXTRAC_SIGN(vc);//4Byte
    e_c = FP32_EXTRAC_EXP(vc); //8bits exponent
    m_c = (uint64_t)(FP32_EXTRAC_MAN(vc));    //23 bits mantissa  [Normal:1.m Denormal:0.m]

    //Denormal
    if (e_c == 0){
        e_c = 1;
    }
    ReverseMan(s_c > 0, &m_c);

    //2.Mult
    int16_t shift_out1 = 20;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        s_mul[i] = s_pa[i] ^ s_pb[i];
        e_mul[i] = e_pa[i] + e_pb[i] - FP16_EXP_BIAS;
        m_mul[i] = (uint64_t)m_pa[i] * m_pb[i];//2.20

        ReverseMan(s_mul[i]>0, m_mul + i);
        m_mul[i] = m_mul[i] << shift_out1;//2.35
    }

    //3.Align exponent and add mantissa
    uint32_t s_add;
    uint64_t m_add = m_mul[0];
    int16_t e_add = e_mul[0];
    for (int i = 1; i < MATRIX_LENGTH; i++){
        AlignManByExp(e_add, &m_add, e_mul[i], m_mul + i);
        e_add = std::max(e_add, e_mul[i]);
        m_add = m_add + m_mul[i];
    }
    s_add = (m_add&FP64_SIGN_MASK) >> FP64_SIGN_INDEX;

    //4.Align exponent to float
    e_add = e_add - shift_out1 - FP16_MAN_LEN * 2 + FP32_MAN_LEN - FP16_EXP_BIAS + FP32_EXP_BIAS;
    if (!m_add){
        return c;
    }

    s_ret = s_add;
    if (s_add != s_c){
        uint64_t sum = GetManSum(e_add, m_add, e_c, m_c);
        s_ret = (sum&FP64_SIGN_MASK) >> FP64_SIGN_INDEX;
        ReverseMan(s_ret > 0, &m_add);
        ReverseMan(s_ret > 0, &m_c);
    }
    else{
        ReverseMan((m_add&FP64_SIGN_MASK) > 0, &m_add);
        ReverseMan((m_c&FP64_SIGN_MASK) > 0, &m_c);
    }

    uint64_t m_trunc_r = 0;
    uint64_t m_trunc_l = 0;
    uint64_t trunc_bit_r = 1;
    uint64_t trunc_mask = 1;
    int16_t e_tmp = 0;

    e_ret = std::max(e_add, e_c);
    e_tmp = std::abs(e_add - e_c);
    if (e_add != e_c){
        uint64_t* m_tmp = MinMan(e_add, &m_add, e_c, &m_c);
        if (e_tmp <= 64){
            trunc_bit_r = 1LLu << (e_tmp - 1);
            trunc_mask = (trunc_bit_r << 1) - 1;
            m_trunc_r = (*m_tmp)&trunc_mask;
            m_trunc_l = ((*m_tmp) << (64 - e_tmp))&(FP64_SIGN_MASK | FP64_ABS_MAX);
        }
        else{
            trunc_bit_r = FP64_SIGN_MASK;
            m_trunc_r = *m_tmp;
            trunc_mask = FP64_SIGN_MASK | FP64_ABS_MAX;
            m_trunc_l = RightShift(*m_tmp, e_tmp - 64);
        }
        *m_tmp = RightShift(*m_tmp, e_tmp);
    }
    //calculate mantissa
    m_ret_l = m_add + m_c;

    if (m_ret_l == 0){
        ret = 0;
        return ret;
    }
    uint64_t m_min = 1LLu << FP32_MAN_LEN;
    uint64_t m_max = m_min << 1;
    //Denormal
    while (m_ret_l<m_min && e_ret>0){//the value of m_ret should not be smaller than 2^23
        m_ret_l = m_ret_l << 1;
        m_ret_l += (uint64_t)(e_tmp > 0 && e_tmp <= 64 && (trunc_bit_r&m_trunc_r));
        m_ret_l += (uint64_t)(e_tmp > 64 && (FP64_SIGN_MASK&m_trunc_l));
        m_trunc_l = m_trunc_l << 1;
        trunc_bit_r >>= 1;
        e_ret = e_ret - 1;
    }
    while (m_ret_l >= m_max){//the value of m_ret should be smaller than 2^24
        trunc_bit_r <<= 1;
        trunc_mask = (trunc_mask << 1) + 1;
        m_trunc_l = m_trunc_l >> 1;
        //if (m_ret_l & 1){
        m_trunc_r = m_trunc_r | (trunc_bit_r*(m_ret_l & 1));
        m_trunc_l = m_trunc_l | (FP64_SIGN_MASK*(m_ret_l & 1));
        //}
        m_ret_l = m_ret_l >> 1;
        e_ret = e_ret + 1;
    }

    bool b_last_bit = ((m_ret_l & 1) > 0);
    bool b_trunc_high = 0;
    bool b_trunc_left = 0;
    b_trunc_high = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_SIGN_MASK) > 0);
    b_trunc_left = (ROUND_TO_NEAREST == g_RoundMode) && ((m_trunc_l&FP64_ABS_MAX) > 0);
    b_trunc_left = (b_trunc_left || (e_tmp > 64 && (m_trunc_r&(trunc_mask^trunc_bit_r))));

    m_ret_l = ManRoundToNearest(b_last_bit, b_trunc_high, b_trunc_left, m_ret_l);

    while (m_ret_l >= m_max || e_ret < 0){
        m_ret_l = m_ret_l >> 1;
        e_ret = e_ret + 1;
    }
    uint16_t shift_out3 = (e_ret == 0 && m_ret_l < (FP32_MAN_HIDE_BIT << 1));
    m_ret_l = m_ret_l >> shift_out3;

    uint32_t m_ret = (uint32_t)m_ret_l;
    uint32_t ret_tmp = FP32_CONSTRUCTOR(s_ret, e_ret, m_ret);
    ret = *((float *)&ret_tmp);

    return ret;
}


static void mma_mul(fp16_t fpas[], fp16_t fpbs[], int16_t shift_out, int e_bias, uint16_t *s_mul, int16_t *e_mul, int64_t *m_mul){
    uint16_t s_pa[MATRIX_LENGTH];
    int16_t e_pa[MATRIX_LENGTH];
    uint16_t m_pa[MATRIX_LENGTH];
    uint16_t s_pb[MATRIX_LENGTH];
    int16_t e_pb[MATRIX_LENGTH];
    uint16_t m_pb[MATRIX_LENGTH];

#ifdef FP16_UNIT_DEBUG
    cout<<"************mul************"<<endl;
#endif
    //2.Extract and Mult
    for (int i = 0; i < MATRIX_LENGTH; i++){
        ExtractFP16(fpas[i].val, s_pa + i, e_pa + i, m_pa + i);
        ExtractFP16(fpbs[i].val, s_pb + i, e_pb + i, m_pb + i);
        if (FP16_IS_INF(fpas[i].val)){
            e_pa[i] = FP16_MAX_VALID_EXP;
            m_pa[i] = FP16_MAX_MAN;
        }
        else if (FP16_IS_NAN(fpas[i].val)){
            e_pa[i] = 0;
            m_pa[i] = 0;
        }
        if (FP16_IS_INF(fpbs[i].val)){
            e_pb[i] = FP16_MAX_VALID_EXP;
            m_pb[i] = FP16_MAX_MAN;
        }
        else if (FP16_IS_NAN(fpbs[i].val)){
            e_pb[i] = 0;
            m_pb[i] = 0;
        }

        s_mul[i] = s_pa[i] ^ s_pb[i];
        e_mul[i] = e_pa[i] + e_pb[i] - FP16_EXP_BIAS - e_bias;
        m_mul[i] = (uint64_t)m_pa[i] * m_pb[i];//2.20

        if (m_mul[i] == 0){
            e_mul[i] = 0;
        }
        else{
            while (!(m_mul[i] & 0x200000)){//bit21
                m_mul[i] = m_mul[i] << 1;
                e_mul[i] = e_mul[i] - 1;
            }
            m_mul[i] = m_mul[i] << shift_out;
        }
#ifdef FP16_UNIT_DEBUG
        std::printf("m_mul[%2d]=%16lld\n",i, m_mul[i]);
#endif
    }
}

static void mma_add(uint16_t *s_mul, int16_t *e_mul, int64_t *m_mul, uint16_t &s_ret, int16_t &e_ret, uint64_t &m_ret){
    int16_t e_max = -255;//less than double FP32_EXP_BIAS
    for (int32_t i = 0; i < MATRIX_LENGTH + 1; i++){
        if (e_mul[i] > e_max&&m_mul[i]){
            e_max = e_mul[i];
        }
#ifdef FP16_UNIT_DEBUG
        std::printf("m_mul[%2d]=%16lld\n", i, m_mul[i]);
#endif
    }

    //3.Align exponent and add mantissa
    int64_t m_add = 0;
#ifdef FP16_UNIT_DEBUG
    cout << "************add************" << endl;
    cout << "e_max=" << e_max << endl;
    cout << "mul[i]                    exp  e_delta           m_tmp               m_add" << endl;
#endif
    int64_t m_tmp;
    for (int i = 0; i < MATRIX_LENGTH + 1; i++){
        uint16_t e_tmp = (uint16_t)(e_max - e_mul[i]);
        m_tmp = m_mul[i];
        if (e_tmp){
            if (e_tmp>=52){
                m_tmp = 0;
            }
            else{
                m_tmp = m_tmp >> e_tmp;
            }
        }
        if (s_mul[i]){
            m_tmp = -m_tmp;
        }
        m_add += m_tmp;
#ifdef FP16_UNIT_DEBUG
        cout << "mul[" <<setw(2)<<i<<"]="<<setw(16)<<m_mul[i]<<" e="<<setw(3)<<e_mul[i]<<" ed="<<setw(3)<<e_tmp<<" m="<<setw(17)<<m_tmp<<setw(20)<< m_add << endl;
        //std::printf("mul[%2d]=%16lld ed=%3d m=%17lld %20lld\n", i, m_mul[i], e_tmp, i, m_tmp, m_add);
#endif
    }
    e_ret = e_max;
    s_ret = m_add < 0 ? 1 : 0;
    m_ret = (uint64_t)std::abs(m_add);
}

/********************************************************************************************/
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+      */
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16  */
/********************************************************************************************/
fp16_t hf_mma(fp16_t fpas[], fp16_t fpbs[], fp16_t c, int len){
    fp16_t ret;
    uint16_t s_ret = 0, m_pc, m_ret;
    int16_t e_ret;
    uint64_t m_ret_l;
    /*
     * 1.xxxxxxxxxx x 1.xxxxxxxxxx  1.10x1.10=>2.20 2bit integer and 20bit decimal, total 22bit mantissia
     * shift left 30bit to keep 51bit decimal precision, total 52bit mantissia
     */
    int16_t shift_out = 0;

    uint16_t s_mul[17];
    int16_t  e_mul[17];
    int64_t m_mul[17];
    int16_t e_bias = shift_out + FP16_MAN_LEN;
    mma_mul(fpas, fpbs, shift_out, e_bias, s_mul, e_mul, m_mul);

    int16_t man_len = FP16_MAN_LEN + 1;
    /* 1.xxx...xxx 1.10 1bit integer and 10bit decimal, total 11bit mantissia
     * shift left 41bit to keep 51bit decimal precision, total 52bit mantissia
     */
    int16_t shift_out_c = man_len + shift_out;
    ExtractFP16(c.val, s_mul + MATRIX_LENGTH, e_mul + MATRIX_LENGTH, &m_pc);
    if (FP16_IS_INF(c.val)){
        e_mul[MATRIX_LENGTH] = FP16_MAX_VALID_EXP;
        m_pc = FP16_MAX_MAN | FP16_MAN_HIDE_BIT;
    }
    else if (FP16_IS_NAN(c.val)){
        e_mul[MATRIX_LENGTH] = 0;
        m_pc = 0;
    }
    e_mul[MATRIX_LENGTH] = e_mul[MATRIX_LENGTH] - shift_out_c;
    m_mul[MATRIX_LENGTH] = (int64_t)m_pc << (uint16_t)shift_out_c;
    while (m_mul[MATRIX_LENGTH]&&(!(m_mul[MATRIX_LENGTH] & 0x200000))){//bit21
        m_mul[MATRIX_LENGTH] = m_mul[MATRIX_LENGTH] << 1;
        e_mul[MATRIX_LENGTH] = e_mul[MATRIX_LENGTH] - 1;
    }

    mma_add(s_mul, e_mul, m_mul, s_ret, e_ret, m_ret_l);
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld  e_ret=%d\n", m_ret_l, e_ret);
#endif
    if (m_ret_l == 0){
        ret = 0;
        return ret;
    }
    uint64_t m_min = 1LLu << (FP16_MAN_LEN + shift_out_c);
    uint64_t m_max = m_min << 1;
    //Denormal
    while (m_ret_l<m_min && e_ret>-shift_out_c){//the value of m_ret should not be smaller than 2^10
        m_ret_l = m_ret_l << 1;
        e_ret = e_ret - 1;
    }
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld  e_ret=%d\n", m_ret_l, e_ret);
#endif
    int16_t shift_right = 0;
    while (m_ret_l >= m_max || e_ret<-shift_out_c){//the value of m_ret should be smaller than 2^11
        if ((shift_right < 5 || e_ret<-shift_out_c)&&(m_max<FP64_SIGN_MASK)){
            shift_right++;
            m_max <<= 1;
        }
        else{
            m_ret_l >>= 1;
        }
        e_ret = e_ret + 1;
    }
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld  e_ret=%hd shift_right=%hd\n", m_ret_l, e_ret, shift_right);
#endif
    if ((e_ret + shift_out_c == 0) && ((m_ret_l >> (shift_out + shift_right + man_len)) < (FP16_MAN_HIDE_BIT << 1))){
        shift_right++;
    }
    /*
     *remove last 30bit
     */
    m_ret_l = m_ret_l >> (uint16_t)shift_out;
    bool needRound = IsRoundOne(m_ret_l, shift_right + man_len);
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld   roundone=%d\n", m_ret_l, needRound);
#endif
    m_ret = (uint16_t)(m_ret_l >> (shift_right + man_len));
    e_ret += shift_out_c;
#ifdef FP16_UNIT_DEBUG
    cout << "m_ret=" << setw(19) << m_ret << "  e_ret=" << e_ret << endl;
#endif
    if (needRound){
        m_ret++;
    }

    m_max = FP16_MAN_HIDE_BIT << 1;
    while (m_ret >= m_max || e_ret < 0){
        m_ret = m_ret >> 1;
        e_ret = e_ret + 1;
    }
#ifdef FP16_UNIT_DEBUG
    cout << "m_ret=" << setw(19) << m_ret << "  e_ret=" << e_ret << endl;
#endif
    //uint16_t shift_out3 = (e_ret == 0 && m_ret < (FP16_MAN_HIDE_BIT << 1));
    //m_ret = m_ret >> shift_out3;

    Fp16Normalize(e_ret, m_ret);
    ret.val = FP16_CONSTRUCTOR(s_ret, e_ret, m_ret);
#ifdef FP16_UNIT_DEBUG
    cout << "ret.val=" << setw(5) << ret.val << " ("<<s_ret<<"," << e_ret<<","<<m_ret<<")" << endl;
#endif
    return ret;
}

/********************************************************************************************/
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+      */
/*    fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp16*fp16+fp32  */
/********************************************************************************************/
float hf_mma(fp16_t fpas[], fp16_t fpbs[], float c, int len){
    float ret;
    uint32_t m_ret;
    int16_t e_ret;
    uint16_t s_ret = 0;
    uint64_t m_ret_l;
    /*
     * 1.xxxxxxxxxx x 1.xxxxxxxxxx  1.10x1.10=>2.20 2bit integer and 20bit decimal, total 22bit mantissia
     * shift left 30bit to keep 51bit decimal precision, total 52bit mantissia
     */
    int16_t shift_out = 30;

    uint16_t s_mul[17];
    int16_t  e_mul[17];
    int64_t m_mul[17];
    int16_t e_bias = shift_out + FP16_MAN_LEN * 2 - FP32_MAN_LEN - FP32_EXP_BIAS + FP16_EXP_BIAS;
    mma_mul(fpas, fpbs, shift_out, e_bias, s_mul, e_mul, m_mul);

    uint32_t vc = *((uint32_t *)&c);
    s_mul[MATRIX_LENGTH] = (uint16_t)FP32_EXTRAC_SIGN(vc);//4Byte
    e_mul[MATRIX_LENGTH] = (int16_t)FP32_EXTRAC_EXP(vc); //8bits exponent
    m_mul[MATRIX_LENGTH] = (int64_t)(FP32_EXTRAC_MAN(vc));    //23 bits mantissa  [Normal:1.m Denormal:0.m]
    //Denormal
    if (e_mul[MATRIX_LENGTH] == 0){
        e_mul[MATRIX_LENGTH] = 1;
    }
    if (FP32_IS_INF(vc)){
        e_mul[MATRIX_LENGTH] = FP32_MAX_EXP - 1;
        m_mul[MATRIX_LENGTH] = FP32_MAX_MAN | FP32_MAN_HIDE_BIT;
    }
    else if (FP32_IS_NAN(vc)){
        e_mul[MATRIX_LENGTH] = 0;
        m_mul[MATRIX_LENGTH] = 0;
    }
    /* 1.xxx...xxx 1.10 1bit integer and 10bit decimal, total 11bit mantissia
     * shift left 41bit to keep 51bit decimal precision, total 52bit mantissia
     */
    int16_t shift_out_c = 22 + shift_out - 24;
    e_mul[MATRIX_LENGTH] = e_mul[MATRIX_LENGTH] - shift_out_c;
#ifdef FP16_UNIT_DEBUG
    std::printf("************hf_mma************\n");
    std::printf("mc=%17lld\n", m_mul[MATRIX_LENGTH]);
#endif
    m_mul[MATRIX_LENGTH] = m_mul[MATRIX_LENGTH] << shift_out_c;
    while (m_mul[MATRIX_LENGTH] && (!(m_mul[MATRIX_LENGTH] & 0x8000000000000LLu))){//bit51
        m_mul[MATRIX_LENGTH] = m_mul[MATRIX_LENGTH] << 1;
        e_mul[MATRIX_LENGTH] = e_mul[MATRIX_LENGTH] - 1;
    }
#ifdef FP16_UNIT_DEBUG
    std::printf("mc=%17lld\n", m_mul[MATRIX_LENGTH]);
#endif
    mma_add(s_mul, e_mul, m_mul, s_ret, e_ret, m_ret_l);
#ifdef FP16_UNIT_DEBUG
    std::printf("mc=%17lld\n", m_mul[MATRIX_LENGTH]);
#endif
    if (m_ret_l == 0){
        ret = 0;
        return ret;
    }
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld\n", m_ret_l);
#endif
    uint64_t m_min = 1LLu << (FP32_MAN_LEN + shift_out_c);
    uint64_t m_max = m_min << 1;
    //Denormal
    while (m_ret_l<m_min && e_ret>-shift_out_c){//the value of m_ret should not be smaller than 2^23
        m_ret_l = m_ret_l << 1;
        e_ret = e_ret - 1;
    }
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld\n", m_ret_l);
#endif
    int16_t shift_right = 0;
    while (m_ret_l >= m_max || e_ret<-shift_out_c){//the value of m_ret should be smaller than 2^24
        if ((shift_right < 5 || e_ret < -shift_out_c) && (m_max < FP64_SIGN_MASK)){
            shift_right++;
            m_max <<= 1;
        }
        else{
            m_ret_l >>= 1;
        }
        e_ret = e_ret + 1;
    }
    if ((e_ret + shift_out_c == 0) && ((m_ret_l >> (shift_right + shift_out_c)) < (FP32_MAN_HIDE_BIT << 1))){
        shift_right++;
    }
    bool needRound = IsRoundOne(m_ret_l, shift_right + shift_out_c);
#ifdef FP16_UNIT_DEBUG
    std::printf("m_ret_l=%17lld shift_right=%d roundone=\n", m_ret_l, shift_right, needRound);
#endif
    m_ret = (uint32_t)(m_ret_l >> (shift_right + shift_out_c));
    e_ret += shift_out_c;

    if (needRound){
        m_ret++;
    }

    m_max = FP32_MAN_HIDE_BIT << 1;
    while (m_ret >= m_max || e_ret < 0){
        m_ret = m_ret >> 1;
        e_ret = e_ret + 1;
    }
    //uint16_t shift_out3 = (e_ret == 0 && m_ret < (FP32_MAN_HIDE_BIT << 1));
    //m_ret = m_ret >> shift_out3;
    Fp32Normalize(e_ret, m_ret);
    uint32_t ret_tmp = FP32_CONSTRUCTOR(s_ret, (uint32_t)e_ret, m_ret);
    ret = *((float *)&ret_tmp);

    return ret;
}

float d_mma(fp16_t fpas[], fp16_t fpbs[], float c, int len){
    float ret;
    double da;
    double db;
    double dc = c;
    double res = 0;
    for (int i = 0; i < len; i++){
        da = fpas[i];
        db = fpbs[i];
        res += da*db;
    }
    res += dc;
    ret = (float)res;
    return ret;
}

static const uint8_t recip_sqrt_estimate[256] = {
    255, 253, 251, 249, 247, 245, 243, 242, 240, 238, 236, 234, 233, 231, 229, 228,
    226, 224, 223, 221, 219, 218, 216, 215, 213, 212, 210, 209, 207, 206, 204, 203,
    201, 200, 198, 197, 196, 194, 193, 192, 190, 189, 188, 186, 185, 184, 183, 181,
    180, 179, 178, 176, 175, 174, 173, 172, 170, 169, 168, 167, 166, 165, 164, 163,
    162, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146,
    145, 144, 143, 142, 141, 140, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131,
    131, 130, 129, 128, 127, 126, 126, 125, 124, 123, 122, 121, 121, 120, 119, 118,
    118, 117, 116, 115, 114, 114, 113, 112, 111, 111, 110, 109, 109, 108, 107, 106,
    105, 104, 103, 101, 100, 99, 97, 96, 95, 93, 92, 91, 90, 88, 87, 86,
    85, 84, 82, 81, 80, 79, 78, 77, 76, 75, 74, 72, 71, 70, 69, 68,
    67, 66, 65, 64, 63, 62, 61, 60, 60, 59, 58, 57, 56, 55, 54, 53,
    52, 51, 51, 50, 49, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40,
    39, 38, 38, 37, 36, 35, 35, 34, 33, 33, 32, 31, 30, 30, 29, 28,
    28, 27, 26, 26, 25, 24, 24, 23, 22, 22, 21, 20, 20, 19, 19, 18,
    17, 17, 16, 16, 15, 14, 14, 13, 13, 12, 11, 11, 10, 10, 9, 9,
    8, 8, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0
};

fp16_t hf_half(fp16_t fp){
    uint16_t hf_s, hf_m, ret_m;
    int16_t hf_e, ret_e;
    ExtractFP16(fp.val, &hf_s, &hf_e, &hf_m);
    fp16_t ret;

    if (hf_e > 1){
        ret_e = hf_e - 1;
        ret_m = hf_m;
    }
    else{
        ret_e = 0;
        ret_m = hf_m >> 1;
        if ((hf_m & 1) && (ret_m & 1)){
            ret_m++;
        }
    }
    if ((ret_m > FP16_MAN_MASK) && (!ret_e)){
        ret_e = 1;
        ret_m = 0;
    }
    ret.val = FP16_CONSTRUCTOR(hf_s, ret_e, ret_m);
    return ret;
}

static inline uint16_t fp16_normalise(uint16_t mnt, int16_t *exp){
    int16_t shift;
    if (!mnt) {
        return 0;
    }
    for (shift = 8; shift; shift >>= 1) {
        if (!(mnt >> (16 - shift))) {
            mnt <<= shift;
            *exp -= shift;
        }
    }
    return mnt;
}

static inline uint32_t fp32_normalise(uint32_t mnt, int *exp){
    int shift;
    if (!mnt) {
        return 0;
    }
    for (shift = 16; shift; shift >>= 1) {
        if (!(mnt >> (32 - shift))) {
            mnt <<= shift;
            *exp -= shift;
        }
    }
    return mnt;
}

static inline uint16_t hf_recipEstimate(uint16_t val){
    uint16_t hf_s, hf_m;
    int16_t hf_e;
    ExtractFP16(val, &hf_s, &hf_e, &hf_m);
    uint16_t ret;
    int16_t ret_e;
    uint16_t ret_m = 0;
    if (hf_e > 29){
        ret_e = 0;
        uint32_t m_tmp = 1 << (49 - hf_e);
        while (m_tmp > hf_m){
            m_tmp = m_tmp - hf_m;
            ret_m++;
        }
        if ((m_tmp << 1) > hf_m || (((m_tmp << 1) == hf_m) && (ret_m & 1))){
            ret_m++;
        }
    }
    else{
        hf_e += 5;
        hf_m = fp16_normalise(hf_m, &hf_e);
        ret_e = 29 - hf_e;
        ret_m = (((uint32_t)1 << 19) / (hf_m >> 6 | 1) + 1) >> 1;
        ret_m <<= 2;
        if (ret_e == 0) {
            ret_m >>= 1;
        }
        else if (ret_e == -1) {
            ret_m >>= 2;
            ret_e = 0;
        }
    }
    ret = FP16_CONSTRUCTOR(hf_s, ret_e, ret_m);
    return ret;
}

static inline uint16_t hf_recipStepFuzed(uint16_t val){
    fp16_t fpTwo(16384);
    //fpTwo.val = 16384;
    fp16_t negfp;
    negfp.val = ((~(val))&FP16_SIGN_MASK) | (val&FP16_ABS_MAX);
    uint16_t est = hf_recipEstimate(val);
    fp16_t ret(est);

    fp16_t x(val);
    float tmp = ret;
    float fx = x;
    for (int i = 0; i < 2; i++){
        //ret = ret*(fpTwo - fp*ret);
        tmp = tmp*(2.0f - fx*tmp);
    }
    ret = tmp;

    //Xn+1=Xnx(2.0-axXn); a is input, iteration 4 times
    //for (int i = 0; i < 4; i++){
    //    //ret = ret*(fpTwo - fp*ret);
    //    ret = ret*hf_mla(negfp, ret, fpTwo);
    //}
    return ret.val;
}

fp16_t hf_recip(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    uint16_t val;
    fp16_t ret;
    val = fp.val;

    if (FP16_IS_ZERO(val)){
        ret.val = (val&FP16_SIGN_MASK) | FP16_MAX;
    }
    else if (!(val&FP16_EXP_MASK) && ((val&FP16_MAN_MASK) <= 256)) {//e=0,man[9:8]=0 denormal man<=256
        ret.val = (val&FP16_SIGN_MASK) | FP16_MAX;
    }
    else{
        ret.val = hf_recipStepFuzed(val);
    }
    return ret;
}

static inline uint16_t hf_recipSqrtEstimate(uint16_t val){
    uint16_t hf_s, hf_m, ret_m;
    int16_t hf_e, ret_e;
    ExtractFP16(val, &hf_s, &hf_e, &hf_m);
    uint16_t ret;

    hf_e += 5;
    hf_m = fp16_normalise(hf_m, &hf_e);
    hf_m = recip_sqrt_estimate[(~hf_e & 1) << 7 | (hf_m >> 8 & 127)];
    ret_m = hf_m << 2;
    ret_e = (44 - hf_e) >> 1;

    ret = FP16_CONSTRUCTOR(hf_s, ret_e, ret_m);
    return ret;
}

static inline uint16_t hf_recipSqrtStepFuzed(uint16_t val){
    fp16_t fpThree(16896);
    //fpThreeHalf.val = 15872;//1.5f
    //fpHalf.val = 14336;
    fp16_t x(val);

    uint16_t est = hf_recipSqrtEstimate(val);
    fp16_t ret(est);
    float tmp = ret;
    float fx = x;
    for (int i = 0; i < 2; i++){
        tmp = 0.5f*(tmp*(3.0f - fx*tmp*tmp));
    }
    ret = tmp;
    //Xn+1=Xnx(3-axXnxXn)/2; a is input, iteration 4 times
    //for (int i = 0; i < 2; i++){
    //    ret = hf_half(ret*(fpThree - x*ret*ret));
    //}
    return ret.val;
}

fp16_t hf_recipsqrt(fp16_t fp){
#ifdef PRINT_INPUT_OVERFLOW_ERROR
    if (FP16_IS_INVALID(fp.val)){
        std::cout << "input fp16_t value is overflow\n";
    }
#endif
    uint16_t val = fp.val;
    fp16_t ret;

    if (FP16_IS_ZERO(val) || val&FP16_SIGN_MASK){
        ret.val = FP16_MAX;
    }
    else if (((val&FP16_EXP_MASK) >> FP16_MAN_LEN) == FP16_MAX_EXP){
        ret.val = 0;
    }
    else if (val == 1){
        ret.val = 27648;//2^12=4096
    }
    else{
        ret.val = hf_recipSqrtStepFuzed(val);
    }
    return ret;
}
