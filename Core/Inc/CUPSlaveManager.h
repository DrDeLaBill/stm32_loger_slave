/*
 *
 * CUPSlaveManager
 * Custom UART Protocol manager - slave
 * Created for connection PC (or Main Device) with indictions_transmitter via UART or another interface
 *
 */
#ifndef INC_CUPSLAVEMANAGER_H_
#define INC_CUPSLAVEMANAGER_H_


#include <stdint.h>

#include "utils.h"
#include "internal_storage.h"


#define MESSAGE_BUFFER_SIZE SD_PAYLOAD_BITS_SIZE(STORAGE_SD_MAX_PAYLOAD_SIZE) + 4


class CUPSlaveManager {

private:
	static const char* MODULE_TAG;

	typedef enum _CUP_command_typedef {
		CUP_CMD_DEVICE = 0x01,
		CUP_CMD_STTNGS,
		CUP_CMD_DATA,
	} CUP_command;

	typedef enum _CUP_error_typedef {
		CUP_NO_ERROR = 0x00,
		CUP_ERROR_COMMAND,
		CUP_ERROR_DATA_LEN,
		CUP_ERROR_DATA_OVERLOAD,
		CUP_ERROR_DATA_DOES_NOT_EXIST,
		CUP_ERROR_CRC,
		CUP_ERROR_TIMEOUT
	} CUP_error;

	typedef enum _CUP_response_state_typedef {
		CUP_RESPONSE_WAIT,
		CUP_RESPONSE_CHECK_CRC,
		CUP_RESPONSE_SUCCESS,
		CUP_RESPONSE_ERROR
	} CUP_response_state;

	CUP_response_state response_state;
	CUP_error cup_error_type;

	void set_response_state_success();
	void set_response_state_error(CUP_error error_type);

public:
	typedef struct _CUP_message {
		uint8_t command;
		uint16_t data_len;
		uint8_t data[SD_PAYLOAD_BITS_SIZE(STORAGE_SD_MAX_PAYLOAD_SIZE)];
		uint8_t crc8;
	} CUP_message;

private:
	const uint8_t CUP_WAIT_TIMEOUT = 50;

	uint16_t data_counter = 0;

	void (CUPSlaveManager::*curr_status_action) (uint8_t msg);

	template <typename T>
	bool fill_param(T* param, uint8_t msg) {
		T delta = (T)msg << (8 * this->data_counter++);
 		*param += delta;

		if (this->data_counter < sizeof(*param)) {
			return false;
		}

		this->data_counter = 0;
		return true;
	}

	void set_new_status(void (CUPSlaveManager::*status_action) (uint8_t));
	void status_wait(uint8_t msg);
	void status_status(uint8_t msg);
	void status_data_len(uint8_t msg);
	void status_data(uint8_t msg);
	void status_check_crc(uint8_t msg);
	void status_request_success(uint8_t msg);
	void status_request_error(uint8_t msg);

	void send_success_response();
	void send_error_response();
	void send_buffer(uint8_t* buffer, uint16_t len);
	void reset_data();
	void load_request_data();
	uint16_t load_message_to_buffer(uint8_t* buffer);
	uint8_t get_message_crc();
	uint8_t get_CRC8(uint8_t* buffer, uint16_t size);

	uint16_t load_data_to_buffer_handler(uint8_t* buffer);
	void load_settings_data_handler();

public:
	struct __attribute__((packed)) device_info_t {
		uint8_t device_type;
		uint8_t device_version;
		uint32_t id_base1;
		uint32_t id_base2;
		uint32_t id_base3;
		uint32_t id_base4;
	} device_info;

	CUP_message message;

	CUPSlaveManager();
	void proccess();

	void char_data_handler(uint8_t msg);
	void send_byte(uint8_t msg);
	void timeout();

	template <typename T>
	static uint16_t deserialize(uint8_t* buffer, T* variable) {
		for (uint8_t* ptr = buffer + sizeof(*variable) - 1; ptr >= buffer; ptr--) {
			*variable <<= 8;
			*variable += *ptr;
		}
		return sizeof(*variable);
	}

	template <typename T>
	static uint16_t serialize(uint8_t* buffer, T variable) {
		for (uint8_t i = 0; i < sizeof(variable); i++, buffer++) {
			*buffer = (uint8_t)(variable & 0xFF);
			variable >>= 8;
		}
		return sizeof(variable);
	}

};


#endif
