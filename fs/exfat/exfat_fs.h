// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2012-2013 Samsung Electronics Co., Ltd.
 */

#ifndef _EXFAT_FS_H
#define _EXFAT_FS_H

<<<<<<< HEAD
#include <linux/types.h>
#include <linux/magic.h>
#include <asm/byteorder.h>

#ifndef MSDOS_SUPER_MAGIC
#define MSDOS_SUPER_MAGIC       0x4d44          /* MD */
#endif

#ifndef EXFAT_SUPER_MAGIC
#define EXFAT_SUPER_MAGIC       (0x2011BAB0UL)
#endif /* EXFAT_SUPER_MAGIC */
=======
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/ratelimit.h>
#include <linux/nls.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
#include <linux/magic.h>
#else
#define EXFAT_SUPER_MAGIC       0x2011BAB0UL
#endif

#define EXFAT_VERSION		"5.19.1"

#define EXFAT_ROOT_INO		1
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)

#ifndef EXFAT_SUPER_MAGIC
#define EXFAT_SUPER_MAGIC       (0x5EC5DFA4UL)
#endif /* EXFAT_SUPER_MAGIC */

#define EXFAT_ROOT_INO          1

/* FAT types */
#define FAT12                   0x01    // FAT12
#define FAT16                   0x0E    // Win95 FAT16 (LBA)
#define FAT32                   0x0C    // Win95 FAT32 (LBA)
#define EXFAT                   0x07    // exFAT

/* directory file name */
#define DOS_CUR_DIR_NAME        ".          "
#define DOS_PAR_DIR_NAME        "..         "

#ifdef __LITTLE_ENDIAN
#define UNI_CUR_DIR_NAME        ".\0"
#define UNI_PAR_DIR_NAME        ".\0.\0"
#else
#define UNI_CUR_DIR_NAME        "\0."
#define UNI_PAR_DIR_NAME        "\0.\0."
#endif

/* file name lengths */
/* NOTE :
 * The maximum length of input or output is limited to 256 including NULL,
 * But we allocate 4 extra bytes for utf8 translation reside in last position,
 * because utf8 can uses memory upto 6 bytes per one character.
 * Therefore, MAX_CHARSET_SIZE supports upto 6 bytes for utf8
 */
#define MAX_UNINAME_BUF_SIZE       (((MAX_NAME_LENGTH+1)*2)+4)
#define MAX_DOSNAME_BUF_SIZE       ((DOS_NAME_LENGTH+2)+6)
#define MAX_VFSNAME_BUF_SIZE       ((MAX_NAME_LENGTH+1)*MAX_CHARSET_SIZE)
#define MAX_CHARSET_SIZE        6       // max size of multi-byte character
#define MAX_NAME_LENGTH         255     // max len of file name excluding NULL
#define DOS_NAME_LENGTH         11      // DOS file name length excluding NULL

#define SECTOR_SIZE_BITS	9	/* VFS sector size is 512 bytes */

#define DENTRY_SIZE		32	/* directory entry size */
#define DENTRY_SIZE_BITS	5

#define MAX_FAT_DENTRIES	65536   /* FAT allows 65536 directory entries */
#define MAX_EXFAT_DENTRIES	8388608 /* exFAT allows 8388608(256MB) directory entries */

/* PBR entries */
#define PBR_SIGNATURE	0xAA55
#define EXT_SIGNATURE	0xAA550000
#define VOL_LABEL	"NO NAME    " /* size should be 11 */
#define OEM_NAME	"MSWIN4.1"  /* size should be 8 */
#define STR_FAT12	"FAT12   "  /* size should be 8 */
#define STR_FAT16	"FAT16   "  /* size should be 8 */
#define STR_FAT32	"FAT32   "  /* size should be 8 */
#define STR_EXFAT	"EXFAT   "  /* size should be 8 */

#define VOL_CLEAN	0x0000
#define VOL_DIRTY	0x0002

#define FAT_VOL_DIRTY	0x01

