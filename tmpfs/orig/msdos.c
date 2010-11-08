/*
 * Program for interacting with an MSDOS filesystem.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

/*
 * Constants.
 */
/* #define BYTES_PER_SECTOR                        512 */
#define BITS_PER_FAT_ENTRY                      12
#define BYTES_PER_DIR_ENTRY                     32
/* #define SECTOR_OFFSET                           31 */

#define BYTES_PER_SECTOR                        (dosp->boot_sector->bytes_per_sector)
#define SECTORS_PER_CLUSTER                     (dosp->boot_sector->sectors_per_cluster)
#define BYTES_PER_CLUSTER                       (BYTES_PER_SECTOR*SECTORS_PER_CLUSTER)
#define NUMBER_OF_LOGICAL_CLUSTERS              (dosp->boot_sector->number_of_logical_clusters)
#define SECTORS_PER_FAT                         (dosp->boot_sector->sectors_per_fat)
#define NUMBER_OF_FATS                          (dosp->boot_sector->number_of_fats)
#define BOOT_RECORD_SECTORS                     (dosp->boot_sector->boot_record_sectors)
#define NUMBER_OF_ROOT_ENTRIES                  (dosp->boot_sector->number_of_root_entries)
#define CLUSTER__SECTOR_OFFSET                  (ROOT_DIR_SECTOR-(2*SECTORS_PER_CLUSTER)+ROOT_DIR_LEN)

#define TOTAL_FILENAME_LEN                      (DE_FILENAME_LEN+DE_EXTENSION_LEN)

#define MAX_PATH_LEN                            1024
#define BITS_PER_BYTE                           8

/*
 * Sector numbers for the DOS things.
 */
#define BOOT_SECTOR                             0
/* 
 * #define FIRST_FAT_SECTOR                        1
 * #define FIRST_FAT_LEN                           9
 * #define SECOND_FAT_SECTOR                       10
 * #define SECOND_FAT_LEN                          9
 * #define ROOT_DIR_SECTOR                         19
 */
#define FIRST_FAT_SECTOR                        (BOOT_SECTOR+1)
#define FIRST_FAT_LEN                           SECTORS_PER_FAT
#define SECOND_FAT_SECTOR                       (FIRST_FAT_SECTOR+SECTORS_PER_FAT)
#define SECOND_FAT_LEN                          SECTORS_PER_FAT
#define ROOT_DIR_SECTOR                         (BOOT_SECTOR+NUMBER_OF_FATS*SECTORS_PER_FAT+BOOT_RECORD_SECTORS)
#define ROOT_DIR_LEN                            (NUMBER_OF_ROOT_ENTRIES*BYTES_PER_DIR_ENTRY/BYTES_PER_SECTOR)

/*
 * Struct for the boot sector.
 */
#define BS_JUMP_OFF                             0
#define BS_JUMP_LEN                             3

#define BS_MANUFACTURER_NAME_OFF                0x03
#define BS_MANUFACTURER_NAME_LEN                8

#define BS_BYTES_PER_SECTOR_OFF                 0x0b
#define BS_BYTES_PER_SECTOR_LEN                 2

#define BS_SECTORS_PER_CLUSTER_OFF              0x0d
#define BS_SECTORS_PER_CLUSTER_LEN              1

#define BS_BOOT_RECORD_SECTORS_OFF              0x0e
#define BS_BOOT_RECORD_SECTORS_LEN              2

#define BS_NUMBER_OF_FATS_OFF                   0x10
#define BS_NUMBER_OF_FATS_LEN                   1

#define BS_NUMBER_OF_ROOT_ENTRIES_OFF           0x11
#define BS_NUMBER_OF_ROOT_ENTRIES_LEN           2

/* #define BS_NUMBER_OF_LOGICAL_SECTORS_OFF        0x13 */
/* #define BS_NUMBER_OF_LOGICAL_SECTORS_LEN        2    */
#define BS_NUMBER_OF_LOGICAL_CLUSTERS_OFF       0x13
#define BS_NUMBER_OF_LOGICAL_CLUSTERS_LEN       2

#define BS_MEDIA_TYPE_OFF                       0x15
#define BS_MEDIA_TYPE_LEN                       1

#define BS_SECTORS_PER_FAT_OFF                  0x16
#define BS_SECTORS_PER_FAT_LEN                  2

#define BS_SECTORS_PER_TRACK_OFF                0x18
#define BS_SECTORS_PER_TRACK_LEN                2

#define BS_NUMBER_OF_HEADS_OFF                  0x1a
#define BS_NUMBER_OF_HEADS_LEN                  2

#define BS_NUMBER_OF_HIDDEN_SECTORS_OFF         0x1c
#define BS_NUMBER_OF_HIDDEN_SECTORS_LEN         2

#define BS_BOOTSTRAP_PROGRAM                    0x1e

#define BS_SIGNATURE_OFF                        0x1fe
#define BS_SIGNATURE_LEN                        2

typedef struct msdos_boot_sector {
    unsigned jump[BS_JUMP_LEN+1];
    char manufacturer_name[BS_MANUFACTURER_NAME_LEN+1];
    unsigned bytes_per_sector;
    unsigned sectors_per_cluster;
    unsigned boot_record_sectors;
    unsigned number_of_fats;
    unsigned number_of_root_entries;
    unsigned number_of_logical_clusters;
    unsigned media_type;
    unsigned sectors_per_fat;
    unsigned sectors_per_track;
    unsigned number_of_heads;
    unsigned number_of_hidden_sectors;
    unsigned signature[BS_SIGNATURE_LEN+1];
} MSDOSBootSector;

/*
 * Directory entries.
 */
#define DE_FILENAME_OFF                         0
#define DE_FILENAME_LEN                         8

#define DE_EXTENSION_OFF                        8
#define DE_EXTENSION_LEN                        3

#define DE_ATTR_OFF                             0x0b
#define DE_ATTR_LEN                             1

#define ATTR_READ_ONLY_SHIFT                    0
#define ATTR_HIDDEN_SHIFT                       1
#define ATTR_SYSTEM_SHIFT                       2
#define ATTR_VLABEL_SHIFT                       3
#define ATTR_SUBDIR_SHIFT                       4
#define ATTR_ARCHIVE_SHIFT                      5

#define DE_RESERVED_OFF                         0x0c
#define DE_RESERVED_LEN                         10

#define DE_TIME_OFF                             0x16
#define DE_TIME_LEN                             2
#define DE_TIME_SEC(i)                          (((i) * 2) % 64)
#define DE_TIME_MIN(i)                          (((i) / 32) % 64)
#define DE_TIME_HOUR(i)                         (((i) / 2048))
#define DE_TIME(sec, min, hour)                 (((hour) * 2048) | ((min) * 32) | ((sec) / 2))

#define DE_DATE_OFF                             0x18
#define DE_DATE_LEN                             2
#define DE_DATE_DAY(i)                          ((i) % 32)
#define DE_DATE_MONTH(i)                        (((i) / 32) % 32)
#define DE_DATE_YEAR(i)                         (((i) / 512) + 1980)
#define DE_DATE(day, mon, year)                 ((((year) - 1980) * 512) | ((mon) * 32) | (day))

#define DE_FIRST_CLUSTER_OFF                    0x1a
#define DE_FIRST_CLUSTER_LEN                    2

#define DE_FILESIZE_OFF                         0x1c
#define DE_FILESIZE_LEN                         4

typedef struct msdos_time_stamp {
    unsigned second;
    unsigned char minute;
    unsigned hour;
    unsigned day;
    unsigned month;
    unsigned year;
    char monthStr[4];
} MSDOSTimeStamp;

typedef struct msdos_dir_entry MSDOSDirEntry;
struct msdos_dir_entry {
    unsigned char filename[DE_FILENAME_LEN+1];
    unsigned char extension[DE_EXTENSION_LEN+1];
    unsigned char attr;
    unsigned attr_read_only : 1;
    unsigned attr_hidden : 1;
    unsigned attr_system : 1;
    unsigned attr_vlabel : 1;
    unsigned attr_subdir : 1;
    unsigned attr_archive : 1;
    unsigned char reserved[DE_RESERVED_LEN];
    unsigned time;
    unsigned date;
    MSDOSTimeStamp timestamp;
    unsigned first_cluster;
    unsigned filesize;
    MSDOSDirEntry *parent;
    unsigned char path[MAX_PATH_LEN];
    unsigned char tname[TOTAL_FILENAME_LEN+1]; /* Trimmed name + extension */
};


/*
 * Filesystem info.
 */
typedef struct msdos {
    int fd;                             /* File descriptor for the device */

    /*
     * TODO: you probably want to add additional fields,
     * like the current directory, and a copy of the FAT
     * table for the filesystem.
     */
    MSDOSBootSector *boot_sector;
    unsigned short *fat_table;
    unsigned fat_entries;

    MSDOSDirEntry *root_dir;
    MSDOSDirEntry *current_dir;
} MSDOS;

/************************************************************************
 * I/O
 ************************************************************************/

/*
 * DO NOT use the read/write functions directly.
 * Use these block operations instead.
 */

int msdos_block_read(MSDOS *dosp, int block, char *buffer);
int msdos_block_write(MSDOS *dosp, int block, char *buffer);
int msdos_nblock_read(MSDOS *dosp, int block, int num, char *buffer);
int msdos_nblock_write(MSDOS *dosp, int block, int num, char *buffer);

int msdos_sector_read(MSDOS *dosp, int sector, char *buffer);
int msdos_sector_write(MSDOS *dosp, int sector, const char *buffer);
int msdos_cluster_read(MSDOS *dosp, int cluster, char *buffer);
int msdos_cluster_write(MSDOS *dosp, int cluster, char *buffer);

