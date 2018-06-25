#include <Python.h>
#include "fp.h"

#define PREPARE_ONE_FP16_PARA       \
    uint16_t ui , ret;              \
    fp16_t fp, fpRet;               \
    int result;                     \
    ui = *((uint16_t *)&x);         \
    fp.val = ui;                    \

#define PREPARE_TWO_FP16_PARA       \
    uint16_t u1, u2, ret;           \
    fp16_t fp1, fp2, fpRet;         \
    int result;                     \
    u1 = *((uint16_t *)&x);         \
    u2 = *((uint16_t *)&y);         \
    fp1.val = u1;                   \
    fp2.val = u2;                   \

#define PREPARE_TWO_FP32_PARA       \
    float x, y;                     \
    uint32_t ret;                   \
    x = *((float *)&ux);            \
    y = *((float *)&uy);            \

#define PREPARE_WRAPP_ONE_INT_PARA              \
    int x;                                      \
    if (!PyArg_ParseTuple(args, "i", &x))       \
    {                                           \
        return NULL;                            \
    }                                           \

#define PREPARE_WRAPP_TWO_INT_PARA              \
    int x, y;                                   \
    if (!PyArg_ParseTuple(args, "ii", &x, &y))  \
    {                                           \
        return NULL;                            \
    }                                           \

#define PREPARE_WRAPP_ONE_UINT_PARA             \
    uint32_t ux;                                \
    if (!PyArg_ParseTuple(args, "l", &ux))      \
    {                                           \
        return NULL;                            \
    }                                           \

#define PREPARE_WRAPP_TWO_UINT_PARA             \
    uint32_t ux, uy;                            \
    if (!PyArg_ParseTuple(args, "ll", &ux, &uy))\
    {                                           \
        return NULL;                            \
    }                                           \

int Compare(int x, int y, fpCompareType type){
    PREPARE_TWO_FP16_PARA
    switch (type){
        case EQUAL:             result = (fp1 == fp2); break;
        case NOT_EQUAL:         result = (fp1 != fp2); break;
        case GREATER_THAN:      result = (fp1 > fp2);  break;
        case GREATER_EQUAL:     result = (fp1 >= fp2); break;
        case LESS_THAN:         result = (fp1 < fp2);  break;
        case LESS_EQUAL:        result = (fp1 <= fp2); break;
    }
    return result;
}

int Add(int x, int y)
{
    /*uint16_t u1, u2, ret;
    fp16_t fp1, fp2, fpRet;
    int result;
    u1 = *((uint16_t *)&x);
    u2 = *((uint16_t *)&y);
    fp1.val = u1;
    fp2.val = u2;*/
    PREPARE_TWO_FP16_PARA
    fpRet = fp1 + fp2;
    ret = fpRet.val;
    result = ret;
    //printf("x=%d,y=%d,u1=%u,u2=%u,ret=%u,result=%d\n", x, y, u1, u2, ret, result);
    return result;
}
int Sub(int x, int y)
{
    PREPARE_TWO_FP16_PARA
    fpRet = fp1 - fp2;
    ret = fpRet.val;
    result = ret;
    return result;
}
int Mul(int x, int y)
{
    PREPARE_TWO_FP16_PARA
    fpRet = fp1 * fp2;
    ret = fpRet.val;
    result = ret;
    return result;
}
int Div(int x, int y)
{
    PREPARE_TWO_FP16_PARA
    fpRet = fp1 / fp2;
    ret = fpRet.val;
    result = ret;
    return result;
}

