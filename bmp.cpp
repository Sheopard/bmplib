#include "bmp.h"

int32_t BMP::width() const {
	return iHeader.width;
}

int32_t BMP::height() const {
	return iHeader.height;
}

BMP24::BMP24(int height, int width) {
	fHeader.type = 0x4d42;
	fHeader.size = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + width * height * 3;
	fHeader.offsetBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	iHeader.size = sizeof(BITMAPINFOHEADER);
	iHeader.bitCount = 24;
	iHeader.compression = 0;
	iHeader.width = width;
	iHeader.height = height;
	iHeader.planes = 1;	

	pixels.resize(height);
	for (uint32_t i = 0; i < height; ++i) {
		pixels[i].resize(width, {0, 0, 0});
	}
}

BMP24::BMP24(const std::string& filename) {
	this->load(filename);
}

void BMP24::write(const std::string& filename) {
	std::ofstream fout(filename, std::ios_base::binary);

	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));	

	const int width = this->width();
	const int height = this->height();
	
	int padding = 4 - (width * 3) % 4;
	uint8_t zero_byte = 0;
	for (uint32_t i = 0; i < height; ++i) {
		for (uint32_t j = 0; j < width; ++j) {
			fout.write(reinterpret_cast<char*>(&pixels[i][j]), sizeof(RGBTRIPLE));
		}

		if (padding != 4) {
			std::cout << "here\n";
			fout.write(reinterpret_cast<char*>(&zero_byte), padding);
		}
	}
	fout.close();
}

void BMP24::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios_base::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	if (fHeader.type != 0x4d42) {
		throw std::runtime_error("Error: Invalid BMP type!");
	}

	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));
	if (iHeader.size != 40) {
		throw std::runtime_error("Error: Invalid InfoHeader size!");
	}

	if (iHeader.bitCount != 24) {
		throw std::runtime_error("Error: Invalid InfoHeader bit count!");
	}

	const int width = this->width();
	const int height = this->height();

	pixels.resize(height);
	for (uint32_t i = 0; i < height; ++i) {
		pixels[i].resize(width);
	}

	int padding = 4 - (width * 3) % 4;
	for (uint32_t i = 0; i < height; ++i) {
		for (uint32_t j = 0; j < width; ++j) {
			fin.read(reinterpret_cast<char*>(&pixels[i][j]), sizeof(RGBTRIPLE));
		}

		if (padding != 4) {
			fin.seekg(padding, std::ios_base::cur);
		}
	}
	fin.close();
}

BMP8 BMP24::grayscale() {
	const int width = this->width();
	const int height = this->height();

  	std::vector<uint8_t> dest(height * width);
  
  	for (int i = 0; i < height; i++) {
  		for (int j = 0; j < width; j++) {
  			dest[i * width + j] = 0.299 * pixels[i][j].red + 0.587 * pixels[i][j].green + 0.114 * pixels[i][j].blue;
  		}
  	}

	BMP8 image(height, width, dest);

	return image;
}

//std::vector<std::vector<int>> BMP24::get_component(char color) {
//	std::vector<std::vector<int>> result(pixels.size(), std::vector<int>(pixels[0].size()));
//
//	if (color == 'r') {
//		for (uint32_t i = 0; i < iHeader.height; i++) {
//			for (uint32_t j = 0; j < iHeader.width; j++) {
//				result[i][j] = pixels[i][j].red;
//			}
//		}
//	}
//	else if (color == 'g') {
//		for (uint32_t i = 0; i < iHeader.height; i++) {
//			for (uint32_t j = 0; j < iHeader.width; j++) {
//				result[i][j] = pixels[i][j].green;
//			}
//		}
//	}
//	else if (color == 'b') {
//		for (uint32_t i = 0; i < iHeader.height; i++) {
//			for (uint32_t j = 0; j < iHeader.width; j++) {
//				result[i][j] = pixels[i][j].blue;
//			}
//		}
//	}
//
//	return result;
//}

BMP8::BMP8(const int height, const int width, const std::vector<uint8_t>& src) {
	fHeader.type = 0x4d42;
	fHeader.size = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + width * height + 4 * 256;
	fHeader.reserved1 = 0;
	fHeader.reserved2 = 0;
	fHeader.offsetBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	iHeader.size = sizeof(BITMAPINFOHEADER);
	iHeader.width = width;
	iHeader.height = height;
	iHeader.planes = 1;	
	iHeader.bitCount = 8;
	iHeader.compression = 0;
	iHeader.sizeImage = width * height;
	iHeader.XPelsPerMeter = 0;
	iHeader.YPelsPerMeter = 0;
	iHeader.colorUsed = 256;
	iHeader.colorImportant = 0;

	pallete.resize(256);
  	for (uint16_t i = 0; i < 256; i++) {
  		pallete[i].red = pallete[i].blue = pallete[i].green = static_cast<uint8_t>(i);
  	}

	std::copy(src.begin(), src.end(), std::back_inserter(pixels));
}

BMP8::BMP8(const std::string& filename) {
	this->load(filename);
}

void BMP8::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	if (fHeader.type != 0x4d42) {
		throw std::runtime_error("Error: Invalid BMP type!");
	}

	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));
	if (iHeader.size != 40) {
		throw std::runtime_error("Error: Invalid InfoHeader size!");
	}

	if (iHeader.bitCount != 8) {
		throw std::runtime_error("Error: Invalid InfoHeader bit count!");
	}

	const int image_size = this->width() * this->height();

	pallete.resize(256);
	for (uint16_t i = 0; i < 256; ++i) {
		fin.read(reinterpret_cast<char*>(&pallete[i]), sizeof(RGBTRIPLE));
		fin.seekg(1, std::ios_base::cur);
	}
	
	pixels.resize(image_size);
	const int pix_size = pixels.size();
	for (uint32_t i = 0; i < pix_size; ++i) {
		fin.read(reinterpret_cast<char*>(&pixels[i]), 1);
	}
	
	fin.close();
}

void BMP8::write(const std::string& filename) {
	std::ofstream fout(filename, std::ios_base::binary);
	
	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));

	for (uint16_t i = 0; i < 256; ++i) {
		fout.write(reinterpret_cast<char*>(&pallete[i]), sizeof(RGBTRIPLE) + 1);
	}

	const int pix_size = pixels.size();
	for (uint32_t i = 0; i < pix_size; ++i) {
		fout.write(reinterpret_cast<char*>(&pixels[i]), 1);
	}

	fout.close();
}
