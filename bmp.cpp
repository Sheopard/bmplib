#include "bmp.h"

BMP24::BMP24(std::string filename) {
	this->load(filename);
}

void BMP24::write(std::string filename, std::vector<std::vector<RGBTRIPLE>>& dest) {
	FILE *fout;
	fopen_s(&fout, filename.c_str(), "wb");
	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
	fwrite(&iHeader, sizeof(iHeader), 1, fout);

	int padding = 4 - (iHeader.width * 3) % 4;
	for (uint32_t i = 0; i < iHeader.height; i++) {
		for (uint32_t j = 0; j < iHeader.width; j++) {
			fwrite(&dest[i][j], sizeof(RGBTRIPLE), 1, fout);
		}

		if (padding != 4) {
			fwrite(0, 1, padding, fout);
		}
	}

	fclose(fout);
}

void BMP24::load(std::string& filename) {
	FILE *fin;
	fopen_s(&fin, filename.c_str(), "rb");

	if (fin == NULL) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fread(&fHeader, sizeof(fHeader) - 2, 1, fin);
	fread(&iHeader, sizeof(iHeader), 1, fin);

	pixels.resize(iHeader.height);
	for (uint32_t i = 0; i < iHeader.height; i++) {
		pixels[i].resize(iHeader.width);
	}

	int padding = 4 - (iHeader.width * 3) % 4;
	for (uint32_t i = 0; i < iHeader.height; i++) {
		for (uint32_t j = 0; j < iHeader.width; j++) {
			fread(&pixels[i][j], sizeof(RGBTRIPLE), 1, fin);
		}

		if (padding != 4) {
			fseek(fin, padding, SEEK_CUR);
		}
	}

	fclose(fin);
}

void BMP24::to_bmp8(std::string& filename) {
	int dest_wb = ((iHeader.width * 8 + 31) / 32) * 4;
	int dest_size = dest_wb * iHeader.height;

	fHeader.size = 54 + 4 * 256 + dest_size;
	iHeader.bitCount = 8;
	iHeader.colorUsed = 256;
	iHeader.colorImportant = 0;
	iHeader.sizeImage = dest_size;

	std::vector<RGBTRIPLE> pallete(256);
	for (uint32_t i = 0; i < 256; i++) {
		pallete[i].red = pallete[i].blue = pallete[i].green = (uint8_t)i;
	}

	std::vector<uint8_t> dest(dest_size);

	uint8_t blue = 0, red = 0, green = 0;
	for (int i = iHeader.height - 1; i >= 0; i--) {
		for (int j = 0; j < iHeader.width; j++) {
			dest[i * dest_wb + j] = 0.299 * pixels[i][j].red + 0.587 * pixels[i][j].green + 0.114 * pixels[i][j].blue;
		}
	}

	FILE *fout;
	fopen_s(&fout, filename.c_str(), "wb");
	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
	fwrite(&iHeader, sizeof(iHeader), 1, fout);
	for (int i = 0; i < 256; i++) {
		fwrite(&pallete[i], sizeof(RGBTRIPLE) + 1, 1, fout);
	}
	for (int i = 0; i < dest_size; i++) {
		fwrite(&dest[i], 1, 1, fout);
	}

	fclose(fout);
}

std::vector<std::vector<int>> BMP24::get_component(char color) {
	std::vector<std::vector<int>> result(pixels.size(), std::vector<int>(pixels[0].size()));

	if (color == 'r') {
		for (uint32_t i = 0; i < iHeader.height; i++) {
			for (uint32_t j = 0; j < iHeader.width; j++) {
				result[i][j] = pixels[i][j].red;
			}
		}
	}
	else if (color == 'g') {
		for (uint32_t i = 0; i < iHeader.height; i++) {
			for (uint32_t j = 0; j < iHeader.width; j++) {
				result[i][j] = pixels[i][j].green;
			}
		}
	}
	else if (color == 'b') {
		for (uint32_t i = 0; i < iHeader.height; i++) {
			for (uint32_t j = 0; j < iHeader.width; j++) {
				result[i][j] = pixels[i][j].blue;
			}
		}
	}

	return result;
}

// ==================================================================

BMP8::BMP8(std::string filename) {
	this->load(filename);
}

void BMP8::load(std::string filename) {
	FILE *fin;
	fopen_s(&fin, filename.c_str(), "rb");

	if (fin == NULL) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fread(&fHeader, sizeof(fHeader) - 2, 1, fin);
	fread(&iHeader, sizeof(iHeader), 1, fin);

	pallete.resize(256);
	for (uint32_t i = 0; i < 256; i++) {
		fread(&pallete[i], sizeof(RGBTRIPLE), 1, fin);
		fseek(fin, 1, SEEK_CUR);
	}

	pixels.resize(iHeader.width * iHeader.height);
	for (int i = 0; i < pixels.size(); i++) {
		fread(&pixels[i], 1, 1, fin);
	}

	fclose(fin);
}

void BMP8::write(std::string filename, std::vector<uint8_t>& pix) {
	FILE *fout;
	fopen_s(&fout, filename.c_str(), "wb");
	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
	fwrite(&iHeader, sizeof(iHeader), 1, fout);

	for (int i = 0; i < 256; i++) {
		fwrite(&pallete[i], sizeof(RGBTRIPLE) + 1, 1, fout);
	}

	for (int i = 0; i < pix.size(); i++) {
		fwrite(&pix[i], 1, 1, fout);
	}

	fclose(fout);
}

void BMP8::write(std::string filename) {
	FILE *fout;
	fopen_s(&fout, filename.c_str(), "wb");
	fwrite(&fHeader, sizeof(fHeader) - 2, 1, fout);
	fwrite(&iHeader, sizeof(iHeader), 1, fout);

	for (int i = 0; i < 256; i++) {
		fwrite(&pallete[i], sizeof(RGBTRIPLE) + 1, 1, fout);
	}

	for (int i = 0; i < pixels.size(); i++) {
		fwrite(&pixels[i], 1, 1, fout);
	}

	fclose(fout);
}