/* max number of clusters */
#define FAT12_THRESHOLD         4087        // 2^12 - 1 + 2 (clu 0 & 1)
#define FAT16_THRESHOLD         65527       // 2^16 - 1 + 2
#define FAT32_THRESHOLD         268435457   // 2^28 - 1 + 2
#define EXFAT_THRESHOLD         268435457   // 2^28 - 1 + 2

/* dentry types */
#define MSDOS_DELETED		0xE5	/* deleted mark */
#define MSDOS_UNUSED		0x00	/* end of directory */

#define EXFAT_UNUSED		0x00	/* end of directory */
#define IS_EXFAT_DELETED(x)	((x) < 0x80) /* deleted file (0x01~0x7F) */
#define EXFAT_INVAL		0x80	/* invalid value */
#define EXFAT_BITMAP		0x81	/* allocation bitmap */
#define EXFAT_UPCASE		0x82	/* upcase table */
#define EXFAT_VOLUME		0x83	/* volume label */
#define EXFAT_FILE		0x85	/* file or dir */
#define EXFAT_STREAM		0xC0	/* stream entry */
#define EXFAT_NAME		0xC1	/* file name entry */
#define EXFAT_ACL		0xC2	/* stream entry */

/* specific flag */
#define MSDOS_LAST_LFN		0x40

/* file attributes */
#define ATTR_NORMAL             0x0000
#define ATTR_READONLY           0x0001
#define ATTR_HIDDEN             0x0002
#define ATTR_SYSTEM             0x0004
#define ATTR_VOLUME             0x0008
#define ATTR_SUBDIR             0x0010
#define ATTR_ARCHIVE            0x0020
#define ATTR_SYMLINK            0x0040
#define ATTR_EXTEND             (ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
				 ATTR_VOLUME) /* 0x000F */

#define ATTR_EXTEND_MASK        (ATTR_EXTEND | ATTR_SUBDIR | ATTR_ARCHIVE)
#define ATTR_RWMASK             (ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME | \
				 ATTR_SUBDIR | ATTR_ARCHIVE | ATTR_SYMLINK)/* 0x007E */

/* file creation modes */
#define FM_REGULAR              0x00
#define FM_SYMLINK              0x40

/* time modes */
#define TM_CREATE               0
#define TM_MODIFY               1
#define TM_ACCESS               2

/* checksum types */
#define CS_DIR_ENTRY            0
#define CS_PBR_SECTOR           1
#define CS_DEFAULT              2

/*
 * ioctl command
 */
#define EXFAT_IOCTL_GET_VOLUME_ID	_IOR('r', 0x12, __u32)

/* FAT12/16 BIOS parameter block (64 bytes) */
typedef struct {
	__u8	jmp_boot[3];
	__u8	oem_name[8];

	__u8	sect_size[2];		/* unaligned */
	__u8	sect_per_clus;
	__le16	num_reserved;		/* . */
	__u8	num_fats;
	__u8	num_root_entries[2];	/* unaligned */
	__u8	num_sectors[2];		/* unaligned */
	__u8	media_type;
	__le16  num_fat_sectors;
	__le16  sectors_in_track;
	__le16  num_heads;
	__le32	num_hid_sectors;	/* . */
	__le32	num_huge_sectors;

	__u8	phy_drv_no;
	__u8	state;			/* used by WindowsNT for mount state */
	__u8	ext_signature;
	__u8	vol_serial[4];
	__u8	vol_label[11];
	__u8	vol_type[8];
	__le16	dummy;
} bpb16_t;

/* FAT32 BIOS parameter block (64 bytes) */
typedef struct {
	__u8	jmp_boot[3];
	__u8	oem_name[8];

	__u8	sect_size[2];		/* unaligned */
	__u8	sect_per_clus;
	__le16	num_reserved;
	__u8	num_fats;
	__u8	num_root_entries[2];	/* unaligned */
	__u8	num_sectors[2];		/* unaligned */
	__u8	media_type;
	__le16  num_fat_sectors;	/* zero */
	__le16  sectors_in_track;
	__le16  num_heads;
	__le32	num_hid_sectors;	/* . */
	__le32	num_huge_sectors;

	__le32	num_fat32_sectors;
	__le16	ext_flags;
	__u8	fs_version[2];
	__le32	root_cluster;		/* . */
	__le16	fsinfo_sector;
	__le16	backup_sector;
	__le16	reserved[6];		/* . */
} bpb32_t;

