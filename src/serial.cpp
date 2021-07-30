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
	// wait 20ms between bytes
	// WARNING this value is sensitive to baud rate
	// low baud rates will likely require a higher value
	port_timeout.ReadIntervalTimeout = 20;
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

	printf("Read %d/%d bytes\n", bytes, nr_bytes);

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



bool Serial::start_sendcommand(uint16_t command) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::send_command, this, command);

		return true;
	}

	return false;
}

bool Serial::start_pollthread(uint16_t command, uint8_t* read_buffer, uint32_t nr_read, bool continuous) {
	if (!serialthread_open && port_open) {
		//this->write_buffer = write_buffer;
		//this->nr_write = nr_write;
		//this->read_buffer = read_buffer;
		//this->nr_read = nr_read;
		if (continuous) {
			continuousthread = std::thread(&Serial::continuous_poll, this, command, read_buffer, nr_read);
			pollthread_continuous = true;
		}
		else {
			serialthread = std::thread(&Serial::single_poll, this, command, read_buffer, nr_read);
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

bool Serial::start_vec3setthread(CTRL_Param parameter, float* value) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_set_vec3, this, parameter, value);

		return true;
	}

	return false;
}

bool Serial::start_vec3readthread(CTRL_Param parameter, float* writeback) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_read_vec3, this, parameter, writeback);

		return true;
	}

	return false;
}

bool Serial::start_vec3savethread(CTRL_Param parameter) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_save_vec3, this, parameter);

		return true;
	}

	return false;
}

bool Serial::start_scalarsetthread(CTRL_Param parameter, float* value) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_set_scalar, this, parameter, value);

		return true;
	}

	return false;
}

bool Serial::start_scalarreadthread(CTRL_Param parameter, float* writeback) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_read_scalar, this, parameter, writeback);

		return true;
	}

	return false;
}

bool Serial::start_scalarsavethread(CTRL_Param parameter) {
	if (!serialthread_open && port_open) {
		serialthread = std::thread(&Serial::ctrl_save_scalar, this, parameter);

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


void Serial::join_continuousthread() {
	if (continuousthread_open) {
		continuousthread_open = false;
		continuousthread.join();
		continuousthread_done = false;
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

bool Serial::get_continuousState() {
	return continuousthread_open;
}

bool Serial::get_pollDone() {
	return serialthread_done;
}

//bool Serial::get_pollContinuous() {
//	return pollthread_continuous;
//}

void Serial::send_command(uint16_t command) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	Transfer_Request request = createTransferRequest(command);
	write(request.reg, sizeof(request.reg));
	SDL_Delay(20);

	serialthread_done = true;
	timeReturn();
}

void Serial::single_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read) {
	// set serialthread_open flag to true
	serialthread_open = true;

	Transfer_Request request = createTransferRequest(command);

	// poll data from processor
	write(request.reg, sizeof(request.reg));
	read(read_buffer, nr_read);

	serialthread_done = true;
}

void Serial::continuous_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read) {
	// set serialthread_open flag true
	continuousthread_open = true;

	Transfer_Request request = createTransferRequest(command);

	Timer threadTimer;

	// continuous polling loop
	while (continuousthread_open) {
		if (timerequest) {
			timerequest = false;
			while (!timereturned);
			timereturned = false;
		}

		// operation fails close the port
		// the rest of the program will recognise the closed
		// port
		if (!write(request.reg, sizeof(request.reg))) close();
		if (!read(read_buffer, nr_read)) close();

		threadTimer.setFrameCap(rate);

		threadTimer.frameDelay();
	}
}

void Serial::eeprom_write_data(uint32_t address, uint8_t data) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0041);

	write(request.reg, sizeof(request.reg));
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
	timeReturn();
}

void Serial::eeprom_read_data(uint32_t address, uint8_t* writeback) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0040);

	write(request.reg, sizeof(request.reg));
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
	timeReturn();
}