int msdos_nsector_read(MSDOS *dosp, int sector, int num, char *buffer);
int msdos_nsector_write(MSDOS *dosp, int sector, int num, char *buffer);
int msdos_ncluster_read(MSDOS *dosp, int sector, int num, char *buffer);
int msdos_ncluster_write(MSDOS *dosp, int sector, int num, char *buffer);


/*
 * Read a logical block from the file.
 * Returns -1 on error.
 */
int msdos_block_read(MSDOS *dosp, int block, char *buffer)
{
    if (block == 0)
        return msdos_sector_read(dosp, block, buffer);
    else
        return msdos_cluster_read(dosp, block, buffer);
}

/*
 * Write a logical block to the file.
 * Returns -1 on error.
 */
int msdos_block_write(MSDOS *dosp, int block, char *buffer)
{
    if (block == 0)
        return msdos_sector_write(dosp, block, buffer);
    else
        return msdos_cluster_write(dosp, block, buffer);
}

/*
 * Read n logical blocks from the file.
 * Returns -1 on error.
 */
int msdos_nblock_read(MSDOS *dosp, int block, int num, char *buffer)
{
    if (block == 0)
        return msdos_nsector_read(dosp, block, num, buffer);
    else
        return msdos_ncluster_read(dosp, block, num, buffer);
}

/*
 * Write n logical blocks to the file.
 * Returns -1 on error.
 */
int msdos_nblock_write(MSDOS *dosp, int block, int num, char *buffer)
{
    if (block == 0)
        return msdos_nsector_write(dosp, block, num, buffer);
    else
        return msdos_ncluster_write(dosp, block, num, buffer);
}


/*
 * Read a sector from the file.
 * Returns -1 on error.
 */
int msdos_sector_read(MSDOS *dosp, int sector, char *buffer)
{
    int dest, len;

    /* Seek to the sector */
    dest = lseek(dosp->fd, sector * BYTES_PER_SECTOR, SEEK_SET);
    if (dest < 0) 
    {
        perror("msdos_sector_read: lseek");
        return -1;
    }

    /* Assume the read happens all at once */
    len = read(dosp->fd, buffer, BYTES_PER_SECTOR);
    if (len != BYTES_PER_SECTOR)
    {
        perror("msdos_sector_read: read");
        return -1;
    }

    return len;
}


/*
 * Write a sector to the file.
 * Returns -1 on error.
 */
int msdos_sector_write(MSDOS *dosp, int sector, const char *buffer)
{
    int dest, len;

    /* Seek to the sector */
    dest = lseek(dosp->fd, sector * BYTES_PER_SECTOR, SEEK_SET);
    if (dest < 0) 
    {
        perror("msdos_sector_write: lseek");
        return -1;
    }

    /* Assume the read happens all at once */
    len = write(dosp->fd, buffer, BYTES_PER_SECTOR);
    if (len != BYTES_PER_SECTOR) 
    {
        perror("msdos_sector_read: read");
        return -1;
    }

    return len;
}

/*
 * Read a cluster from the file.
 * Returns -1 on error.
 */
int msdos_cluster_read(MSDOS *dosp, int cluster, char *buffer)
{
    if (cluster == 0) 
    {
       perror("msdos_cluster_read: no cluster 0.");
       return -1;
    }
    else if (cluster > NUMBER_OF_LOGICAL_CLUSTERS) 
    {
        perror("msdos_cluster_read: cluster too high\n");
        return -1;
    }

    else
        return msdos_nsector_read(dosp, cluster*SECTORS_PER_CLUSTER + CLUSTER__SECTOR_OFFSET, SECTORS_PER_CLUSTER, buffer);

}

/*
 * Write a cluster to the file.
 * Returns -1 on error.
 */
int msdos_cluster_write(MSDOS *dosp, int cluster, char *buffer)
{
    if (cluster == 0) 
    {
       perror("msdos_cluster_write: no cluster 0.");
       return -1;
    }
    else if (cluster > NUMBER_OF_LOGICAL_CLUSTERS)
    {
        perror("msdos_cluster_write: cluster too high\n");
        return -1;
    }
    else
        return msdos_nsector_write(dosp, cluster*SECTORS_PER_CLUSTER + CLUSTER__SECTOR_OFFSET, SECTORS_PER_CLUSTER, buffer);
}

/*
 * Read n sectors from the file.
 * Returns -1 on error.
 */
