uint16_t fp16_div(uint16_t a, uint16_t b, int mode, int *flags)
{
    int a_sgn, a_exp, b_sgn, b_exp, x_sgn, x_exp;
    uint16_t a_mnt, b_mnt, x;
    uint32_t x_mnt;

    fp16_unpack(&a_sgn, &a_exp, &a_mnt, a, mode, flags);
    fp16_unpack(&b_sgn, &b_exp, &b_mnt, b, mode, flags);

    //if ((x = fp16_process_NaNs(a, b, mode, flags)))
    //    return x;

    // Handle infinities and zeroes:
    if ((a_exp == 31 && b_exp == 31) || (!a_mnt && !b_mnt)) {
        *flags |= FPLIB_IOC;
        return fp16_defaultNaN();
    }
    if (a_exp == 31 || !b_mnt) {
        if (a_exp != 31)
            *flags |= FPLIB_DZC;
        return fp16_infinity(a_sgn ^ b_sgn);
    }
    if (!a_mnt || b_exp == 31)
        return fp16_zero(a_sgn ^ b_sgn);

    // Divide, setting bottom bit if inexact:
    a_mnt = fp16_normalise(a_mnt, &a_exp);
    x_sgn = a_sgn ^ b_sgn;
    x_exp = a_exp - b_exp + 36;
    x_mnt = ((uint32_t)a_mnt << 10) / b_mnt;
    x_mnt |= (x_mnt * b_mnt != (uint32_t)a_mnt << 10);

    // Normalise and convert to 32 bits, collapsing error into bottom bit:
    x_mnt = fp32_normalise(x_mnt, &x_exp);
    x_mnt = x_mnt >> 15 | !!(uint16_t)(x_mnt << 1);

    uint16_t ret = fp16_round_(x_sgn, x_exp, x_mnt, mode&3,mode, flags);
    return ret;
}
//http://www.gem5.org/docs/html/fplib_8cc_source.html