/* FAT32 EXTEND BIOS parameter block (32 bytes) */
typedef struct {
	__u8	phy_drv_no;
	__u8	state;			/* used by WindowsNT for mount state */
	__u8	ext_signature;
	__u8	vol_serial[4];
	__u8	vol_label[11];
	__u8	vol_type[8];
	__le16  dummy[3];
} bsx32_t;

/* EXFAT BIOS parameter block (64 bytes) */
typedef struct {
	__u8	jmp_boot[3];
	__u8	oem_name[8];
	__u8	res_zero[53];
} bpb64_t;

/* EXFAT EXTEND BIOS parameter block (56 bytes) */
typedef struct {
	__le64	vol_offset;
	__le64	vol_length;
	__le32	fat_offset;
	__le32	fat_length;
	__le32	clu_offset;
	__le32	clu_count;
	__le32	root_cluster;
	__le32	vol_serial;
	__u8	fs_version[2];
	__le16	vol_flags;
	__u8	sect_size_bits;
	__u8	sect_per_clus_bits;
	__u8	num_fats;
	__u8	phy_drv_no;
	__u8	perc_in_use;
	__u8	reserved2[7];
} bsx64_t;

/* FAT32 PBR (64 bytes) */
typedef struct {
	bpb16_t bpb;
} pbr16_t;

/* FAT32 PBR[BPB+BSX] (96 bytes) */
typedef struct {
	bpb32_t bpb;
	bsx32_t bsx;
} pbr32_t;

/* EXFAT PBR[BPB+BSX] (120 bytes) */
typedef struct {
	bpb64_t bpb;
	bsx64_t bsx;
} pbr64_t;

/* Common PBR[Partition Boot Record] (512 bytes) */
typedef struct {
	union {
		__u8	raw[64];
		bpb16_t f16;
		bpb32_t f32;
		bpb64_t f64;
	} bpb;
	union {
		__u8	raw[56];
		bsx32_t f32;
		bsx64_t f64;
	} bsx;
	__u8	boot_code[390];
	__le16	signature;
} pbr_t;

/* FAT32 filesystem information sector (512 bytes) */
typedef struct {
	__le32	signature1;              // aligned
	__u8	reserved1[480];
	__le32	signature2;              // aligned
	__le32	free_cluster;            // aligned
	__le32	next_cluster;            // aligned
	__u8    reserved2[14];
	__le16	signature3[2];
} fat32_fsi_t;

<<<<<<< HEAD
/* FAT directory entry (32 bytes) */
typedef struct {
	__u8       dummy[32];
} DENTRY_T;

typedef struct {
	__u8	name[DOS_NAME_LENGTH];	/* 11 chars */
	__u8	attr;
	__u8	lcase;
	__u8	create_time_ms;
	__le16	create_time;             // aligned
	__le16	create_date;             // aligned
	__le16	access_date;             // aligned
	__le16	start_clu_hi;            // aligned
	__le16	modify_time;             // aligned
	__le16	modify_date;             // aligned
	__le16	start_clu_lo;            // aligned
	__le32	size;                    // aligned
} DOS_DENTRY_T;
=======
struct exfat_dir_entry {
	struct exfat_chain dir;
	int entry;
	unsigned int type;
	unsigned int start_clu;
	unsigned char flags;
	unsigned short attr;
	loff_t size;
	unsigned int num_subdirs;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
	struct timespec64 atime;
	struct timespec64 mtime;
	struct timespec64 crtime;
#else
	struct timespec atime;
	struct timespec mtime;
	struct timespec crtime;
#endif
	struct exfat_dentry_namebuf namebuf;
};

/*
 * exfat mount in-memory data
 */
