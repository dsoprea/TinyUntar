#ifndef __UNTAR_H
#define __UNTAR_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define IS_BASE256_ENCODED(buffer) (((unsigned char)buffer[0] & 0x80) > 0)
#define GET_NUM_BLOCKS(filesize) (int)ceil((double)filesize / (double)TAR_BLOCK_SIZE)
#define GET_LAST_BLOCK_PORTION_SIZE(filesize) (filesize % TAR_BLOCK_SIZE)

#ifdef _MSC_VER
	#define strtoull _strtoui64
	#define snprintf _snprintf
#endif

#define TAR_T_NORMAL1 0
#define TAR_T_NORMAL2 '0'
#define TAR_T_HARD '1'
#define TAR_T_SYMBOLIC '2'
#define TAR_T_CHARSPECIAL '3'
#define TAR_T_BLOCKSPECIAL '4'
#define TAR_T_DIRECTORY '5'
#define TAR_T_FIFO '6'
#define TAR_T_CONTIGUOUS '7'
#define TAR_T_GLOBALEXTENDED 'g'
#define TAR_T_EXTENDED 'x'

enum entry_type_e { T_NORMAL, T_HARDLINK, T_SYMBOLIC, T_CHARSPECIAL, 
                    T_BLOCKSPECIAL, T_DIRECTORY, T_FIFO, T_CONTIGUOUS, 
                    T_GLOBALEXTENDED, T_EXTENDED, T_OTHER };

#define TAR_BLOCK_SIZE 512

#define TAR_HT_PRE11988 1
#define TAR_HT_P10031 2

// Describes a header for TARs conforming to pre-POSIX.1-1988 .
struct header_s
{
    const char filename[100];
    const char filemode[8];
    const char uid[8];
    const char gid[8];
    const char filesize[12];
    const char mtime[12];
    const char checksum[8];
    const char type;
    const char link_target[100];

    const char ustar_indicator[6];
    const char ustar_version[2];
    const char user_name[32];
    const char group_name[32];
    const char device_major[8];
    const char device_minor[8];
};

typedef struct header_s header_t;

struct header_translated_s
{
    char filename[101];
    unsigned long long filemode;
    unsigned long long uid;
    unsigned long long gid;
    unsigned long long filesize;
    unsigned long long mtime;
    unsigned long long checksum;
    enum entry_type_e type;
    char link_target[101];

    char ustar_indicator[6];
    char ustar_version[3];
    char user_name[32];
    char group_name[32];
    unsigned long long device_major;
    unsigned long long device_minor;
};

typedef struct header_translated_s header_translated_t;

typedef int (*entry_header_callback_t)(header_translated_t *header, 
                                       int entry_index);

typedef int (*entry_data_callback_t)(header_translated_t *header, 
                                     int entry_index, 
                                     char *block, 
                                     int length);

typedef int (*entry_end_callback_t)(header_translated_t *header, 
                                    int entry_index);

struct entry_callbacks_s
{
    entry_header_callback_t header_cb;
    entry_data_callback_t data_cb;
    entry_end_callback_t end_cb;
};

typedef struct entry_callbacks_s entry_callbacks_t;

int read_tar(const char *file_path, entry_callbacks_t *callbacks);
void dump_header(header_translated_t *header);
unsigned long long decode_base256(const char *buffer);
char *trim(char *raw, int length);
int parse_header(const char buffer[512], header_t *header);
int translate_header(header_t *raw_header, header_translated_t *parsed);
enum entry_type_e get_type_from_char(char raw_type);

#endif

