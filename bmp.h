#ifndef BMP_IMAGE_H_
#define BMP_IMAGE_H_

#include <iostream>
#include <fstream>
#include <vector>

struct Image {
	struct BITMAPINFOHEADER {
		int size;
		int width;
		int height;
		short int planes;
		short int bitCount;
		int compression;
		int sizeImage;
		int XPelsPerMeter;
		int YPelsPerMeter;
		int colorUsed;
		int colorImportant;
	};

	struct BITMAPFILEHEADER {
		short int type;
		int size;
		short int reserved1;
		short int reserved2;
		int offsetBits;
	};

	struct RGBTRIPLE {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
	};
	
	BITMAPFILEHEADER fHeader;
	BITMAPINFOHEADER iHeader;

	virtual ~Image() {};
public:
	virtual void write(const std::string& filename) = 0;
private:
	virtual void load(const std::string& filename) = 0;	
};


class BMP8 : protected Image {
private:
	std::vector<uint8_t> pixels;
	std::vector<RGBTRIPLE> pallete;
public:
	BMP8(const std::string& filename);
	void write(const std::string& filename, std::vector<unsigned char>& pix);
	void write(const std::string& filename) override final;
private:
	void load(const std::string& filename) override final;
};

class BMP24 : protected Image {
private:
  	std::vector<std::vector<RGBTRIPLE>> pixels;
  
public:
  	BMP24(const std::string& filename);
	void write(const std::string& filename) override final;
  	void write(const std::string& filename, std::vector<std::vector<RGBTRIPLE>>& dest);
  	//int width() { return iHeader.width; };
  	//int height() { return iHeader.height; };
  	//RGBTRIPLE get_pixel(int i, int j) { return pixels[j][i]; };
  	//std::vector<std::vector<RGBTRIPLE>> get_pixels() { return pixels; };
  	//std::vector<std::vector<int>> get_component(char color);
  	//void to_bmp8(std::string& filename);
  
private:
	void load(const std::string& filename) override final;
};

#endif // !BMP_IMAGE_H_
