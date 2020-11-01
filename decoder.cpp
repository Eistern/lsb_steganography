#include <iostream>
#include <fstream>
#include "lodepng/lodepng.h"

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

    size_t total_bytes = image_data.size();
    size_t processed_bytes = 0;
    unsigned char collect_result;
    while (processed_bytes < total_bytes / 4) {
        collect_result = 0;
        collect_result += image_data[processed_bytes * 4] & 0b00000011u;
        collect_result += (image_data[processed_bytes * 4 + 1] & 0b00000011u) << 2u;
        collect_result += (image_data[processed_bytes * 4 + 2] & 0b00000011u) << 4u;
        collect_result += (image_data[processed_bytes * 4 + 3] & 0b00000011u) << 6u;
        output.write(reinterpret_cast<const char *>(&collect_result), 1);
        processed_bytes++;
    }
    output.close();
    return 0;
}