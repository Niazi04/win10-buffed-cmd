#include <iostream>
#include <fstream>


#if __cplusplus >= 201703L
    #define cpp17FILESYSTEM 1
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <sys/stat.h>
#endif


uint64_t u64GetFileSize(const std::string &sFileName)
{
#if cpp17FILESYSTEM
    uint64_t size = fs::file_size(sFileName);

    if (size > 0)
    {
        return size;
    } 
    std::cout << "shit went wring in cpp17FILESYSTEM" << std::endl;
    return 0;
        
#elif defined(_WIN32)
    #include <windows.h>
    HANDLE hFileHandle = CreateFileA(
        sFileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    LARGE_INTEGER size;
    if  (!GetFileSizeEx(hFileHandle, &size)){
        CloseHandle(hFileHandle);
        return 0;
    }

    CloseHandle(hFileHandle);
    return size.QuadPart;
#elif defined(__linux__)
    struct stat stat_buf;
    if (stat(sFileName.c_str(), &stat_buf) != 0) return 0;
    return stat_buf.st_size;
#else
    /*
        Taken from ->  https://cplusplus.com/reference/istream/istream/tellg/
    */

    uint64_t length = 0;
    std::ifstream is (sFileName, std::ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg (0, is.end);
        length = is.tellg();
        is.seekg (0, is.beg);

        char * buffer = new char [length];

        is.read (buffer,length);

        is.close();
        delete[] buffer;
    }
    return length;
#endif
}

// TODO: make the file size human readable
// TODO: throw and error if file(s) don't exist
// TODO: check the file size if relative filepath is given
int main (int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Specify a file name!" << std::endl;
    return 0;
  } else if (argc == 2) {
    std::cout << argv[argc-1] << ": " << u64GetFileSize(argv[argc-1]) << std::endl;
    return 0;
  } else {
    for (size_t i = 1; i < argc; i++) {
        std::cout << argv[i] << ": " << u64GetFileSize(argv[i]) << std::endl;
    }
  }
  
  return 0;
}