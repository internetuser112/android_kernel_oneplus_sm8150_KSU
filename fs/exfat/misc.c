// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2012-2013 Samsung Electronics Co., Ltd.
 *
 *  misc.c: Helper function for checksum and handling exFAT errors
 */

/*
 *  linux/fs/fat/misc.c
 *
 *  Written 1992,1993 by Werner Almesberger
 *  22/11/2000 - Fixed fat_date_unix2dos for dates earlier than 01/01/1980
 *		 and date_dos2unix for date==0 by Igor Zhbanov(bsg@uniyar.ac.ru)
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
<<<<<<< HEAD
#include <linux/time.h>
#include "exfat.h"
#include "version.h"

#ifdef CONFIG_EXFAT_UEVENT
static struct kobject exfat_uevent_kobj;

int exfat_uevent_init(struct kset *exfat_kset)
{
	int err;
	struct kobj_type *ktype = get_ktype(&exfat_kset->kobj);

	exfat_uevent_kobj.kset = exfat_kset;
	err = kobject_init_and_add(&exfat_uevent_kobj, ktype, NULL, "uevent");
	if (err)
		pr_err("[EXFAT] Unable to create exfat uevent kobj\n");

	return err;
}

void exfat_uevent_uninit(void)
{
	kobject_del(&exfat_uevent_kobj);
	memset(&exfat_uevent_kobj, 0, sizeof(struct kobject));
}

void exfat_uevent_ro_remount(struct super_block *sb)
{
	struct block_device *bdev = sb->s_bdev;
	dev_t bd_dev = bdev ? bdev->bd_dev : 0;

	char major[16], minor[16];
	char *envp[] = { major, minor, NULL };

	snprintf(major, sizeof(major), "MAJOR=%d", MAJOR(bd_dev));
	snprintf(minor, sizeof(minor), "MINOR=%d", MINOR(bd_dev));

	kobject_uevent_env(&exfat_uevent_kobj, KOBJ_CHANGE, envp);
}
#endif
=======
#include <linux/blk_types.h>

#include "exfat_raw.h"
#include "exfat_fs.h"
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)

/*
 * exfat_fs_error reports a file system problem that might indicate fa data
 * corruption/inconsistency. Depending on 'errors' mount option the
 * panic() is called, or error message is printed FAT and nothing is done,
 * or filesystem is remounted read-only (default behavior).
 * In case the file system is remounted read-only, it can be made writable
 * again by remounting it.
 */
void __exfat_fs_error(struct super_block *sb, int report, const char *fmt, ...)
{
	struct exfat_mount_options *opts = &EXFAT_SB(sb)->options;
	va_list args;
	struct va_format vaf;
	struct block_device *bdev = sb->s_bdev;
	dev_t bd_dev = bdev ? bdev->bd_dev : 0;

	if (report) {
		va_start(args, fmt);
		vaf.fmt = fmt;
		vaf.va = &args;
		pr_err("exFAT-fs (%s[%d:%d]): ERR: %pV\n",
			sb->s_id, MAJOR(bd_dev), MINOR(bd_dev), &vaf);
		va_end(args);
	}

	if (opts->errors == EXFAT_ERRORS_PANIC) {
<<<<<<< HEAD
		panic("exFAT-fs (%s[%d:%d]): fs panic from previous error\n",
			sb->s_id, MAJOR(bd_dev), MINOR(bd_dev));
	} else if (opts->errors == EXFAT_ERRORS_RO && !EXFAT_IS_SB_RDONLY(sb)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
		sb->s_flags |= MS_RDONLY;
#else
		sb->s_flags |= SB_RDONLY;
#endif
		pr_err("exFAT-fs (%s[%d:%d]): file-system has been set to "
			"read-only\n", sb->s_id, MAJOR(bd_dev), MINOR(bd_dev));
		exfat_uevent_ro_remount(sb);
=======
		panic("exFAT-fs (%s): fs panic from previous error\n",
			sb->s_id);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
	} else if (opts->errors == EXFAT_ERRORS_RO && !sb_rdonly(sb)) {
		sb->s_flags |= SB_RDONLY;
#else
	} else if (opts->errors == EXFAT_ERRORS_RO &&
			!(sb->s_flags & MS_RDONLY)) {
		sb->s_flags |= MS_RDONLY;
#endif
		exfat_err(sb, "Filesystem has been set read-only");
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)
	}
}
EXPORT_SYMBOL(__exfat_fs_error);

