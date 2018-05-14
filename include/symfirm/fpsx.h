#pragma once

#include <cstdint>
#include <cstdio>
#include <optional>
#include <vector>
#include <string>
#include <memory>

// Based on the work of reversing Phoenix, including: https://github.com/fonix232
// Reference: https://github.com/flaithbheartaigh/nokias60cenrepeditor/blob/master/S60/S60.Lib.Firmware/S60.Lib.Firmware.cs

// Some of the code are translated from Hungary to English

enum class content_type: uint8_t {
    code = 0x54,
    data = 0x5D
};

enum class block_type: uint8_t {
    block_type_17 = 0x17,
    block_type_27_rofs_hash = 0x27,
    block_type_28_core_cert = 0x28,
    block_type_2e = 0x2e,
    block_type_30 = 0x30
};

enum class demysterious {
    val0,
    val1, 
    val2,
    val3,
    val4
};

enum class mem_type: uint8_t {
    cmt,
    ape
};

struct fpsx_header {
    uint8_t signature;
    uint32_t header_size;
    uint32_t total_block;
    std::vector<uint8_t> unk;
};

struct fpsx_block_header {
    uint32_t cnt_size;
    uint16_t checksum;
    mem_type flash_mem_type;
    uint32_t loc;
    uint32_t len;
};

struct fpsx_code_block {
    uint8_t processor_type;
    uint16_t unk1;
    uint16_t crcq;
    uint8_t cnst1;
    uint32_t cnt_size;
    uint32_t loc;
};

struct fpsx_signed_block {
    uint8_t unk1[17];
    uint32_t cnt_size;
    uint32_t location;
};

struct fpsx_data_block {
    uint8_t hash[16];
    uint8_t hash_crc[4];
    uint8_t des[12];
    uint8_t processor_type;
    uint8_t unk1[4];
    uint32_t cnt_size;
    uint32_t loc;
};

struct fpsx_signed_data_block: public fpsx_data_block {
    uint8_t certq[16];
};

struct fpsx_block_type_17 {
    mem_type flash_mem_type;
    uint16_t unk1;
    uint16_t checksum;
    uint8_t cnst1;
    uint32_t size;
    uint32_t off;
};

struct fpsx_block_type_2e {
    mem_type flash_mem_type;
    uint16_t unk1;
    uint16_t checksum;
    uint8_t des[12];
    uint16_t size;
    uint16_t off;
};

struct fpsx_block_type_27_rofs_hash {
    uint8_t md5q[16];
    uint32_t unk1;
    uint8_t des[12];
    mem_type flash_mem_type;
    uint16_t unk2;
    uint16_t size;
    uint16_t checksum;
    uint16_t off;
};

struct fpsx_block_type_28_core_cert: public fpsx_block_type_27_rofs_hash {
    uint8_t sha1[20];
    uint16_t unk3;
};

struct fpsx_block_type_30 {
    mem_type flash_mem_type;
    uint16_t unk1;
    uint16_t checksum;
    uint16_t size;
    uint16_t off;
};

struct fpsx_block {
    block_type blck_type;
    uint8_t unk1;
    content_type cnt_type;
    uint8_t block_size;

    struct {
        fpsx_block_type_17 t17;
        fpsx_block_type_27_rofs_hash t27;
        fpsx_block_type_28_core_cert t28;
        fpsx_block_type_2e t2e;
        fpsx_block_type_30 t30;
        short block_readed;
    } block_header;

    uint8_t blck_header_size;
    uint8_t checksum;

    uint64_t cnt_offset;
    uint32_t cnt_size;
};

struct fpsx {
    FILE* handler;
    fpsx_header header;
    std::vector<fpsx_block> blcks;
};

std::optional<fpsx> parse_fpsx(const std::string& path);
bool extract_fpsx(fpsx& file, const std::string& dir);