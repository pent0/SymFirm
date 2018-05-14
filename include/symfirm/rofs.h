#pragma once

#include <cstdint>
#include <string>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

enum {
    rofs_format_ver = 0x200
};

struct rofs_header {
    uint32_t magic;	
	uint8_t header_size;
	uint8_t	reversed;
	uint16_t rofs_format_ver;
	uint32_t dir_tree_off;	// offset to start of directory structure
	uint32_t dir_tree_size;		// size in bytes of directory
	uint32_t dir_file_entries_off;	// offset to start of file entries
	uint32_t dir_file_entries_size;	// size in bytes of file entry block
	int64_t	time;
	version	iImageVersion;		// licensee image version
	uint32_t image_size; // rofs image size
	uint32_t checksum;
	uint32_t max_image_size;
};

// This maybe extended in Symbian^3, unfortunately there is no source
// code
struct rofs_header_ext: public rofs_header {};

// Non-portable so I have to defined
using u16str = std::basic_string<uint16_t>;

// As usual, each entry is aligned by 4, so the struct size may add some padding
struct rofs_entry {
    uint16_t entry_size; // Total size of entry, header + name + any padding
	uint8_t uids[4];		// A copy of all the UID info
	uint8_t name_off;	// offset of iName from start of entry
	uint8_t	attrib;			// standard file attributes
	uint32_t file_size;		// real size of file in bytes (may be different from size in image)
							// for subdirectories this is the total size of the directory
							// block entry excluding padding
	uint32_t file_addr;	// address in image of file start
	uint8_t attrib_ext;		// extra ROFS attributes (these are inverted so 0 = enabled)
	uint8_t name_len;	// length of name
	u16str name;
};

struct rofs_dir {
    uint16_t dir_size;
    uint8_t padding;
    uint8_t first_entry_off;
    uint32_t file_block_addr;
    uint32_t file_block_size;
    rofs_entry subdir;
};

struct rofs {

};