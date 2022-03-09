import struct

kernel = open("../out/kernel.elf", 'rb')

print(".data")
print(".global kernel_elf")
print("kernel_elf:")

while True:
    raw = kernel.read(1)
    if len(raw) == 0:
        break
    else:
        byte2int = raw + b'\x00\x00\x00'  # 小端序 # python对byte很不友好
        print(".byte 0x%x" % struct.unpack("i", byte2int)[0])
    

kernel.close()