/**
 * __exfat_msg() - print preformated EXFAT specific messages.
 * All logs except what uses exfat_fs_error() should be written by __exfat_msg()
 */
void __exfat_msg(struct super_block *sb, const char *level, int st, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;
	struct block_device *bdev = sb->s_bdev;
	dev_t bd_dev = bdev ? bdev->bd_dev : 0;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	/* level means KERN_ pacility level */
	printk("%sexFAT-fs (%s[%d:%d]): %pV\n", level,
			sb->s_id, MAJOR(bd_dev), MINOR(bd_dev), &vaf);
	va_end(args);
}
EXPORT_SYMBOL(__exfat_msg);

void exfat_log_version(void)
{
	pr_info("exFAT: file-system version %s\n", EXFAT_VERSION);
}
EXPORT_SYMBOL(exfat_log_version);

/* <linux/time.h> externs sys_tz
 * extern struct timezone sys_tz;
 */
#define UNIX_SECS_1980    315532800L

#if BITS_PER_LONG == 64
#define UNIX_SECS_2108    4354819200L
#endif

/* days between 1970/01/01 and 1980/01/01 (2 leap days) */
#define DAYS_DELTA_DECADE    (365 * 10 + 2)
/* 120 (2100 - 1980) isn't leap year */
#define NO_LEAP_YEAR_2100    (120)
#define IS_LEAP_YEAR(y)    (!((y) & 0x3) && (y) != NO_LEAP_YEAR_2100)

#define SECS_PER_MIN    (60)
#define SECS_PER_HOUR   (60 * SECS_PER_MIN)
#define SECS_PER_DAY    (24 * SECS_PER_HOUR)

<<<<<<< HEAD
#define MAKE_LEAP_YEAR(leap_year, year)                         \
	do {                                                    \
		/* 2100 isn't leap year */                      \
		if (unlikely(year > NO_LEAP_YEAR_2100))         \
			leap_year = ((year + 3) / 4) - 1;       \
		else                                            \
			leap_year = ((year + 3) / 4);           \
	} while (0)

/* Linear day numbers of the respective 1sts in non-leap years. */
static time_t accum_days_in_year[] = {
	/* Month : N 01  02  03  04  05  06  07  08  09  10  11  12 */
	0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 0, 0, 0,
};

/* Convert a FAT time/date pair to a UNIX date (seconds since 1 1 70). */
void exfat_time_fat2unix(struct exfat_sb_info *sbi, struct timespec_compat *ts,
		DATE_TIME_T *tp)
=======
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
static void exfat_adjust_tz(struct timespec64 *ts, u8 tz_off)
#else
static void exfat_adjust_tz(struct timespec *ts, u8 tz_off)
#endif
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)
{
	time_t year = tp->Year;
	time_t ld; /* leap day */

<<<<<<< HEAD
	MAKE_LEAP_YEAR(ld, year);
=======
static inline int exfat_tz_offset(struct exfat_sb_info *sbi)
{
	if (sbi->options.sys_tz)
		return -sys_tz.tz_minuteswest;
	return sbi->options.time_offset;
}

/* Convert a EXFAT time/date pair to a UNIX date (seconds since 1 1 70). */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
void exfat_get_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 tz, __le16 time, __le16 date, u8 time_cs)
#else
void exfat_get_entry_time(struct exfat_sb_info *sbi, struct timespec *ts,
		u8 tz, __le16 time, __le16 date, u8 time_cs)
