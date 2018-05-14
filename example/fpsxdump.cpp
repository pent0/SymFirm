#include <symfirm/fpsx.h>

#include <cstring>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "No path specified. Try again." << std::endl;
        return 0;
    }

    std::string output;

    for (uint32_t i = 1; i < argc - 1; i++) {
        if (strncmp(argv[i], "-o", 2) == 0) {
            i++;
            output = argv[i];
        }
    }

    std::string path = argv[argc-1];
    auto fpsx_res = parse_fpsx(path);

    if (!fpsx_res) {
        std::cout << "Error with parsing FPSX file. Maybe file doesn't exist." << std::endl;
        return 0;
    }

    fpsx fpsx_file = fpsx_res.value();
    bool res_write = extract_fpsx(fpsx_file, output);

    if (!res_write) {
        std::cout << "Error with extracting FPSX. Either output path is invalid or there are issues with file." << std::endl;
        return 0;
    }

    return 0;
}