uint32_t FP16ToFP32(int x){
    uint16_t ui;
    float f;
    uint32_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    f = fp;
    ret = *((uint32_t *)&f);
    return ret;
}
float FP16ToFloat(int x){
    uint16_t ui;
    float f;
    uint32_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    f = fp;
    return f;
}
int FP32ToFP16(uint32_t ux){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    //printf("x=%u\n", ux);
    float f = *((float*)&ux);
    //printf("f=%f\n", f);
    fp = f;
    uiRet = fp.val;
    //printf("uiRet=%u\n", uiRet);
    ret = uiRet;
    //printf("ret=%d\n", ret);
    return ret;
}
int FloatToFP16(float f){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    //printf("x=%u\n", ux);
    //printf("f=%f\n", f);
    fp = f;
    uiRet = fp.val;
    //printf("uiRet=%u\n", uiRet);
    ret = uiRet;
    //printf("ret=%d\n", ret);
    return ret;
}
int FP16ToInt32F(int x){
    uint16_t ui;
    int i;
    uint32_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    i = fp.toInt32F();
    //float f1, f2;
    //f1 = fp;
    //f2 = (float)i;
    //if (f2 > f1){
    //    i--;
    //}
    return i;
}
int FP16ToInt32C(int x){
    uint16_t ui;
    int i;
    uint32_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    i = fp.toInt32C();
    //float f1, f2;
    //f1 = fp;
    //f2 = (float)i;
    //if (f2 < f1){
    //    i++;
    //}
    return i;
}
int FP16ToInt32(int x){
    uint16_t ui;
    int i;
    uint32_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    i = fp.toInt32();
    return i;
}
uint8_t FP16ToUInt8(int x){
    uint16_t ui;
    uint8_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    ret = fp;
    return ret;
}
int8_t FP16ToInt8(int x){
    uint16_t ui;
    int8_t ret;
    ui = *((uint16_t *)&x);
    fp16_t fp(ui);
    ret = fp;
    return ret;
}
int UInt8ToFP16(uint8_t ui){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    //printf("x=%u\n", ux);
    fp = ui;
    uiRet = fp.val;
    //printf("uiRet=%u\n", uiRet);
    ret = uiRet;
    //printf("ret=%d\n", ret);
    return ret;
}
int Int8ToFP16(int8_t i){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    //printf("x=%u\n", ux);
    fp = i;
    uiRet = fp.val;
    //printf("uiRet=%u\n", uiRet);
    ret = uiRet;
    //printf("ret=%d\n", ret);
    return ret;
}

int Int32ToFP16(int x){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    fp = x;
    uiRet = fp.val;
    ret = uiRet;
    return ret;
}
int UInt32ToFP16(uint32_t ux){
    uint16_t uiRet;
    int ret;
    fp16_t fp;
    fp = ux;
    uiRet = fp.val;
    ret = uiRet;
    return ret;
}

int Fp16Math(int x, FPMathMethodType mathType){
    PREPARE_ONE_FP16_PARA
    switch (mathType)
    {
        case MATH_RCP:      fpRet = hf_rcp(fp);     break;
        case MATH_SQRT:     fpRet = hf_sqrt(fp);    break;
        case MATH_RSQRT:    fpRet = hf_rsqrt(fp);   break;
        case MATH_ABS:      fpRet = hf_abs(fp);     break;
        case MATH_EXP:      fpRet = hf_exp(fp);     break;
        case MATH_LN:       fpRet = hf_ln(fp);      break;
        case MATH_LOG2:     fpRet = hf_log2(fp);    break;
        case MATH_LOG10:    fpRet = hf_log10(fp);   break;
        case MATH_POW2:     fpRet = hf_pow2(fp);    break;
        case MATH_POW10:    fpRet = hf_pow10(fp);   break;
        case MATH_SIN:      fpRet = hf_sin(fp);     break;
        case MATH_COS:      fpRet = hf_cos(fp);     break;
        default:            fpRet.val = 0;          break;
    }
    ret = fpRet.val;
    result = ret;
    return result;
}

