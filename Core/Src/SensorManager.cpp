#include "SensorManager.h"

#include <fatfs.h>

#include "DeviceStateBase.h"
#include "SettingsManager.h"
#include "RecordManager.h"

extern "C"
{
	#include "internal_storage.h"
	#include "modbus/mb.h"
	#include "modbus/mb-packet.h"
}


#define MODBUS_CHECK(condition, time) if (!wait_event(condition, time)) {return;}

#define MODBUS_RESPONSE_TIME          100 //ms

#define MODBUS_REQ_TIMEOUT            100 //ms

#define MODBUS_MAX_ERRORS             5


const char* SensorManager::MODULE_TAG = "SENS";
SensorManager::sensor_read_status SensorManager::sens_status = SENS_WAIT;
uint8_t SensorManager::current_slave_id = RESERVED_IDS_COUNT;


SensorManager::SensorManager() {
	DeviceStateBase();
	this->current_action = &SensorManager::start_action;
	this->read_time = SettingsManager::sttngs->sens_read_period;

    mb_set_tx_handler(SensorManager::send_request);
    mb_set_master_process_handler(SensorManager::master_process);
}

void SensorManager::proccess() {
	(this->*current_action)();
}

void SensorManager::sleep() {
	this->current_action = &SensorManager::sleep_action;
}

void SensorManager::awake() {
	this->current_action = &SensorManager::start_action;
}

void SensorManager::start_action() {
	SensorManager::current_slave_id = RESERVED_IDS_COUNT;
	SensorManager::sens_status = SENS_WAIT;
	this->error_count = 0;
	this->current_action = &SensorManager::request_action;
}

void SensorManager::wait_action() {
	if (Util_TimerPending(&this->wait_timer)) {
		return;
	}
	this->current_action = &SensorManager::request_action;
}

void SensorManager::request_action() {
	if (this->current_slave_id >= LOW_MB_SENS_COUNT) {
		this->current_slave_id = RESERVED_IDS_COUNT;
		this->current_action = &SensorManager::wait_action;
		this->write_sensors_data();
		Util_TimerStart(&this->wait_timer, this->read_time);
		return;
	}

	if (this->current_slave_id < RESERVED_IDS_COUNT) {
		this->current_slave_id = RESERVED_IDS_COUNT;
	}

	mb_packet_s tmp_packet = {0};
	mb_packet_request_read_holding_registers(&tmp_packet, this->current_slave_id, 0x0000, 0x0001);
	mb_tx_packet_handler(&tmp_packet);

	SensorManager::sens_status = SENS_WAIT;
	Util_TimerStart(&this->wait_timer, MODBUS_REQ_TIMEOUT);
	this->current_action = &SensorManager::wait_response_action;
}

void SensorManager::wait_response_action() {
	if (SensorManager::sens_status == SENS_SUCCESS) {
		this->current_action = &SensorManager::success_response_action;
		return;
	}
	if (SensorManager::sens_status == SENS_ERROR) {
		this->current_action = &SensorManager::error_response_action;
		return;
	}
	if (Util_TimerPending(&this->wait_timer)) {
		return;
	}
    mb_rx_timeout_handler();
	this->current_action = &SensorManager::wait_action;
}

void SensorManager::success_response_action() {
	this->write_sensors_data();
	this->update_modbus_slave_id();
	this->current_action = &SensorManager::request_action;
}

void SensorManager::error_response_action() {
	// TODO: обработка ошибок modbus
	this->error_count++;
	if (this->error_count >= MODBUS_MAX_ERRORS) {
		this->registrate_modbus_error();
		this->update_modbus_slave_id();
		this->error_count = 0;
	}
	this->current_action = &SensorManager::request_action;
}

void SensorManager::register_action() {}

void SensorManager::sleep_action() {}

void SensorManager::write_sensors_data() {
	RecordManager::sd_record.v1.payload_record.record_id = RecordManager::get_new_record_id();
	RecordManager::sd_record.v1.payload_record.record_time = HAL_GetTick();
	RecordManager::save();
	RecordManager::clear_buf();
}

void SensorManager::send_request(uint8_t *data,uint8_t Len) {
   HAL_UART_Transmit(&LOW_MB_UART, data, Len, MODBUS_REQ_TIMEOUT);
}

void SensorManager::master_process(mb_packet_s* packet) {
    if(packet->type == MB_PACKET_TYPE_ERROR) {
    	SensorManager::sens_status = SENS_ERROR;
    	return;
    }
	RecordManager::sd_record.v1.payload_record.sensors_values[SensorManager::current_slave_id] = packet->Data[0];
	SensorManager::sens_status = SENS_SUCCESS;
	LOG_DEBUG(SensorManager::MODULE_TAG, "[%02d] A:%d ", packet->device_address, packet->Data[0]);
}


void SensorManager::update_modbus_slave_id() {
	do {
		this->current_slave_id++;
		if (SettingsManager::sttngs->low_sens_status[current_slave_id] >= SENSOR_TYPE_THERMAL) {
			return;
		}
	} while (this->current_slave_id < sizeof(SettingsManager::sttngs->low_sens_status));
}

void SensorManager::registrate_modbus_error() {
	SettingsManager::sttngs->low_sens_status[this->current_slave_id] = SENSOR_ERROR;
}