struct exfat_mount_options {
	kuid_t fs_uid;
	kgid_t fs_gid;
	unsigned short fs_fmask;
	unsigned short fs_dmask;
	/* permission for setting the [am]time */
	unsigned short allow_utime;
	/* charset for filename input/display */
	char *iocharset;
	/* on error: continue, panic, remount-ro */
	enum exfat_error_mode errors;
	unsigned utf8:1, /* Use of UTF-8 character set */
		 sys_tz:1, /* Use local timezone */
		 discard:1, /* Issue discard requests on deletions */
		 keep_last_dots:1; /* Keep trailing periods in paths */
	int time_offset; /* Offset of timestamps from UTC (in minutes) */
};
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)

/* FAT extended directory entry (32 bytes) */
typedef struct {
	__u8	order;
	__u8	unicode_0_4[10];
	__u8	attr;
	__u8	sysid;
	__u8	checksum;
	__le16	unicode_5_10[6];	// aligned
	__le16	start_clu;		// aligned
	__le16	unicode_11_12[2];	// aligned
} EXT_DENTRY_T;

/* EXFAT file directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	num_ext;
	__le16	checksum;		// aligned
	__le16	attr;			// aligned
	__le16	reserved1;
	__le16	create_time;		// aligned
	__le16	create_date;		// aligned
	__le16	modify_time;		// aligned
	__le16	modify_date;		// aligned
	__le16	access_time;		// aligned
	__le16	access_date;		// aligned
	__u8	create_time_ms;
	__u8	modify_time_ms;
	__u8	access_time_ms;
	__u8	reserved2[9];
} FILE_DENTRY_T;

/* EXFAT stream extension directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	flags;
	__u8	reserved1;
	__u8	name_len;
	__le16	name_hash;		// aligned
	__le16	reserved2;
	__le64	valid_size;		// aligned
	__le32	reserved3;		// aligned
	__le32	start_clu;		// aligned
	__le64	size;			// aligned
} STRM_DENTRY_T;

/* EXFAT file name directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	flags;
	__le16	unicode_0_14[15];	// aligned
} NAME_DENTRY_T;

/* EXFAT allocation bitmap directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	flags;
	__u8	reserved[18];
	__le32  start_clu;		// aligned
	__le64	size;			// aligned
} BMAP_DENTRY_T;

/* EXFAT up-case table directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	reserved1[3];
	__le32	checksum;		// aligned
	__u8	reserved2[12];
	__le32	start_clu;		// aligned
	__le64	size;			// aligned
} CASE_DENTRY_T;

/* EXFAT volume label directory entry (32 bytes) */
typedef struct {
	__u8	type;
	__u8	label_len;
	__le16	unicode_0_10[11];	// aligned
	__u8	reserved[8];
} VOLM_DENTRY_T;

<<<<<<< HEAD
#endif /* _EXFAT_FS_H */
=======
#define EXFAT_CACHE_VALID	0

/*
 * EXFAT file system inode in-memory data
 */
struct exfat_inode_info {
	struct exfat_chain dir;
	int entry;
	unsigned int type;
	unsigned short attr;
	unsigned int start_clu;
	unsigned char flags;
	/*
	 * the copy of low 32bit of i_version to check
	 * the validation of hint_stat.
	 */
	unsigned int version;

	/* hint for cluster last accessed */
	struct exfat_hint hint_bmap;
	/* hint for entry index we try to lookup next time */
	struct exfat_hint hint_stat;
	/* hint for first empty entry */
	struct exfat_hint_femp hint_femp;

	spinlock_t cache_lru_lock;
	struct list_head cache_lru;
	int nr_caches;
	/* for avoiding the race between alloc and free */
	unsigned int cache_valid_id;

	/*
	 * NOTE: i_size_ondisk is 64bits, so must hold ->inode_lock to access.
	 * physically allocated size.
	 */
	loff_t i_size_ondisk;
	/* block-aligned i_size (used in cont_write_begin) */
	loff_t i_size_aligned;
	/* on-disk position of directory entry or 0 */
	loff_t i_pos;
	/* hash by i_location */
	struct hlist_node i_hash_fat;
	/* protect bmap against truncate */
	struct rw_semaphore truncate_lock;
	struct inode vfs_inode;
	/* File creation time */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
	struct timespec64 i_crtime;
#else
	struct timespec i_crtime;
#endif
};