int Max(int x,int y){
    PREPARE_TWO_FP16_PARA
    fpRet = hf_max(fp1,fp2);
    ret = fpRet.val;
    result = ret;
    return result;
}
int Min(int x, int y){
    PREPARE_TWO_FP16_PARA
    fpRet = hf_min(fp1, fp2);
    ret = fpRet.val;
    result = ret;
    return result;
}
int Mla(int x, int y, int z){
    uint16_t u1, u2, u3, ret;
    fp16_t fp1, fp2, fp3, fpRet;
    int result;
    u1 = *((uint16_t *)&x);
    u2 = *((uint16_t *)&y);
    u3 = *((uint16_t *)&z);
    fp1.val = u1;
    fp2.val = u2;
    fp3.val = u3;
    fpRet = hf_mla(fp1, fp2, fp3);
    ret = fpRet.val;
    result = ret;
    return result;
}
int Deq(uint32_t x, int sc){
    uint16_t scale = *((uint16_t *)&sc);
    fp16_t fpScale(scale);
    fp16_t fpRet = deq(x, fpScale);
    //printf("fpRet=%u\n", fpRet.val);
    uint16_t ret = fpRet.val;
    int result = ret;
    //printf("result=%d\n", result);
    return result;
}

uint32_t FMix(int x, int y, uint32_t z){
    uint16_t u1, u2;
    fp16_t fp1, fp2;
    float f3, fret;
    uint32_t result;
    u1 = *((uint16_t *)&x);
    u2 = *((uint16_t *)&y);
    f3 = *((float *)&z);
    fp1.val = u1;
    fp2.val = u2;
    fret = hf_mla(fp1, fp2, f3);
    result = *((uint32_t *)&fret);
    return result;
}

int MultAddFP16(int x[], int y[], int z)
{
    uint16_t u3,ur;
    fp16_t fp1[MATRIX_LENGTH], fp2[MATRIX_LENGTH],fp;
    fp16_t fpRet;
    int result;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        int tmp1 = x[i];
        int tmp2 = y[i];
        uint16_t tmpU1 = *((uint16_t *)&tmp1);
        uint16_t tmpU2 = *((uint16_t *)&tmp2);
        fp1[i].val = tmpU1;
        fp2[i].val = tmpU2;
    }
    u3 = *((uint16_t *)&z);
    fp.val = u3;
    fpRet = hf_mma_ex(fp1, fp2, fp, MATRIX_LENGTH);
    ur = fpRet.val;
    result = ur;
    return result;
}

uint32_t MultAddFP32(int x[], int y[], float z)
{
    uint32_t u3;
    fp16_t fp1[MATRIX_LENGTH], fp2[MATRIX_LENGTH];
    float f, fRet;
    uint32_t result;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        int tmp1 = x[i];
        int tmp2 = y[i];
        uint16_t tmpU1 = *((uint16_t *)&tmp1);
        uint16_t tmpU2 = *((uint16_t *)&tmp2);
        fp1[i].val = tmpU1;
        fp2[i].val = tmpU2;
    }
    fRet = hf_mma_ex(fp1, fp2, z, MATRIX_LENGTH);

    result = *((uint32_t *)&fRet);
    //printf("fRet=%f,result=%u\n",fRet,result);
    return result;
}

uint32_t FMax(uint32_t ux, uint32_t uy) {
    PREPARE_TWO_FP32_PARA
    ret = (x > y) ? ux : uy;
    return ret;
}
uint32_t FMin(uint32_t ux, uint32_t uy) {
    PREPARE_TWO_FP32_PARA
    ret = (x < y) ? ux : uy;
    return ret;
}

int32_t FP32ToInt32(uint32_t ux){
    float x;
    x = *((float*)&ux);
    int32_t ret = x;
    return ret;
}
int32_t FloatToInt32(float x){
    int32_t ret = (int32_t)x;
    return ret;
}
uint32_t Int32ToFP32(int32_t x){
    uint32_t ret;
    float f = (float)x;
    ret = *((uint32_t *)&f);
    return ret;
}
float Int32ToFloat(int32_t x){
    float f = (float)x;
    return f;
}

PyObject* WrappEQ(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, EQUAL));
}
PyObject* WrappNE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, NOT_EQUAL));
}
PyObject* WrappGT(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, GREATER_THAN));
}
PyObject* WrappGE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, GREATER_EQUAL));
}
PyObject* WrappLT(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, LESS_THAN));
}
PyObject* WrappLE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Compare(x, y, LESS_EQUAL));
}