int msdos_nsector_read(MSDOS *dosp, int sector, int num, char *buffer)
{
    int i, ret, len=0;
    char *ptr=buffer;
    for (i=0; i<num; i++)
    {
        ret = msdos_sector_read(dosp, sector + i, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
    }
    return len;
}

/*
 * Write n sectors to the file.
 * Returns -1 on error.
 */
int msdos_nsector_write(MSDOS *dosp, int sector, int num, char *buffer)
{
    int i, ret, len=0;
    char *ptr=buffer;
    for (i=0; i<num; i++)
    {
        ret = msdos_sector_write(dosp, sector + i, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
    }
    return len;
}

/*
 * Read n clusters from the file.
 * Returns -1 on error.
 */
int msdos_ncluster_read(MSDOS *dosp, int cluster, int num, char *buffer)
{
    int i, ret, len=0;
    char *ptr=buffer;
    for (i=0; i<num; i++)
    {
        ret = msdos_cluster_read(dosp, cluster + i, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
    }
    return len;
}

/*
 * Write n clusters to the file.
 * Returns -1 on error.
 */
int msdos_ncluster_write(MSDOS *dosp, int cluster, int num, char *buffer)
{
    int i, ret, len=0;
    char *ptr=buffer;
    for (i=0; i<num; i++)
    {
        ret = msdos_cluster_write(dosp, cluster + i, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
    }
    return len;
}


/************************************************************************
 * TOOLS
 ************************************************************************/

void convert_to_timestamp(MSDOSDirEntry *dirp);
void convert_from_timestamp(MSDOSDirEntry *dirp);
void convert_from_attr_byte(MSDOSDirEntry *dirp);
void convert_to_attr_byte(MSDOSDirEntry *dirp);
void msdos_read_boot_sector(MSDOS *dosp);
void msdos_write_boot_sector(MSDOS *dosp);
void msdos_read_fat_table(MSDOS *dosp);
void msdos_write_fat_table(MSDOS *dosp);
void msdos_read_dir_entry(MSDOS *dosp, MSDOSDirEntry *parent, MSDOSDirEntry *child, char *buffer);
void msdos_write_dir_entry(MSDOS *dosp, MSDOSDirEntry *dirp, char *buffer);
MSDOSDirEntry *msdos_read_new_dir_entry(MSDOS *dosp, MSDOSDirEntry *parent, char *buffer);
void msdos_close(MSDOS *dosp);
MSDOS *msdos_open(const char *devname);


/*
 * Get an unsigned value from a buffer.
 */
unsigned get_unsigned(unsigned char *bufp, int len)
{
    unsigned value = 0;
    int shift = 0;
    while(len--) 
    {
        value |= *bufp++ << shift;
        shift += 8;
    }
    return value;
}

void put_unsigned(unsigned char *bufp, int len, unsigned value)
{
    while(len--) 
    {
        *bufp++ = value;
        value >>= 8;
    }
}

void DE_MONTH_NAME(int month, char *monthStr)
{
    switch(month+1)
    {
        case 1:  
            strcpy(monthStr, "Jan");
            break;
        case 2:  
            strcpy(monthStr, "Feb");
            break;
        case 3:  
            strcpy(monthStr, "Mar");
            break;
        case 4:  
            strcpy(monthStr, "Apr");
            break;
        case 5:  
            strcpy(monthStr, "May");
            break;
        case 6:  
            strcpy(monthStr, "Jun");
            break;
        case 7:  
            strcpy(monthStr, "Jul");
            break;
        case 8:  
            strcpy(monthStr, "Aug");
            break;
        case 9:  
            strcpy(monthStr, "Sep");
            break;
        case 10:  
            strcpy(monthStr, "Oct");
            break;
        case 11:  
            strcpy(monthStr, "Nov");
            break;
        case 12:  
            strcpy(monthStr, "Dec");
            break;
        default:  
            strcpy(monthStr, "Err");
            break;
    }
}

void DE_NOW(MSDOSDirEntry *dirp)
{
    time_t sec;
    struct tm *now;
    MSDOSTimeStamp *ts;
    sec = time(NULL);
    now = localtime(&sec);
    ts = &dirp->timestamp;

    ts->second = now->tm_sec;
    ts->minute = now->tm_min;
    ts->hour = now->tm_hour;
    ts->day = now->tm_mday;
    ts->month = now->tm_mon;
    ts->year = (now->tm_year)+1900;
    DE_MONTH_NAME(ts->month, ts->monthStr);

    convert_from_timestamp(dirp);
}

/* remove trailing whitespace from a filename */
void ws_trim(char *str)
{
    int i=0;
    while (str[i]!='\0')
        i++;
    i--;
    while (str[i]==' ')
    {
        str[i]='\0';
        i--;
    }
}

/* pad a filename with whitespace */
void ws_pad(char *str, int len)
{
    int i=0;
    for(i=0;i<len;i++)
    {
        if (str[i]=='\0')
        {
            str[i]=' ';
            str[i+1]='\0';
        }
    }
}

void convert_to_timestamp(MSDOSDirEntry *dirp)
{
    MSDOSTimeStamp *ts = &dirp->timestamp;
    ts->second = DE_TIME_SEC(dirp->time);
    ts->minute = DE_TIME_MIN(dirp->time);
    ts->hour = DE_TIME_HOUR(dirp->time);
    ts->day = DE_DATE_DAY(dirp->date);
    ts->month = DE_DATE_MONTH(dirp->date);
    ts->year = DE_DATE_YEAR(dirp->date);
    DE_MONTH_NAME(ts->month, ts->monthStr);
}


void convert_from_timestamp(MSDOSDirEntry *dirp)
{
    MSDOSTimeStamp *ts = &dirp->timestamp;
    dirp->time = DE_TIME(ts->second, ts->minute, ts->hour);
    dirp->date = DE_DATE(ts->day, ts->month, ts->year);
}


void convert_from_attr_byte(MSDOSDirEntry *dirp)
{
    int attr = dirp->attr;
    dirp->attr_read_only = (0x1 & (attr >> ATTR_READ_ONLY_SHIFT));
    dirp->attr_hidden = (0x1 & (attr >> ATTR_HIDDEN_SHIFT));
    dirp->attr_system = (0x1 & (attr >> ATTR_SYSTEM_SHIFT));
    dirp->attr_vlabel = (0x1 & (attr >> ATTR_VLABEL_SHIFT));
    dirp->attr_subdir = (0x1 & (attr >> ATTR_SUBDIR_SHIFT));
    dirp->attr_archive = (0x1 & (attr >> ATTR_ARCHIVE_SHIFT));
}

void convert_to_attr_byte(MSDOSDirEntry *dirp)
{
    int attr = dirp->attr;
    attr |= (dirp->attr_read_only << ATTR_READ_ONLY_SHIFT);
    attr |= (dirp->attr_hidden << ATTR_HIDDEN_SHIFT);
    attr |= (dirp->attr_system << ATTR_SYSTEM_SHIFT);
    attr |= (dirp->attr_vlabel << ATTR_VLABEL_SHIFT);
    attr |= (dirp->attr_subdir << ATTR_SUBDIR_SHIFT); 
    attr |= (dirp->attr_archive << ATTR_ARCHIVE_SHIFT); 

    dirp->attr = attr;
}

int wipe_file_fat(MSDOS *dosp, MSDOSDirEntry *dirp)
{
    int temp, cluster = dirp->first_cluster;
    while (!((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) || (cluster > 0xfff))
            return -1;
        temp = cluster;
        cluster = dosp->fat_table[cluster];
        dosp->fat_table[temp] = 0x000;
    }
    dosp->fat_table[cluster] = 0x000;
    return 0;
}

int insert_file_fat(MSDOS *dosp, int start, int prev)
{
    int cluster;
    cluster = (start + 1) % NUMBER_OF_LOGICAL_CLUSTERS;

    while ((cluster != start) && (dosp->fat_table[cluster] != 0x000))
    {
        cluster = (cluster + 1) % NUMBER_OF_LOGICAL_CLUSTERS;
    }
    if (cluster == start)
        return -1;
    
    if (prev == 0xfff)
        dosp->fat_table[cluster] = 0xfff;
    else
    {
        dosp->fat_table[cluster] = dosp->fat_table[prev];
        dosp->fat_table[prev] = cluster;
    }
    return cluster;
}


int count_clusters(MSDOS *dosp, MSDOSDirEntry *dirp)
{
    int cluster = dirp->first_cluster;
    int count = 0;
    if (dirp->filename[0] == 0xe5)
        return -1;
    while (!((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) || (cluster > 0xfff))
            return -1;
        cluster = dosp->fat_table[cluster];
        count++;
    }
    return count;
}

int get_last_cluster(MSDOS *dosp, MSDOSDirEntry *dirp)
{
    int temp, cluster = dirp->first_cluster;
    if (dirp->filename[0] == 0xe5)
        return -1;
    while (!((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) || (cluster > 0xfff))
            return -1;
        temp = cluster;
        cluster = dosp->fat_table[cluster];
    }
    return temp;
}

char valid_dos_char(const char c)
{
    if ( (c == '"') || (c == '*') || (c == '+') || (c == ',') || 
         (c == '.') || (c == '/') || (c == ':') || (c == ';') ||
         (c == '<') || (c == '=') || (c == '>') || (c == '?') || 
         (c == '[') || (c == '\\') || (c == ']') || (c == '|') )
        return '$';
    else
        return toupper(c);
}

void valid_dos_name(const char *name, char *fname, char *ext)
{
    int i, ext_pos, max;
    i=0;
    while (name[i]!='\0')
        i++;
    max=i;
    while ((name[i]!='.') && (i>0))
    {
        i--;
    }
    ext_pos = i;
    if (ext_pos!=0)
    {    
        for (i=0;(((i+ext_pos)<max)&&(i<DE_EXTENSION_LEN));i++)
            ext[i]=name[i+ext_pos+1];
    }
    else
    {
        ext_pos=max;
    }

    for (i=0;((i<ext_pos)&&(i<DE_FILENAME_LEN));i++)
        fname[i]=name[i];

    for (i=0;i<DE_FILENAME_LEN;i++)
        fname[i] = valid_dos_char(fname[i]);
    for (i=0;i<DE_EXTENSION_LEN;i++)
        ext[i] = valid_dos_char(ext[i]);

    if ((unsigned char)fname[0]==0xe5)
        fname[0]=0x05;
    if (fname[0]==' ')
        fname[0]='$';
}

void msdos_print_dir_entry_name_override(MSDOSDirEntry *dirp, char *name_override)
{
    MSDOSTimeStamp *ts = &dirp->timestamp;
    char out_name[TOTAL_FILENAME_LEN+1];
    
    if (name_override[0] != '\0')
       strcpy(out_name, name_override);
    else
    {
       strcpy(out_name, dirp->tname);
    }
    if (out_name[0]==0x05)
        out_name[0]=0xe5;

    if (dirp->attr_read_only) 
        printf("R");
    else
        printf("W");
    if (dirp->attr_hidden) 
        printf("H");
    else
        printf("-");
    if (dirp->attr_system) 
        printf("S");
    else
        printf("-");
    if (dirp->attr_vlabel) 
        printf("V");
    else
        printf("-");
    if (dirp->attr_subdir) 
        printf("D");
    else
        printf("-");
    if (dirp->attr_archive) 
        printf("A");
    else
        printf("-");
    printf("  %s %02d, %02d; %02d:%02d:%02d  ", ts->monthStr, ts->day, ts->year, 
                                  ts->hour, ts->minute, ts->second);
    printf("%11d  %-13s  -> %03x (%d)\n", dirp->filesize, out_name, dirp->first_cluster, dirp->first_cluster);
}

void msdos_print_dir_entry(MSDOSDirEntry *dirp)
{
    msdos_print_dir_entry_name_override(dirp, "");
}

int read_dir_entry_contents(MSDOS *dosp, MSDOSDirEntry *dirp, char *buffer)
{
   int ret, len = 0;
   unsigned cluster = dirp->first_cluster;
   char *ptr = buffer;
   if (cluster == 0x000)
       return -1;
   while ((cluster != 0x000) && (!((0xff8 <= cluster) && (cluster <= 0xfff))))
   {
        if ((cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) || (cluster > 0xfff))
            return -1;
        ret = msdos_cluster_read(dosp, cluster, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
        cluster = dosp->fat_table[cluster];
    }
    return len;
}

int write_dir_entry_contents(MSDOS *dosp, MSDOSDirEntry *dirp, char *buffer)
{
   int ret, len = 0;
   unsigned cluster = dirp->first_cluster;
   char *ptr = buffer;
   if (cluster == 0x000)
       return -1;
   while ((cluster != 0x000) && (!((0xff8 <= cluster) && (cluster <= 0xfff))))
   {
        if ((cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) || (cluster > 0xfff))
            return -1;
        ret = msdos_cluster_write(dosp, cluster, ptr);
        if (ret == -1)
            return -1;
        len += ret;
        ptr += ret;
        cluster = dosp->fat_table[cluster];
    }
    return len;
}

int read_dir_from_disk(MSDOS *dosp, MSDOSDirEntry *dirp, char **buffer)
{
    int ret, clusters_read;
    if (!(dirp->path[1]))
    {
        clusters_read = ROOT_DIR_LEN*BYTES_PER_SECTOR;
        *buffer = (char *) malloc(sizeof(char)*clusters_read);
        ret = msdos_nsector_read(dosp, ROOT_DIR_SECTOR, ROOT_DIR_LEN, *buffer);
    }
    else
    {
        clusters_read = count_clusters(dosp, dirp)*BYTES_PER_CLUSTER;
        *buffer = (char *) malloc(sizeof(char)*clusters_read);
        ret = read_dir_entry_contents(dosp, dirp, *buffer);
    }
    if (ret == -1)
    {
        perror("Error reading directory from disk.\n");
        free(*buffer);
        return -1;
    }
    return clusters_read;
}

int write_dir_to_disk(MSDOS *dosp, MSDOSDirEntry *dirp, char **buffer)
{
    int ret, clusters_write;
    if (!(dirp->path[1]))
    {
        clusters_write = ROOT_DIR_LEN*BYTES_PER_SECTOR;
        ret = msdos_nsector_write(dosp, ROOT_DIR_SECTOR, ROOT_DIR_LEN, *buffer);
    }
    else
    {
        clusters_write = count_clusters(dosp, dirp)*BYTES_PER_CLUSTER;
        if (clusters_write <= 0)
            return -1;
        ret = write_dir_entry_contents(dosp, dirp, *buffer);
    }
    if (ret == -1)
    {
        perror("Error writing directory from disk.\n");
        free(*buffer);
        return -1;
    }
    return clusters_write;
}

int remove_dir_entry(MSDOS *dosp, MSDOSDirEntry *dirp)
{
    char *buffer;
    int i, max_read;
    MSDOSDirEntry *parent = dirp->parent;

    max_read = read_dir_from_disk(dosp, parent, &buffer);
    if (max_read == -1)
    {
        perror("Directory read error.\n");
        return -1;
    }

    for(i=0;i<max_read;i+=BYTES_PER_DIR_ENTRY)
    {
           if (((get_unsigned(buffer+i+DE_ATTR_OFF, DE_ATTR_LEN) >> ATTR_VLABEL_SHIFT) & 0x1) == 1)
               continue;
           if (strncmp(dirp->filename, buffer+i+DE_FILENAME_OFF, DE_FILENAME_LEN)==0)
           {
               buffer[i+DE_FILENAME_OFF] = 0xe5;
               max_read = write_dir_to_disk(dosp, parent, &buffer);
               if (max_read == -1)
               {
                   perror("Directory write error.\n");
                   free(buffer);
                   return -1;
               }
               free(buffer);
               return 0;
           }
    }
    free(buffer);
    return -1;
}

int add_dir_entry(MSDOS *dosp, MSDOSDirEntry *dirp)
{
    char *buffer, *tmp;
    int i, max_read, last_cluster, num_clusters, cluster;
    MSDOSDirEntry *parent = dirp->parent;

    max_read = read_dir_from_disk(dosp, parent, &buffer);

    if (max_read == -1)
    {
        perror("Directory read error.\n");
        return -1;
    }

    for(i=0;i<max_read;i+=BYTES_PER_DIR_ENTRY)
    {
        if (((unsigned char) buffer[i+DE_FILENAME_OFF] == 0xe5) || (buffer[i+DE_FILENAME_OFF] == 0x00))
            break;
    }

    if (!(i<max_read))
    {
        if (parent == dosp->root_dir)
        {
            perror("Max number of entries for root dir already reached.\n");
            free(buffer);
            return -1;
        }

        /* Last sector of directory is full, so add a new one to the chain. */
        last_cluster = get_last_cluster(dosp, dirp);
        cluster = insert_file_fat(dosp, last_cluster, last_cluster);

        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
              || (cluster == -1) || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
        {
            perror("Directory entry allocation error.\n");
            free(buffer);
            return -1;
        }

        num_clusters = count_clusters(dosp, parent);
        tmp = (char *) malloc(sizeof(char)*num_clusters*BYTES_PER_CLUSTER);
        memset(tmp, '\0', sizeof(char)*num_clusters*BYTES_PER_CLUSTER);
        memcpy(tmp, buffer, sizeof(char)*(num_clusters-1)*BYTES_PER_CLUSTER);
        free(buffer);
        buffer = tmp;
        tmp = NULL;
     }

    msdos_write_dir_entry(dosp, dirp, buffer+i);
    max_read = write_dir_to_disk(dosp, parent, &buffer);
    if (max_read == -1)
    {
        perror("Directory write error.\n");
        free(buffer);
        return -1;
    }

    free(buffer);
    return 0;
}


MSDOSDirEntry* find_dir_entry(MSDOS *dosp, MSDOSDirEntry *parent, const char *name)
{

    char *buffer;
    int i, max_read;

    MSDOSDirEntry *dirp, *cur_dir = dosp->current_dir;
    dirp = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    memset(dirp, '\0', sizeof(MSDOSDirEntry));

    max_read = read_dir_from_disk(dosp, cur_dir, &buffer);
    if (max_read == -1)
    {
        perror("Directory read error.\n");
        free(dirp);
        return 0;
    }

    for(i=0;i<max_read;i+=BYTES_PER_DIR_ENTRY)
    {
        msdos_read_dir_entry(dosp, cur_dir, dirp, buffer+i);
        if (dirp->filename[0] == 0x00)
            break;
        if (dirp->attr_vlabel==1)
            continue;
        if (strcmp(dirp->tname, name)==0)
        {
            free(buffer);
            return dirp;
        }
    }
    free(dirp);
    free(buffer);
    return 0;    
}

/* Use "not-empty" so that an error isn't confuse with true */
int dir_not_empty(MSDOS *dosp, MSDOSDirEntry *parent)
{
    char *buffer;
    int i, max_read;

    MSDOSDirEntry *dirp, *cur_dir = dosp->current_dir;
    dirp = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    memset(dirp, '\0', sizeof(MSDOSDirEntry));

    max_read = read_dir_from_disk(dosp, parent, &buffer);
    if (max_read == -1)
    {
        perror("Directory read error.\n");
        free(dirp);
        return -1;
    }

    for(i=0;i<max_read;i+=BYTES_PER_DIR_ENTRY)
    {
        msdos_read_dir_entry(dosp, cur_dir, dirp, buffer+i);
        if (dirp->filename[0] == 0x00)
            break;
        if ((strcmp(dirp->tname, ".")==0) || (strcmp(dirp->tname, "..")==0))
            continue;
        if (dirp->filename[0]!=0xe5)
        {
            free(buffer);
            free(dirp);
            return 1;
        }
    }
    free(dirp);
    free(buffer);
    return 0;    
}


/************************************************************************
 * IMPLEMENTATION
 ************************************************************************/

/*
 * TODO: put your implementation here.
 */
int msdos_print_boot_sector(MSDOS *dosp)
{
    int i;

    printf("  Boot Sector Info: \n");
    printf("    Jump to Bootstrap:");
        for(i=0;i<BS_JUMP_LEN;i++)
            printf(" %02x", dosp->boot_sector->jump[i]);
        printf("\n");
    printf("    Manufacturer Name:  %s\n", (dosp->boot_sector->manufacturer_name));
    printf("    Bytes per Sector:  %d\n", dosp->boot_sector->bytes_per_sector);
    printf("    Sectors per Cluster:  %d\n", dosp->boot_sector->sectors_per_cluster);
    printf("    Boot Record Sectors:  %d\n", dosp->boot_sector->boot_record_sectors);
    printf("    Number of Fats:  %d\n", dosp->boot_sector->number_of_fats);
    printf("    Number of Root Entries:  %d\n", dosp->boot_sector->number_of_root_entries);
    printf("    Number of Logical Clusters:  %d\n", dosp->boot_sector->number_of_logical_clusters);
    printf("    Media Type:  %x\n", dosp->boot_sector->media_type);
    printf("    Sectors per Fat:  %d\n", dosp->boot_sector->sectors_per_fat);
    printf("    Sectors per Track:  %d\n", dosp->boot_sector->sectors_per_track);
    printf("    Number of Heads:  %d\n", dosp->boot_sector->number_of_heads);
    printf("    Number of Hidden Sectors:  %d\n", dosp->boot_sector->number_of_hidden_sectors);
    printf("    Signature:");
        for(i=0;i<BS_SIGNATURE_LEN;i++)
            printf(" %02x", dosp->boot_sector->signature[i]);
        printf("\n");

    return 0;
}

int msdos_print_fat(MSDOS *dosp)
{
    long i;
    for(i=0;i<dosp->fat_entries;i++)
        printf("FAT Entry 0x%03lx:  0x%03x   (%ld:  %d)\n", i, dosp->fat_table[i], i, dosp->fat_table[i]);
    return 0;
}

int msdos_ls(MSDOS *dosp)
{
    char *buffer;
    int i, max_read=0, total_bytes=0, num_files=0, num_dirs=0;
    MSDOSDirEntry *dirp, *cur_dir = dosp->current_dir;
    max_read = read_dir_from_disk(dosp, cur_dir, &buffer);
    if (max_read == -1)
    {
        perror("Directory read error.\n");
        return -1;
    }

    printf("    Current Volume is %s\n", dosp->root_dir->tname);
    printf("    Directory of   %s\n", cur_dir->path);

/* Apparently this is included in the FAT system, at least on the floppy image.
   But in case it wasn't here's code to print the directories . and ..
   Code in chdir is more efficient for using these, even though it is available in the FAT.
   if (cur_dir->path[1])
    {
        msdos_print_dir_entry_name_override(cur_dir, ".");
        msdos_print_dir_entry_name_override(cur_dir->parent, "..");
    }
*/
    dirp = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    memset(dirp, '\0', sizeof(MSDOSDirEntry));

    for(i=0;i<max_read;i+=BYTES_PER_DIR_ENTRY)
    {
        msdos_read_dir_entry(dosp, cur_dir, dirp, buffer+i);
        if (dirp->filename[0] == 0x00)
            break;
        if (!(dirp->filename[0] == 0xe5))
        {
             msdos_print_dir_entry(dirp);
             total_bytes += dirp->filesize;
            if (dirp->attr_subdir)
                 num_dirs += 1;
             else
                 num_files += 1;
        }
    }
    
    printf("        %d File(s)      %d bytes\n", num_files, total_bytes);
    printf("        %d Dirs(s)\n", num_dirs);
    printf("\n");
    free(buffer);
    free(dirp);
    return 0;
}

/* Duplicate ls call, just because -> compatible with original dos. */
int msdos_dir(MSDOS *dosp)
{
    return msdos_ls(dosp);
}

int msdos_chdir(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *new_dir;
    if (strcmp(".", name) == 0)
        return 0;
    else if (strcmp("..", name) == 0)
    {
        if (dosp->current_dir == dosp->root_dir)
        {
            printf("No parent of root directory.\n");
            return -1;
        }
        new_dir = dosp->current_dir->parent;
        free(dosp->current_dir);
    }
    else
    {
        new_dir = find_dir_entry(dosp, dosp->current_dir, name);
        if ((!(new_dir)) || (!(new_dir->attr_subdir)))
        {
            printf("No such directory:  %s\n", name);
            free(new_dir);
            return -1;
        }
    }
    dosp->current_dir = new_dir;
    return 0;
}

/* Duplicate cd call due to ill specification */
int msdos_cd(MSDOS *dosp, const char *name)
{
    return msdos_chdir(dosp,name);
}

/* Print current directory name -> useful command to have.*/
int msdos_pwd(MSDOS *dosp)
{
     printf("  %s\n", dosp->current_dir->path);
     return 0;
}

/* Make file, or update time -> useful command to have.*/
int msdos_touch(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep;
    char *buffer, dos_name[TOTAL_FILENAME_LEN+1];
    char filename[DE_FILENAME_LEN+1], extension[DE_EXTENSION_LEN+1];
    int ret;
    unsigned cluster;

    memset(dos_name, 0, TOTAL_FILENAME_LEN+1);
    memset(filename, 0, DE_FILENAME_LEN+1);
    memset(extension, 0, DE_EXTENSION_LEN+1);


    valid_dos_name(name, filename, extension);
    strcpy(dos_name, filename);
    ws_trim(dos_name);
    ws_pad(filename, DE_FILENAME_LEN);
    ws_pad(extension, DE_EXTENSION_LEN);

    if (!(strcmp(extension, "   ") == 0))
    {
        strcat(dos_name, ".");
        strcat(dos_name, extension);
        ws_trim(dos_name);
    }
    if (dos_name[0]==0x05)
        dos_name[0]=0xe5;

    filep = find_dir_entry(dosp, dosp->current_dir, dos_name);
    if (filep)
    {

        if (filep->attr_read_only == 1)
        {
            printf("No access permissions to file: %s", name);
            free(filep);
            return -1;
        }

        /* Rather than actually updating the directory entry, be lazy.
           remove the entry, then make a new one with the updated time. */
        if (remove_dir_entry(dosp, filep))
        {
            printf("Update error. Filesystem possibly corrupted.\n");
            msdos_write_fat_table(dosp);
            free(filep);
            return -1;
        }

        DE_NOW(filep);

        if (add_dir_entry(dosp, filep))
        {
            printf("Update error. Filesystem possibly corrupted.\n");
            msdos_write_fat_table(dosp);
            free(filep);
            return -1;
        }

        free(filep);
        return 0;
    }

    buffer = (char *) malloc(sizeof(char)*BYTES_PER_CLUSTER);
    filep = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    if ((buffer == 0) || (filep ==0))
    {
        fprintf(stderr, "Out of memory\n");
        return -1;
    }
    memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);
    memset(filep, '\0', sizeof(MSDOSDirEntry));
    strcpy(filep->filename, filename);
    strcpy(filep->extension, extension);
    filep->parent = dosp->current_dir;

    strcpy(filep->tname, dos_name);
    strcpy(filep->path, filep->parent->path);
    if (filep->parent != dosp->root_dir)
        strcat(filep->path, "\\");
    strcat(filep->path, filep->tname);

    DE_NOW(filep);

    filep->filesize = 0;
    filep->attr_subdir = 0;
    convert_to_attr_byte(filep);

    filep->first_cluster = insert_file_fat(dosp, dosp->current_dir->first_cluster, 0xfff);

    cluster = filep->first_cluster;

    if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
          || (cluster == -1) || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        perror("File allocation error.\n");
        free(buffer);
        free(filep);
        return -1;
    }

    memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);

    ret = msdos_cluster_write(dosp, cluster, buffer);
    if (ret == -1)
    {
        perror("File write error.\n");
        free(buffer);
        free(filep);
        return -1;
    }


    if (add_dir_entry(dosp, filep))
    {
        printf("Creation error. Filesystem possibly corrupted.\n");
        msdos_write_fat_table(dosp);
        free(buffer);
        free(filep);
        return -1;
    }

    msdos_write_fat_table(dosp);
    free(buffer);
    free(filep);
    return 0;
}

int msdos_rm(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep;
    filep = find_dir_entry(dosp, dosp->current_dir, name);
    if ((!(filep)) || (filep->attr_subdir))
    {
        printf("No such file:  %s\n", name);
        free(filep);
        return -1;
    }
    
    if (filep->first_cluster <= 0)
    {
        printf("Invalid file: %s\n", name);
        free(filep);
        return -1;
    }

    if (filep->attr_read_only == 1)
    {
        printf("No access permissions for file: %s", name);
        free(filep);
        return -1;
    }

    if (wipe_file_fat(dosp, filep) || remove_dir_entry(dosp, filep))
    {
        printf("Deletion error. Filesystem possibly corrupted.\n");
        free(filep);
        msdos_write_fat_table(dosp);
        return -1;
    }

    free(filep);
    msdos_write_fat_table(dosp);
    return 0;
}

/* Duplicate rm call, just because -> compatible with original dos. */
int msdos_erase(MSDOS *dosp, const char *name)
{
    return msdos_rm(dosp, name);
}

int msdos_mkdir(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep, *child, *parent;
    char *buffer, dos_name[TOTAL_FILENAME_LEN+1];
    char filename[DE_FILENAME_LEN+1], extension[DE_EXTENSION_LEN+1];
    int ret;
    unsigned cluster;

    memset(dos_name, 0, TOTAL_FILENAME_LEN+1);
    memset(filename, 0, DE_FILENAME_LEN+1);
    memset(extension, 0, DE_EXTENSION_LEN+1);

    valid_dos_name(name, filename, extension);
    strcpy(dos_name, filename);
    ws_trim(dos_name);
    if (dos_name[0]==0x05)
        dos_name[0]=0xe5;

    ws_pad(filename, DE_FILENAME_LEN);
    ws_pad(extension, DE_EXTENSION_LEN);

    filep = find_dir_entry(dosp, dosp->current_dir, dos_name);
    if (filep)
    {
        printf("Directory already exists:  %s\n", name);
        printf("  Not overwritten.\n");
        free(filep);
        return -1;
    }

    buffer = (char *) malloc(sizeof(char)*BYTES_PER_CLUSTER);
    filep = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    if ((buffer == 0) || (filep ==0))
    {
        fprintf(stderr, "Out of memory\n");
        return -1;
    }
    memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);
    memset(filep, '\0', sizeof(MSDOSDirEntry));
    strcpy(filep->filename, filename);
    strcpy(filep->extension, extension);
    filep->parent = dosp->current_dir;

    strcpy(filep->tname, dos_name);
    strcpy(filep->path, filep->parent->path);
    if (filep->parent != dosp->root_dir)
        strcat(filep->path, "\\");
    strcat(filep->path, filep->tname);

    DE_NOW(filep);

    filep->filesize = 0;
    filep->attr_subdir = 1;
    convert_to_attr_byte(filep);

    filep->first_cluster = insert_file_fat(dosp, dosp->current_dir->first_cluster, 0xfff);

    cluster = filep->first_cluster;

    if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
          || (cluster == -1) || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        perror("Directory allocation error.\n");
        free(buffer);
        free(filep);
        return -1;
    }

    memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);

    ret = msdos_cluster_write(dosp, cluster, buffer);
    if (ret == -1)
    {
        perror("File write error.\n");
        free(buffer);
        free(filep);
        return -1;
    }

    /* make . and .. entries -- copy the directory entries, 
       then hack them a little to use add_dir_entry for simplicity */
    child = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    parent = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    if ((child == 0) || (parent ==0))
    {
        fprintf(stderr, "Out of memory\n");
        
        return -1;
    }
    memcpy(child, filep, sizeof(MSDOSDirEntry));
    memcpy(parent, filep->parent, sizeof(MSDOSDirEntry));

    strcpy(child->filename, ".");
    strcpy(parent->filename, "..");
    ws_pad(child->filename, DE_FILENAME_LEN);
    ws_pad(parent->filename, DE_FILENAME_LEN);

    child->parent = filep;
    parent->parent = filep;


    if ((add_dir_entry(dosp, filep)) || (add_dir_entry(dosp, child)) || (add_dir_entry(dosp, parent)))
    {
        printf("Creation error. Filesystem possibly corrupted.\n");
        msdos_write_fat_table(dosp);
        free(buffer);
        free(filep);
        free(child);
        free(parent);
        return -1;
    }
    msdos_write_fat_table(dosp);

    free(buffer);
    free(filep);
    free(child);
    free(parent);
    return 0;
}

