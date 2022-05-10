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
	_CTRL_PARAM_START = 0,
	_PID_X,
	_PID_Y,
	_PID_Z,
	_X_MIX,
	_Y_MIX,
	_Z_MIX,
	_POSITION_PID,
	_WAYPOINT_THRESHOLD,
	_THRO_CONFIG,
	_CHANNEL_TRIM,
	_CHANNEL_REVERSE,
	_HEADING_PID,
	_ALTITUDE_PID,
	_ELEVATOR_TURN_P,
	_FLIGHT_MODE,

	_CTRL_VOLATILE_PARAM_START = 8192,

	_NAV_PARAM_START = 16384,
	_KALMAN_POSITION_UNCERTAINTY,
	_KALMAN_VELOCITY_UNCERTAINTY,
	_KALMAN_ORIENTATION_UNCERTAINTY,
	_KALMAN_ORIENTATION_MEASUREMENT_UNCERTAINTY,
	_KALMAN_GNSS_HORIZONTAL_UNCERTAINTY_MUL,
	_KALMAN_GNSS_VERTICAL_UNCERTAINTY_MUL,
	_KALMAN_BARO_VARIANCE,
	_KALMAN_ACCEL_VARIANCE,
	_KALMAN_ANGULARVELOCITY_VARIANCE,
	_KALMAN_GNSS_ZEROLAT,
	_KALMAN_GNSS_ZEROLONG,
	_MAG_A_1, // mag distortion matrix
	_MAG_A_2,
	_MAG_A_3,
	_MAG_B, // mag offset vector
	_ACCEL_B, // accel offset vector
	_GYRO_B, // gyro offset vector
	_BARO_HEIGHT_CAL,

	_NAV_VOLATILE_PARAM_START = 24576
} CTRL_Param;


#define NAV_DEVICE_ID 0xd5d5
#define DEVICE_ID 0xd6d6


typedef enum {
	REQUEST_HEADER_START,
	TRANSFER_REQUEST_HEADER,
	EEPROM_READ_REQUEST_HEADER,
	EEPROM_READ_N_REQUEST_HEADER,
	EEPROM_WRITE_REQUEST_HEADER,
	EEPROM_WRITE_N_REQUEST_HEADER,
	SET_VEC3_REQUEST_HEADER,
	READ_VEC3_REQUEST_HEADER,
	SAVE_VEC3_REQUEST_HEADER,
	SET_SCALAR_REQUEST_HEADER,
	READ_SCALAR_REQUEST_HEADER,
	SAVE_SCALAR_REQUEST_HEADER
} RequestHeader;

typedef enum {
	NAV_REQUEST_HEADER_START,
	NAV_SET_VEC3_REQUEST_HEADER,
	NAV_READ_VEC3_REQUEST_HEADER,
	NAV_SET_SCALAR_REQUEST_HEADER,
	NAV_READ_SCALAR_REQUEST_HEADER
} NAV_RequestHeader;


// nav_data_packet
// packet received from navigation computer
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


// nav_selftest_packet
// packet nagivation computer returns after selftest command
typedef struct {
	uint16_t device_id;

	uint8_t device_code;
	uint8_t imu_code;
	uint8_t mag_code;
	uint8_t baro_code;

	uint32_t crc;
} NAV_Selftest_Packet_Type;

#define NAV_SELFTEST_PACKET_CRC_OK 0xD2248BF3


typedef union {
	NAV_Selftest_Packet_Type bit;

	uint8_t reg[sizeof(NAV_Selftest_Packet_Type)];
} NAV_Selftest_Packet;


//#define TRANSFER_REQUEST_HEADER 0x0003

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


#define NAV_ACK_OK 0x0000
#define NAV_ACK_ERROR 0xFFFF

// packet returned to CTRL computer when more data is required to fulfill request
typedef struct {
	uint16_t device_id;

	uint16_t status_code;

	uint32_t crc;
} NAV_ACK_Packet_Type;

typedef union {
	NAV_ACK_Packet_Type bit;

	uint8_t reg[sizeof(NAV_ACK_Packet_Type)];
} NAV_ACK_Packet;


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


typedef struct {
	uint16_t device_id;

	uint8_t status;
	uint8_t nr_bytes;
	uint8_t data[64];

	uint32_t crc;
} CTRL_EEPROM_Read_N_Packet_Type;


typedef union {
	CTRL_EEPROM_Read_N_Packet_Type bit;

	uint8_t reg[sizeof(CTRL_EEPROM_Read_N_Packet_Type)];
} CTRL_EEPROM_Read_N_packet;


//#define EEPROM_READ_REQUEST_HEADER 0x0001

typedef struct {
	uint16_t header;

	uint32_t address;

	uint32_t crc;
} EEPROM_Read_Request_type;


typedef union {
	EEPROM_Read_Request_type bit;

	uint8_t reg[sizeof(EEPROM_Read_Request_type)];
} EEPROM_Read_Request;


//#define EEPROM_READ_N_REQUEST_HEADER 0x0004

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


//#define EEPROM_WRITE_REQUEST_HEADER 0x0002

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


//#define EEPROM_WRITE_N_REQUEST_HEADER 0x0005

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


//#define SET_VEC3_REQUEST_HEADER 0x0006