#endif
{
	u16 t = le16_to_cpu(time);
	u16 d = le16_to_cpu(date);
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)

	if (IS_LEAP_YEAR(year) && (tp->Month) > 2)
		ld++;

	ts->tv_sec =  tp->Second  + tp->Minute * SECS_PER_MIN
			+ tp->Hour * SECS_PER_HOUR
			+ (year * 365 + ld + accum_days_in_year[tp->Month]
			+ (tp->Day - 1) + DAYS_DELTA_DECADE) * SECS_PER_DAY;

	if (!sbi->options.tz_utc)
		ts->tv_sec += sys_tz.tz_minuteswest * SECS_PER_MIN;

<<<<<<< HEAD
=======
	if (tz & EXFAT_TZ_VALID)
		/* Adjust timezone to UTC0. */
		exfat_adjust_tz(ts, tz & ~EXFAT_TZ_VALID);
	else
		ts->tv_sec -= exfat_tz_offset(sbi) * SECS_PER_MIN;
}

/* Convert linear UNIX date to a EXFAT time/date pair. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
void exfat_set_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 *tz, __le16 *time, __le16 *date, u8 *time_cs)
#else
#undef EXFAT_MAX_TIMESTAMP_SECS
#define EXFAT_MAX_TIMESTAMP_SECS 0xffffffff
void exfat_set_entry_time(struct exfat_sb_info *sbi, struct timespec *ts,
		u8 *tz, __le16 *time, __le16 *date, u8 *time_cs)
#endif
{
	struct tm tm;
	u16 t, d;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
	if (ts->tv_sec < EXFAT_MIN_TIMESTAMP_SECS) {
		ts->tv_sec = EXFAT_MIN_TIMESTAMP_SECS;
		ts->tv_nsec = 0;
	}
	else if (ts->tv_sec > EXFAT_MAX_TIMESTAMP_SECS) {
		ts->tv_sec = EXFAT_MAX_TIMESTAMP_SECS;
		ts->tv_nsec = 0;
	}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
	time64_to_tm(ts->tv_sec, 0, &tm);
#else
	time_to_tm(ts->tv_sec, 0, &tm);
#endif
	t = (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
	d = ((tm.tm_year - 80) <<  9) | ((tm.tm_mon + 1) << 5) | tm.tm_mday;

	*time = cpu_to_le16(t);
	*date = cpu_to_le16(d);

	/* time_cs field represent 0 ~ 199cs(1990 ms) */
	if (time_cs)
		*time_cs = (tm.tm_sec & 1) * 100 +
			ts->tv_nsec / (10 * NSEC_PER_MSEC);

	/*
	 * Record 00h value for OffsetFromUtc field and 1 value for OffsetValid
	 * to indicate that local time and UTC are the same.
	 */
	*tz = EXFAT_TZ_VALID;
}

/* atime has only a 2-second resolution */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
void exfat_truncate_atime(struct timespec64 *ts)
#else
void exfat_truncate_atime(struct timespec *ts)
#endif
{
	ts->tv_sec = round_down(ts->tv_sec, 2);
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)
	ts->tv_nsec = 0;
}

/* Convert linear UNIX date to a FAT time/date pair. */
void exfat_time_unix2fat(struct exfat_sb_info *sbi, struct timespec_compat *ts,
		DATE_TIME_T *tp)
{
	time_t second = ts->tv_sec;
	time_t day, month, year;
	time_t ld; /* leap day */

	if (!sbi->options.tz_utc)
		second -= sys_tz.tz_minuteswest * SECS_PER_MIN;

	/* Jan 1 GMT 00:00:00 1980. But what about another time zone? */
	if (second < UNIX_SECS_1980) {
		tp->Second  = 0;
		tp->Minute  = 0;
		tp->Hour = 0;
		tp->Day  = 1;
		tp->Month  = 1;
		tp->Year = 0;
		return;
	}
#if (BITS_PER_LONG == 64)
	if (second >= UNIX_SECS_2108) {
		tp->Second  = 59;
		tp->Minute  = 59;
		tp->Hour = 23;
		tp->Day  = 31;
		tp->Month  = 12;
		tp->Year = 127;
		return;
	}
#endif

	day = second / SECS_PER_DAY - DAYS_DELTA_DECADE;
	year = day / 365;

	MAKE_LEAP_YEAR(ld, year);
	if (year * 365 + ld > day)
		year--;

	MAKE_LEAP_YEAR(ld, year);
	day -= year * 365 + ld;

	if (IS_LEAP_YEAR(year) && day == accum_days_in_year[3]) {
		month = 2;
	} else {
		if (IS_LEAP_YEAR(year) && day > accum_days_in_year[3])
			day--;
		for (month = 1; month < 12; month++) {
			if (accum_days_in_year[month + 1] > day)
				break;
		}
	}
	day -= accum_days_in_year[month];

	tp->Second  = second % SECS_PER_MIN;
	tp->Minute  = (second / SECS_PER_MIN) % 60;
	tp->Hour = (second / SECS_PER_HOUR) % 24;
	tp->Day  = day + 1;
	tp->Month  = month;
	tp->Year = year;
}