/* Duplicate mkdir call, just because -> compatible with original dos. */
int msdos_md(MSDOS *dosp, const char *name)
{
    return msdos_md(dosp, name);
}

int msdos_rmdir(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep;
    filep = find_dir_entry(dosp, dosp->current_dir, name);

    if ((!(filep)) || (!(filep->attr_subdir)))
    {
        printf("No such directory:  %s\n", name);
        free(filep);
        return -1;
    }
   
    if ((filep->first_cluster <= 0) || (strcmp(name, ".")==0) || (strcmp(name, "..")==0))
    {
        printf("Invalid directory: %s\n", name);
        free(filep);
        return -1;
    }

    if (filep->attr_read_only == 1)
    {
        printf("No access permissions to directory: %s", name);
        free(filep);
        return -1;
    }

    if (dir_not_empty(dosp, filep))
    {
        printf("Directory not empty: %s\n", name);
        free(filep);
        return -1;
    }

    if (wipe_file_fat(dosp, filep) || remove_dir_entry(dosp, filep))
    {
        printf("Deletion error. Filesystem possibly corrupted.\n");
        free(filep);
        msdos_write_fat_table(dosp);
        return -1;
    }

    free(filep);
    msdos_write_fat_table(dosp);
    return 0;
}

/* Duplicate rmdir call, just because -> compatible with original dos. */
int msdos_rd(MSDOS *dosp, const char *name)
{
    return msdos_rmdir(dosp, name);
}

