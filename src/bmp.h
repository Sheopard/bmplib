#ifndef BMP_IMAGE_H_
#define BMP_IMAGE_H_

#include <iostream>
#include <fstream>
#include <vector>

#include "bmp_exceptions.h"

namespace Images {

class BMP {
protected:	
	#pragma pack(1)
	struct BITMAPFILEHEADER {
		uint16_t type;
		uint32_t size;
		uint16_t reserved1;
		uint16_t reserved2;
		uint32_t offsetBits;
	};
	#pragma pack(0)

	struct BITMAPINFOHEADER {
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

	struct RGBTRIPLE {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
	};
public:
	virtual ~BMP() = default;
	int32_t width() const;
	int32_t height() const;
	virtual void write(const std::string& filename) = 0;
private:
    virtual void make_bmp(int32_t height, int32_t width) = 0;
	virtual void recover() = 0;
	virtual void load(const std::string& filename) = 0;	
	virtual void load_pixels(const std::string& filename, const uint32_t offset) = 0;
};


class BMP8 final : public BMP {
	std::vector<uint8_t> pixels;
	std::vector<RGBTRIPLE> pallete;
public:
	BMP8(const int32_t height, const int32_t width, const std::vector<uint8_t>& src);
	explicit BMP8(const std::string& filename);
    BMP8(const BMP8& rhs);
    BMP8(BMP8&& rhs) noexcept;

    BMP8& operator=(const BMP8& rhs);
    BMP8& operator=(BMP8&& rhs);

	void write(const std::string& filename) override;
	std::vector<uint8_t> get_pixels() const;
private:
    void make_bmp(int32_t height, int32_t width) override;
	void recover() override;
	void load(const std::string& filename) override;
	void load_pixels(const std::string& filename, const uint32_t offset = 0) override;
};

class BMP24 final : public BMP {
	std::vector<RGBTRIPLE> pixels;
public:
	BMP24(const int32_t height, const int32_t width);
  	explicit BMP24(const std::string& filename);
    BMP24(const BMP24& rhs);
    BMP24(BMP24&& rhs) noexcept;

    BMP24& operator=(const BMP24& rhs);
    BMP24& operator=(BMP24&& rhs);

	void write(const std::string& filename) override;
  	std::vector<uint8_t> get_component(const char color);
  	BMP8 grayscale() const;
private:
    void make_bmp(int32_t height, int32_t width) override;
	void recover() override;
	void load(const std::string& filename) override;
	void load_pixels(const std::string& filename, const uint32_t offset = 0) override;
};

} // namespace Images

#endif // !BMP_IMAGE_H_
