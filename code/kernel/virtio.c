//
//  virtio.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

#include "virtio.h"
#include "memlayout.h"
#include "types.h"
#include "spinlock.h"
#include "string.h"
#include "proc.h"

extern void panic(char * s);

#define VIRTIO0 P2V_WO(VIRTIO0_BASE)

#define VIRTIO_MMIO_MAGIC_VALUE        0x000 // 0x74726976
#define VIRTIO_MMIO_VERSION        0x004 // version; 1 is legacy
#define VIRTIO_MMIO_DEVICE_ID        0x008 // device type; 1 is net, 2 is disk
#define VIRTIO_MMIO_VENDOR_ID        0x00c // 0x554d4551
#define VIRTIO_MMIO_DEVICE_FEATURES    0x010
#define VIRTIO_MMIO_DRIVER_FEATURES    0x020
#define VIRTIO_MMIO_GUEST_PAGE_SIZE    0x028 // page size for PFN, write-only
#define VIRTIO_MMIO_QUEUE_SEL        0x030 // select queue, write-only
#define VIRTIO_MMIO_QUEUE_NUM_MAX    0x034 // max size of current queue, read-only
#define VIRTIO_MMIO_QUEUE_NUM        0x038 // size of current queue, write-only
#define VIRTIO_MMIO_QUEUE_ALIGN        0x03c // used ring alignment, write-only
#define VIRTIO_MMIO_QUEUE_PFN        0x040 // physical page number for queue, read/write
#define VIRTIO_MMIO_QUEUE_READY        0x044 // ready bit
#define VIRTIO_MMIO_QUEUE_NOTIFY    0x050 // write-only
#define VIRTIO_MMIO_INTERRUPT_STATUS    0x060 // read-only
#define VIRTIO_MMIO_INTERRUPT_ACK    0x064 // write-only
#define VIRTIO_MMIO_STATUS        0x070 // read/write

// status register bits, from qemu virtio_config.h
#define VIRTIO_CONFIG_S_ACKNOWLEDGE    1
#define VIRTIO_CONFIG_S_DRIVER        2
#define VIRTIO_CONFIG_S_DRIVER_OK    4
#define VIRTIO_CONFIG_S_FEATURES_OK    8

// device feature bits
#define VIRTIO_BLK_F_RO              5    /* Disk is read-only */
#define VIRTIO_BLK_F_SCSI            7    /* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE     11    /* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ             12    /* support more than one vq */
#define VIRTIO_F_ANY_LAYOUT         27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#define NUM 8

// a single descriptor, from the spec.
struct virtq_desc {
    uint64 addr;
    uint32 len;
    uint16 flags;
    uint16 next;
};
#define VRING_DESC_F_NEXT  1 // chained with another descriptor
#define VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
struct virtq_avail {
    uint16 flags; // always zero
    uint16 idx;   // driver will write ring[idx] next
    uint16 ring[NUM]; // descriptor numbers of chain heads
    uint16 unused;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
struct virtq_used_elem {
    uint32 id;   // index of start of completed descriptor chain
    uint32 len;
};

struct virtq_used {
    uint16 flags; // always zero
    uint16 idx;   // device increments when it adds a ring[] entry
    struct virtq_used_elem ring[NUM];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#define VIRTIO_BLK_T_IN  0 // read the disk
#define VIRTIO_BLK_T_OUT 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
struct virtio_blk_req {
    uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
    uint32 reserved;
    uint64 sector;
};










#define R(r) ((volatile uint32 *)(VIRTIO0 + (r)))

static struct disk {
    // the virtio driver and device mostly communicate through a set of
    // structures in RAM. pages[] allocates that memory. pages[] is a
    // global (instead of calls to kalloc()) because it must consist of
    // two contiguous pages of page-aligned physical memory.
    char pages[2*PGSIZE];
    
    // pages[] is divided into three regions (descriptors, avail, and
    // used), as explained in Section 2.6 of the virtio specification
    // for the legacy interface.
    // https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
    
