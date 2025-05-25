#include "MetaDecoder.h"
#include <algorithm>
#include <fstream>
#include <iostream>

uint32_t calculateFrameSize(const std::array<uint8_t, 4>& bytes) {
    return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | bytes[3];
}

MetadataBlock::MetadataBlock(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct)
    : identifier(id), blockSize(sz), settings(fl), content(ct) {}

void MetadataBlock::display() const {
    std::cout << identifier << " [" << blockSize << " bytes]";
}

std::string getBlockDescription(const std::string& id) {
    static const std::unordered_map<std::string, std::string> descriptors = {
        {"TIT2", "Track name"}, {"TPE1", "Artist"}, {"TALB", "Collection"},
        {"TRCK", "Track number"}, {"TYER", "Year"}, {"TDRC", "Date"},
        {"TCON", "Category"}, {"COMM", "Note"}, {"TENC", "Encoder"},
        {"TCOM", "Writer"}, {"TBPM", "Tempo"}, {"TSSE", "Settings"},
        {"TPOS", "Disk number"}
    };
    auto found = descriptors.find(id);
    return found != descriptors.end() ? found->second : "";
}

TextMetadata::TextMetadata(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct)
    : MetadataBlock(id, sz, fl, ct) {}

void TextMetadata::display() const {
    std::string textContent;
    if (!content.empty()) {
        textContent.assign(content.begin() + 1, content.end());
    }
    std::cout << "[*] " << identifier;
    auto desc = getBlockDescription(identifier);
    if (!desc.empty()) std::cout << " (" << desc << ")";
    std::cout << ": " << textContent;
}

WebLink::WebLink(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct)
    : MetadataBlock(id, sz, fl, ct) {}

void WebLink::display() const {
    std::string link(content.begin(), content.end());
    std::cout << "[*] " << identifier << " (URL): " << link;
}

UserComment::UserComment(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct)
    : MetadataBlock(id, sz, fl, ct) {}

void UserComment::display() const {
    if (content.size() < 4) {
        std::cout << "[?] Invalid comment block";
        return;
    }
    std::string lang(content.begin() + 1, content.begin() + 4);
    auto textStart = std::find(content.begin() + 4, content.end(), 0);
    std::string note(textStart + 1, content.end());
    std::cout << "[*] " << identifier << " [" << lang << "]: " << note;
}

std::unique_ptr<MetadataBlock> createMetadataBlock(const std::string& id, uint32_t size, uint16_t flags, const std::vector<uint8_t>& data) {
    if (id.empty()) return std::make_unique<GenericBlock>(id, size, flags, data);
    switch (id[0]) {
        case 'T': return std::make_unique<TextMetadata>(id, size, flags, data);
        case 'W': return std::make_unique<WebLink>(id, size, flags, data);
        case 'C': return id == "COMM" ? std::make_unique<UserComment>(id, size, flags, data) : nullptr;
        default: return std::make_unique<GenericBlock>(id, size, flags, data);
    }
}

void parseMetadataBlocks(std::ifstream& file, uint32_t totalSize) {
    uint32_t processed = 0;
    while (processed + 10 <= totalSize) {
        std::string blockId(4, '\0');
        file.read(&blockId[0], 4);
        if (file.gcount() != 4 || blockId == "\0\0\0\0") break;

        std::array<uint8_t, 4> sizeData{};
        file.read(reinterpret_cast<char*>(sizeData.data()), 4);
        auto blockSize = calculateFrameSize(sizeData);

        uint16_t blockFlags = 0;
        file.read(reinterpret_cast<char*>(&blockFlags), 2);

        if (blockSize == 0) break;

        std::vector<uint8_t> blockContent(blockSize);
        file.read(reinterpret_cast<char*>(blockContent.data()), blockSize);
        if (file.gcount() != static_cast<int>(blockSize)) break;

        processed += 10 + blockSize;
        auto block = createMetadataBlock(blockId, blockSize, blockFlags, blockContent);
        if (block) {
            block->display();
            std::cout << '\n';
        }
    }
}