#include "id3v2_parser.h"

#include <algorithm>
#include <cstring>

std::unique_ptr<Frame> Frame::CreateFrame(const std::string& frame_id, const std::vector<uint8_t>& data) {
    if (frame_id[0] == 'T' && frame_id != "TXXX") {
        return std::make_unique<TextFrame>(frame_id, data);
    }
    else if (frame_id == "TXXX") {
        return std::make_unique<TextFrame>(frame_id, data);
    }
    else if (frame_id[0] == 'W' && frame_id != "WXXX") {
        return std::make_unique<UrlFrame>(frame_id, data);
    }
    else if (frame_id == "WXXX") {
        return std::make_unique<UrlFrame>(frame_id, data);
    }
    else if (frame_id == "COMM") {
        return std::make_unique<CommentFrame>(frame_id, data);
    }
    else if (frame_id == "USLT") {
        return std::make_unique<UnsynchronizedLyricsFrame>(frame_id, data);
    }
    return nullptr; // Неизвестный фрейм
}

TextFrame::TextFrame(const std::string& frame_id, const std::vector<uint8_t>& data) {
    id_ = frame_id;
    size_t offset = 0;
    if (!data.empty()) {
        uint8_t encoding = data[offset++];
        encoding_ = (encoding == 0x00) ? "ISO-8859-1" : (encoding == 0x03) ? "UTF-8" : "Unknown";
        value_ = Id3v2Parser::ReadString(data, offset, encoding, true);
    }
}

void TextFrame::Print() const {
    std::cout << id_ << ": " << value_ << std::endl;
}

UrlFrame::UrlFrame(const std::string& frame_id, const std::vector<uint8_t>& data) {
    id_ = frame_id;
    size_t offset = 0;
    if (frame_id == "WXXX" && !data.empty()) {
        uint8_t encoding = data[offset++];
        description_ = Id3v2Parser::ReadString(data, offset, encoding, true);
        url_ = Id3v2Parser::ReadString(data, offset, 0x00, false); // URL всегда ISO-8859-1
    }
    else if (!data.empty()) {
        url_ = Id3v2Parser::ReadString(data, offset, 0x00, false);
    }
}

void UrlFrame::Print() const {
    std::cout << id_ << ": " << url_;
    if (!description_.empty()) {
        std::cout << " (" << description_ << ")";
    }
    std::cout << std::endl;
}

CommentFrame::CommentFrame(const std::string& frame_id, const std::vector<uint8_t>& data) {
    id_ = frame_id;
    size_t offset = 0;
    if (data.size() >= 4) {
        uint8_t encoding = data[offset++];
        encoding_ = (encoding == 0x00) ? "ISO-8859-1" : (encoding == 0x03) ? "UTF-8" : "Unknown";
        language_ = std::string(data.begin() + offset, data.begin() + offset + 3);
        offset += 3;
        description_ = Id3v2Parser::ReadString(data, offset, encoding, true);
        text_ = Id3v2Parser::ReadString(data, offset, encoding, true);
    }
}

void CommentFrame::Print() const {
    std::cout << id_ << " (" << language_ << ", " << description_ << "): " << text_ << std::endl;
}

UnsynchronizedLyricsFrame::UnsynchronizedLyricsFrame(const std::string& frame_id, const std::vector<uint8_t>& data) {
    id_ = frame_id;
    size_t offset = 0;
    if (data.size() >= 4) {
        uint8_t encoding = data[offset++];
        encoding_ = (encoding == 0x00) ? "ISO-8859-1" : (encoding == 0x03) ? "UTF-8" : "Unknown";
        language_ = std::string(data.begin() + offset, data.begin() + offset + 3);
        offset += 3;
        description_ = Id3v2Parser::ReadString(data, offset, encoding, true);
        lyrics_ = Id3v2Parser::ReadString(data, offset, encoding, true);
    }
}

void UnsynchronizedLyricsFrame::Print() const {
    std::cout << id_ << " (" << language_ << ", " << description_ << "): " << lyrics_ << std::endl;
}

Id3v2Parser::Id3v2Parser(const std::string& filename) : file_(filename, std::ios::binary), tag_size_(0) {
}

bool Id3v2Parser::Parse() {
    if (!file_.is_open()) {
        std::cerr << "Cannot open file" << std::endl;
        return false;
    }

    if (!ReadHeader()) {
        return false;
    }

    std::vector<uint8_t> buffer(tag_size_);
    file_.read(reinterpret_cast<char*>(buffer.data()), tag_size_);
    size_t offset = 0;

    while (offset < tag_size_ && offset + kFrameHeaderSize <= buffer.size()) {
        std::string frame_id(buffer.begin() + offset, buffer.begin() + offset + 4);
        offset += 4;

        // Читаем размер фрейма из буфера
        if (offset + 4 > buffer.size()) break;
        uint32_t frame_size = 0;
        for (int i = 0; i < 4; ++i) {
            frame_size = (frame_size << 7) | (buffer[offset + i] & 0x7F);
        }
        offset += 4;

        // Пропускаем флаги
        offset += 2;

        if (frame_size == 0 || offset + frame_size > buffer.size()) break;

        std::vector<uint8_t> frame_data(buffer.begin() + offset, buffer.begin() + offset + frame_size);
        auto frame = Frame::CreateFrame(frame_id, frame_data);
        if (frame) {
            frames_.push_back(std::move(frame));
        }
        offset += frame_size;
    }

    return true;
}

void Id3v2Parser::PrintTags() const {
    for (const auto& frame : frames_) {
        frame->Print();
    }
}

bool Id3v2Parser::ReadHeader() {
    std::vector<char> header(kHeaderSize);
    file_.read(header.data(), kHeaderSize);
    if (file_.gcount() != kHeaderSize || std::strncmp(header.data(), kTagIdentifier.c_str(), 3) != 0) {
        std::cerr << "Invalid ID3v2 header" << std::endl;
        return false;
    }

    if (static_cast<uint8_t>(header[3]) != 0x04 || static_cast<uint8_t>(header[4]) != 0x00) {
        std::cerr << "Unsupported ID3v2 version" << std::endl;
        return false;
    }

    file_.seekg(6);
    tag_size_ = ReadSynchsafeInt(file_);
    return true;
}

uint32_t Id3v2Parser::ReadSynchsafeInt(std::ifstream& file) {
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t byte;
        file.read(reinterpret_cast<char*>(&byte), 1);
        result = (result << 7) | (byte & 0x7F);
    }
    return result;
}

std::string Id3v2Parser::ReadString(const std::vector<uint8_t>& data, size_t& offset, uint8_t encoding, bool terminated) {
    std::string result;
    size_t end = offset;
    while (end < data.size()) {
        if (terminated && data[end] == 0x00 && (encoding != 0x01 || (end + 1 < data.size() && data[end + 1] == 0x00))) {
            break;
        }
        end++;
    }
    result = std::string(data.begin() + offset, data.begin() + end);
    offset = end + (terminated ? (encoding == 0x01 ? 2 : 1) : 0);
    return result;
}