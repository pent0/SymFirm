#include <symfirm/fpsx.h>
#include "utils.h"

fpsx_block_type_2e read_block_2e(FILE* file) {
    fpsx_block_type_2e blck_2e;

    fread(&blck_2e, 1, sizeof(fpsx_block_type_2e), file);

    blck_2e.size = utils::mbswap(blck_2e.size);
    blck_2e.off = utils::mbswap(blck_2e.off);

    return blck_2e;
}

fpsx_block_type_30 read_block_30(FILE* file) {
    fpsx_block_type_30 blck_30;

    fread(&blck_30, 1, sizeof(fpsx_block_type_30), file);

    blck_30.size = utils::mbswap(blck_30.size);
    blck_30.off = utils::mbswap(blck_30.off);

    return blck_30;
}

fpsx_block_type_17 read_block_17(FILE* file) {
    fpsx_block_type_17 blck_17;

    fread(&blck_17, 1, sizeof(fpsx_block_type_17), file);

    blck_17.size = utils::mbswap(blck_17.size);
    blck_17.off = utils::mbswap(blck_17.off);    

    return blck_17;
}

fpsx_block_type_27_rofs_hash read_block_27(FILE* file) {
    fpsx_block_type_27_rofs_hash blck_27;

    fread(&blck_27, 1, sizeof(fpsx_block_type_27_rofs_hash), file);

    blck_27.size = utils::mbswap(blck_27.size);
    blck_27.off = utils::mbswap(blck_27.off);

    return blck_27;
}

fpsx_block_type_28_core_cert read_block_28(FILE* file) {
    fpsx_block_type_28_core_cert blck_28;

    fread(&blck_28, 1, sizeof(fpsx_block_type_28_core_cert), file);

    blck_28.size = utils::mbswap(blck_28.size);
    blck_28.off = utils::mbswap(blck_28.off);

    return blck_28;
}

void patch_block_header(FILE* file, fpsx_block& blck) {
    auto blck_type = blck.blck_type;
    auto cnt_type = blck.cnt_type;

    blck.block_header.block_readed = -1;

    if (cnt_type == content_type::code ||
        cnt_type == content_type::data) {
        switch (blck_type) {
            case block_type::block_type_2e: {
                blck.block_header.t2e = read_block_2e(file);
                blck.block_header.block_readed = 0x2E;
                blck.cnt_size = blck.block_header.t2e.size;
            
                break;
            }

            case block_type::block_type_30: {
                blck.block_header.t30 = read_block_30(file);
                blck.block_header.block_readed = 0x30;
                blck.cnt_size = blck.block_header.t30.size;
            
                break;
            }

            default: {
                blck.block_header.t17 = read_block_17(file);
                blck.block_header.block_readed = 0x17;
                blck.cnt_size = blck.block_header.t17.size;
            
                break;
            }
        }
    }

    // If the content type doesn't fall in these cases
    switch (blck_type) {
        case block_type::block_type_27_rofs_hash: {
            blck.block_header.t27 = read_block_27(file);
            blck.block_header.block_readed = 0x27;
            blck.cnt_size = blck.block_header.t27.size;
            
            break;
        }

        case block_type::block_type_28_core_cert: {
            blck.block_header.t28 = read_block_28(file);
            blck.block_header.block_readed = 0x28;
            blck.cnt_size = blck.block_header.t28.size;
            
            break;
        }

        default: {
            break;
        }
    }
}

fpsx_block read_block(FILE* file) {
    fpsx_block blck;

    fread(&blck.blck_type, 1, 1, file);
    fread(&blck.unk1, 1, 1, file);
    fread(&blck.cnt_type, 1, 1, file);
    fread(&blck.block_size, 1, 1, file);

    auto crr_pos = ftell(file);

    patch_block_header(file, blck);

    if (ftell(file) - crr_pos != blck.block_size) {
        fseek(file, crr_pos + blck.block_size, SEEK_SET);
    }

    fread(&blck.checksum, 1, 1, file);

    // Again a check
    blck.cnt_offset = ftell(file);

    return blck;
}

fpsx_header read_header(FILE* file) {
    fpsx_header res;

    fread(&res.signature, 1, 1, file);
    fread(&res.header_size, 1, 4,  file);
    fread(&res.total_block, 1, 4, file);

    res.unk.resize(res.header_size - 4);

    fread(res.unk.data(), 1, res.unk.size(), file);

    // Swap bytes order
    res.header_size = utils::mbswap(res.header_size);
    res.total_block = utils::mbswap(res.total_block);

    return res;
}

std::optional<fpsx> parse_fpsx(const std::string& path) {
    fpsx fpsx_file;
    fpsx_file.handler = fopen(path.c_str(), "rb");

    if (fpsx_file.handler == nullptr) {
        return std::make_optional<fpsx>();
    }

    fpsx_file.header = read_header(fpsx_file.handler);
    fpsx_file.blcks.resize(fpsx_file.header.total_block);

    for (auto& blck: fpsx_file.blcks) {
        blck = read_block(fpsx_file.handler);
    }

    return fpsx_file;
}

bool write_block(FILE* target, FILE* src, fpsx_block blck) {
    if (target == nullptr || src == nullptr) {
        return false;
    }
    
    size_t cnt_size = blck.cnt_size;
    size_t readed = 0;
    size_t wrote = 0;

    const size_t chunk_size = 10000;

    std::vector<char> temp;

    fseek(src, blck.cnt_offset, SEEK_SET);

    while (readed < cnt_size) {
        auto will_read = std::min(chunk_size, cnt_size - readed);
        temp.resize(will_read);

        readed += fread(temp.data(), 1, will_read, src);
        wrote += fwrite(temp.data(), 1, will_read, target);
    }

    if (readed != wrote) {
        return false;
    }

    return true;
}

bool extract_fpsx(fpsx& file, const std::string& res) {
    content_type old_type = file.blcks[0].cnt_type;
    bool first_code = false;

    for (auto& blck: file.blcks) {
        if (blck.cnt_type == content_type::data) {
            FILE* temp;

            if (old_type == content_type::data) {
                first_code = true;
                temp = fopen(res.c_str(), "wb");
            } else {
                temp = fopen(res.c_str(), "ab");
            }

            if (temp == nullptr) {
                return false;
            }

            if (!write_block(temp, file.handler, blck)) {
                fclose(temp);
                return false;
            };

            // POSIX won't clean things for me
            fclose(temp);
        }

        old_type = blck.cnt_type;
    }

    return true;
}