#include "serial.h"

//#include <SDL.h>


// global serial object
Serial serial;

NAV_Data_Packet nav_data_packet;
NAV_Selftest_Packet nav_selftest_packet;
CTRL_ACK_Packet ctrl_ack_packet;


bool  Serial::open_port(const char* port, uint32_t baud) {
	if (port_open) return false;

	// port name buffer
	char port_name[16];
	strcpy_s(port_name, "\\\\.\\");
	strcat_s(port_name, port);

	// open port and create reference
	port_handle = CreateFileA(
		static_cast<LPCSTR>(port_name),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	// check port opened correctly
	if (port_handle == INVALID_HANDLE_VALUE) {
		printf("Error opening serial port %s\n", port);
		return false;
	}


	// create port configration struct
	DCB port_parameters = { 0 };
	port_parameters.DCBlength = sizeof(port_parameters);

	// get current state of port
	if (!GetCommState(port_handle, &port_parameters)) {
		printf("Error getting current parameters of port\n");
		return false;
	}

	// modify current port state
	port_parameters.BaudRate = static_cast<DWORD>(baud);
	port_parameters.ByteSize = 8;
	port_parameters.StopBits = ONESTOPBIT;
	port_parameters.Parity = NOPARITY;

	// apply parameters to port
	if (!SetCommState(port_handle, &port_parameters)) {
		printf("Error applying parameters to %s\n", port);
		return false;
	}

	this->baud = baud;


	// create port timeout structure
	COMMTIMEOUTS port_timeout;
	// wait 5ms between bytes
	port_timeout.ReadIntervalTimeout = 5;
	// ignore all other parameters
	port_timeout.ReadTotalTimeoutMultiplier = 0;
	port_timeout.ReadTotalTimeoutConstant = 1000;
	port_timeout.WriteTotalTimeoutMultiplier = 0;
	port_timeout.WriteTotalTimeoutConstant = 100;

	if (!SetCommTimeouts(port_handle, &port_timeout)) {
		printf("Error setting port timeouts\n");
		return false;
	}


	// define port open
	port_open = true;

	return true;
}


bool Serial::set_baud(uint32_t baud) {
	// create port configration struct
	DCB port_parameters = { 0 };
	port_parameters.DCBlength = sizeof(port_parameters);

	// modify current port state
	port_parameters.BaudRate = static_cast<DWORD>(baud);
	port_parameters.ByteSize = 8;
	port_parameters.StopBits = ONESTOPBIT;
	port_parameters.Parity = NOPARITY;

	// apply parameters to port
	if (!SetCommState(port_handle, &port_parameters)) {
		printf("Error applying parameters to %s\n", port);
		return false;
	}

	this->baud = baud;

	return true;
}


bool Serial::get_status() {
	return port_open;
}


uint32_t Serial::get_baud() {
	return baud;
}


bool Serial::read(uint8_t* buffer, uint32_t nr_bytes) {
	//ClearCommError(port_handle, NULL, NULL);
	//PurgeComm(port_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
	//ClearCommBreak(port_handle);

	DWORD bytes;

	if (!ReadFile(port_handle, static_cast<LPVOID>(buffer), static_cast<DWORD>(nr_bytes), &bytes, NULL)) {
		printf("Error reading data from port\n");
		return false;
	}
	return true;
	//return static_cast<uint32_t>(bytes);
}


bool Serial::write(uint8_t* buffer, uint32_t nr_bytes) {
	//ClearCommError(port_handle, NULL, NULL);
	//PurgeComm(port_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
	//ClearCommBreak(port_handle);

	DWORD bytes;

	if (!WriteFile(port_handle, static_cast<LPCVOID>(buffer), static_cast<DWORD>(nr_bytes), &bytes, NULL)) {
		printf("Error writing data to port\n");
		return false;
	}
	return true;
}


void Serial::close() {
	if (port_open) {
		port_open = false;
		CloseHandle(port_handle);
	}
}


Serial::~Serial() {
	close();
}



bool Serial::start_pollthread(uint8_t* write_buffer, uint32_t nr_write, uint8_t* read_buffer, uint32_t nr_read, bool continuous) {
	if (!serialthread_open && port_open) {
		this->write_buffer = write_buffer;
		this->nr_write = nr_write;
		this->read_buffer = read_buffer;
		this->nr_read = nr_read;
		if (continuous) {
			serialthread = std::thread(&Serial::continuous_poll, this);
			pollthread_continuous = true;
		}
		else {
			serialthread = std::thread(&Serial::single_poll, this);
			pollthread_continuous = false;
		}
		return true;
	}

	return false;
}

bool Serial::start_eepromwritethread(uint32_t address, uint8_t data) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::eeprom_write_data, this, address, data);

		return true;
	}

	return false;
}

bool Serial::start_eepromreadthread(uint32_t address, uint8_t* writeback) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::eeprom_read_data, this, address, writeback);

		return true;
	}

	return false;
}


void Serial::join_pollthread() {
	if (serialthread_open) {
		// change pollthread_open first
		// continuous poll thread listens to this variable
		serialthread_open = false;
		serialthread.join();
		serialthread_done = false;
	}
}


void Serial::set_pollRate(float rate) {
	this->rate = rate;
}

float Serial::get_pollRate() {
	return rate;
}

bool Serial::get_pollState() {
	return serialthread_open;
}

bool Serial::get_pollDone() {
	return serialthread_done;
}

bool Serial::get_pollContinuous() {
	return pollthread_continuous;
}


void Serial::single_poll() {
	// set serialthread_open flag to true
	serialthread_open = true;

	// poll data from processor
	write(write_buffer, nr_write);
	read(read_buffer, nr_read);

	serialthread_done = true;
}

void Serial::continuous_poll() {
	// set serialthread_open flag true
	serialthread_open = true;

	Timer threadTimer;

	// continuous polling loop
	while (serialthread_open) {
		// operation fails close the port
		// the rest of the program will recognise the closed
		// port
		if (!write(write_buffer, nr_write)) close();
		if (!read(read_buffer, nr_read)) close();

		threadTimer.setFrameCap(rate);

		threadTimer.frameDelay();
	}
}

void Serial::eeprom_write_data(uint32_t address, uint8_t data) {
	// set serialthread_open flag to true
	serialthread_open = true;

	uint8_t command = 0x41;
	write(&command, 1);
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		EEPROM_Write_Request eeprom_write_request;
		eeprom_write_request.bit.header = EEPROM_WRITE_REQUEST_HEADER;
		eeprom_write_request.bit.data = data;
		eeprom_write_request.bit.address = address;
		eeprom_write_request.bit.crc = crc32(eeprom_write_request.reg, sizeof(eeprom_write_request.reg) - 4);
		write(eeprom_write_request.reg, sizeof(eeprom_write_request.reg));
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
}

void Serial::eeprom_read_data(uint32_t address, uint8_t* writeback) {
	// set serialthread_open flag to true
	serialthread_open = true;

	uint8_t command = 0x40;
	write(&command, 1);
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		EEPROM_Read_Request eeprom_read_request;
		eeprom_read_request.bit.header = EEPROM_READ_REQUEST_HEADER;
		eeprom_read_request.bit.address = address;
		eeprom_read_request.bit.crc = crc32(eeprom_read_request.reg, sizeof(eeprom_read_request.reg) - 4);
		write(eeprom_read_request.reg, sizeof(eeprom_read_request.reg));

		CTRL_EEPROM_Read_packet eeprom_read_packet;
		read(eeprom_read_packet.reg, sizeof(eeprom_read_packet.reg));
		*writeback = eeprom_read_packet.bit.data;
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
}