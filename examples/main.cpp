#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "bmp.h"

using namespace Images;

int main(int argc, char* argv[]) {

	/* BMP24 example */
	std::unique_ptr<BMP24> image = std::make_unique<BMP24>("in.bmp");	
	image->write("out.bmp");
	
	/* BMP24 to BMP8 example */
	std::unique_ptr<BMP8> image8 = std::make_unique<BMP8>(image->grayscale());
	image8->write("out8.bmp");

	return 0;
}