static inline struct exfat_sb_info *EXFAT_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct exfat_inode_info *EXFAT_I(struct inode *inode)
{
	return container_of(inode, struct exfat_inode_info, vfs_inode);
}

/*
 * If ->i_mode can't hold 0222 (i.e. ATTR_RO), we use ->i_attrs to
 * save ATTR_RO instead of ->i_mode.
 *
 * If it's directory and !sbi->options.rodir, ATTR_RO isn't read-only
 * bit, it's just used as flag for app.
 */
static inline int exfat_mode_can_hold_ro(struct inode *inode)
{
	struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);

	if (S_ISDIR(inode->i_mode))
		return 0;

	if ((~sbi->options.fs_fmask) & 0222)
		return 1;
	return 0;
}

/* Convert attribute bits and a mask to the UNIX mode. */
static inline mode_t exfat_make_mode(struct exfat_sb_info *sbi,
		unsigned short attr, mode_t mode)
{
	if ((attr & ATTR_READONLY) && !(attr & ATTR_SUBDIR))
		mode &= ~0222;

	if (attr & ATTR_SUBDIR)
		return (mode & ~sbi->options.fs_dmask) | S_IFDIR;

	return (mode & ~sbi->options.fs_fmask) | S_IFREG;
}

/* Return the FAT attribute byte for this inode */
static inline unsigned short exfat_make_attr(struct inode *inode)
{
	unsigned short attr = EXFAT_I(inode)->attr;

	if (S_ISDIR(inode->i_mode))
		attr |= ATTR_SUBDIR;
	if (exfat_mode_can_hold_ro(inode) && !(inode->i_mode & 0222))
		attr |= ATTR_READONLY;
	return attr;
}

static inline void exfat_save_attr(struct inode *inode, unsigned short attr)
{
	if (exfat_mode_can_hold_ro(inode))
		EXFAT_I(inode)->attr = attr & (ATTR_RWMASK | ATTR_READONLY);
	else
		EXFAT_I(inode)->attr = attr & ATTR_RWMASK;
}

static inline bool exfat_is_last_sector_in_cluster(struct exfat_sb_info *sbi,
		sector_t sec)
{
	return ((sec - sbi->data_start_sector + 1) &
		((1 << sbi->sect_per_clus_bits) - 1)) == 0;
}

static inline sector_t exfat_cluster_to_sector(struct exfat_sb_info *sbi,
		unsigned int clus)
{
	return ((sector_t)(clus - EXFAT_RESERVED_CLUSTERS) << sbi->sect_per_clus_bits) +
		sbi->data_start_sector;
}

static inline int exfat_sector_to_cluster(struct exfat_sb_info *sbi,
		sector_t sec)
{
	return ((sec - sbi->data_start_sector) >> sbi->sect_per_clus_bits) +
		EXFAT_RESERVED_CLUSTERS;
}

static inline bool is_valid_cluster(struct exfat_sb_info *sbi,
		unsigned int clus)
{
	return clus >= EXFAT_FIRST_CLUSTER && clus < sbi->num_clusters;
}

/* super.c */
int exfat_set_volume_dirty(struct super_block *sb);
int exfat_clear_volume_dirty(struct super_block *sb);

/* fatent.c */
#define exfat_get_next_cluster(sb, pclu) exfat_ent_get(sb, *(pclu), pclu)

int exfat_alloc_cluster(struct inode *inode, unsigned int num_alloc,
		struct exfat_chain *p_chain, bool sync_bmap);
int exfat_free_cluster(struct inode *inode, struct exfat_chain *p_chain);
int exfat_ent_get(struct super_block *sb, unsigned int loc,
		unsigned int *content);
int exfat_ent_set(struct super_block *sb, unsigned int loc,
		unsigned int content);
int exfat_count_ext_entries(struct super_block *sb, struct exfat_chain *p_dir,
		int entry, struct exfat_dentry *p_entry);
