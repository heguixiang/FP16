#encoding:utf-8
#compile command
#python 2.7   g++ -std=c++11 -fPIC -shared compress.cc zip.cpp -I/usr/include/python2.7 -o compress.so
#python 3.5   g++ -std=c++11 -fPIC -shared compress.cc zip.cpp -I/usr/include/python3.5 -o compress.so
import compress as coms
from array import array
import sys
import binascii as bin

print(sys.stdout.encoding)

def printByteArray(btArray):
    iLen=len(btArray)
    if(sys.version_info.major==3):
        for i in range(0,iLen):
            if(i%64==0):
                sys.stdout.write('\n')
            sys.stdout.write("%2x "%int(btArray[i]))
    else:
        i=0
        for item in btArray:
            x=int(hex(item),16)
            if(i%64==0):
                sys.stdout.write('\n')
            sys.stdout.write("%2x "%x)
            i=i+1
    sys.stdout.flush()
    print("")

#设置调试模式，打印日志
coms.SetDebugOn(True)

print("########Test InitDict\n")
printByteArray(bytearray("aaaaaabbbb", encoding = "utf8"))
print(coms.InitDict(bytearray("aaaaaabbbb", encoding = "utf8"),10))


print("read data from file\n")
in_file_name="data/rand-quant-2048.data"
in_file=open(in_file_name,'rb')
intput=in_file.read()  #.decode('utf-8')
iLen=in_file.tell()
print("python read data content: length="+str(iLen))
printByteArray(bytearray(intput))
print("\ncontent in bytearray format:")
print("type of read content:"+str(type(intput)))

print("read data finished!\n")

cmptype=1
print("########Test CompressFile\n")
#压缩 输入数据需要转换为bytearray
#压缩成功时：参数1为内部压缩函数返回值，参数二为压缩后的结果，参数三为压缩类型CMP_TYPE转换为整型数
#压缩失败时：参数1为内部压缩函数返回值，后续参数为None
com_res,com_data,com_type=coms.CompressFile(bytearray(intput),iLen,cmptype,1)
print("########CompressFile finished!")

coms.SetDebugOn(False)

print(com_res)
if(com_data!=None):
    if(com_type!=None):
        print("com_type="+str(com_type))
    if(com_data!=None):
        com_len=len(com_data)
        #print(com_data)
        print("com_len="+str(com_len))
        print("CompressedFile Result:\n")
        printByteArray(com_data)

        print("#########Test DeCompressFile\n")
        #解压 输入数据需要转换为bytearray 
        #解压成功时：参数1为内部压缩函数返回值，参数二为解压缩后的结果
        #解压失败时：参数1为内部压缩函数返回值，后续参数为None
        de_com_ret,de_com_data=coms.DeCompressFile(com_data,com_len)
        if(de_com_data!=None):
            origin_len=len(de_com_data)
            print("Origin data length=%d, Origin data:"%origin_len)
            if(origin_len!=iLen):
                print("Compress and DeCompress failed: inputLen=%d,decompressLen=%d"%(iLen,origin_len))
            else:
                printByteArray(de_com_data)  
            
            #与原始数据二进制比对
            hasNEqual=False

            if(sys.version_info.major==3):
                for i in range(0,origin_len):
                    if(intput[i]!=de_com_data[i]):
                        hasNEqual=True
                        print("index=%4d inputData:%2x DeCompressData:%2x"%(i,intput[i],de_com_data[i]))
            else:
                i=0
                for item in bytearray(intput):
                    x=int(hex(item),16)
                    if(x!=de_com_data[i]):
                        hasNEqual=True
                        print("index=%4d inputData:%2x DeCompressData:%2x"%(i,x,de_com_data[i]))
                    i=i+1
            print("")
            if(hasNEqual):
                print("Compress and DeCompress failed!")
            else:
                print("Compress and DeCompress successed!")
print("Test Release\n")
coms.Release()
print("Test Finished!!!")
