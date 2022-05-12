import struct
import os

in_path = "../user/src/init.bin"
out_path = "./initcode.S"

try:
    file_in = open(in_path, 'rb')
except Exception as e:
    print("出现错误: ", e)
    print("1. 请检查init.bin文件是否创建。")
    print("可在user目录下make init 创建。")
    print("2. 或检查当前目录: ", os.getcwd())
    exit(-1)
    
file_out = open(out_path, 'w')

file_out.write(".data\n")
file_out.write(".global initcode\n")
file_out.write("initcode:\n")

while True:
    raw = file_in.read(1)
    if len(raw) == 0:
        break
    else:
        byte2int = raw + b'\x00\x00\x00'  # 小端序 # python对byte很不友好
        file_out.write(".byte 0x%x\n" % struct.unpack("<i", byte2int)[0])
    
file_out.write(".global initcode_len\n")
file_out.write("initcode_len:\n")
file_out.write(".8byte 0x%x\n" % os.stat(in_path).st_size)

file_in.close()
file_out.close()
