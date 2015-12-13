#ifndef OSDP_LIB
#define OSDP_LIB

#include <stdlib.h>
#include <inttypes.h>
#include <openssl/md5.h>

#define WINDOW_SIZE 256
#define RABIN_MOD (4096)
#define FIS_BASE_NAME "/info/"
#define FDATA_BASE_NAME "/data/"

/*
 * This function is used to get sithe of the next chunk
 * Errors are encrupted in negative values.
 * Maximal and minimal size of the chunk are fixed.
 * Use this function to get ephasize identical chunks from similar files.
*/

#define MIN_CHUNK_SIZE (4*1024)
#define MAX_CHUNK_SIZE (40*1024)
#define MAX_SIZE_FOR_REPRESENTATIVE_CHUNK_FINDING (1000*10*1024)

#if MIN_CHUNK_SIZE < WINDOW_SIZE
#error "WINDOW_SIZE should be less then MIN_CHUNK_SIZE"
#endif

#if  RABIN_MOD > MAX_CHUNK_SIZE
#error "RABIN_MOD plus MIN_CHUNK_SIZE is an average block size"
#endif

ssize_t get_offset_to_next_chunk(char *buf, ssize_t length);

int get_representative_hash(char *result, char *file_beginning, ssize_t length);

//int get_chunk_len( char * buff, ssize_t max_len );
int save_chunk_data(char *path_for_data_to_save, char *data, ssize_t length);

int get_md5_of_chunk(unsigned char *result, char *data, ssize_t size);
//char* get_hash_of_chunk( char* chunk, ssize_t chunk_size);


typedef struct {
    char md5_hash[MD5_DIGEST_LENGTH*2 + 1];
    ssize_t offset_in_file;
    ssize_t length_of_chunk;
} chunk_info;

typedef struct {
    int_fast32_t ref_counter;
    ssize_t length_of_chunk;
} saved_chunk_header;

int get_hash_string_representation(char *string_representation, unsigned char *md5_hash);
int store_file(const char *name, const char *base_dir);
int restore_file(const char *name, const char* new_name , const char *base_dir);

#endif