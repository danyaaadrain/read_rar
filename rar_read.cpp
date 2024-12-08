#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <locale>
#include <windows.h>
using namespace std;
#pragma pack(push, 1)
struct rar_header
{
    uint16_t header_crc;
    uint8_t header_type;
    uint16_t header_flags;
    uint16_t header_size;
};
struct file_header 
{
    uint32_t PackSize;
    uint32_t UnpSize;
    uint8_t HostOS;
    uint32_t FileCRC;
    uint32_t FileTime;
    uint8_t UnpVer;
    uint8_t Method;
    uint16_t NameSize;
    uint32_t FileAttr;
};
#pragma pack(pop)
uint32_t currentOffset;
uint32_t filename_offset;
int file_size;
uint32_t filename_size;

void read_rar(vector <char>& data) {
    rar_header* p_header = reinterpret_cast <rar_header*> (&data[currentOffset]);
    cout << "Header type: 0x" << hex << (int)p_header->header_type << endl;
    cout << "Header size: 0x" << hex << (int)p_header->header_size << endl;
    cout << "Current offset: 0x" << hex << currentOffset << endl << endl << endl;
    currentOffset += p_header->header_size;
    while (true) {
        rar_header* p_header = reinterpret_cast <rar_header*> (&data[currentOffset]);
        if (p_header->header_type != 0x74) {
            cout << "All files in RAR are read!" << endl;
            break;
        }
        filename_offset = currentOffset + sizeof(rar_header) + sizeof(file_header);
        file_header* f_header = reinterpret_cast <file_header*>(&data[currentOffset + 0x7]);
        filename_size = f_header->NameSize;
        char* contentHEX = reinterpret_cast<char*>(&data[filename_offset]);
        string utf8line(contentHEX, filename_size);
        vector<wchar_t> utf16content(filename_size);
        MultiByteToWideChar(CP_UTF8, 0, utf8line.c_str(), filename_size, utf16content.data(), utf16content.size());
        wstring file_name(utf16content.begin(), utf16content.end());  
        cout << "Current offset: 0x" << hex << (int)currentOffset << endl;
        cout << "Current offset: 0x" << hex << (int)currentOffset << endl;
        cout << "File name size: 0x" << hex << (int)filename_size << endl;
        cout << "Packed data size: 0x" << hex << (int)f_header->PackSize << endl;
        wcout << L"File name: " << file_name << endl << endl;
        currentOffset += p_header->header_size + f_header->PackSize;
    }
}
int main() {
    setlocale(LC_ALL, "Russian");
    ifstream rar_file("Example.rar", ios::binary);
    if (!rar_file.is_open()){
        cout << "Error while opening" << endl;
        return 1;
    }
    rar_file.seekg(0, ios::end);
    file_size = rar_file.tellg();
    rar_file.seekg(0, ios::beg);
    cout << "RAR size: " << file_size << endl;
    vector <char> rar_data(file_size, 0);
    rar_file.read(rar_data.data(), file_size);
    currentOffset = 7;
    read_rar(rar_data);
    return 0;
}