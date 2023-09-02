#ifndef BMP_IMAGE_H_
#define BMP_IMAGE_H_

#include <iostream>
#include <fstream>
#include <vector>

#include "bmp_exceptions.h"

namespace Images 
{

class Bmp 
{
protected:	
    #pragma pack(1)
    struct BITMAPFILEHEADER 
    {
        uint16_t type;
        uint32_t size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offsetBits;
    };
    #pragma pack(0)

    struct BITMAPINFOHEADER 
    {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitCount;
        uint32_t compression;
        uint32_t sizeImage;
        int32_t XPelsPerMeter;
        int32_t YPelsPerMeter;
        uint32_t colorUsed;
        uint32_t colorImportant;
    };

    BITMAPFILEHEADER fHeader;
    BITMAPINFOHEADER iHeader;

    struct RGBTRIPLE 
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };
public:
    virtual ~Bmp() = default;
    int32_t width() const;
    int32_t height() const;
    virtual void write( const std::string& filename ) = 0;
private:
    virtual void make_bmp( int32_t height, int32_t width ) = 0;
    virtual void recover() = 0;
    virtual void load( const std::string& filename ) = 0;	
    virtual void load_pixels( const std::string& filename, const uint32_t offset ) = 0;
};


class Bmp8 final : public Bmp 
{
    std::vector<uint8_t> pixels;
    std::vector<RGBTRIPLE> pallete;
public:
    Bmp8( const int32_t height, const int32_t width, const std::vector<uint8_t>& src );
    explicit Bmp8( const std::string& filename );
    Bmp8( const Bmp8& rhs );
    Bmp8( Bmp8&& rhs ) noexcept;

    Bmp8& operator=( const Bmp8& rhs );
    Bmp8& operator=( Bmp8&& rhs );

    void write( const std::string& filename ) override;
    std::vector<uint8_t> get_pixels() const;
private:
    void make_bmp( int32_t height, int32_t width ) override;
    void recover() override;
    void load( const std::string& filename ) override;
    void load_pixels( const std::string& filename, const uint32_t offset = 0 ) override;
};

class Bmp24 final : public Bmp 
{
    std::vector<RGBTRIPLE> pixels;
public:
    Bmp24(  const int32_t height, const int32_t width );
    explicit Bmp24( const std::string& filename );
    Bmp24( const Bmp24& rhs );
    Bmp24( Bmp24&& rhs ) noexcept;

    Bmp24& operator=( const Bmp24& rhs );
    Bmp24& operator=( Bmp24&& rhs );

    void write( const std::string& filename ) override;
    std::vector<uint8_t> get_component( const char color );
    Bmp8 grayscale() const;
private:
    void make_bmp( int32_t height, int32_t width ) override;
    void recover() override;
    void load( const std::string& filename ) override;
    void load_pixels( const std::string& filename, const uint32_t offset = 0 ) override;
};

} // namespace Images
#endif // !BMP_IMAGE_H_