    // the first region of pages[] is a set (not a ring) of DMA
    // descriptors, with which the driver tells the device where to read
    // and write individual disk operations. there are NUM descriptors.
    // most commands consist of a "chain" (a linked list) of a couple of
    // these descriptors.
    // points into pages[].
    struct virtq_desc *desc;
    
    // next is a ring in which the driver writes descriptor numbers
    // that the driver would like the device to process.  it only
    // includes the head descriptor of each chain. the ring has
    // NUM elements.
    // points into pages[].
    struct virtq_avail *avail;
    
    // finally a ring in which the device writes descriptor numbers that
    // the device has finished processing (just the head of each chain).
    // there are NUM used ring entries.
    // points into pages[].
    struct virtq_used *used;
    
    // our own book-keeping.
    char free[NUM];  // is a descriptor free?
    uint16 used_idx; // we've looked this far in used[2..NUM].
    
    // track info about in-flight operations,
    // for use when completion interrupt arrives.
    // indexed by first descriptor index of chain.
    struct {
        struct buf *b;
        char status;
    } info[NUM];
    
    // disk command headers.
    // one-for-one with descriptors, for convenience.
    struct virtio_blk_req ops[NUM];
    
    struct spinlock vdisk_lock;
    
} __attribute__ ((aligned (PGSIZE))) disk;

void
virtio_disk_init(void)
{
    uint32 status = 0;
    
    initlock(&disk.vdisk_lock, "virtio_disk");
    
    if(*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
       *R(VIRTIO_MMIO_VERSION) != 1 ||
       *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
       *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
        panic("could not find virtio disk");
    }
    
    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    *R(VIRTIO_MMIO_STATUS) = status;
    
    status |= VIRTIO_CONFIG_S_DRIVER;
    *R(VIRTIO_MMIO_STATUS) = status;
    
    // negotiate features
    uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;
    
    // tell device that feature negotiation is complete.
    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    *R(VIRTIO_MMIO_STATUS) = status;
    
    // tell device we're completely ready.
    status |= VIRTIO_CONFIG_S_DRIVER_OK;
    *R(VIRTIO_MMIO_STATUS) = status;
    
    *R(VIRTIO_MMIO_GUEST_PAGE_SIZE) = PGSIZE;
    
    // initialize queue 0.
    *R(VIRTIO_MMIO_QUEUE_SEL) = 0;
    uint32 max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
    if(max == 0)
        panic("virtio disk has no queue 0");
    if(max < NUM)
        panic("virtio disk max queue too short");
    *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;
    memset(disk.pages, 0, sizeof(disk.pages));
    *R(VIRTIO_MMIO_QUEUE_PFN) = ((uint64)disk.pages) >> PGSHIFT;
    
    // desc = pages -- num * virtq_desc
    // avail = pages + 0x40 -- 2 * uint16, then num * uint16
    // used = pages + 4096 -- 2 * uint16, then num * vRingUsedElem
    
    disk.desc = (struct virtq_desc *) disk.pages;
    disk.avail = (struct virtq_avail *)(disk.pages + NUM*sizeof(struct virtq_desc));
    disk.used = (struct virtq_used *) (disk.pages + PGSIZE);
    
    // all NUM descriptors start out unused.
    for(int i = 0; i < NUM; i++)
        disk.free[i] = 1;
    
    // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
static int
alloc_desc()
{
    for(int i = 0; i < NUM; i++){
        if(disk.free[i]){
            disk.free[i] = 0;
            return i;
        }
    }
    return -1;
}

// mark a descriptor as free.
static void
free_desc(int i)
{
    if(i >= NUM)
        panic("free_desc 1");
    if(disk.free[i])
        panic("free_desc 2");
    disk.desc[i].addr = 0;
    disk.desc[i].len = 0;
    disk.desc[i].flags = 0;
    disk.desc[i].next = 0;
    disk.free[i] = 1;
    proc_wakeup_for_reason(&disk.free[0]);
}

// free a chain of descriptors.
static void
free_chain(int i)
{
    while(1){
        int flag = disk.desc[i].flags;
        int nxt = disk.desc[i].next;
        free_desc(i);
        if(flag & VRING_DESC_F_NEXT)
            i = nxt;
        else
            break;
    }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
static int
alloc3_desc(int *idx)
{
    for(int i = 0; i < 3; i++){
        idx[i] = alloc_desc();
        if(idx[i] < 0){
            for(int j = 0; j < i; j++)
                free_desc(idx[j]);
            return -1;
        }
    }
    return 0;
}

void
virtio_disk_rw(struct buf *b, int write)
{
    uint64 sector = b->blockno * (BSIZE / 512);
    
    acquire(&disk.vdisk_lock);
    
    // the spec's Section 5.2 says that legacy block operations use
    // three descriptors: one for type/reserved/sector, one for the
    // data, one for a 1-byte status result.
    
    // allocate the three descriptors.
    int idx[3];
    while(1){
        if(alloc3_desc(idx) == 0) {
            break;
        }
        proc_sleep_for_reason(&disk.vdisk_lock, &disk.free[0]);
    }
    
    // format the three descriptors.
    // qemu's virtio-blk.c reads them.
    
    struct virtio_blk_req *buf0 = &disk.ops[idx[0]];
    
    if(write)
        buf0->type = VIRTIO_BLK_T_OUT; // write the disk
    else
        buf0->type = VIRTIO_BLK_T_IN; // read the disk
    buf0->reserved = 0;
    buf0->sector = sector;
    
    disk.desc[idx[0]].addr = (uint64) buf0;
    disk.desc[idx[0]].len = sizeof(struct virtio_blk_req);
    disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
    disk.desc[idx[0]].next = idx[1];
    
    disk.desc[idx[1]].addr = (uint64) b->data;
    disk.desc[idx[1]].len = BSIZE;
    if(write)
        disk.desc[idx[1]].flags = 0; // device reads b->data
    else
        disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
    disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
    disk.desc[idx[1]].next = idx[2];
    
    disk.info[idx[0]].status = 0xff; // device writes 0 on success
    disk.desc[idx[2]].addr = (uint64) &disk.info[idx[0]].status;
    disk.desc[idx[2]].len = 1;
    disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
    disk.desc[idx[2]].next = 0;
    
    // record struct buf for virtio_disk_intr().
    b->disk = 1;
    disk.info[idx[0]].b = b;
    
    // tell the device the first index in our chain of descriptors.
    disk.avail->ring[disk.avail->idx % NUM] = idx[0];
    
    __sync_synchronize();
    
    // tell the device another avail ring entry is available.
    disk.avail->idx += 1; // not % NUM ...
    
    __sync_synchronize();
    
    *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number
    
    // Wait for virtio_disk_intr() to say request has finished.
    while(b->disk == 1) {
        proc_sleep_for_reason(&disk.vdisk_lock, b);
    }
    
    disk.info[idx[0]].b = 0;
    free_chain(idx[0]);
    
    release(&disk.vdisk_lock);
}

void virtio_disk_intr(void)
{
    acquire(&disk.vdisk_lock);
    
    // the device won't raise another interrupt until we tell it
    // we've seen this interrupt, which the following line does.
    // this may race with the device writing new entries to
    // the "used" ring, in which case we may process the new
    // completion entries in this interrupt, and have nothing to do
    // in the next interrupt, which is harmless.
    *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;
    
    __sync_synchronize();
    
    // the device increments disk.used->idx when it
    // adds an entry to the used ring.
    
    while(disk.used_idx != disk.used->idx){
        __sync_synchronize();
        int id = disk.used->ring[disk.used_idx % NUM].id;
        
        if(disk.info[id].status != 0)
            panic("virtio_disk_intr status");
        
        struct buf *b = disk.info[id].b;
        b->disk = 0;   // disk is done with buf
        proc_wakeup_for_reason(b);
        
        disk.used_idx += 1;
    }
    
    release(&disk.vdisk_lock);
}
