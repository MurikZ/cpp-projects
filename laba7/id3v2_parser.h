#ifndef ID3V2_PARSER_H
#define ID3V2_PARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <fstream>
#include <iostream>

// Базовый класс для всех фреймов
class Frame {
public:
    virtual ~Frame() = default;
    virtual void Print() const = 0;
    static std::unique_ptr<Frame> CreateFrame(const std::string& frame_id, const std::vector<uint8_t>& data);
protected:
    std::string id_;
};

// Текстовый фрейм (TXXX, TIT2, TPE1 и т.д.)
class TextFrame : public Frame {
public:
    TextFrame(const std::string& frame_id, const std::vector<uint8_t>& data);
    void Print() const override;

private:
    std::string encoding_;
    std::string value_;
};

// URL-фрейм (WXXX, WCOM и т.д.)
class UrlFrame : public Frame {
public:
    UrlFrame(const std::string& frame_id, const std::vector<uint8_t>& data);
    void Print() const override;

private:
    std::string url_;
    std::string description_; // Для WXXX
};

// Комментарий (COMM)
class CommentFrame : public Frame {
public:
    CommentFrame(const std::string& frame_id, const std::vector<uint8_t>& data);
    void Print() const override;

private:
    std::string encoding_;
    std::string language_;
    std::string description_;
    std::string text_;
};

// Несинхронизированные тексты (USLT)
class UnsynchronizedLyricsFrame : public Frame {
public:
    UnsynchronizedLyricsFrame(const std::string& frame_id, const std::vector<uint8_t>& data);
    void Print() const override;

private:
    std::string encoding_;
    std::string language_;
    std::string description_;
    std::string lyrics_;
};

// Класс для парсинга тега ID3v2
class Id3v2Parser {
public:
    explicit Id3v2Parser(const std::string& filename);
    bool Parse();
    void PrintTags() const;
    static std::string ReadString(const std::vector<uint8_t>& data, size_t& offset, uint8_t encoding, bool terminated);

private:
    const uint32_t kHeaderSize = 10;
    const uint32_t kFrameHeaderSize = 10;
    const std::string kTagIdentifier = "ID3";
    std::ifstream file_;
    std::vector<std::unique_ptr<Frame>> frames_;
    uint32_t tag_size_;

    bool ReadHeader();
    uint32_t ReadSynchsafeInt(std::ifstream& file);
};

#endif // ID3V2_PARSER_H