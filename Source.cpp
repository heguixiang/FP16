#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <string>
//#include <math.h>
#include "fp16_t.h"


using namespace std;

typedef unsigned long long ULL;
template <typename T>
class Number {
    //http://blog.csdn.net/qq_20480611/article/details/52564530
private:
    T t;
public:
    Number(T t = 0) :t(t) {}
    Number(string& bitstr)
    {
        t = parseNumber(bitstr);
    }
    Number(const char* bitstr)
    {
        t = parseNumber(bitstr);
    }
    Number& operator = (string& bitstr)
    {
        t = parseNumber(bitstr);
        return *this;
    }

    string toString()
    {
        int pos = 8 * sizeof(T);
        char* bitsequence = (char*)malloc(pos + 1);
        memset(bitsequence, 0, pos + 1);

        bitsequence[pos] = 0;
        ULL n = *(ULL*)&t;
        while (pos--) {
            bitsequence[pos] = (n & 1) + '0';
            n >>= 1;
        }
        return bitsequence;
    }

    T parseNumber(const string& bitstr)
    {
        ULL n = bitstr[0] - '0';
        for (unsigned i = 1; i < bitstr.length(); i++) {
            n <<= 1;
            n |= bitstr[i] - '0';
        }
        return *(T*)&n;
    }

    T getVal()
    {
        return t;
    }
};

//#define COMMON

void fToFp(float f){
    fp16_t fp;
    fp = f;
    Number<uint16_t> ui = fp.val;
    cout << "float :" << f << " -> fp16  (uint16_t):" << fp.val<<" binary:"<< ui.toString()<< endl;
}

void fpToF(int val){
    fp16_t fp;
    fp.val = val;
    float f = fp;
    cout << "fp16(uint16_t value):" << val << " convert to float is:" << f << endl;
}

void main(int argc, char* argv[]){
    if (argc == 2){
        float f = atof(argv[1]);
        fToFp(f);
    }
    else if (argc == 3){
        int type = atoi(argv[1]);
        if (type){
            float f = atof(argv[2]);
            fToFp(f);
        }
        else{
            int input = atoi(argv[2]);
            fpToF(input);
        }
    }
    else if(argc==1){
        int tp;
        while (true){
            cout << "��0�� fp16->float , otherwise float->fp16\nconvert type:";
            cin >> tp;
            if (tp){
                cout << "input a float number:";
                float f;
                cin >> f;
                fToFp(f);
            }
            else{
                cout << "input a fp16 (uint16_t) number:";
                int input;
                cin >> input;
                fpToF(input);
            }
        }
    }

    return;
}