int msdos_save(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep;
    char *buffer;
    int fd, ret, i, clusters_read;
    unsigned cluster;

    filep = find_dir_entry(dosp, dosp->current_dir, name);
    if ((!(filep)) || (filep->attr_subdir))
    {
        printf("No such file:  %s\n", name);
        free(filep);
        return -1;
    }
    
    clusters_read = count_clusters(dosp, filep);
    if (clusters_read <= 0)
    {
        printf("Invalid file: %s\n", name);
        free(filep);
        return -1;
    }

    buffer = (char *) malloc(sizeof(char)*BYTES_PER_CLUSTER);
    if (buffer == 0) 
    {
        fprintf(stderr, "Out of memory\n");
        free(filep);
        return -1;
    }

    fd = open(name, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd < 0)
    {
        perror(name);
        perror("  Not overwritten.\n");
        return -1;
    }


    cluster = filep->first_cluster;
    for(i=1;i<=clusters_read;i++)
    {
        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
                       || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
        {
            perror("File allocation error.\n");
            close(fd);
            free(buffer);
            free(filep);
            return -1;
        }
        ret = msdos_cluster_read(dosp, cluster, buffer);
        if (ret == -1)
        {
            perror("File read error.\n");
            close(fd);
            free(buffer);
            free(filep);
            return -1;
        }

        /* If last cluster, and not full, just write extra, rather than whole cluster */
        if ((i==clusters_read) && ((filep->filesize % BYTES_PER_CLUSTER)!=0))  
            write(fd, buffer, filep->filesize % BYTES_PER_CLUSTER);
        else
            write(fd, buffer, BYTES_PER_CLUSTER);

        cluster = dosp->fat_table[cluster];

    }

    close(fd);
    free(buffer);
    free(filep);

    return 0;
}

