#include "type.h"
#include "harddisk.h"
#include "disk.h"
#include "fsmem.h"
#include "screen.h"
#if 0
typedef struct tag_par{
    u8 boot_signature;
    u8 start_head;
    u8 start_sector;
    u8 start_cylinder;
    u8 system_signature;
    u8 end_head;
    u8 end_sector;
    u8 end_cylinder;
    u32 nrb_before;
    u32 nrb_inside;
} partition_t;

static void show_partition(partition_t *ppt) {
    kprintf("boot sig:%x, sys sig:%x\n", ppt->boot_signature, ppt->system_signature);
    kprintf("start head:%d, start sector:%d, start cylinder:%d\n", ppt->start_head, ppt->start_sector, ppt->start_cylinder);
    kprintf("end head:%d, end sector:%d, end cylinder:%d\n", ppt->end_head, ppt->end_sector, ppt->end_cylinder);
    kprintf("nrb before:%d, nrb inside:%d\n", ppt->nrb_before, ppt->nrb_inside);
}

void show_boot_sector(u8* buf) {
    kprintf("\nshow_boot_sector Last Word:%x\n", (buf[510]<< 8 | buf[511]));
    partition_t *ppt = (partition_t *)((u8*)buf + 0x1BE);
    for (int idx = 0; idx < 2; idx++) {
        show_partition(ppt);
    }
    kprintf("show partition table.\n");
}


u32 get_first_partition_offset(u8 *buf) {
    kprintf("get_first_partition_offset\n");
    partition_t *ppt = (partition_t *)((u8*)buf + 0x1BE);
    return ppt->nrb_before;
}

#include <stdint.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int s32;

// little endian is default for intel x86
typedef uint32_t __le32;
typedef uint16_t __le16;
typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;


struct ext2_super_block
{
    /*
        total number of inodes, both used and free, in the file system
        It must be equal to the sum of the inodes defined in each block group.
        This value must be lower or equal to (s_inodes_per_group * number of block groups)
    */
    __le32 s_inodes_count; /* Inodes count */
    /*
        the total number of blocks in the system including all used, free and reserved.
        This value must be lower or equal to (s_blocks_per_group * number of block groups).
        It can be lower than the previous calculation if the last block group has a smaller number of blocks than s_blocks_per_group du to volume size.
        It must be equal to the sum of the blocks defined in each block group
    */
    __le32 s_blocks_count; /* Blocks count */
    /*
        the total number of blocks reserved for the usage of the super user.
    */
    __le32 s_r_blocks_count; /* Reserved blocks count */

    // This is a sum of all free blocks of all the block groups.
    __le32 s_free_blocks_count; /* Free blocks count */

    // This is a sum of all free inodes of all the block groups.
    __le32 s_free_inodes_count; /* Free inodes count */

    // the id of the block containing the superblock structure.
    __le32 s_first_data_block; /* First Data Block */

    // block size = 1024 << s_log_block_size;
    // 0 for BLOCK_SIZE = 1024
    __le32 s_log_block_size; /* Block size */

    // equals to s_log_block_size
    __le32 s_log_frag_size; /* Fragment size */

    // the total number of blocks per group
    __le32 s_blocks_per_group; /* # Blocks per group */

    // equals to s_blocks_per_group
    __le32 s_frags_per_group; /* # Fragments per group */

    // the total number of inodes per group
    // 8 * BLOCK_SIZE = 1024 * 8 = 8192
    __le32 s_inodes_per_group; /* # Inodes per group */

    __le32 s_mtime;           /* Mount time */
    __le32 s_wtime;           /* Write time */
    __le16 s_mnt_count;       /* Mount count */
    __le16 s_max_mnt_count;   /* Maximal mount count */
    __le16 s_magic;           /* Magic signature */
    __le16 s_state;           /* File system state ,16bit value indicating the file system state. When the file system is mounted, this state is set to EXT2_ERROR_FS. After the file system was cleanly unmounted, this value is set to EXT2_VALID_FS. */
    __le16 s_errors;          /* Behaviour when detecting errors , 16bit value indicating what the file system driver should do when an error is detected. */
    __le16 s_minor_rev_level; /* minor revision level */
    __le32 s_lastcheck;       /* time of last check */
    __le32 s_checkinterval;   /* max. time between checks */
    __le32 s_creator_os;      /* OS */
    __le32 s_rev_level;       /* Revision level */
    __le16 s_def_resuid;      /* Default uid for reserved blocks ,16bit value used as the default user id for reserved blocks.*/
    __le16 s_def_resgid;      /* Default gid for reserved blocks */
    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     *
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    // EXT2_GOOD_OLD_FIRST_INO  =  11
    __le32 s_first_ino; /* First non-reserved inode */

    //
    __le16 s_inode_size;             /* size of inode structure */
    __le16 s_block_group_nr;         /* block group # of this superblock */
    __le32 s_feature_compat;         /* compatible feature set */
    __le32 s_feature_incompat;       /* incompatible feature set */
    __le32 s_feature_ro_compat;      /* readonly-compatible feature set */
    __u8 s_uuid[16];                 /* 128-bit uuid for volume */
    char s_volume_name[16];          /* volume name */
    char s_last_mounted[64];         /* directory where last mounted */
    __le32 s_algorithm_usage_bitmap; /* For compression */
    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_COMPAT_PREALLOC flag is on.
     */
    __u8 s_prealloc_blocks;     /* Nr of blocks to try to preallocate*/
    __u8 s_prealloc_dir_blocks; /* Nr to preallocate for dirs */
    __u16 s_padding1;
    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    __u8 s_journal_uuid[16]; /* uuid of journal superblock */
    __u32 s_journal_inum;    /* inode number of journal file */
    __u32 s_journal_dev;     /* device number of journal file */
    __u32 s_last_orphan;     /* start of list of inodes to delete */
    __u32 s_hash_seed[4];    /* HTREE hash seed */
    __u8 s_def_hash_version; /* Default hash version to use */
    __u8 s_reserved_char_pad;
    __u16 s_reserved_word_pad;
    __le32 s_default_mount_opts;
    __le32 s_first_meta_bg; /* First metablock block group */
    __u32 s_reserved[190];  /* Padding to the end of the block */
} __attribute__((packed));

typedef struct ext2_super_block super_block_t;
#define trace kprintf
void show_super(super_block_t *super) {
    if (super) {
        trace("=====Super Block=====\n");
        trace("volume name:%s, uuid:%x-%x-%x-%x\n", super->s_volume_name, super->s_uuid[0],
            super->s_uuid[1],super->s_uuid[2],super->s_uuid[3]);
        trace("inode count:%d, avail inode:%d\n", super->s_inodes_count, super->s_free_inodes_count);
        trace("block count:%d, avail block:%d\n", super->s_blocks_count, super->s_free_blocks_count);
        trace("log :%d, first data block:%d\n", super->s_log_block_size, super->s_first_data_block);
        trace("magic num:%x, inode size: %d\n", super->s_magic, super->s_inode_size);
        trace("s_first_ino:%d, \n", super->s_first_ino);
    }
}

void init_hd() {
    disk_t disk;
    disk.major = 1;
    disk.minor = 0;
    disk.partition = 0;

    u8 *buf = (u8*)kmalloc(512);
    read_sector(&disk, 0, 1, buf);
    show_boot_sector(buf);
    u32 offset = get_first_partition_offset(buf);
    kfree(buf);
    u8 *super_buffer = (u8*)kmalloc(1024);
    read_sector(&disk, offset + 2, 2, super_buffer);
    show_super((super_block_t*)super_buffer);
    kfree(super_buffer);
}
#endif //
