#ifndef BMP_IMAGE_H_
#define BMP_IMAGE_H_

#include <iostream>
#include <fstream>
#include <vector>

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
	virtual ~BMP() {};
	int32_t width() const;
	int32_t height() const;
	virtual void write(const std::string& filename) = 0;
private:
	virtual void load(const std::string& filename) = 0;	
};


class BMP8 : public BMP {
	std::vector<uint8_t> pixels;
	std::vector<RGBTRIPLE> pallete;
public:
	BMP8(const int height, const int width, const std::vector<uint8_t>& src);
	BMP8(const std::string& filename);
	void write(const std::string& filename) override final;
private:
	void load(const std::string& filename) override final;
};

class BMP24 : public BMP {
  	std::vector<std::vector<RGBTRIPLE>> pixels;
public:
	BMP24(int height, int width);
  	BMP24(const std::string& filename);
	void write(const std::string& filename) override final;
  	//std::vector<std::vector<int>> get_component(char color);
  	BMP8 grayscale();
  
private:
	void load(const std::string& filename) override final;
};

#endif // !BMP_IMAGE_H_