typedef struct {
	uint16_t header;

	uint16_t parameter; // cast from CTRL_Param
	float data[3];

	uint32_t crc;
} Set_Vec3_Request_Type;


typedef union {
	Set_Vec3_Request_Type bit;

	uint8_t reg[sizeof(Set_Vec3_Request_Type)];
} Set_Vec3_Request;


typedef struct {
	uint16_t header;

	uint16_t parameter;

	uint32_t crc;
} Read_Vec3_Request_Type;

typedef union {
	Read_Vec3_Request_Type bit;

	uint8_t reg[sizeof(Read_Vec3_Request_Type)];
} Read_Vec3_Request;


typedef struct {
	uint16_t device_id;

	float data[3];

	uint32_t crc;
} Read_Vec3_Response_Type;

typedef union {
	Read_Vec3_Response_Type bit;

	uint8_t reg[sizeof(Read_Vec3_Response_Type)];
} Read_Vec3_Response;


typedef struct {
	uint16_t header;

	uint16_t parameter;

	uint32_t crc;
} Save_Vec3_Request_Type;

typedef union {
	Save_Vec3_Request_Type bit;

	uint8_t reg[sizeof(Save_Vec3_Request_Type)];
} Save_Vec3_Request;


typedef struct {
	uint16_t header;

	uint16_t parameter; // cast from CTRL_Param
	float data;

	uint32_t crc;
} Set_Scalar_Request_Type;


typedef union {
	Set_Scalar_Request_Type bit;

	uint8_t reg[sizeof(Set_Scalar_Request_Type)];
} Set_Scalar_Request;


typedef struct {
	uint16_t device_id;

	float data;

	uint32_t crc;
} Read_Scalar_Response_Type;

typedef union {
	Read_Scalar_Response_Type bit;

	uint8_t reg[sizeof(Read_Scalar_Response_Type)];
} Read_Scalar_Response;


typedef struct {
	uint16_t header;

	uint16_t parameter;

	uint32_t crc;
} Read_Scalar_Request_Type;

typedef union {
	Read_Scalar_Request_Type bit;

	uint8_t reg[sizeof(Read_Scalar_Request_Type)];
} Read_Scalar_Request;


typedef struct {
	uint16_t header;

	uint16_t parameter;

	uint32_t crc;
} Save_Scalar_Request_Type;

typedef union {
	Save_Scalar_Request_Type bit;

	uint8_t reg[sizeof(Save_Scalar_Request_Type)];
} Save_Scalar_Request;


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

	bool portBusy = false;

	// bool pointer array for transfer requests
	std::queue<bool*> timeQueue;

	// number of attempts
	int resender_attempts = 3;

	// cannot pass variables from local stack to thread so I'll just
	// put them in the class
	//uint8_t* write_buffer;
	//uint32_t nr_write;
	//uint8_t* read_buffer;
	//uint32_t nr_read;
	float rate = 10;
	void send_command(uint16_t command);
	void single_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read);
	void continuous_poll(uint16_t command, uint8_t* read_buffer, uint32_t nr_read);
	void eeprom_write_data(uint32_t address, uint8_t data);
	void eeprom_read_data(uint32_t address, uint8_t* writeback);
	void eeprom_write_n_data(uint32_t address, uint8_t* data, uint8_t size); // max 64
	void eeprom_read_n_data(uint32_t address, uint8_t* writeback, uint8_t size);
	void ctrl_set_vec3(CTRL_Param parameter, float* value);
	void ctrl_read_vec3(CTRL_Param parameter, float* writeback);
	void ctrl_save_vec3(CTRL_Param parameter);
	void ctrl_set_scalar(CTRL_Param parameter, float* value);
	void ctrl_read_scalar (CTRL_Param parameter, float* writeback);
	void ctrl_save_scalar(CTRL_Param parameter);

	Transfer_Request createTransferRequest(uint16_t command);

	// functions for single actions to share port time with continuous actions
	void timeRequest();
	void timeReturn();

	bool check_ack_packet(CTRL_ACK_Packet packet);
public:
	bool open_port(const char* port, uint32_t baud);

	bool set_baud(uint32_t baud);
	bool get_status();
	uint32_t get_baud();

	bool read(uint8_t* buffer, uint32_t nr_bytes);
	bool write(uint8_t* buffer, uint32_t nr_bytes);
	void close();

	bool start_sendcommand(uint16_t command);
	bool start_pollthread(uint16_t command, uint8_t* read_buffer, uint32_t nr_read, bool continuous);
	bool start_eepromwritethread(uint32_t address, uint8_t data);
	bool start_eepromreadthread(uint32_t address, uint8_t* writeback);
	bool start_eepromwritenthread(uint32_t address, uint8_t* data, uint8_t size); // max 64
	bool start_eepromreadnthread(uint32_t address, uint8_t* writeback, uint8_t size); // max 64
	bool start_vec3setthread(CTRL_Param parameter, float* value);
	bool start_vec3readthread(CTRL_Param parameter, float* writeback);
	bool start_vec3savethread(CTRL_Param parameter);
	bool start_scalarsetthread(CTRL_Param parameter, float* value);
	bool start_scalarreadthread(CTRL_Param parameter, float* writeback);
	bool start_scalarsavethread(CTRL_Param parameter);
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