PyObject* WrappAdd(PyObject* self, PyObject* args)
{
    /*int x, y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y))
    {
        return NULL;
    }*/
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Add(x, y));
}
PyObject* WrappSub(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Sub(x, y));
}
PyObject* WrappMul(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Mul(x, y));
}
PyObject* WrappDiv(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Div(x, y));
}

PyObject* WrappFP16ToFP32(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    uint32_t ui = FP16ToFP32(x);
    return Py_BuildValue("l", ui);
}

PyObject* WrappFP16ToFloat(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    float f = FP16ToFloat(x);
    double d = f;
    return Py_BuildValue("d", d);
}

PyObject* WrappFP32ToFP16(PyObject* self, PyObject* args)
{
    uint32_t ux;
    if (!PyArg_ParseTuple(args, "l", &ux))
    {
        return NULL;
    }
    //printf("c=%u\n", ux);
    int32_t ret = FP32ToFP16(ux);
    return Py_BuildValue("i", ret);
}

PyObject* WrappFloatToFP16(PyObject* self, PyObject* args)
{
    double d;
    if (!PyArg_ParseTuple(args, "d", &d))
    {
        return NULL;
    }
    float f = (float)d;
    //printf("c=%u\n", ux);
    int32_t ret = FloatToFP16(f);
    return Py_BuildValue("i", ret);
}

PyObject* WrappFP16ToInt32F(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    int32_t i = FP16ToInt32F(x);
    return Py_BuildValue("i", i);
}

PyObject* WrappFP16ToInt32C(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    int32_t i = FP16ToInt32C(x);
    return Py_BuildValue("i", i);
}

PyObject* WrappFP16ToInt32(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    int32_t i = FP16ToInt32(x);
    return Py_BuildValue("i", i);
}

PyObject* WrappFP16ToUInt8(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    uint8_t ui = FP16ToUInt8(x);
    return Py_BuildValue("i", ui);
}

PyObject* WrappFP16ToInt8(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    int8_t i = FP16ToInt8(x);
    return Py_BuildValue("i", i);
}

PyObject* WrappUInt8ToFP16(PyObject* self, PyObject* args)
{
    uint8_t ui;
    if (!PyArg_ParseTuple(args, "i", &ui))
    {
        return NULL;
    }
    //printf("c=%u\n", ui);
    int32_t ret = UInt8ToFP16(ui);
    return Py_BuildValue("i", ret);
}

PyObject* WrappInt8ToFP16(PyObject* self, PyObject* args)
{
    int8_t i;
    if (!PyArg_ParseTuple(args, "i", &i))
    {
        return NULL;
    }
    //printf("c=%u\n", ui);
    int32_t ret = Int8ToFP16(i);
    return Py_BuildValue("i", ret);
}

PyObject* WrappUInt32ToFP16(PyObject* self, PyObject* args)
{
    uint32_t ui;
    if (!PyArg_ParseTuple(args, "l", &ui))
    {
        return NULL;
    }
    //printf("c=%u\n", ui);
    int32_t ret = UInt32ToFP16(ui);
    return Py_BuildValue("i", ret);
}

PyObject* WrappInt32ToFP16(PyObject* self, PyObject* args)
{
    int32_t i;
    if (!PyArg_ParseTuple(args, "i", &i))
    {
        return NULL;
    }
    //printf("c=%u\n", ui);
    int32_t ret = Int32ToFP16(i);
    return Py_BuildValue("i", ret);
}

