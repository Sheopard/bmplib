#include "bmp.h"

int32_t BMP::width() const {
	return iHeader.width;
}

int32_t BMP::height() const {
	return iHeader.height;
}

BMP24::BMP24(int width, int height) {
	fHeader.type = 0x4d42;

	iHeader.size = sizeof(BITMAPINFOHEADER);
	iHeader.bitCount = 24;
	iHeader.compression = 0;
	iHeader.width = width;
	iHeader.height = height;
	iHeader.planes = 1;	

	pixels.resize(height);
	for (uint32_t i = 0; i < height; ++i) {
		pixels[i].resize(width, {255, 0, 0});
	}
}

BMP24::BMP24(const std::string& filename) {
	this->load(filename);

	std::cout << "==============================\n";
	std::cout << std::hex << fHeader.type << std::dec << "\n";
	fHeader.reserved2 = 0;
	fHeader.size = 0;

	std::cout << fHeader.size << "\n";
	std::cout << fHeader.reserved1 << "\n";
	std::cout << fHeader.reserved2 << "\n";
	std::cout << fHeader.offsetBits << "\n";

	std::cout << "==============================\n";
	std::cout << iHeader.size << "\n";
	std::cout << iHeader.width << "\n";
	std::cout << iHeader.height << "\n";
	std::cout << iHeader.planes << "\n";
	std::cout << iHeader.bitCount << "\n";
	std::cout << iHeader.compression << "\n";
	std::cout << iHeader.sizeImage << "\n";
	std::cout << iHeader.XPelsPerMeter << "\n";
	std::cout << iHeader.YPelsPerMeter << "\n";
	std::cout << iHeader.colorUsed << "\n";
	std::cout << iHeader.colorImportant << "\n";
}

void BMP24::write(const std::string& filename) {
	std::ofstream fout(filename, std::ios_base::binary);

	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader) - 2);
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));	

	const int width = this->width();
	const int height = this->height();
	
	int padding = 4 - (width * 3) % 4;
	for (uint32_t i = 0; i < height; ++i) {
		for (uint32_t j = 0; j < width; ++j) {
			fout.write(reinterpret_cast<char*>(&pixels[i][j]), sizeof(RGBTRIPLE));
		}

		if (padding != 4) {
			fout.write(0, 1);
		}
	}
	fout.close();
}

void BMP24::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios_base::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}


	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader) - 2);
	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));

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

void BMP24::to_bmp8(const std::string& filename) {
	const int width = this->width();
	const int height = this->height();

  	int dest_wb = ((width * 8 + 31) / 32) * 4;
  	int dest_size = dest_wb * height;
  
  	fHeader.size = 54 + 4 * 256 + dest_size;
  	iHeader.bitCount = 8;
  	iHeader.colorUsed = 256;
  	iHeader.colorImportant = 0;
  	iHeader.sizeImage = dest_size;
  
  	std::vector<RGBTRIPLE> pallete(256);
  	for (uint16_t i = 0; i < 256; i++) {
  		pallete[i].red = pallete[i].blue = pallete[i].green = (uint8_t)i;
  	}
  
  	std::vector<uint8_t> dest(dest_size);
  
  	uint8_t blue = 0, red = 0, green = 0;
  	for (int i = height - 1; i >= 0; i--) {
  		for (int j = 0; j < width; j++) {
  			dest[i * dest_wb + j] = 0.299 * pixels[i][j].red + 0.587 * pixels[i][j].green + 0.114 * pixels[i][j].blue;
  		}
  	}
  
	std::ofstream fout(filename, std::ios_base::binary);

	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader) - 2);
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));

  	for (uint16_t i = 0; i < 256; i++) {
		fout.write(reinterpret_cast<char*>(&pallete[i]), sizeof(RGBTRIPLE) + 1);
  	}

  	for (int i = 0; i < dest_size; i++) {
		fout.write(reinterpret_cast<char*>(&dest[i]), 1);
  	}
  
	fout.close();
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

// ==================================================================

BMP8::BMP8(const std::string& filename) {
	this->load(filename);
}

void BMP8::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader) - 2);
	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));

	const int image_size = this->width() * this->height();

	pallete.resize(256);
	for (uint16_t i = 0; i < 256; ++i) {
		fin.read(reinterpret_cast<char*>(&pallete[i]), sizeof(RGBTRIPLE));
		fin.seekg(1, std::ios_base::cur);
	}
	
	pixels.resize(image_size);
	const int pix_size = pixels.size();
	for (uint16_t i = 0; i < pix_size; ++i) {
		fin.read(reinterpret_cast<char*>(&pixels[i]), 1);
	}
	
	fin.close();

//	FILE *fin;
//	fopen_s(&fin, filename.c_str(), "rb");
//
//	if (fin == NULL) {
//		throw std::ios_base::failure("Error: Image not open");
//	}
//
//	fread(&fHeader, sizeof(fHeader) - 2, 1, fin);
//	fread(&iHeader, sizeof(iHeader), 1, fin);
//
//	pallete.resize(256);
//	for (uint32_t i = 0; i < 256; i++) {
//		fread(&pallete[i], sizeof(RGBTRIPLE), 1, fin);
//		fseek(fin, 1, SEEK_CUR);
//	}
//
//	pixels.resize(iHeader.width * iHeader.height);
//	for (int i = 0; i < pixels.size(); i++) {
//		fread(&pixels[i], 1, 1, fin);
//	}
//
//	fclose(fin);
}

void BMP8::write(const std::string& filename, std::vector<uint8_t>& pix) {
//	FILE *fout;
//	fopen_s(&fout, filename.c_str(), "wb");
//	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
//	fwrite(&iHeader, sizeof(iHeader), 1, fout);
//
//	for (int i = 0; i < 256; i++) {
//		fwrite(&pallete[i], sizeof(RGBTRIPLE) + 1, 1, fout);
//	}
//
//	for (int i = 0; i < pix.size(); i++) {
//		fwrite(&pix[i], 1, 1, fout);
//	}
//
//	fclose(fout);
}

void BMP8::write(const std::string& filename) {
	std::ofstream fout(filename, std::ios_base::binary);
	
	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader) - 2);
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));

	for (uint16_t i = 0; i < 256; ++i) {
		fout.write(reinterpret_cast<char*>(&pallete[i]), sizeof(RGBTRIPLE));
	}

	const int pix_size = pixels.size();
	for (uint16_t i = 0; i < pix_size; ++i) {
		fout.write(reinterpret_cast<char*>(&pixels[i]), 1);
	}

	fout.close();


//	FILE *fout;
//	fopen_s(&fout, filename.c_str(), "wb");
//	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
//	fwrite(&iHeader, sizeof(iHeader), 1, fout);
//
//	for (int i = 0; i < 256; i++) {
//		fwrite(&pallete[i], sizeof(RGBTRIPLE) + 1, 1, fout);
//	}
//
//	for (int i = 0; i < pixels.size(); i++) {
//		fwrite(&pixels[i], 1, 1, fout);
//	}
//
//	fclose(fout);
}
