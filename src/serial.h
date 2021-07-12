#ifndef SERIAL_H
#define SERIAL_H

#include <cstdint>
#include <stdlib.h>
#include <iostream>
#include <thread>

#include <Windows.h>

#include "time.h"
#include "util.h"


typedef enum {
	_PID_X,
	_PID_Y,
	_PID_Z
} CTRL_Param;


typedef struct { // aligned is for CRC calculation
	uint16_t device_id;

	// kalman filter output

	float position_x;
	float position_y;
	float position_z;

	float velocity_x;
	float velocity_y;
	float velocity_z;

	float accel_x;
	float accel_y;
	float accel_z;

	float accelraw_x;
	float accelraw_y;
	float accelraw_z;

	float orientation_x;
	float orientation_y;
	float orientation_z;

	float accelmagorientation_x;
	float accelmagorientation_y;
	float accelmagorientation_z;

	float angularvelocity_x;
	float angularvelocity_y;
	float angularvelocity_z;

	float mag_x;
	float mag_y;
	float mag_z;

	// gps output

	float latitude;
	float longitude;
	float gps_height;
	float h_acc; // horizontal accuracy
	float v_acc; // vertical accuracy
	uint16_t gps_satellites; // number of satellites
	uint32_t gps_watchdog; // time since last data in ms

	// raw data output

	float pressure; // pa
	float imu_temperature;
	float baro_temperature;

	float debug1;
	float debug2;


	uint32_t crc;
} Nav_Data_Packet_Type;


typedef union { // union to ease accessing of data
	Nav_Data_Packet_Type bit;

	uint8_t reg[sizeof(Nav_Data_Packet_Type)];
} NAV_Data_Packet;


typedef struct {
	uint16_t device_id;

	uint8_t device_code;
	uint8_t imu_code;
	uint8_t mag_code;
	uint8_t baro_code;

	uint32_t crc;
} NAV_Selftest_Packet_Type;


typedef union {
	NAV_Selftest_Packet_Type bit;

	uint8_t reg[sizeof(NAV_Selftest_Packet_Type)];
} NAV_Selftest_Packet;


#define TRANSFER_REQUEST_HEADER 0x0003

// packet sent by computer to request transfer
typedef struct {
	uint16_t header;

	uint16_t command;

	uint32_t crc;
} Transfer_Request_Type;


typedef union {
	Transfer_Request_Type bit;

	uint8_t reg[sizeof(Transfer_Request_Type)];
} Transfer_Request;


#define CTRL_ACK_OK 0x0000
#define CTRL_ACK_ERROR 0xffff

// packet returned by CTRL computer after packet
// send request from host computer
typedef struct {
	uint16_t device_id;

	uint16_t status_code;

	uint32_t crc;
} CTRL_ACK_Packet_Type;


typedef union {
	CTRL_ACK_Packet_Type bit;

	uint8_t reg[sizeof(CTRL_ACK_Packet_Type)];
} CTRL_ACK_Packet;


typedef struct {
	uint16_t device_id;

	uint8_t status;
	uint8_t data;

	uint32_t crc;
} CTRL_EEPROM_Read_Packet_Type;


typedef union {
	CTRL_EEPROM_Read_Packet_Type bit;

	uint8_t reg[sizeof(CTRL_EEPROM_Read_Packet_Type)];
} CTRL_EEPROM_Read_packet;


#define EEPROM_READ_REQUEST_HEADER 0x0001;

typedef struct {
	uint16_t header;

	uint32_t address;

	uint32_t crc;
} EEPROM_Read_Request_type;


typedef union {
	EEPROM_Read_Request_type bit;

	uint8_t reg[sizeof(EEPROM_Read_Request_type)];
} EEPROM_Read_Request;


#define EEPROM_READ_N_REQUEST_HEADER 0x0004

typedef struct {
	uint16_t header;

	uint8_t size;
	uint32_t address;

	uint32_t crc;
} EEPROM_Read_N_Request_type;


