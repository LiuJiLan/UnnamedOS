//
//  defs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/18.
//

#ifndef defs_h
#define defs_h

#include "type.h"
#include "proc.h"
#include "spinlock.h"

//开头有/**/的函数表示参数相较xv6有所改动或者是自己添加的

//main
/**/regs_t get_mhartid(void);

//plic.c
/**/void plicinit(void);

struct buf;
struct context;
//    struct file;
//    struct inode;
//    struct pipe;
struct proc;
//    struct rtcdate;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;

//    // bio.c
//    void            binit(void);
//    struct buf*     bread(uint, uint);
//    void            brelse(struct buf*);
//    void            bwrite(struct buf*);
//
//    // console.c
//    void            consoleinit(void);
//    void            cprintf(char*, ...);
//    void            consoleintr(int(*)(void));
//    void            panic(char*) __attribute__((noreturn));
void            panic(char*);   //  暂时用来debug, 未实现
//  有些时候当断点用, 暂时还需要返回


//    // exec.c
//    int             exec(char*, char**);
//
//    // file.c
//    struct file*    filealloc(void);
//    void            fileclose(struct file*);
//    struct file*    filedup(struct file*);
//    void            fileinit(void);
//    int             fileread(struct file*, char*, int n);
//    int             filestat(struct file*, struct stat*);
//    int             filewrite(struct file*, char*, int n);
//
//    // fs.c
//    void            readsb(int dev, struct superblock *sb);
//    int             dirlink(struct inode*, char*, uint);
//    struct inode*   dirlookup(struct inode*, char*, uint*);
//    struct inode*   ialloc(uint, short);
//    struct inode*   idup(struct inode*);
//    void            iinit(int dev);
//    void            ilock(struct inode*);
//    void            iput(struct inode*);
//    void            iunlock(struct inode*);
//    void            iunlockput(struct inode*);
//    void            iupdate(struct inode*);
//    int             namecmp(const char*, const char*);
//    struct inode*   namei(char*);
//    struct inode*   nameiparent(char*, char*);
//    int             readi(struct inode*, char*, uint, uint);
//    void            stati(struct inode*, struct stat*);
//    int             writei(struct inode*, char*, uint, uint);
//
//    // ide.c
//    void            ideinit(void);
//    void            ideintr(void);
//    void            iderw(struct buf*);
//
//    // ioapic.c
//    void            ioapicenable(int irq, int cpu);
//    extern uchar    ioapicid;
//    void            ioapicinit(void);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*, void*);
void            kinit2(void*, void*);

//    // kbd.c
//    void            kbdintr(void);
//
//    // lapic.c
//    void            cmostime(struct rtcdate *r);
//    int             lapicid(void);
//    extern volatile uint*    lapic;
//    void            lapiceoi(void);
//    void            lapicinit(void);
//    void            lapicstartap(uchar, uint);
//    void            microdelay(int);
//
//    // log.c
//    void            initlog(int dev);
//    void            log_write(struct buf*);
//    void            begin_op();
//    void            end_op();

// mp.c
extern int      ismp;
void            mpinit(void);

//    // picirq.c
//    void            picenable(int);
//    void            picinit(void);
//
//    // pipe.c
//    int             pipealloc(struct file**, struct file**);
//    void            pipeclose(struct pipe*, int);
//    int             piperead(struct pipe*, char*, int);
//    int             pipewrite(struct pipe*, char*, int);


// proc.c
//    int             cpuid(void);
//    void            exit(void);
//    int             fork(void);
//    int             growproc(int);
//    int             kill(int);
struct cpu*     mycpu(void);
//    struct proc*    myproc();
void            pinit(void);
//    void            procdump(void);
//    void            scheduler(void) __attribute__((noreturn));
//    void            sched(void);
//    void            setproc(struct proc*);
//    void            sleep(void*, struct spinlock*);
//    void            userinit(void);
//    int             wait(void);
//    void            wakeup(void*);
//    void            yield(void);
//
//    // swtch.S
//    void            swtch(struct context**, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
//void            getcallerpcs(void*, uint*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
int             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

//  string.h的参数参考
//  https://opensource.apple.com/source/xnu/xnu-6153.41.3/osfmk/libsa/string.h.auto.html
//  safestrcpy()除外

// string.c
/**/int             memcmp(const void *, const void *, size_t);
/**/void*           memmove(void *, const void *, size_t);
/**/void*           memset(void *, int, size_t);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char *);
/**/int             strncmp(const char *, const char *, size_t);
/**/char*           strncpy(char *, const char *, size_t);


//    // syscall.c
//    int             argint(int, int*);
//    int             argptr(int, char**, int);
//    int             argstr(int, char**);
//    int             fetchint(uint, int*);
//    int             fetchstr(uint, char**);
//    void            syscall(void);
//
//    // timer.c
//    void            timerinit(void);
//
//    // trap.c
//    void            idtinit(void);
//    extern uint     ticks;
//    void            tvinit(void);
//    extern struct spinlock tickslock;
//
//    // uart.c
//    void            uartinit(void);
//    void            uartintr(void);
//    void            uartputc(int);

// vm.c
//    void            seginit(void);
void            kvmalloc(void);
pde_t*          setupkvm(void);
//    char*           uva2ka(pde_t*, char*);
//    int             allocuvm(pde_t*, uint, uint);
//    int             deallocuvm(pde_t*, uint, uint);
//    void            freevm(pde_t*);
//    void            inituvm(pde_t*, char*, uint);
//    int             loaduvm(pde_t*, char*, struct inode*, uint, uint);
//    pde_t*          copyuvm(pde_t*, uint);
//    void            switchuvm(struct proc*);
void            switchkvm(void);
//    int             copyout(pde_t*, uint, void*, uint);
//    void            clearpteu(pde_t *pgdir, char *uva);






//  获取定长数组的元素个数
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#endif /* defs_h */