/* Duplicate save call, for clarity. */
int msdos_read(MSDOS *dosp, const char *name)
{
    return msdos_save(dosp, name);
}

/* Duplicate save call, for clarity. */
int msdos_export(MSDOS *dosp, const char *name)
{
    return msdos_save(dosp, name);
}

/* I don't like the use of write here, as the direction is ambiguous. 
   Call it load, instead, to mirror "save", though "write" has been included as well. */
int msdos_load(MSDOS *dosp, const char *name)
{
    MSDOSDirEntry *filep;
    char *buffer, dos_name[TOTAL_FILENAME_LEN+1];
    char filename[DE_FILENAME_LEN+1], extension[DE_EXTENSION_LEN+1];
    int fd, ret, i, clusters_write, max_write;
    unsigned cluster;

    memset(dos_name, 0, TOTAL_FILENAME_LEN+1);
    memset(filename, 0, DE_FILENAME_LEN+1);
    memset(extension, 0, DE_EXTENSION_LEN+1);

    struct stat fstats;

    ret = stat(name , &fstats);
        
    if (ret == -1)
    {
        printf("No such file: %s\n", name);
        return -1;
    }

    if (S_ISDIR(fstats.st_mode))
    {
        printf("Cannot load a directory.\n");
        return -1;      
    }

    fd = open(name, O_RDONLY | O_EXCL, 0666);
    if (fd < 0)
    {
        perror(name);
        return -1;
    }

    valid_dos_name(name, filename, extension);
    strcpy(dos_name, filename);
    ws_trim(dos_name);
    ws_pad(filename, DE_FILENAME_LEN);
    ws_pad(extension, DE_EXTENSION_LEN);

    if (!(strcmp(extension, "   ") == 0))
    {
        strcat(dos_name, ".");
        strcat(dos_name, extension);
        ws_trim(dos_name);
    }
    if (dos_name[0]==0x05)
        dos_name[0]=0xe5;

    filep = find_dir_entry(dosp, dosp->current_dir, dos_name);
    if (filep)
    {
        printf("File already exists:  %s\n", name);
        printf("  Not overwritten.\n");
        free(filep);
        return -1;
    }

    buffer = (char *) malloc(sizeof(char)*BYTES_PER_CLUSTER);
    filep = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    if ((buffer == 0) || (filep ==0)) 
    {
        fprintf(stderr, "Out of memory\n");
        close(fd);
        return -1;
    }
    memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);
    memset(filep, '\0', sizeof(MSDOSDirEntry));
    strcpy(filep->filename, filename);
    strcpy(filep->extension, extension);
    filep->parent = dosp->current_dir;

    strcpy(filep->tname, dos_name);
    strcpy(filep->path, filep->parent->path);
    if (filep->parent != dosp->root_dir)
        strcat(filep->path, "\\");
    strcat(filep->path, filep->tname);

    DE_NOW(filep);

    filep->filesize = fstats.st_size;
    filep->parent = dosp->current_dir;
    filep->first_cluster = insert_file_fat(dosp, dosp->current_dir->first_cluster, 0xfff);

    cluster = filep->first_cluster;

    if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
          || (cluster == -1) || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
    {
        perror("File allocation error.\n");
        close(fd);
        free(buffer);
        free(filep);
        return -1;
    }


    max_write = filep->filesize;
    clusters_write = max_write/BYTES_PER_CLUSTER + ((max_write%BYTES_PER_CLUSTER)!=0);

    for(i=1;i<=clusters_write;i++)
    {
        memset(buffer, '\0', sizeof(char)*BYTES_PER_CLUSTER);
        /* If last cluster, and not full, just read extra, rather than whole cluster */
        if ((i==clusters_write) && ((filep->filesize % BYTES_PER_CLUSTER)!=0))  
            read(fd, buffer, filep->filesize % BYTES_PER_CLUSTER);
        else
            read(fd, buffer, BYTES_PER_CLUSTER); 
        ret = msdos_cluster_write(dosp, cluster, buffer);
        if (ret == -1)
        {
            perror("12File write error.\n");
            close(fd);
            free(buffer);
            free(filep);
            return -1;
        }
        if (i != clusters_write)
            cluster = insert_file_fat(dosp, cluster, cluster);

        if ((cluster == 0x000) || (cluster == 0xff7) || ((0xff0 <= cluster) && (cluster <= 0xff6)) 
              || (cluster == -1) || (cluster > 0xfff) || ((0xff8 <= cluster) && (cluster <= 0xfff)))
        {
            perror("File allocation error.\n");
            close(fd);
            free(buffer);
            free(filep);
            return -1;
        }

    }

    if (add_dir_entry(dosp, filep))
    {
        printf("Creation error. Filesystem possibly corrupted.\n");
        msdos_write_fat_table(dosp);
        free(buffer);
        free(filep);
        close(fd);
        return -1;
    }

    msdos_write_fat_table(dosp);

    close(fd);
    free(buffer);
    free(filep);

    return 0;
}

/* Duplicate load call, for clarity. */
int msdos_write(MSDOS *dosp, const char *name)
{
    return msdos_load(dosp, name);
}

/* Duplicate load call, for clarity. */
int msdos_import(MSDOS *dosp, const char *name)
{
    return msdos_load(dosp, name);
}

/* Make a local system call -> nice for testing purposes. */
int msdos_local(MSDOS *dosp, const char *name)
{
printf("%s\n", name);
    return system(name);
}

/* Duplicate local call, for ease, used as l<command>. */
int msdos_l_local(MSDOS *dosp, const char *name)
{
    return msdos_local(dosp, name);
}

/************************************************************************
 * FILESYSTEM
 ************************************************************************/

/* Read boot_sector into memory */
void msdos_read_boot_sector(MSDOS *dosp)
{

    int i=0;
    char *buffer = (char *) malloc(sizeof(char)*BYTES_PER_SECTOR);
    memset(buffer, 0, sizeof(char)*BYTES_PER_SECTOR);
    msdos_sector_read(dosp, BOOT_SECTOR, buffer);

    for(i=0;i<BS_JUMP_LEN;i++)
        dosp->boot_sector->jump[i] = get_unsigned(buffer + BS_JUMP_OFF+i, 1);
    for(i=0;i<BS_MANUFACTURER_NAME_LEN;i++)
        dosp->boot_sector->manufacturer_name[i] = get_unsigned(buffer + BS_MANUFACTURER_NAME_OFF+i, 1);
    dosp->boot_sector->bytes_per_sector = get_unsigned(buffer + BS_BYTES_PER_SECTOR_OFF, BS_BYTES_PER_SECTOR_LEN);
    dosp->boot_sector->sectors_per_cluster = get_unsigned(buffer + BS_SECTORS_PER_CLUSTER_OFF, BS_SECTORS_PER_CLUSTER_LEN);
    dosp->boot_sector->boot_record_sectors = get_unsigned(buffer + BS_BOOT_RECORD_SECTORS_OFF, BS_BOOT_RECORD_SECTORS_LEN);
    dosp->boot_sector->number_of_fats = get_unsigned(buffer + BS_NUMBER_OF_FATS_OFF, BS_NUMBER_OF_FATS_LEN);
    dosp->boot_sector->number_of_root_entries = get_unsigned(buffer + BS_NUMBER_OF_ROOT_ENTRIES_OFF, BS_NUMBER_OF_ROOT_ENTRIES_LEN);
    dosp->boot_sector->number_of_logical_clusters = get_unsigned(buffer + BS_NUMBER_OF_LOGICAL_CLUSTERS_OFF, BS_NUMBER_OF_LOGICAL_CLUSTERS_LEN);
    dosp->boot_sector->media_type = get_unsigned(buffer + BS_MEDIA_TYPE_OFF, BS_MEDIA_TYPE_LEN);
    dosp->boot_sector->sectors_per_fat = get_unsigned(buffer + BS_SECTORS_PER_FAT_OFF, BS_SECTORS_PER_FAT_LEN);
    dosp->boot_sector->sectors_per_track = get_unsigned(buffer + BS_SECTORS_PER_TRACK_OFF, BS_SECTORS_PER_TRACK_LEN);
    dosp->boot_sector->number_of_heads = get_unsigned(buffer + BS_NUMBER_OF_HEADS_OFF, BS_NUMBER_OF_HEADS_LEN);
    dosp->boot_sector->number_of_hidden_sectors = get_unsigned(buffer + BS_NUMBER_OF_HIDDEN_SECTORS_OFF, BS_NUMBER_OF_HIDDEN_SECTORS_LEN);
    for(i=0;i<BS_SIGNATURE_LEN;i++)
        dosp->boot_sector->signature[i] = get_unsigned(buffer + BS_SIGNATURE_OFF+i, 1);

    free(buffer);
}

