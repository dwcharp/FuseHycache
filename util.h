#ifndef _UTIL_H_
#define _UTIL_H_

int ht_insert(const char *key, const char *val);
int ht_remove(const char *key);
ENTRY* ht_search(const char *key);

int zht_init();
int zht_free();
int zht_insert(const char *key, const char *value);
int zht_lookup(const char *key, char *val);
int zht_remove(const char *key);

int net_getmyip(char *ip);

/**
 * forward declaration
 */

// SSD utilities 
int ssd_is_full();
int ssd_is_full_2();
void get_ssd_path(char ssd_fpath[PATH_MAX], const char *hdd_fpath);
void get_hdd_path(char hdd_fpath[PATH_MAX], const char *ssd_fpath);
int move_file_ssd(const char *ssd_fpath);
int move_file_hdd(const char *hdd_fpath);
void copy_dir_ssd(const char *ssd_fpath, mode_t mode);
int get_used_ssd();
int sum_size_ssd(const char *name, const struct stat *sb, int type);
int is_symlink_ssd(const char *fpath_ssd);

// LRU utilities
void insque_lru(inode_t *elem);
void remque_lru();
void rmelem_lru(const char* fname);
inode_t* findelem_lru(const char *fname);

// LFU utilities
void insque_lfu(inode_t *elem);
void remque_lfu();
void rmelem_lfu(const char *fname);
inode_t* get_pos_lfu(inode_t *elem);
inode_t* findelem_lfu(const char *fname);

//SCC utilities
inode_t* findelem_scc(const char *fname);
void ins_Q(inode_t *elem);
void rem_Q(inode_t *victim);
void insque_scc(const char *fname);
void remque_scc(char *fname);
void rmelem_scc(const char *fname);

//ARC utilities
inode_t **arc_heads;
inode_t **arc_tails;
arc_p   *arc;
/* Helper function to append an elem to a list */

inode_t* findelem_list(int, const char*);
void remque_head_arc(int);
inode_t* remove_node(int list_index);
void rmelem_list(int list_index, const char *fname);
void rmelem_arc(const char *fname);
void remque_arc(char *fname);
void addtolist(int list_index, inode_t *elem);
int get_size(int);
void replace(const char *fname);
int arc_cache_has_files();


// Debug utilities
void print_lru();
void print_lfu();
void print_debug();
void print_used_ssd();
int  fusion_error(char* str);

#endif
