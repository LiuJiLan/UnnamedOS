//
//  stat.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/3/4.
//

#ifndef stat_h
#define stat_h

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

struct stat {
    short type;  // Type of file
    int dev;     // File system's disk device
    unsigned int ino;    // Inode number
    short nlink; // Number of links to file
    unsigned int size;   // Size of file in bytes
};

#endif /* stat_h */