/* Write boot_sector from memory to disk */
void msdos_write_boot_sector(MSDOS *dosp)
{
    int i=0;
    char *buffer = (char *) malloc(sizeof(char)*BYTES_PER_SECTOR);
    memset(buffer, 0, sizeof(char)*BYTES_PER_SECTOR);

    for(i=0;i<BS_JUMP_LEN;i++)
        put_unsigned(buffer + BS_JUMP_OFF + i, 1, dosp->boot_sector->jump[i]);
    for(i=0;i<BS_MANUFACTURER_NAME_LEN;i++)
        put_unsigned(buffer + BS_MANUFACTURER_NAME_OFF + i, 1, dosp->boot_sector->manufacturer_name[i]);
    put_unsigned(buffer + BS_BYTES_PER_SECTOR_OFF, BS_BYTES_PER_SECTOR_LEN, dosp->boot_sector->bytes_per_sector);
    put_unsigned(buffer + BS_SECTORS_PER_CLUSTER_OFF, BS_SECTORS_PER_CLUSTER_LEN, dosp->boot_sector->sectors_per_cluster);
    put_unsigned(buffer + BS_BOOT_RECORD_SECTORS_OFF, BS_BOOT_RECORD_SECTORS_LEN, dosp->boot_sector->boot_record_sectors);
    put_unsigned(buffer + BS_NUMBER_OF_FATS_OFF, BS_NUMBER_OF_FATS_LEN, dosp->boot_sector->number_of_fats);
    put_unsigned(buffer + BS_NUMBER_OF_ROOT_ENTRIES_OFF, BS_NUMBER_OF_ROOT_ENTRIES_LEN, dosp->boot_sector->number_of_root_entries);
    put_unsigned(buffer + BS_NUMBER_OF_LOGICAL_CLUSTERS_OFF, BS_NUMBER_OF_LOGICAL_CLUSTERS_LEN, dosp->boot_sector->number_of_logical_clusters);
    put_unsigned(buffer + BS_MEDIA_TYPE_OFF, BS_MEDIA_TYPE_LEN, dosp->boot_sector->media_type);
    put_unsigned(buffer + BS_SECTORS_PER_FAT_OFF, BS_SECTORS_PER_FAT_LEN, dosp->boot_sector->sectors_per_fat);
    put_unsigned(buffer + BS_SECTORS_PER_TRACK_OFF, BS_SECTORS_PER_TRACK_LEN, dosp->boot_sector->sectors_per_track);
    put_unsigned(buffer + BS_NUMBER_OF_HEADS_OFF, BS_NUMBER_OF_HEADS_LEN, dosp->boot_sector->number_of_heads);
    put_unsigned(buffer + BS_NUMBER_OF_HIDDEN_SECTORS_OFF, BS_NUMBER_OF_HIDDEN_SECTORS_LEN, dosp->boot_sector->number_of_hidden_sectors);
    for(i=0;i<BS_SIGNATURE_LEN;i++)
        put_unsigned(buffer + BS_SIGNATURE_OFF + i, 1, dosp->boot_sector->signature[i]);
    
    msdos_sector_write(dosp, BOOT_SECTOR, buffer);
    free(buffer);
}

/* Read FAT table into memory */
void msdos_read_fat_table(MSDOS *dosp)
{
    int i,k, ret;
    unsigned char *raw_fat = (char *) malloc(sizeof(char)*SECTORS_PER_FAT*BYTES_PER_SECTOR);

    memset(raw_fat, 0, sizeof(char)*SECTORS_PER_FAT*BYTES_PER_SECTOR);

    /* If ignore possiblity of corrupt fat -> would just load in additional fat(s) to check */
    /* msdos_nsector_read(dosp, BOOT_RECORD_SECTORS+SECTORS_PER_FAT, SECTORS_PER_FAT, raw_fat); */

    /* But I'll do it as if some sectors could be corrupted.
       If so, try the same section of the FAT, from the next entry */
    for(i=0;i<SECTORS_PER_FAT;i++)
    {
        for(k=0;k<NUMBER_OF_FATS;k++)
        {
            ret = msdos_sector_read(dosp, BOOT_RECORD_SECTORS+i+SECTORS_PER_FAT*k, raw_fat+i*BYTES_PER_SECTOR);
            if (ret != -1)
                break;
        }
    }

    k=0;
    /* Convert raw fat to unpacked table */
    for(i=0;i<SECTORS_PER_FAT*BYTES_PER_SECTOR;i+=3)
    {
        dosp->fat_table[k] = ((0x0f & raw_fat[i+1])<<8) | (raw_fat[i]);
        dosp->fat_table[k+1] = (raw_fat[i+2]<<4) | (raw_fat[i+1]>>4);
        k+=2;
    }

    free(raw_fat);
}

/* Write FAT table from memory to disk */
void msdos_write_fat_table(MSDOS *dosp)
{
    int i,k;
    unsigned char *raw_fat = (char *) malloc(sizeof(char)*SECTORS_PER_FAT*BYTES_PER_SECTOR);

    memset(raw_fat, 0, sizeof(char)*SECTORS_PER_FAT*BYTES_PER_SECTOR);
    i=0;

    /* Convert table to packed raw fat */
    for(k=0;k<dosp->fat_entries;k+=2)
    {
        raw_fat[i] = (char) (dosp->fat_table[k]);
        raw_fat[i+1] = (char) (((0xf0 & (dosp->fat_table[k+1]<<4))) | (0x0f & (dosp->fat_table[k]>>8)));
        raw_fat[i+2] = (char) (dosp->fat_table[k+1]>>4);
        i+=3;
    }

    /* Write to all FAT tables, assume no fails*/
    for(i=0;i<NUMBER_OF_FATS;i++)    
        msdos_nsector_write(dosp, 1+i*SECTORS_PER_FAT, SECTORS_PER_FAT, raw_fat);
    free(raw_fat);
}

void msdos_read_dir_entry(MSDOS *dosp, MSDOSDirEntry *parent, MSDOSDirEntry *child, char *buffer)
{
    int i;
    memset(child, '\0', sizeof(MSDOSDirEntry));

    for(i=0;i<DE_FILENAME_LEN;i++)
        child->filename[i] = get_unsigned(buffer + DE_FILENAME_OFF + i, 1);
    for(i=0;i<DE_EXTENSION_LEN;i++)
        child->extension[i] = get_unsigned(buffer + DE_EXTENSION_OFF + i, 1);
    child->attr = get_unsigned(buffer + DE_ATTR_OFF, DE_ATTR_LEN);
    for(i=0;i<DE_RESERVED_LEN;i++)
        child->reserved[i] = get_unsigned(buffer + DE_RESERVED_OFF + i, 1);
    child->time = get_unsigned(buffer + DE_TIME_OFF, DE_TIME_LEN);
    child->date = get_unsigned(buffer + DE_DATE_OFF, DE_DATE_LEN);
    child->first_cluster = get_unsigned(buffer + DE_FIRST_CLUSTER_OFF, DE_FIRST_CLUSTER_LEN);
    child->filesize = get_unsigned(buffer + DE_FILESIZE_OFF, DE_FILESIZE_LEN);

    child->parent = parent;

    convert_to_timestamp(child);
    convert_from_attr_byte(child);
    strcpy(child->tname, child->filename);

    ws_trim(child->tname);
    if (!((child->attr_subdir) || (child->attr_vlabel) || (strcmp(child->extension, "   ") == 0)))
    {
        strcat(child->tname, ".");
        strcat(child->tname, child->extension);
        ws_trim(child->tname);
    }
    if (child->tname[0]==0x05)
        child->tname[0]=0xe5;

    strcpy(child->path, parent->path);
    if (parent != dosp->root_dir)
        strcat(child->path, "\\");
    strcat(child->path, child->tname);
}

void msdos_write_dir_entry(MSDOS *dosp, MSDOSDirEntry *dirp, char *buffer)
{
    int i;
    convert_from_timestamp(dirp);
    convert_to_attr_byte(dirp);
    for(i=0;i<DE_FILENAME_LEN;i++)
        put_unsigned(buffer + DE_FILENAME_OFF + i, 1, dirp->filename[i]);
    for(i=0;i<DE_EXTENSION_LEN;i++)
        put_unsigned(buffer + DE_EXTENSION_OFF + i, 1, dirp->extension[i]);
    put_unsigned(buffer + DE_ATTR_OFF, DE_ATTR_LEN, dirp->attr);
    for(i=0;i<DE_RESERVED_LEN;i++)
        put_unsigned(buffer + DE_RESERVED_OFF + i, 1, dirp->reserved[i]);
    put_unsigned(buffer + DE_TIME_OFF, DE_TIME_LEN, dirp->time);
    put_unsigned(buffer + DE_DATE_OFF, DE_DATE_LEN, dirp->date);
    put_unsigned(buffer + DE_FIRST_CLUSTER_OFF, DE_FIRST_CLUSTER_LEN, dirp->first_cluster);
    put_unsigned(buffer + DE_FILESIZE_OFF, DE_FILESIZE_LEN, dirp->filesize);
}

