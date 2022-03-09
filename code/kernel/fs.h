//
//  fs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/3/2.
//

#ifndef fs_h
#define fs_h

//  fs.h文件记录了xv6fs文件系统的结构
//  也为创建一个虚拟的xv6fs文件系统打下基础
//  我们暂时使用的是xv6中Makefile中有关mkfs的部分
//  (运行在HOST机上)
//  由于我们要重新编译, 并有一些修改
//  1、fs.h文件中struct dirent在MacOS上与本地的<sys/dirent.h>相冲突
//  2、在MacOS下会引发"index(argv[i], '/') == 0"的断言,
//      注释掉之后能运行, 但不知道会不会有影响
//  考虑到通用性, 我们之后改为使用Python这样的脚本语言

//  为了保护mkfs结果的一致性, 所以记得要__attribute__((packed))

#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

struct superblock {
    unsigned int size;         // Size of file system image (blocks)
    unsigned int nblocks;      // Number of data blocks
    unsigned int ninodes;      // Number of inodes.
    unsigned int nlog;         // Number of log blocks
    unsigned int logstart;     // Block number of first log block
    unsigned int inodestart;   // Block number of first inode block
    unsigned int bmapstart;    // Block number of first free map block
}__attribute__((packed));

//  dinode的一些内部结构[书P340]
//  12个直接索引, 一个间接索引
//  间接索引为指向一个block,
//  所以那个block又可以存BSIZE / sizeof(unsigned int)个
//  所以算下来xv6fs最大文件大小是70KiB;

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(unsigned int))
#define MAXFILE (NDIRECT + NINDIRECT)

struct dinode {
    short type;           // File type
    short major;          // Major device number (T_DEV only)
    short minor;          // Minor device number (T_DEV only)
    short nlink;          // Number of links to inode in file system
    unsigned int size;            // Size of file (bytes)
    unsigned int addrs[NDIRECT+1];   // Data block addresses
}__attribute__((packed));

// Inodes per block.
//  每个block可以存储都少个struct dinode;
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
//  计算是第几个block存储的目标struct dinode;
//  sb = superblock
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b/BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
//  观察struct dirent;的结构, DIRSIZ + inum的2 bytes正好是16 bytes
#define DIRSIZ 14

struct dirent {
    unsigned short inum;
    char name[DIRSIZ];
}__attribute__((packed));

#endif /* fs_h */
