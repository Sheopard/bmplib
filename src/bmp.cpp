#include "bmp.h"

int32_t Images::BMP::width() const {
	return iHeader.width;
}

int32_t Images::BMP::height() const {
	return iHeader.height;
}

Images::BMP8::BMP8(const int32_t height, const int32_t width, const std::vector<uint8_t>& src) {
    this->make_bmp(height, width);

	pallete.resize(256);
  	for (uint16_t i = 0; i < 256; i++) {
  		pallete[i].red = pallete[i].blue = pallete[i].green = static_cast<uint8_t>(i);
  	}

	std::copy(src.begin(), src.end(), std::back_inserter(pixels));
}

Images::BMP8::BMP8(const std::string& filename) {
	try {
		this->load(filename);
		this->load_pixels(filename);
	} catch (FileHeaderTypeException& e) {
        throw;
	} catch (InfoHeaderSizeException& e) {
		int old_offset = fHeader.offsetBits >= 54 ? fHeader.offsetBits - 54 : 0;
		this->recover();
		this->load_pixels(filename, old_offset);
	} catch (InfoHeaderBitException& e) {
        throw;
	} catch (std::ios_base::failure& e) {
        throw;
	}
}

Images::BMP8::BMP8(const BMP8& rhs)  {
   fHeader = rhs.fHeader;
   iHeader = rhs.iHeader;
   std::copy(rhs.pallete.begin(), rhs.pallete.end(), std::back_inserter(pallete));
   std::copy(rhs.pixels.begin(), rhs.pixels.end(), std::back_inserter(pixels)); 
}

Images::BMP8::BMP8(BMP8&& rhs) noexcept {
    fHeader = std::move(rhs.fHeader);
    iHeader = std::move(rhs.iHeader);
    pallete = std::move(rhs.pallete);
    pixels = std::move(rhs.pixels);
}

Images::BMP8& Images::BMP8::operator=(const BMP8& rhs) {
    if (this == &rhs) {
        return *this;
    }

    BMP8 result(rhs);
    std::swap(result.fHeader, this->fHeader);
    std::swap(result.iHeader, this->iHeader);
    std::swap(result.pallete, this->pallete);
    std::swap(result.pixels, this->pixels);

    return *this;
}

Images::BMP8& Images::BMP8::operator=(BMP8&& rhs) {
    BMP8 result(std::move(rhs));
    
    std::swap(result.fHeader, this->fHeader);
    std::swap(result.iHeader, this->iHeader);
    std::swap(result.pallete, this->pallete);
    std::swap(result.pixels, this->pixels);

    return *this;
}

