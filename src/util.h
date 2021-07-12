#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable: 4996)
#endif

int readFileToString(const char* filePath, std::string& writeback);

constexpr uint32_t CRC32_POLY = 0xEDB88320;
constexpr uint32_t CRC32_CHECK = 0x2144DF1C;

void gen_crc32_table();
uint32_t crc32b(void* data, uint32_t size);
uint32_t crc32(void* data, uint32_t size);

#endif