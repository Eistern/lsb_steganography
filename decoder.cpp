#include <iostream>
#include <fstream>
#include "lodepng/lodepng.h"

#define LAST_BYTE_MASK 0x03u

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_to_decode_into.png>" << std::endl;
        return 1;
    }

    std::vector<unsigned char> image_data;
    unsigned width, height;
    unsigned return_code = lodepng::decode(image_data, width, height, argv[1]);
    if (return_code != 0) {
        std::cerr << "Could not load PNG file" << std::endl;
        return 1;
    }

    std::ofstream output("result.txt", std::ios_base::out & std::ios_base::binary);

    size_t total_bytes = image_data.size() / 4;

    uint64_t saved_bytes = 0;
    for (int bit_tuples = 0; bit_tuples < 32; ++bit_tuples) {
        saved_bytes += (image_data[bit_tuples] & LAST_BYTE_MASK) << (bit_tuples * 2u);
    }

    size_t processed_bytes = 8;
    unsigned char collect_result;
    while (processed_bytes - 8 < saved_bytes && processed_bytes < total_bytes) {
        collect_result = 0;
        collect_result += image_data[processed_bytes * 4] & LAST_BYTE_MASK;
        collect_result += (image_data[processed_bytes * 4 + 1] & LAST_BYTE_MASK) << 2u;
        collect_result += (image_data[processed_bytes * 4 + 2] & LAST_BYTE_MASK) << 4u;
        collect_result += (image_data[processed_bytes * 4 + 3] & LAST_BYTE_MASK) << 6u;
        output.write(reinterpret_cast<const char *>(&collect_result), 1);
        processed_bytes++;
    }
    output.close();
    return 0;
}