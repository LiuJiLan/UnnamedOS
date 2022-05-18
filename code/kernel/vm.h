//
//  vm.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/27.
//

#ifndef vm_h
#define vm_h

#include "types.h"

//  变量说明:
//  k / u: kernel / user
//  p / v: physical / virtual
//  a:     address
//  pgtbl: page table
//  pte:   page table entry
//  perm:  permission (RWX and something else)

//  注意所有pgtbl使用的是内核页表的虚拟地址

//  注意riscv所有的分页都是12 + N * 9的模式
//  所以每个表项必然是512项, 4KiB大

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access

//  MAX User Virtual Address
//  注1:与页表的种类相关, 例如Sv48这个值为(1L << (48 - 1))
//      PS:此处我们只讨论RV64的页表
//  注2:其实MAXUVA应该为(1L << (39 - 1)) - 1
//  注3:(写给不熟RV的同学)
//  RV虚拟地址的最高位(例Sv39就是第39位)会符号扩展,
//  只有这样的才是合法虚拟地址, 而用户进程位于低地址空间
#define MAXUVA (1L << (39 - 1))

typedef uint64 pte_t;
typedef pte_t* pgtbl_t;

void kvminit(void);
int kvmmap(uptr_t pa, uint64 sz, int perm);
void vm_2_kpgtbl(void);

pgtbl_t vm_init_upgtbl(void);
void vm_delete_upgtbl(pgtbl_t upgtbl);
int vm_kva_map_uva(pgtbl_t upgtbl, uptr_t kva, uptr_t uva);
void * vm_uva_inverse_kva(pgtbl_t upgtbl, uptr_t uva);
int vm_memmove(pgtbl_t upgtbl, uptr_t kva, uptr_t uva, size_t n, int dir);
int vm_deep_copy(pgtbl_t u_dst, pgtbl_t u_src, uptr_t uva_pg_start, int PG_SZ);
void vm_2_proc_upgtbl(pgtbl_t upgtbl);

#endif /* vm_h */

//  所有分配多个页的函数其实应该递归的实现(或类似的正确的实现)
//  因为这样可以在中途失败时回收之前的页
//  例如我的操作需要分配4个页, 编号0~3
//  那么递归函数可以写成
//    i = 0
//
//    func(i) -> int:
//        pgtbl[i] = alloc    # 只是抽象的表示第几个页
//            if alloc_successful:
//                if is_the_last_to_alloc:
//                    return 0
//                ret = func(i+1)
//                if ret == -1:
//                    free(pgtbl[i])  # 失败就回收
//                else
//                    return 0
//            else
//                return -1
//
//        return 0

//  例如现在proc中的proc_load_bin函数中的实现就是一种错误实现
//  或者参考xv6-riscv中读盘的设计:
//    static int
//    alloc3_desc(int *idx)
//    {
//      for(int i = 0; i < 3; i++){
//        idx[i] = alloc_desc();
//        if(idx[i] < 0){
//          for(int j = 0; j < i; j++)
//            free_desc(idx[j]);
//          return -1;
//        }
//      }
//      return 0;
//    }