MSDOSDirEntry *msdos_read_new_dir_entry(MSDOS *dosp, MSDOSDirEntry *parent, char *buffer)
{
    MSDOSDirEntry *child;
   
    child = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    memset(child, '\0', sizeof(MSDOSDirEntry));

    if (child == 0)
        return 0;
    msdos_read_dir_entry(dosp, parent, child, buffer);
    return child;
}


/*
 * Close the DOS filesystem.
 */
void msdos_close(MSDOS *dosp)
{
    MSDOSDirEntry *temp_dir;
    msdos_write_fat_table(dosp);
    if (dosp->fd >= 0)
        close(dosp->fd);
    /*
     * TODO: if you have added more fields to the
     *    MSDOS struct, you may need to delete them when the
     *    filesystem is closed.
     */

    free(dosp->boot_sector);
    free(dosp->fat_table);

    while(dosp->root_dir != dosp->current_dir)
    {
        temp_dir = dosp->current_dir->parent;
        free(dosp->current_dir);
        dosp->current_dir = temp_dir;
    }
    free(dosp->root_dir);

    free(dosp);
}

/*
 * Open an initialize a DOS filesystem.
 */
MSDOS *msdos_open(const char *devname)
{
    MSDOS *dosp;
    char *buffer;
    int ret;

    /* Allocate space */
    dosp = (MSDOS *) malloc(sizeof(MSDOS));
    if (dosp == 0) 
    {
        fprintf(stderr, "%s: out of memory\n", devname);
        return 0;
    }
    memset(dosp, '\0', sizeof(MSDOS));

    /* Open the device read/write */
    dosp->fd = open(devname, O_RDWR, 0666);
    if (dosp->fd < 0) 
    {
        perror(devname);
        msdos_close(dosp);
        return 0;
    }

    /*
     * TODO: If you are adding more fields
     *    to the MSDOS structure, initialize them here.
     */

    dosp->boot_sector = (MSDOSBootSector *) malloc(sizeof(MSDOSBootSector));
    if (dosp->boot_sector == 0) 
    {
        fprintf(stderr, "%s: out of memory\n", devname);
        msdos_close(dosp);
        return 0;
    }
    memset(dosp->boot_sector, '\0', sizeof(MSDOSBootSector));

    buffer = (char *) malloc(sizeof(char)*BS_BYTES_PER_SECTOR_LEN); 

    /* Grab a few appropriate bytes off of disk, to get BYTES_PER_SECTOR) */
    ret = lseek(dosp->fd, BS_BYTES_PER_SECTOR_OFF, SEEK_SET);
    if (ret < 0) 
    {
        perror("msdos_open: lseek");
        msdos_close(dosp);
        return 0;
    }

    ret = read(dosp->fd, buffer, BS_BYTES_PER_SECTOR_LEN);
    if (ret != BS_BYTES_PER_SECTOR_LEN) 
    {
        perror("msdos_open: read");
        msdos_close(dosp);
        return 0;
    }
    dosp->boot_sector->bytes_per_sector = get_unsigned(buffer, BS_BYTES_PER_SECTOR_LEN);


    msdos_read_boot_sector(dosp);

    dosp->fat_entries = SECTORS_PER_FAT*BYTES_PER_SECTOR*BITS_PER_BYTE/BITS_PER_FAT_ENTRY;
    dosp->fat_table = (short *) malloc(sizeof(short)*dosp->fat_entries);
    if (dosp->fat_table == 0) 
    {
        fprintf(stderr, "%s: out of memory\n", devname);
        msdos_close(dosp);
        return 0;
    }
    memset(dosp->fat_table, 0, sizeof(short)*dosp->fat_entries);
    msdos_read_fat_table(dosp);

    dosp->root_dir = (MSDOSDirEntry *) malloc(sizeof(MSDOSDirEntry));
    if ((dosp->root_dir == 0))
    {
        fprintf(stderr, "%s: out of memory\n", devname);
        msdos_close(dosp);
        return 0;
    }
    memset(dosp->root_dir, '\0', sizeof(MSDOSDirEntry));

    /* assume first directory entry of root sector is volume label -> grab it, then fix a few values */
    /* It's a bit of a hack, but oh, well, it's better than duplicating all the code. */

    free(buffer);
    buffer = (char *) malloc(sizeof(char)*BYTES_PER_SECTOR); 

    ret = msdos_sector_read(dosp, ROOT_DIR_SECTOR, buffer);
    if (ret == -1)
    {
        perror("Error reading disk.\n");
        return 0;
    }

    msdos_read_dir_entry(dosp, dosp->root_dir, dosp->root_dir, buffer);
    strcpy(dosp->root_dir->path, "\\");
    dosp->root_dir->parent = NULL;
    dosp->root_dir->attr=0;
    convert_from_attr_byte(dosp->root_dir);

    dosp->current_dir = dosp->root_dir; 
    free(buffer);

    return dosp;
}

/************************************************************************
 * MAIN FUNCTION
 ************************************************************************/

/*
 * Main loop.
 */
static void shell(MSDOS *dosp)
{
    char buffer[100];
    char command[100], command_orig[100], arg[100], arg_orig[100];
    char *c;
    /* Read the entire line */
    while(1) 
    {
        /* Prompt and get input */
        printf("%s:%s> ", dosp->root_dir->tname, dosp->current_dir->path);
        fflush(stdout);
        command[0] = 0;
        arg[0] = 0;

        /* Read args */
        if (fgets(buffer, sizeof(buffer), stdin) == 0)
            break;
/*        if (sscanf(buffer, "%s %s\n", command, arg) == 0)
            continue; */

        if (sscanf(buffer, "%s", command) == 0) 
            continue;
        strncpy(arg, buffer+strlen(command)+1, sizeof(arg)-strlen(command)-1);
        if (arg[0] != '\0')
            *strchr(arg, '\n') = '\0';

        if (command[0]==0)
            continue;

        /* keep copy of command, just in case. */
        strcpy(command_orig, command);

        /* convert command to lower case */
        for (c = command; *c != '\0'; c++)
        {
            *c = tolower(*c);
        }

        /* keep copy of arg for case-sensitive commands, like write. */
        strcpy(arg_orig, arg);

        /* convert arg to upper case */
        for (c = arg; *c != '\0'; c++)
        {
            *c = toupper(*c);
        }

        /* Do commands */
        if (strcmp(command, "boot") == 0)
            msdos_print_boot_sector(dosp);
        else if (strcmp(command, "fat") == 0)
            msdos_print_fat(dosp);
        else if (strcmp(command, "ls") == 0)
            msdos_ls(dosp);
        else if (strcmp(command, "dir") == 0)
            msdos_dir(dosp);
        else if (strcmp(command, "cd") == 0)
            msdos_cd(dosp, arg);
        else if (strcmp(command, "chdir") == 0)
            msdos_chdir(dosp, arg);
        else if (strcmp(command, "pwd") == 0)
            msdos_pwd(dosp);
        else if (strcmp(command, "touch") == 0)
            msdos_touch(dosp, arg);
        else if (strcmp(command, "rm") == 0)
            msdos_rm(dosp, arg);
        else if (strcmp(command, "erase") == 0)
            msdos_erase(dosp, arg);
        else if (strcmp(command, "mkdir") == 0)
            msdos_mkdir(dosp, arg);
        else if (strcmp(command, "md") == 0)
            msdos_md(dosp, arg);
        else if (strcmp(command, "rmdir") == 0)
            msdos_rmdir(dosp, arg);
        else if (strcmp(command, "rd") == 0)
            msdos_rd(dosp, arg);
        else if (strcmp(command, "save") == 0)
            msdos_save(dosp, arg);
        else if (strcmp(command, "read") == 0)
            msdos_read(dosp, arg);
        else if (strcmp(command, "export") == 0)
            msdos_export(dosp, arg);
        else if (strcmp(command, "load") == 0)
            msdos_load(dosp, arg_orig);
        else if (strcmp(command, "write") == 0)
            msdos_write(dosp, arg_orig);
        else if (strcmp(command, "import") == 0)
            msdos_import(dosp, arg_orig);
        else if (strcmp(command, "local") == 0)
            msdos_local(dosp, arg_orig);
        else if (strncmp(command, "l",1) == 0)
            {strcat(command_orig, " ");
             strcat(command_orig, arg_orig);
             msdos_l_local(dosp, command_orig+1);}
        else if (strcmp(command, "exit") == 0)
            break;
        else if (strcmp(command, "quit") == 0)
            break;
        else
            fprintf(stderr, "unknown command: %s\n", command);
    }
}

int main(int argc, char **argv)
{
    MSDOS *dosp;
    /* Usage: msdos devname */
    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <devname>\n", argv[0]);
        return 1;
    }

    /* Read the filesystem */
    dosp = msdos_open(argv[1]);
    if (dosp == 0) 
    {
        fprintf(stderr, "%s: can't open %s\n", argv[0], argv[1]);
        return 1;
    }

    /* Main loop */
    shell(dosp);
    /* Close the filesystem */
    msdos_close(dosp);
    return 0;
}


