#ifndef _FAT_H
#define _FAT_H

#include "loader.h"


#pragma pack(push, 1)

struct S_FAT1216_BPB {
	bit8u  jmp[3];
	char   oem_name[8];
	bit16u bytes_per_sect;
	bit8u  sect_per_clust;
	bit16u sect_reserved;
	bit8u  fats;
	bit16u root_entrys;
	bit16u sectors;
	bit8u  descriptor;
	bit16u sect_per_fat;
	bit16u sect_per_trk;
	bit16u heads;
	bit32u hidden_sects;
	bit32u sect_extnd;
	bit8u  drive_num;  // not FAT specific
	bit8u  resv;
	bit8u  sig;
	bit32u serial;
	char   label[11];
	char   sys_type[8];
};

struct S_FAT32_BPB {
	bit8u  jmp[3];
	char   oem_name[8];
	bit16u bytes_per_sect;
	bit8u  sect_per_clust;
	bit16u sect_reserved;
	bit8u  fats;
	bit16u root_entrys;
	bit16u sectors;
	bit8u  descriptor;
	bit16u sect_per_fat;
	bit16u sect_per_trk;
	bit16u heads;
	bit32u hidden_sects;
	bit32u sect_extnd;
	bit32u sect_per_fat32;
	bit16u ext_flags;      // bit 8 = write to all copies of FAT(s).  bit0:3 = which fat is active
	bit16u fs_version;
	bit32u root_base_cluster;
	bit16u fs_info_sec;
	bit16u backup_boot_sec;
	bit8u  reserved[12];
	bit8u  drive_num;       // not FAT specific
	bit8u  resv;
	bit8u  sig;
	bit32u serial;
	char   label[11];
	char   sys_type[8];
};

struct S_FAT_ROOT {
  bit8u  name[8];    // name
  bit8u  ext[3];     // ext
  bit8u  attrb;      // attribute
  union U_TYPE {
    bit8u  resv[10];   // reserved in fat12/16
    struct S_FAT32 {
      bit8u  nt_resv;    // reserved for WinNT
      bit8u  crt_time_tenth; // millisecond stamp at creation time
      bit16u crt_time;   // time file was created
      bit16u crt_date;   // date file was created
      bit16u crt_last;   // date file was last accessed
      bit16u strtclsthi; // hi word of FAT32 starting cluster
    } fat32;
  } type;
  bit16u time;       // time
  bit16u date;       // date
  bit16u strtclst;   // starting cluster number
  bit32u filesize;   // file size in bytes
};

#pragma pack(pop)

struct S_FAT_DATA {
  void *root_dir;
  void *fat_loc;
  int  root_entries;
  int  sec_per_fat;
  int  sect_per_clust;
  int  sec_resv;
  int  num_fats;
};


bool fat_load_data(struct S_FAT_DATA *);

void convert_fat83(const struct S_FAT_ROOT *, char *);
bit32u fat_get_next_cluster(void *, bit32u);

#endif   // _FAT_H