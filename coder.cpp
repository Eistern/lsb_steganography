#include <iostream>
#include <fstream>
#include "lodepng/lodepng.h"

#define LAST_BYTE_MASK 0x03u

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <image_to_code_into.png> <crypto_text>" << std::endl;
        return 1;
    }

    std::vector<unsigned char> image_data;
    unsigned width, height;
    unsigned return_code = lodepng::decode(image_data, width, height, argv[1]);
    if (return_code != 0) {
        std::cerr << "Could not load PNG file" << std::endl;
        return 1;
    }

    std::ifstream text(argv[2], std::ios_base::binary);
    if (!text.is_open()) {
        std::cerr << "Could not open text file" << std::endl;
        return 1;
    }
    size_t total_bytes = image_data.size() / 4;
    uint64_t written_bytes = 8;
    unsigned char input_buffer;
    while (!text.eof() && written_bytes < total_bytes) {
        text.read(reinterpret_cast<char *>(&input_buffer), 1);
        if (text.eof()) {
            break;
        }
        image_data[written_bytes * 4] &= !LAST_BYTE_MASK;
        image_data[written_bytes * 4] |= (LAST_BYTE_MASK & (input_buffer >> 0u));
        image_data[written_bytes * 4 + 1] &= !LAST_BYTE_MASK;
        image_data[written_bytes * 4 + 1] |= (LAST_BYTE_MASK & (input_buffer >> 2u));
        image_data[written_bytes * 4 + 2] &= !LAST_BYTE_MASK;
        image_data[written_bytes * 4 + 2] |= (LAST_BYTE_MASK & (input_buffer >> 4u));
        image_data[written_bytes * 4 + 3] &= !LAST_BYTE_MASK;
        image_data[written_bytes * 4 + 3] |= (LAST_BYTE_MASK & (input_buffer >> 6u));
        written_bytes++;
    }
    written_bytes -= 8;
    for (int bit_tuples = 0; bit_tuples < 32; ++bit_tuples) {
        image_data[bit_tuples] &= !LAST_BYTE_MASK;
        image_data[bit_tuples] |= (LAST_BYTE_MASK & (written_bytes >> (2u * bit_tuples)));
    }
    text.close();
    if (written_bytes >= total_bytes) {
        std::cerr << "Written only " << written_bytes << ", buffer limit extended. Saving" << std::endl;
    }

    return_code = lodepng::encode("result.png", image_data, width, height);
    if (return_code != 0) {
        std::cerr << "Error saving image";
        return 1;
    }

    return 0;
}