void Images::BMP8::write(const std::string& filename) {
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

std::vector<uint8_t> Images::BMP8::get_pixels() const {
	return pixels;
}

void Images::BMP8::make_bmp(int32_t height, int32_t width) {
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
}

void Images::BMP8::recover() {
	const int width = this->width();
	const int height = this->height();
    this->make_bmp(height, width);
}

void Images::BMP8::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	if (fHeader.type != 0x4d42) {
		throw FileHeaderTypeException("Error: Invalid BMP type!");
	}

	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));
	if (iHeader.size != 40) {
		throw InfoHeaderSizeException("Error: Invalid InfoHeader size!");
	}

	if (iHeader.bitCount != 8) {
		throw InfoHeaderBitException("Error: Invalid InfoHeader bit count!");
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

void Images::BMP8::load_pixels(const std::string& filename, const uint32_t offset) {
	std::ifstream fin(filename, std::ios_base::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.seekg(fHeader.offsetBits, std::ios_base::cur);
	fin.seekg(offset, std::ios_base::cur);

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

Images::BMP24::BMP24(const int32_t height, const int32_t width) {
    this->make_bmp(height, width);
	pixels.resize(height * width);
}

Images::BMP24::BMP24(const std::string& filename) {
	try {
		this->load(filename);
		this->load_pixels(filename);
	} catch (FileHeaderTypeException& e) {
        throw;
	} catch (InfoHeaderSizeException& e) {
		int old_offset = fHeader.offsetBits >= 54 ? fHeader.offsetBits - 54 : 0;
		this->recover();
		this->load_pixels(filename, old_offset);
	} catch (InfoHeaderBitException& e) {
        throw;
	} catch (std::ios_base::failure& e) {
        throw;
	}
}

Images::BMP24::BMP24(const BMP24& rhs) {
    fHeader = rhs.fHeader;
    iHeader = rhs.iHeader;
    std::copy(rhs.pixels.begin(), rhs.pixels.end(), std::back_inserter(pixels));
}

Images::BMP24::BMP24(BMP24&& rhs) noexcept {
    fHeader = std::move(rhs.fHeader);
    iHeader = std::move(rhs.iHeader);
    pixels = std::move(rhs.pixels);
}

Images::BMP24& Images::BMP24::operator=(const BMP24& rhs) {
    if (this == &rhs) {
        return *this;
    }

    BMP24 result(rhs);
    std::swap(result.fHeader, this->fHeader);
    std::swap(result.iHeader, this->iHeader);
    std::swap(result.pixels, this->pixels);

    return *this;
}

Images::BMP24& Images::BMP24::operator=(BMP24&& rhs) {
    BMP24 result(std::move(rhs));
    
    std::swap(result.fHeader, this->fHeader);
    std::swap(result.iHeader, this->iHeader);
    std::swap(result.pixels, this->pixels);

    return *this;
}

void Images::BMP24::write(const std::string& filename) {
	std::ofstream fout(filename, std::ios_base::binary);

	fout.write(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	fout.write(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));	

	const int width = this->width();
	const int height = this->height();
	
	const int padding = 4 - (width * 3) % 4;
	const uint8_t zero_byte = 0;
	for (uint32_t i = 0; i < height; ++i) {
		for (uint32_t j = 0; j < width; ++j) {
			fout.write(reinterpret_cast<char*>(&pixels[i * width + j]), sizeof(RGBTRIPLE));
		}

		if (padding != 4) {
			fout.write(reinterpret_cast<const char*>(&zero_byte), padding);
		}
	}
	fout.close();
}

std::vector<uint8_t> Images::BMP24::get_component(const char color) {
	const int width = this->width();
	const int height = this->height();
	std::vector<uint8_t> result(width * height);

	if (color == 'r') {
		for (uint32_t i = 0; i < height; i++) {
			for (uint32_t j = 0; j < width; j++) {
				result[i * width + j] = pixels[i * width + j].red;
			}
		}
	}
	else if (color == 'g') {
		for (uint32_t i = 0; i < height; i++) {
			for (uint32_t j = 0; j < width; j++) {
				result[i * width + j] = pixels[i * width + j].green;
			}
		}
	}
	else if (color == 'b') {
		for (uint32_t i = 0; i < height; i++) {
			for (uint32_t j = 0; j < width; j++) {
				result[i * width + j] = pixels[i * width + j].blue;
			}
		}
	}

	return result;
}

Images::BMP8 Images::BMP24::grayscale() const {
	const int width = this->width();
	const int height = this->height();

  	std::vector<uint8_t> dest(height * width);
  
  	for (int i = 0; i < height; i++) {
  		for (int j = 0; j < width; j++) {
  			dest[i * width + j] = 0.299 * pixels[i * width + j].red + 0.587 * pixels[i * width + j].green + 0.114 * pixels[i * width + j].blue;
  		}
  	}

	BMP8 image(height, width, dest);

	return image;
}

void Images::BMP24::make_bmp(int32_t height, int32_t width) {
	fHeader.type = 0x4d42;
	fHeader.size = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + width * height * 3;
	fHeader.reserved1 = 0;
	fHeader.reserved2 = 0;
	fHeader.offsetBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	iHeader.size = sizeof(BITMAPINFOHEADER);
	iHeader.width = width;
	iHeader.height = height;
	iHeader.planes = 1;	
	iHeader.bitCount = 24;
	iHeader.compression = 0;
	iHeader.sizeImage = width * height;
	iHeader.XPelsPerMeter = 0;
	iHeader.YPelsPerMeter = 0;
	iHeader.colorUsed = 0;
	iHeader.colorImportant = 0;
}

void Images::BMP24::recover() {
	const int width = this->width();
	const int height = this->height();
    this->make_bmp(height, width);
}

void Images::BMP24::load(const std::string& filename) {
	std::ifstream fin(filename, std::ios_base::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.read(reinterpret_cast<char*>(&fHeader), sizeof(fHeader));
	if (fHeader.type != 0x4d42) {
		throw FileHeaderTypeException("Error: Invalid BMP type!");
	}

	fin.read(reinterpret_cast<char*>(&iHeader), sizeof(iHeader));
	if (iHeader.size != 40) {
		throw InfoHeaderSizeException("Error: Invalid InfoHeader size!");
	}

	if (iHeader.bitCount != 24) {
		throw InfoHeaderBitException("Error: Invalid InfoHeader bit count!");
	}

	fin.close();
}

void Images::BMP24::load_pixels(const std::string& filename, const uint32_t offset) {
	std::ifstream fin(filename, std::ios_base::binary);
	if (!fin.is_open()) {
		throw std::ios_base::failure("Error: Image not open");
	}

	fin.seekg(fHeader.offsetBits, std::ios_base::cur);
	fin.seekg(offset, std::ios_base::cur);

	const int width = this->width();
	const int height = this->height();

	pixels.resize(height * width);
	const int padding = 4 - (width * 3) % 4;
	for (uint32_t i = 0; i < height; ++i) {
		for (uint32_t j = 0; j < width; ++j) {
			fin.read(reinterpret_cast<char*>(&pixels[i * width + j]), sizeof(RGBTRIPLE));
		}

		if (padding != 4) {
			fin.seekg(padding, std::ios_base::cur);
		}
	}
	fin.close();
}