typedef union {
	EEPROM_Read_N_Request_type bit;

	uint8_t reg[sizeof(EEPROM_Read_N_Request_type)];
} EEPROM_Read_N_Request;


#define EEPROM_WRITE_REQUEST_HEADER 0x0002

typedef struct {
	uint16_t header;

	uint32_t address;
	uint8_t data;

	uint32_t crc;
} EEPROM_Write_Request_Type;


typedef union {
	EEPROM_Write_Request_Type bit;

	uint8_t reg[sizeof(EEPROM_Write_Request_Type)];
} EEPROM_Write_Request;


#define EEPROM_WRITE_N_REQUEST_HEADER 0x0005

typedef struct {
	uint16_t header;

	uint8_t size;
	uint32_t address;
	uint8_t data[64];

	uint32_t crc;
} EEPROM_Write_N_Request_Type;


typedef union {
	EEPROM_Write_N_Request_Type bit;

	uint8_t reg[sizeof(EEPROM_Write_N_Request_Type)];
} EEPROM_Write_N_Request;


#define CTRL_SET_VEC3_HEADER 0x0006

//#pragma pack(push, 1)
typedef struct {
	uint16_t header;

	uint16_t parameter; // cast from CTRL_Param
	float data[3];

	uint32_t crc;
} CTRL_Set_Vec3_Type;
//#pragma pack(pop)


typedef union {
	CTRL_Set_Vec3_Type bit;

	uint8_t reg[sizeof(CTRL_Set_Vec3_Type)];
} CTRL_Set_Vec3;


extern NAV_Data_Packet nav_data_packet;
extern NAV_Selftest_Packet nav_selftest_packet;


class Serial {
private:
	char* port = nullptr;
	uint32_t baud = NULL;

	HANDLE port_handle;

	bool port_open = false;

	std::thread serialthread;
	std::thread continuousthread;
	bool serialthread_open = false;
	bool continuousthread_open = false;
	// variable turns true when poll is finished
	bool serialthread_done = false;
	bool continuousthread_done = false;
	// variable true if continuously polling
	bool pollthread_continuous = true;

	bool timerequest = false;
	bool timereturned = false;

	// cannot pass variables from local stack to thread so I'll just
	// put them in the class
	//uint8_t* write_buffer;
	//uint32_t nr_write;
	//uint8_t* read_buffer;
	//uint32_t nr_read;
	float rate = 10;
	void single_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read);
	void continuous_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read);
	void eeprom_write_data(uint32_t address, uint8_t data);
	void eeprom_read_data(uint32_t address, uint8_t* writeback);
	void ctrl_set_vec3(CTRL_Param parameter, float* value);

	Transfer_Request createTransferRequest(uint16_t command);

	// functions for single actions to share port time with continuous actions
	void timeRequest();
	void timeReturn();
public:
	bool open_port(const char* port, uint32_t baud);

	bool set_baud(uint32_t baud);
	bool get_status();
	uint32_t get_baud();

	bool read(uint8_t* buffer, uint32_t nr_bytes);
	bool write(uint8_t* buffer, uint32_t nr_bytes);
	void close();

	bool start_pollthread(uint16_t command, uint8_t* read_buffer, uint32_t nr_read, bool continuous);
	bool start_eepromwritethread(uint32_t address, uint8_t data);
	bool start_eepromreadthread(uint32_t address, uint8_t* writeback);
	bool start_vec3setthread(CTRL_Param parameter, float* value);
	void join_pollthread();
	void join_continuousthread();

	void set_pollRate(float rate);
	float get_pollRate();
	// returns true if pollthread is running
	bool get_pollState();
	bool get_continuousState();
	// returns true if pollthread has finished task
	bool get_pollDone();
	// returns true if last thread was continuous poll
	//bool get_pollContinuous();

	// destructor calls close function
	~Serial();
};


// global serial object
extern Serial serial;


#endif