PyObject* WrappMultAddFP16(PyObject* self, PyObject* args)
{
    int x[MATRIX_LENGTH];
    int y[MATRIX_LENGTH];
    int z;
    PyObject *arrA, *arrB;
    if (!PyArg_ParseTuple(args, "OOi", &arrA, &arrB, &z))
    {
        return NULL;
    }
    int lenA = PyList_Size(arrA);
    int lenB = PyList_Size(arrB);
    if (MATRIX_LENGTH != lenA || MATRIX_LENGTH != lenB){
        return NULL;
    }
    int tmp;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        PyObject* tmpObjA = PyList_GetItem(arrA, i);
        PyArg_Parse(tmpObjA, "i", &tmp);
        x[i] = tmp;
        PyObject* tmpObjB = PyList_GetItem(arrB, i);
        PyArg_Parse(tmpObjB, "i", &tmp);
        y[i] = tmp;
    }
    //printf("WrappMultAddFP16,line=%d\n", __LINE__);
    return Py_BuildValue("i", MultAddFP16(x, y, z));
}

PyObject* WrappMultAddFP32(PyObject* self, PyObject* args)
{
    int x[MATRIX_LENGTH];
    int y[MATRIX_LENGTH];
    double d;
    unsigned int c;
    float f;
    PyObject *arrA;
    PyObject *arrB;
    if (!PyArg_ParseTuple(args, "OOd", &arrA, &arrB, &d))
    {
        return NULL;
    }
    int lenA = PyList_Size(arrA);
    int lenB = PyList_Size(arrB);
    if (MATRIX_LENGTH != lenA || MATRIX_LENGTH != lenB){
        return NULL;
    }
    int tmp;
    for (int i = 0; i < MATRIX_LENGTH; i++){
        PyObject* tmpObjA = PyList_GetItem(arrA, i);
        PyArg_Parse(tmpObjA, "i", &tmp);
        x[i] = tmp;
        PyObject* tmpObjB = PyList_GetItem(arrB, i);
        PyArg_Parse(tmpObjB, "i", &tmp);
        y[i] = tmp;
    }
    c = (unsigned int)d;
    f = *((float*)&c);
    //printf("c=%u,f=%f\n", c,f);
    unsigned int ret = MultAddFP32(x, y, f);
    return Py_BuildValue("l", ret);
}

PyObject* WrappRcp(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_RCP));
}
PyObject* WrappSqrt(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_SQRT));
}
PyObject* WrappRSqrt(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_RSQRT));
}
PyObject* WrappAbs(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_ABS));
}
PyObject* WrappExp(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_EXP));
}
PyObject* WrappLn(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_LN));
}
PyObject* WrappLog2(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_LOG2));
}
PyObject* WrappLog10(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_LOG10));
}
PyObject* WrappPow2(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_POW2));
}
PyObject* WrappPow10(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_POW10));
}
PyObject* WrappSin(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_SIN));
}
PyObject* WrappCos(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    return Py_BuildValue("i", Fp16Math(x, MATH_COS));
}
PyObject* WrappMax(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Max(x,y));
}
PyObject* WrappMin(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_INT_PARA
    return Py_BuildValue("i", Min(x, y));
}
PyObject* WrappDeq(PyObject* self, PyObject* args)
{
    uint32_t x;
    int y;
    if (!PyArg_ParseTuple(args, "li", &x, &y))
    {
        return NULL;
    }
    int ret = Deq(x, y);
    return Py_BuildValue("i", ret);
}
PyObject* WrappMla(PyObject* self, PyObject* args)
{
    int x,y,z;                                    
    if (!PyArg_ParseTuple(args, "iii", &x, &y, &z))
    {                                        
        return NULL;                            
    }
    return Py_BuildValue("i", Mla(x,y,z));
}
PyObject* WrappFMix(PyObject* self, PyObject* args)
{
    int x, y;
    uint32_t z;
    if (!PyArg_ParseTuple(args, "iil", &x, &y, &z))
    {
        return NULL;
    }
    uint32_t ui = FMix(x, y, z);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFMla(PyObject* self, PyObject* args)
{
    uint32_t x, y, z;
    if (!PyArg_ParseTuple(args, "lll", &x, &y, &z))
    {
        return NULL;
    }
    uint32_t ui = fp32Fma(x, y, z);
    return Py_BuildValue("l", ui);
}

PyObject* WrappFEQ(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Eq(ux, uy));
}
PyObject* WrappFNE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Ne(ux, uy));
}
PyObject* WrappFGT(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Gt(ux, uy));
}
PyObject* WrappFGE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Ge(ux, uy));
}
PyObject* WrappFLT(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Lt(ux, uy));
}
PyObject* WrappFLE(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    return Py_BuildValue("i", (int)fp32Le(ux, uy));
}