void Serial::ctrl_set_vec3(CTRL_Param parameter, float* value) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0044);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Set_Vec3_Request set_request;
		set_request.bit.header = SET_VEC3_REQUEST_HEADER;
		set_request.bit.parameter = (uint16_t) parameter;
		set_request.bit.data[0] = value[0];
		set_request.bit.data[1] = value[1];
		set_request.bit.data[2] = value[2];
		set_request.bit.crc = crc32(set_request.reg, sizeof(set_request.reg) - 4);
		write(set_request.reg, sizeof(set_request.reg));
		//printf("0x%04x\n0x%08x\n0x%08x\n", set_request.bit.header, set_request.bit.crc, crc32(set_request.reg, sizeof(set_request.reg)));
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}

void Serial::ctrl_read_vec3(CTRL_Param parameter, float* writeback) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0045);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Read_Vec3_Request read_request;
		read_request.bit.header = READ_VEC3_REQUEST_HEADER;
		read_request.bit.parameter = (uint16_t)parameter;
		read_request.bit.crc = crc32(read_request.reg, sizeof(read_request.reg) - 4);
		write(read_request.reg, sizeof(read_request.reg));
		
		Read_Vec3_Response read_packet;
		read(read_packet.reg, sizeof(read_packet.reg));
		writeback[0] = read_packet.bit.data[0];
		writeback[1] = read_packet.bit.data[1];
		writeback[2] = read_packet.bit.data[2];
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}

void Serial::ctrl_save_vec3(CTRL_Param parameter) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0046);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Save_Vec3_Request save_request;
		save_request.bit.header = SAVE_VEC3_REQUEST_HEADER;
		save_request.bit.parameter = (uint16_t)parameter;
		save_request.bit.crc = crc32(save_request.reg, sizeof(save_request.reg) - 4);
		write(save_request.reg, sizeof(save_request.reg));
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}

void Serial::ctrl_set_scalar(CTRL_Param parameter, float* value) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0047);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Set_Scalar_Request set_request;
		set_request.bit.header = SET_SCALAR_REQUEST_HEADER;
		set_request.bit.parameter = (uint16_t)parameter;
		set_request.bit.data = *value;
		set_request.bit.crc = crc32(set_request.reg, sizeof(set_request.reg) - 4);
		write(set_request.reg, sizeof(set_request.reg));
		//printf("0x%04x\n0x%08x\n0x%08x\n", set_request.bit.header, set_request.bit.crc, crc32(set_request.reg, sizeof(set_request.reg)));
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}

void Serial::ctrl_read_scalar(CTRL_Param parameter, float* writeback) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0048);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Read_Scalar_Request read_request;
		read_request.bit.header = READ_SCALAR_REQUEST_HEADER;
		read_request.bit.parameter = (uint16_t)parameter;
		read_request.bit.crc = crc32(read_request.reg, sizeof(read_request.reg) - 4);
		write(read_request.reg, sizeof(read_request.reg));

		Read_Scalar_Response read_packet;
		read(read_packet.reg, sizeof(read_packet.reg));
		*writeback = read_packet.bit.data;
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}

void Serial::ctrl_save_scalar(CTRL_Param parameter) {
	// set serialthread_open flag to true
	serialthread_open = true;

	timeRequest();

	static Transfer_Request request = createTransferRequest(0x0049);

	write(request.reg, sizeof(request.reg));
	read(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg));
	if (ctrl_ack_packet.bit.status_code == CTRL_ACK_OK && crc32(ctrl_ack_packet.reg, sizeof(ctrl_ack_packet.reg)) == CRC32_CHECK) {
		Save_Scalar_Request save_request;
		save_request.bit.header = SAVE_SCALAR_REQUEST_HEADER;
		save_request.bit.parameter = (uint16_t)parameter;
		save_request.bit.crc = crc32(save_request.reg, sizeof(save_request.reg) - 4);
		write(save_request.reg, sizeof(save_request.reg));
	}
	else {
		std::cout << "Bad response\n";
	}

	serialthread_done = true;
	timeReturn();
}


Transfer_Request Serial::createTransferRequest(uint16_t command) {
	Transfer_Request request;
	request.bit.header = TRANSFER_REQUEST_HEADER;
	request.bit.command = command;
	request.bit.crc = crc32(request.reg, sizeof(request.reg) - 4);
	return request;
}


void Serial::timeRequest() {
	if (continuousthread_open) {
		// set to true
		timerequest = true;
		// wait until reset
		while (timerequest);
	}
}


void Serial::timeReturn() {
	if (continuousthread_open) {
		timereturned = true;
	}
}