int exfat_chain_cont_cluster(struct super_block *sb, unsigned int chain,
		unsigned int len);
int exfat_zeroed_cluster(struct inode *dir, unsigned int clu);
int exfat_find_last_cluster(struct super_block *sb, struct exfat_chain *p_chain,
		unsigned int *ret_clu);
int exfat_count_num_clusters(struct super_block *sb,
		struct exfat_chain *p_chain, unsigned int *ret_count);

/* balloc.c */
int exfat_load_bitmap(struct super_block *sb);
void exfat_free_bitmap(struct exfat_sb_info *sbi);
int exfat_set_bitmap(struct inode *inode, unsigned int clu, bool sync);
void exfat_clear_bitmap(struct inode *inode, unsigned int clu, bool sync);
unsigned int exfat_find_free_bitmap(struct super_block *sb, unsigned int clu);
int exfat_count_used_clusters(struct super_block *sb, unsigned int *ret_count);
int exfat_trim_fs(struct inode *inode, struct fstrim_range *range);

/* file.c */
extern const struct file_operations exfat_file_operations;
int __exfat_truncate(struct inode *inode, loff_t new_size);
void exfat_truncate(struct inode *inode, loff_t size);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0)
int exfat_setattr(struct user_namespace *mnt_userns, struct dentry *dentry,
		  struct iattr *attr);
int exfat_getattr(struct user_namespace *mnt_userns, const struct path *path,
		  struct kstat *stat, unsigned int request_mask,
		  unsigned int query_flags);
#else
int exfat_setattr(struct dentry *dentry, struct iattr *attr);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
int exfat_getattr(const struct path *path, struct kstat *stat,
		unsigned int request_mask, unsigned int query_flags);
#else
int exfat_getattr(struct vfsmount *mnt, struct dentry *dentry,
		struct kstat *stat);
#endif
#endif
int exfat_file_fsync(struct file *file, loff_t start, loff_t end, int datasync);
long exfat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
long exfat_compat_ioctl(struct file *filp, unsigned int cmd,
				unsigned long arg);


/* namei.c */
extern const struct dentry_operations exfat_dentry_ops;
extern const struct dentry_operations exfat_utf8_dentry_ops;

/* cache.c */
int exfat_cache_init(void);
void exfat_cache_shutdown(void);
void exfat_cache_inval_inode(struct inode *inode);
int exfat_get_cluster(struct inode *inode, unsigned int cluster,
		unsigned int *fclus, unsigned int *dclus,
		unsigned int *last_dclus, int allow_eof);

/* dir.c */
extern const struct inode_operations exfat_dir_inode_operations;
extern const struct file_operations exfat_dir_operations;
unsigned int exfat_get_entry_type(struct exfat_dentry *p_entry);
int exfat_init_dir_entry(struct inode *inode, struct exfat_chain *p_dir,
		int entry, unsigned int type, unsigned int start_clu,
		unsigned long long size);
int exfat_init_ext_entry(struct inode *inode, struct exfat_chain *p_dir,
		int entry, int num_entries, struct exfat_uni_name *p_uniname);
int exfat_remove_entries(struct inode *inode, struct exfat_chain *p_dir,
		int entry, int order, int num_entries);
int exfat_update_dir_chksum(struct inode *inode, struct exfat_chain *p_dir,
		int entry);
void exfat_update_dir_chksum_with_entry_set(struct exfat_entry_set_cache *es);
int exfat_calc_num_entries(struct exfat_uni_name *p_uniname);
int exfat_find_dir_entry(struct super_block *sb, struct exfat_inode_info *ei,
		struct exfat_chain *p_dir, struct exfat_uni_name *p_uniname,
		int num_entries, unsigned int type, struct exfat_hint *hint_opt);
int exfat_alloc_new_dir(struct inode *inode, struct exfat_chain *clu);
struct exfat_dentry *exfat_get_dentry(struct super_block *sb,
		struct exfat_chain *p_dir, int entry, struct buffer_head **bh);
