#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#pragma comment(lib, "user32.lib")

std::wstring UTF8ToUTF16(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(
        CP_UTF8, 
        0, 
        utf8.c_str(), 
        (int)utf8.size(), 
        nullptr, 0
    );
    
    std::wstring utf16(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8, 
        0, 
        utf8.c_str(), 
        (int)utf8.size(), 
        &utf16[0], 
        size_needed
    );
    
    return utf16;
}

std::string UTF16ToUTF8(const std::wstring& utf16) {
    // this is not used in this file. I just wrote it for the fuck of it
    if (utf16.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(
        CP_UTF8, 
        0, 
        utf16.c_str(), 
        (int)utf16.size(), 
        nullptr,
        0, 
        nullptr,
        nullptr
    );
    
    std::string utf8(size_needed, 0);

    WideCharToMultiByte(
        CP_UTF8, 
        0, 
        utf16.c_str(), 
        (int)utf16.size(), 
        &utf8[0], 
        size_needed, 
        nullptr,
        nullptr
    );
    
    return utf8;
}

std::string readFileAsUTF8(const std::wstring& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::wcerr << L"Failed to open file: " << filePath << std::endl;
        return "";
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::wcerr << L"Failed to read file: " << filePath << std::endl;
        return "";
    }
    
    return std::string(buffer.begin(), buffer.end());
}

bool copyUTF8ToClipboard(const std::string& utf8_text) {
    if (utf8_text.empty()) return false;
    
    std::wstring utf16_text = UTF8ToUTF16(utf8_text);
    
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard" << std::endl;
        return false;
    }
    
    EmptyClipboard();
    
    // Note: +1 for nullptr terminator
    HGLOBAL hGlobal = GlobalAlloc(
        GMEM_MOVEABLE, 
        (utf16_text.size() + 1) * sizeof(wchar_t)
    );
    
    if (!hGlobal) {
        CloseClipboard();
        return false;
    }
    
    wchar_t* pGlobal = static_cast<wchar_t*>(GlobalLock(hGlobal));
    if (!pGlobal) {
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }
    
    memcpy(pGlobal, utf16_text.c_str(), utf16_text.size() * sizeof(wchar_t));
    pGlobal[utf16_text.size()] = L'\0';  // Add nullptr terminator
    
    GlobalUnlock(hGlobal);
    
    SetClipboardData(CF_UNICODETEXT, hGlobal);
    
    CloseClipboard();
    return true;
}

std::wstring getWidePath(const char* narrow_path) {
    int size_needed = MultiByteToWideChar(
        CP_UTF8, 
        0, 
        narrow_path, 
        -1,  // -1 means nullptr-terminated string
        nullptr,
        0
    );
    
    if (size_needed == 0) { return std::wstring(); }
    
    std::wstring wide_path(size_needed, 0);

    MultiByteToWideChar(
        CP_UTF8, 
        0, 
        narrow_path, 
        -1, 
        &wide_path[0], 
        size_needed
    );
    
    // Remove the nullptr terminator from the string
    wide_path.pop_back();
    
    return wide_path;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: grab <file_path>" << std::endl;
        std::cout << "Example: grab \"C:\\path\\to\\file.txt\"" << std::endl;
        return 1;
    }
    
    std::wstring filePath = getWidePath(argv[1]);
    
    std::string fileContent = readFileAsUTF8(filePath);
    
    if (fileContent.empty()) {
        std::wcout << L"File is empty or could not be read: " << filePath << std::endl;
        return 1;
    }
    
    if (!copyUTF8ToClipboard(fileContent)) {
        std::wcout << L"Failed to copy file to clipboard." << std::endl;
        return 1;
    }
    
    return 0;
}