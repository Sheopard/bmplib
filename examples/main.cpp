#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "bmp.h"

using namespace Images;

int main( int argc, char* argv[] ) 
{
    try 
    {
        /* Bmp24 example */
        std::unique_ptr<Bmp24> image = std::make_unique<Bmp24>( "in.bmp" );	
        image->write( "out.bmp" );

        /* Bmp24 to Bmp8 example */
        std::unique_ptr<Bmp8> image8 = std::make_unique<Bmp8>( image->grayscale() );
        image8->write( "out8.bmp" );
    } 
    catch ( const std::exception& e ) 
    {
        std::cout << e.what() << "\n";
    }

    return 0;
}
