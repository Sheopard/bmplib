#ifndef BMP_EXCEPTIONS_H_
#define BMP_EXCEPTIONS_H_

#include <string>
#include <stdexcept>

class FileHeaderTypeException final: public std::runtime_error 
{
public:
    FileHeaderTypeException( const std::string& what_arg ) : std::runtime_error( what_arg.c_str() ) {}
};

class InfoHeaderSizeException final: public std::runtime_error 
{
public:
	  InfoHeaderSizeException( const std::string& what_arg ) : std::runtime_error( what_arg.c_str() ) {}
};

class InfoHeaderBitException final: public std::runtime_error 
{
public:
    InfoHeaderBitException( const std::string& what_arg ) : std::runtime_error( what_arg.c_str() ) {}
};

#endif // !BMP_EXCEPTIONS_H_