struct exfat_dentry *exfat_get_dentry_cached(struct exfat_entry_set_cache *es,
		int num);
struct exfat_entry_set_cache *exfat_get_dentry_set(struct super_block *sb,
		struct exfat_chain *p_dir, int entry, unsigned int type);
int exfat_free_dentry_set(struct exfat_entry_set_cache *es, int sync);
int exfat_count_dir_entries(struct super_block *sb, struct exfat_chain *p_dir);

/* inode.c */
extern const struct inode_operations exfat_file_inode_operations;
void exfat_sync_inode(struct inode *inode);
struct inode *exfat_build_inode(struct super_block *sb,
		struct exfat_dir_entry *info, loff_t i_pos);
void exfat_hash_inode(struct inode *inode, loff_t i_pos);
void exfat_unhash_inode(struct inode *inode);
struct inode *exfat_iget(struct super_block *sb, loff_t i_pos);
int exfat_write_inode(struct inode *inode, struct writeback_control *wbc);
void exfat_evict_inode(struct inode *inode);
int exfat_block_truncate_page(struct inode *inode, loff_t from);

/* exfat/nls.c */
unsigned short exfat_toupper(struct super_block *sb, unsigned short a);
int exfat_uniname_ncmp(struct super_block *sb, unsigned short *a,
		unsigned short *b, unsigned int len);
int exfat_utf16_to_nls(struct super_block *sb,
		struct exfat_uni_name *uniname, unsigned char *p_cstring,
		int len);
int exfat_nls_to_utf16(struct super_block *sb,
		const unsigned char *p_cstring, const int len,
		struct exfat_uni_name *uniname, int *p_lossy);
int exfat_create_upcase_table(struct super_block *sb);
void exfat_free_upcase_table(struct exfat_sb_info *sbi);

/* exfat/misc.c */
void __exfat_fs_error(struct super_block *sb, int report, const char *fmt, ...)
		__printf(3, 4) __cold;
#define exfat_fs_error(sb, fmt, args...)          \
		__exfat_fs_error(sb, 1, fmt, ## args)
#define exfat_fs_error_ratelimit(sb, fmt, args...) \
		__exfat_fs_error(sb, __ratelimit(&EXFAT_SB(sb)->ratelimit), \
		fmt, ## args)
void exfat_msg(struct super_block *sb, const char *lv, const char *fmt, ...)
		__printf(3, 4) __cold;
#define exfat_err(sb, fmt, ...)						\
	exfat_msg(sb, KERN_ERR, fmt, ##__VA_ARGS__)
#define exfat_warn(sb, fmt, ...)					\
	exfat_msg(sb, KERN_WARNING, fmt, ##__VA_ARGS__)
#define exfat_info(sb, fmt, ...)					\
	exfat_msg(sb, KERN_INFO, fmt, ##__VA_ARGS__)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
void exfat_get_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 tz, __le16 time, __le16 date, u8 time_cs);
void exfat_truncate_atime(struct timespec64 *ts);
void exfat_set_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 *tz, __le16 *time, __le16 *date, u8 *time_cs);
#else
void exfat_get_entry_time(struct exfat_sb_info *sbi, struct timespec *ts,
		u8 tz, __le16 time, __le16 date, u8 time_cs);
void exfat_truncate_atime(struct timespec *ts);
void exfat_set_entry_time(struct exfat_sb_info *sbi, struct timespec *ts,
		u8 *tz, __le16 *time, __le16 *date, u8 *time_cs);
#endif
u16 exfat_calc_chksum16(void *data, int len, u16 chksum, int type);
u32 exfat_calc_chksum32(void *data, int len, u32 chksum, int type);
void exfat_update_bh(struct buffer_head *bh, int sync);
int exfat_update_bhs(struct buffer_head **bhs, int nr_bhs, int sync);
void exfat_chain_set(struct exfat_chain *ec, unsigned int dir,
		unsigned int size, unsigned char flags);
void exfat_chain_dup(struct exfat_chain *dup, struct exfat_chain *ec);

#endif /* !_EXFAT_FS_H */
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)
