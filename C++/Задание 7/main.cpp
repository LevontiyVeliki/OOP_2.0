#include <fstream>
#include <bitset>
#include <iostream>
#include <vector>
#include "lib//MetaDecoder.h"

constexpr int HEADER_LENGTH = 10;

void displayError(const std::string& msg) {
    std::cerr << "[!] " << msg << '\n';
}

bool validateHeader(const std::string& buf) {
    return buf.size() == HEADER_LENGTH && buf.substr(0, 3) == "ID3";
}

struct TagInfo {
    uint8_t version;
    uint8_t revision;
    uint8_t options;
    uint32_t length;
};

TagInfo extractTagData(const std::string& header) {
    return {
        static_cast<uint8_t>(header[3]),
        static_cast<uint8_t>(header[4]),
        static_cast<uint8_t>(header[5]),
        calculateFrameSize({
            static_cast<uint8_t>(header[6]),
            static_cast<uint8_t>(header[7]),
            static_cast<uint8_t>(header[8]),
            static_cast<uint8_t>(header[9])
        })
    };
}

int processAudioFile(const char* path) {
    std::ifstream audioFile(path, std::ios::binary);
    if (!audioFile) {
        displayError("File access denied");
        return 2;
    }

    std::string headerData(HEADER_LENGTH, '\0');
    audioFile.read(&headerData[0], HEADER_LENGTH);

    if (!validateHeader(headerData)) {
        displayError("Invalid audio metadata");
        return 3;
    }

    auto tag = extractTagData(headerData);
    std::cout << "[*] Metadata v" << (int)tag.version << "." << (int)tag.revision
              << ", size: " << tag.length << " bytes\n";

    parseMetadataBlocks(audioFile, tag.length);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        displayError("Specify audio file path");
        return 1;
    }
    return processAudioFile(argv[1]);
}