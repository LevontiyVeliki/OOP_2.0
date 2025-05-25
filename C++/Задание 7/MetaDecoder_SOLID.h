#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <array>

uint32_t calculateFrameSize(const std::array<uint8_t, 4>& data);

struct MetadataBlock {
    std::string identifier;
    uint32_t blockSize;
    uint16_t settings;
    std::vector<uint8_t> content;
    
    MetadataBlock(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct);
    virtual void display() const;
    virtual ~MetadataBlock() = default;
};

struct TextMetadata : MetadataBlock {
    TextMetadata(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct);
    void display() const override;
};

struct WebLink : MetadataBlock {
    WebLink(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct);
    void display() const override;
};

struct UserComment : MetadataBlock {
    UserComment(const std::string& id, uint32_t sz, uint16_t fl, const std::vector<uint8_t>& ct);
    void display() const override;
};

struct GenericBlock : MetadataBlock {
    using MetadataBlock::MetadataBlock;
};

std::string getBlockDescription(const std::string& id);
std::unique_ptr<MetadataBlock> createMetadataBlock(const std::string& id, uint32_t size, uint16_t flags, const std::vector<uint8_t>& data);
void parseMetadataBlocks(std::ifstream& file, uint32_t totalSize);