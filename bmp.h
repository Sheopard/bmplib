#ifndef BMP_IMAGE_H_
#define BMP_IMAGE_H_

#include <iostream>
#include <vector>

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
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

class BMP8 {
private:
	BITMAPFILEHEADER fHeader;
	BITMAPINFOHEADER iHeader;
	std::vector<uint8_t> pixels;
	std::vector<RGBTRIPLE> pallete;
public:
	BMP8(std::string filename);
	void write(std::string filename, std::vector<unsigned char>& pix);
	void write(std::string filename);
	std::vector<uint8_t> get_pixels() {
		return pixels;
	}
private:
	void load(std::string filename);
};

class BMP24 {
private:
	BITMAPFILEHEADER fHeader;
	BITMAPINFOHEADER iHeader;
	std::vector<std::vector<RGBTRIPLE>> pixels;

public:
	BMP24(std::string filename);
	void write(std::string filename, std::vector<std::vector<RGBTRIPLE>>& dest);
	int width() { return iHeader.width; };
	int height() { return iHeader.height; };
	RGBTRIPLE get_pixel(int i, int j) { return pixels[j][i]; };
	std::vector<std::vector<RGBTRIPLE>> get_pixels() { return pixels; };
	std::vector<std::vector<int>> get_component(char color);
	void to_bmp8(std::string& filename);

private:
	void load(std::string& filename);
};

#endif // !BMP_IMAGE_H_
