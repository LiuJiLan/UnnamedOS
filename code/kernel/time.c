//
//  time.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/19.
//

#include "time.h"
#include "param.h"
#include "proc.h"
#include "vm.h"
#include "string.h"

extern void panic(char * s);

static uint64 NMTI_PER_NSEC;
static uint64 NNSEC_PER_MTI;
#define POW_TEN_NINE 1000000000ULL  //  一定要标注ULL
//  BUG原因, 除法的时候前几个常量的位宽太小

void (* time_tick)(void);

static volatile uint64 mti_cnt;
static volatile uint64 nano_cnt;

struct sleep_entry{
    volatile enum {UNSET = 0, SET} is_set;
    uint64 sec;
    uint64 nsec;
};

struct {
    struct sleep_entry sleeptbl[NPROC];
    struct spinlock lock;
} ksleep;

void time_tick_frequency_major(void);
void time_tick_time_major(void);

void time_init(void) {
    ktime.nsec = 0;
    ktime.sec = 0;
    mti_cnt = 0;
    nano_cnt = 0;
    NMTI_PER_NSEC = MECHINE_FREQUENCY / (DEFAULT_INTERVAL * POW_TEN_NINE) + 1;
    NNSEC_PER_MTI = DEFAULT_INTERVAL * POW_TEN_NINE / MECHINE_FREQUENCY;
    
    if (NMTI_PER_NSEC == 1) {
        time_tick = time_tick_time_major;
    } else {
        time_tick = time_tick_frequency_major;
    }
    
    initlock(&ktime.lock, "time");
    memset(&ksleep, 0, sizeof(ksleep));
    initlock(&ksleep.lock, "sleep");
}

void time_tick_frequency_major(void) {
    //  频率很高
    mti_cnt++;
    if (mti_cnt != 0 && mti_cnt % NMTI_PER_NSEC == 0) {
        mti_cnt -= NMTI_PER_NSEC;
        
        acquire(&ktime.lock);
        ktime.nsec++;
        if (ktime.nsec != 0 && ktime.nsec % POW_TEN_NINE == 0) {
            ktime.nsec -= POW_TEN_NINE;
            ktime.sec++;
        }
        release(&ktime.lock);
    }
}

void time_tick_time_major(void) {
    //  一次中断会度过多个nano second
    nano_cnt += NNSEC_PER_MTI;
    
    acquire(&ktime.lock);
    
    ktime.sec += nano_cnt / POW_TEN_NINE;
    nano_cnt %= POW_TEN_NINE;
    ktime.nsec += nano_cnt;
    
    release(&ktime.lock);
}

uint64 time_get_nsec(void) {
    acquire(&ktime.lock);
    uint64 ret = ktime.nsec;
    release(&ktime.lock);
    return ret;
}

uint64 time_get_sec(void) {
    acquire(&ktime.lock);
    uint64 ret = ktime.sec;
    release(&ktime.lock);
    return ret;
}



void time_set_clock(uint64 sec, uint64 nsec, pid_t pid) {
    struct sleep_entry * p = &ksleep.sleeptbl[pid];
    acquire(&ktime.lock);
    acquire(&ksleep.lock);
    
    uint64 sleep_nsec = ktime.nsec + nsec;
    uint64 sleep_sec = sleep_nsec / POW_TEN_NINE + sec;
    sleep_nsec %= POW_TEN_NINE;
    
    p->is_set = SET;
    p->sec = sleep_sec;
    p->nsec = sleep_nsec;
    
    release(&ksleep.lock);
    release(&ktime.lock);
}

void ringtest(char * s) {};

void time_ring_clock(void) {
    //  不用给ktime上锁, 因为这个只由hart0来维护
    //  调用这个函数时时钟一定不会滴答
    struct sleep_entry * p = &ksleep.sleeptbl[0];
    
    //  预存防止每次都重复读取
    uint64 sec = ktime.sec;
    uint64 nsec = ktime.nsec;
    
    //  FOR DEBUG
    char str[10] = "ring:0;";
    
    acquire(&ksleep.lock);
    for (pid_t i = 0; i < NPROC; i++) {
        struct sleep_entry * q = p + i;//  预存防止每次都重复读取
        if (q->is_set == SET && sec >= q->sec && nsec >= q->nsec) {
            //  注意判断顺序, 用&&的短路操作来实现比较
            q->is_set = UNSET;
            proc_wakeup_proc(i);
            
            //  FOR DEBUG
            str[5] += i;
            panic(str);
            ringtest(str);
            str[5] = '0';
        }
    }
    release(&ksleep.lock);
}

typedef struct
{
    uint64 sec;  // 自 Unix 纪元起的秒数
    uint64 usec; // 微秒数
} TimeVal;

void sys_nanosleep(struct proc * proc) {
    //  注意, nanosleep的返回值本来应该是在唤醒的时候再去写入的
    //  BUG!!!
    TimeVal in_time;
    TimeVal out_time = {0, 0};
    vm_memmove(proc->upgtbl, (uptr_t)&in_time, proc->context.a0, sizeof(TimeVal), 1);
    vm_memmove(proc->upgtbl, (uptr_t)&out_time, proc->context.a1, sizeof(TimeVal), 0);
    proc->context.a0 = 0;
    proc->context.sepc += 4;
    
    pid_t mypid = proc->pid;
    
    //  FOR DEBUG
    char str[10] = "clks:0;";
    str[5] += mypid;
    panic(str);
    
    proc_acquire_proctbl_lock();
    proc_acquire_proc_lock(mypid);
    proc_release_proctbl_lock();
    proc->state = INTERRUPTIBLE;
    proc_release_proc_lock(mypid);
    
    time_set_clock(in_time.sec, in_time.usec, mypid);
    //  BUG!!!由于比赛的原因, 这里的传值是有问题的
    
    //  注意不能使用proc_sleep_proc(mypid)
    //  由于proc_sleep_proc会导致运行proc_find_runnable_to_run,
    //  所以必须先运行time_set_clock
    //  但是有可能当time_set_clock之后hart0的time_ring_clock立刻抢到锁
    //  这会导致可能会导致这个进程被重复睡眠
    //  (sys_nanosleep还没有睡眠这个进程, 就先被唤醒了,
    //  接着sys_nanosleep拿到了锁又睡眠了)
    
    proc_find_runnable_to_run(mypid);
}

void sys_gettimeofday(struct proc * proc) {
    TimeVal temp;
    temp.sec = time_get_sec();
    temp.usec = time_get_nsec();
    vm_memmove(proc->upgtbl, (uptr_t)&temp, proc->context.a0, sizeof(TimeVal), 0);
    proc->context.a0 = 0;
    proc->context.sepc += 4;
}