PyObject* WrappFAdd(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Add(ux,uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFSub(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Sub(ux, uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFMul(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Mul(ux, uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFDiv(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Div(ux, uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFMax(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Max(ux, uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFMin(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_TWO_UINT_PARA
    uint32_t ui = fp32Min(ux, uy);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFRcp(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Rcp(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFSqrt(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Sqrt(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFRSqrt(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32RSqrt(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFAbs(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Abs(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFExp(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Exp(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFLn(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Ln(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFLog2(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Log2(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFLog10(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Log10(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFPow2(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Pow2(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFPow10(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Pow10(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFSin(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Sin(ux);
    return Py_BuildValue("l", ui);
}
PyObject* WrappFCos(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_UINT_PARA
    uint32_t ui = fp32Cos(ux);
    return Py_BuildValue("l", ui);
}

PyObject* WrappFP32ToInt32(PyObject* self, PyObject* args)
{
    uint32_t ux;
    if (!PyArg_ParseTuple(args, "l", &ux))
    {
        return NULL;
    }
    int32_t ret = FP32ToInt32(ux);
    return Py_BuildValue("i", ret);
}
PyObject* WrappFloatToInt32(PyObject* self, PyObject* args)
{
    double d;
    if (!PyArg_ParseTuple(args, "d", &d))
    {
        return NULL;
    }
    float f = (float)d;
    int32_t ret = FloatToInt32(f);
    return Py_BuildValue("i", ret);
}

PyObject* WrappInt32ToFP32(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    uint32_t ui = Int32ToFP32(x);
    return Py_BuildValue("l", ui);
}
PyObject* WrappInt32ToFloat(PyObject* self, PyObject* args)
{
    PREPARE_WRAPP_ONE_INT_PARA
    float f = Int32ToFloat(x);
    double d = f;
    return Py_BuildValue("d", d);
}

static PyMethodDef fpy_methods[] = {
    { "EQ",           WrappEQ,  METH_VARARGS, "fp16_t if-equal comparison" },
    { "NE",           WrappNE,  METH_VARARGS, "fp16_t not-equal comparison" },
    { "GT",           WrappGT,  METH_VARARGS, "fp16_t greater-than comparison" },
    { "GE",           WrappGE,  METH_VARARGS, "fp16_t greater-equal comparison" },
    { "LT",           WrappLT,  METH_VARARGS, "fp16_t less-than comparison" },
    { "LE",           WrappLE,  METH_VARARGS, "fp16_t less-equal comparison" },
    { "Add",          WrappAdd, METH_VARARGS, "performing fp16_t addition" },
    { "Sub",          WrappSub, METH_VARARGS, "performing fp16_t subtraction" },
    { "Mul",          WrappMul, METH_VARARGS, "performing fp16_t multiplication" },
    { "Div",          WrappDiv, METH_VARARGS, "performing fp16_t division" },
    { "FP16ToFP32",   WrappFP16ToFP32,   METH_VARARGS, "convert fp16_t to float(uint32_t format)" },
    { "FP16ToFloat",  WrappFP16ToFloat,  METH_VARARGS, "convert fp16_t to float(float format)" },
    { "FP16ToUInt8",  WrappFP16ToUInt8,  METH_VARARGS, "convert fp16_t to uint8_t" },
    { "FP16ToInt8",   WrappFP16ToInt8,   METH_VARARGS, "convert fp16_t to int8_t" },
    { "FP16ToInt32F", WrappFP16ToInt32F, METH_VARARGS, "convert fp16_t to int32_t, with round down mode" },
    { "FP16ToInt32C", WrappFP16ToInt32C, METH_VARARGS, "convert fp16_t to int32_t, with round up mode" },
    { "FP16ToInt32",  WrappFP16ToInt32,  METH_VARARGS, "convert fp16_t to int32_t, with round to nearest, tie to even mode" },
    { "FP32ToFP16",   WrappFP32ToFP16,   METH_VARARGS, "convert float(uint32_t format) to fp16_t" },
    { "FloatToFP16",  WrappFloatToFP16,  METH_VARARGS, "convert float(float format) to fp16_t" },
    { "UInt8ToFP16",  WrappUInt8ToFP16,  METH_VARARGS, "convert uint8_t to fp16_t" },
    { "Int8ToFP16",   WrappInt8ToFP16,   METH_VARARGS, "convert int8_t to fp16_t" },
    { "UInt32ToFP16", WrappUInt32ToFP16, METH_VARARGS, "convert uint32_t to fp16_t" },
    { "Int32ToFP16",  WrappInt32ToFP16,  METH_VARARGS, "convert int32_t to fp16_t" },
    { "MultAddFP16",  WrappMultAddFP16,  METH_VARARGS, "fused array multiplier and adders, both adder and output are fp16_t" },
    { "MultAddFP32",  WrappMultAddFP32,  METH_VARARGS, "fused array multiplier and adders, both adder and output are float(uint32_t format)" },
    { "Rcp",          WrappRcp,   METH_VARARGS, "calculates fp16_t reciprocal" },
    { "Sqrt",         WrappSqrt,  METH_VARARGS, "calculates fp16_t square root" },
    { "RSqrt",        WrappRSqrt, METH_VARARGS, "calculates fp16_t reciprocal square root" },
    { "Abs",          WrappAbs,   METH_VARARGS, "calculates fp16_t absolute value" },
    { "Exp",          WrappExp,   METH_VARARGS, "calculates fp16_t natural exponential" },
    { "Ln",           WrappLn,    METH_VARARGS, "calculates fp16_t  natural logarithm" },
    { "Log2",         WrappLog2,  METH_VARARGS, "calculates fp16_t binary logarithm" },
    { "Log10",        WrappLog10, METH_VARARGS, "calculates fp16_t decimal logarithm" },
    { "Pow2",         WrappPow2,  METH_VARARGS, "calculates fp16_t binary exponential" },
    { "Pow10",        WrappPow10, METH_VARARGS, "calculates fp16_t decimal exponential" },
    { "Sin",          WrappSin,   METH_VARARGS, "calculates fp16_t sine" },
    { "Cos",          WrappCos,   METH_VARARGS, "calculates fp16_t cosine" },
    { "Max",          WrappMax,   METH_VARARGS, "calculates the maximum fp16_t" },
    { "Min",          WrappMin,   METH_VARARGS, "calculates the minimum fp16_t" },
    { "Deq",          WrappDeq,   METH_VARARGS, "DEQ:convert_s32_to_f16*DEQSCALE, and the result exponent + 17" },
    /*fp16_t*fp16_t+fp16_t=fp16_t*/
    { "Mla",          WrappMla,   METH_VARARGS, "fused multiplier and adders, multiplier keeps the full precision" },
    /*fp16_t*fp16_t+float=float(both float param and output are in uint32_t format)*/
    { "FMix",         WrappFMix,  METH_VARARGS, "fused multiplier and adders, multiplier keeps the full precision" },
    /*float*float+float=float(both float param and output are in uint32_t format)*/
    { "FMla",         WrappFMla,  METH_VARARGS, "fused multiplier and adders, multiplier keeps the full precision" },
    { "FEQ",          WrappFEQ,   METH_VARARGS, "float(uint32_t format) if-equal comparison" },
    { "FNE",          WrappFNE,   METH_VARARGS, "float(uint32_t format) not-equal comparison" },
    { "FGT",          WrappFGT,   METH_VARARGS, "float(uint32_t format) greater-than comparison" },
    { "FGE",          WrappFGE,   METH_VARARGS, "float(uint32_t format) greater-equal comparison" },
    { "FLT",          WrappFLT,   METH_VARARGS, "float(uint32_t format) less-than comparison" },
    { "FLE",          WrappFLE,   METH_VARARGS, "float(uint32_t format) less-equal comparison" },
    { "FAdd",         WrappFAdd,  METH_VARARGS, "performing float(uint32_t format) addition" },
    { "FSub",         WrappFSub,  METH_VARARGS, "performing float(uint32_t format) subtraction" },
    { "FMul",         WrappFMul,  METH_VARARGS, "performing float(uint32_t format) multiplication" },
    { "FDiv",         WrappFDiv,  METH_VARARGS, "performing float(uint32_t format) division" },
    { "FRcp",         WrappFRcp,  METH_VARARGS, "calculates float(uint32_t format) reciprocal" },
    { "FSqrt",        WrappFSqrt, METH_VARARGS, "calculates float(uint32_t format) square root" },
    { "FRSqrt",       WrappFRSqrt,METH_VARARGS, "calculates float(uint32_t format) reciprocal square root" },
    { "FAbs",         WrappFAbs,  METH_VARARGS, "calculates float(uint32_t format) absolute value" },
    { "FExp",         WrappFExp,  METH_VARARGS, "calculates float(uint32_t format) natural exponential" },
    { "FLn",          WrappFLn,   METH_VARARGS, "calculates float(uint32_t format) natural logarithm" },
    { "FLog2",        WrappFLog2, METH_VARARGS, "calculates float(uint32_t format) binary logarithm" },
    { "FLog10",       WrappFLog10,METH_VARARGS, "calculates float(uint32_t format) decimal logarithm" },
    { "FPow2",        WrappFPow2, METH_VARARGS, "calculates float(uint32_t format) binary exponential" },
    { "FPow10",       WrappFPow10,METH_VARARGS, "calculates float(uint32_t format) decimal exponential" },
    { "FSin",         WrappFSin,  METH_VARARGS, "calculates float(uint32_t format) sine" },
    { "FCos",         WrappFCos,  METH_VARARGS, "calculates float(uint32_t format) cosine" },
    { "FMax",         WrappFMax,  METH_VARARGS, "calculates the maximum float(uint32_t format)" },
    { "FMin",         WrappFMin,  METH_VARARGS, "calculates the minimum float(uint32_t format)" },
    { "FP32ToInt32",  WrappFP32ToInt32,  METH_VARARGS, "convert float(uint32_t format) to int32_t" },
    { "FloatToInt32", WrappFloatToInt32, METH_VARARGS, "convert float(float format) to int32_t" },
    { "Int32ToFP32",  WrappInt32ToFP32,  METH_VARARGS, "convert int32_t to float(uint32_t format)" },
    { "Int32ToFloat", WrappInt32ToFloat, METH_VARARGS, "convert int32_t to float(float format)" },
    {NULL, NULL}
};


#if PY_MAJOR_VERSION >= 3
/*
addList is the module name, and this is the initialization block of the module.
<desired module name>, <the-info-table>, <module's-docstring>
*/
static struct PyModuleDef fpy =
{
    PyModuleDef_HEAD_INIT,
    "fpy",        /* name of module */
    "",         /* module documentation, may be NULL */
    -1,         /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    fpy_methods
};
extern "C"
PyMODINIT_FUNC PyInit_fpy(void)
{
    return PyModule_Create(&fpy);
}

#else

extern "C"
void initfpy()
{
    Py_InitModule("fpy", fpy_methods);
}
#endif

/***compile command：****************************************************************************************************/
/***g++ -std=c++11 -fPIC -shared fp16_t.cc fp16_math.cc fp16_unit.cc fp32_t.cc fpy.cpp -I/usr/include/python2.7 -o fpy.so**********/
/***g++ -std=c++11 -fPIC -shared fp16_t.cc fp16_math.cc fp16_unit.cc fp32_t.cc fpy.cpp -I/usr/include/python3.5 -o fpy.so**********/
/************************************************************************************************************************/
