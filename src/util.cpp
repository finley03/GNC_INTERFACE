#include "util.h"

//
//int readFileToString(const char* filePath, std::string& writeback) {
//	std::ifstream f(filePath);
//	// check if file opened and therefore exists
//	if (!f.is_open()) {
//		std::cout << "Error: could not find file: " << filePath << std::endl;
//		return false;
//	}
//	// create buffer
//	std::stringstream buffer;
//	buffer << f.rdbuf();
//	f.close();
//	// covert to string and send to writeback string
//	writeback = buffer.str();
//	// return success is this point has been reached
//	return true;
//}


static uint32_t crc32_table[256];


void gen_crc32_table() {
	uint32_t c;

	for (uint32_t i = 0; i < 256; ++i) {
		c = i;
		for (uint32_t k = 0; k < 8; ++k) {
			if ((c & 1) != 0) {
				c = CRC32_POLY ^ (c >> 1);
			}
			else {
				c = c >> 1;
			}
		}
		crc32_table[i] = c;
	}
}


uint32_t crc32(void* data, uint32_t size) {
	uint8_t* buffer = (uint8_t*)data;

	uint32_t crc = 0xffffffff;

	for (uint32_t i = 0; i < size; ++i) {
		crc = (crc >> 8) ^ crc32_table[((crc ^ buffer[i]) & 0xff)];
	}

	return crc ^ 0xffffffff;
}


uint32_t crc32b(void* data, uint32_t size) {
	uint8_t* buffer = (uint8_t*)data;

	uint32_t crc = 0xffffffff;

	for (uint32_t i = 0; i < size; ++i) {
		//crc ^= buffer[i];
		uint8_t byte = buffer[i];
		for (uint32_t j = 0; j < 8; ++j) {
			if ((crc ^ byte) & 1) {
				crc = (crc >> 1) ^ CRC32_POLY;
			}
			else {
				crc >>= 1;
			}
			byte >>= 1;
		}
	}

	return crc ^ 0xffffffff;
}