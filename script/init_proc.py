import struct
import os

in_path = "./code/user/src/init.bin"
out_path = "./code/kernel/initcode.S"

print("预先运行 OBJDUMP 导出二进制.bin文件")
file_in = open(in_path, 'rb')
file_out = open(out_path, 'w')

file_out.write(".data")
file_out.write(".global initcode")
file_out.write("initcode:")

while True:
    raw = kernel.read(1)
    if len(raw) == 0:
        break
    else:
        byte2int = raw + b'\x00\x00\x00'  # 小端序 # python对byte很不友好
        file_out.write(".byte 0x%x" % struct.unpack("<i", byte2int)[0])
    
file_out.write(".global initcode_len")
file_out.write("initcode_len:")
file_out.write(".8byte 0x%x" % os.stat(in_path).st_size)

kernel.close()