TIMESTAMP_T *exfat_tm_now(struct exfat_sb_info *sbi, TIMESTAMP_T *tp)
{
	struct timespec_compat ts;
	DATE_TIME_T dt;

	KTIME_GET_REAL_TS(&ts);
	exfat_time_unix2fat(sbi, &ts, &dt);

	tp->year = dt.Year;
	tp->mon = dt.Month;
	tp->day = dt.Day;
	tp->hour = dt.Hour;
	tp->min = dt.Minute;
	tp->sec = dt.Second;

	return tp;
}

u16 exfat_calc_chksum_2byte(void *data, s32 len, u16 chksum, s32 type)
{
	s32 i;
	u8 *c = (u8 *) data;

	for (i = 0; i < len; i++, c++) {
		if (((i == 2) || (i == 3)) && (type == CS_DIR_ENTRY))
			continue;
		chksum = (((chksum & 1) << 15) | ((chksum & 0xFFFE) >> 1)) + (u16) *c;
	}
	return chksum;
}

#ifdef CONFIG_EXFAT_DBG_MSG
void __exfat_dmsg(int level, const char *fmt, ...)
{
	va_list args;

<<<<<<< HEAD
	/* should check type */
	if (level > EXFAT_MSG_LEVEL)
		return;

	va_start(args, fmt);
	/* fmt already includes KERN_ pacility level */
	vprintk(fmt, args);
	va_end(args);
=======
	for (i = 0; i < len; i++, c++) {
		if (unlikely(type == CS_BOOT_SECTOR &&
			     (i == 106 || i == 107 || i == 112)))
			continue;
		chksum = ((chksum << 31) | (chksum >> 1)) + *c;
	}
	return chksum;
}

void exfat_update_bh(struct buffer_head *bh, int sync)
{
	set_buffer_uptodate(bh);
	mark_buffer_dirty(bh);

	if (sync)
		sync_dirty_buffer(bh);
}

int exfat_update_bhs(struct buffer_head **bhs, int nr_bhs, int sync)
{
	int i, err = 0;

	for (i = 0; i < nr_bhs; i++) {
		set_buffer_uptodate(bhs[i]);
		mark_buffer_dirty(bhs[i]);
		if (sync)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
			write_dirty_buffer(bhs[i], REQ_SYNC);
#else
			write_dirty_buffer(bhs[i], WRITE_SYNC);
#endif
	}

	for (i = 0; i < nr_bhs && sync; i++) {
		wait_on_buffer(bhs[i]);
		if (!err && !buffer_uptodate(bhs[i]))
			err = -EIO;
	}
	return err;
}

void exfat_chain_set(struct exfat_chain *ec, unsigned int dir,
		unsigned int size, unsigned char flags)
{
	ec->dir = dir;
	ec->size = size;
	ec->flags = flags;
}

void exfat_chain_dup(struct exfat_chain *dup, struct exfat_chain *ec)
{
	return exfat_chain_set(dup, ec->dir, ec->size, ec->flags);
>>>>>>> 97f24f46f3cc (Merge remote-tracking branch 'origin/R-base' into R)
}
#endif
