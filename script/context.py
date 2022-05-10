context_org = """
unsigned long ra;
unsigned long sp;
unsigned long gp;
//unsigned long tp;
unsigned long t0;
unsigned long t1;
unsigned long t2;
unsigned long s0;
unsigned long s1;
unsigned long a0;
unsigned long a1;
unsigned long a2;
unsigned long a3;
unsigned long a4;
unsigned long a5;
unsigned long a6;
unsigned long a7;
unsigned long s2;
unsigned long s3;
unsigned long s4;
unsigned long s5;
unsigned long s6;
unsigned long s7;
unsigned long s8;
unsigned long s9;
unsigned long s10;
unsigned long s11;
unsigned long t3;
unsigned long t4;
unsigned long t5;
unsigned long t6;
unsigned long sepc; //  相当于保存了pc
"""

names = []

context_entry = context_org.split("\n")

for entry in context_entry:
    if entry != "":
        #print(entry.split(" "))
        names.append(entry.split(" ")[2].split(";")[0])
        
for name in names:
    print("proc_context->", end="")
    print(name, end="")
    print(" = ", end="")
    print("regs->", end="")
    print(name, end="")
    print(";\n", end="")

"""
print("\n++++++++++\n")

for name in names:
    print("proc_context->", end="")
    print(name, end="")
    print(" = 0;\n", end="")
"""
    
    
print("\n----------\n")

for name in names:
    print("regs->", end="")
    print(name, end="")
    print(" = ", end="")
    print("proc_context->", end="")
    print(name, end="")
    print(";\n", end="")

"""
print("\n++++++++++\n")

for name in names:
    print("regs->", end="")
    print(name, end="")
    print(" = 0;\n", end="")
"""

