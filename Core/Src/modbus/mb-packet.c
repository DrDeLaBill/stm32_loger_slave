/*
 MODBUS Packet Maker
 By Liyanboy74
 https://github.com/liyanboy74/modbus
*/

#include "modbus/mb-packet.h"

#if (MB_MODE==MB_MODE_MASTER) || defined MB_DEBUG

void mb_packet_request_read_coil(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	Packet->device_address=DeviceAddress;
	Packet->func= MB_FUNC_Read_Coils;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_request_read_discrete_inputs(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->func= MB_FUNC_Read_Discrete_Inputs;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_request_read_holding_registers(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->func= MB_FUNC_Read_Holding_Registers;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_request_read_input_registers(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->func= MB_FUNC_Read_Input_Registers;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_request_write_single_coil(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t CAddress,uint16_t Data)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->func= MB_FUNC_Write_Single_Coil;
	Packet->u16_1=CAddress;
	Packet->u16_2=Data;
}

void mb_packet_request_write_single_register(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t RAddress,uint16_t Data)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Fix;
	Packet->func= MB_FUNC_Write_Single_Register;
	Packet->u16_1=RAddress;
	Packet->u16_2=Data;
}

void mb_packet_request_write_multiple_coils(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity,uint8_t len,uint8_t *Data)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Var;
	Packet->func= MB_FUNC_Write_Multiple_Coils;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
	Packet->len= len;
	Packet->Data=Data;
}

void mb_packet_request_write_multiple_registers(mb_packet_s* Packet, uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity,uint8_t len,uint8_t *Data)
{
	Packet->device_address=DeviceAddress;
	Packet->type=MB_PACKET_TYPE_Master_Request_Var;
	Packet->func= MB_FUNC_Write_Multiple_Registers;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
	Packet->len=len;
	Packet->Data=Data;
}

#endif
#if (MB_MODE==MB_MODE_SLAVE) || defined MB_DEBUG

void mb_packet_response_read_coil(mb_packet_s* Packet, uint8_t len,uint8_t *Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Var;
	Packet->func= MB_FUNC_Read_Coils;
	Packet->len= len;
	Packet->Data=Data;
}

void mb_packet_response_read_discrete_inputs(mb_packet_s* Packet, uint8_t len,uint8_t *Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Var;
	Packet->func= MB_FUNC_Read_Discrete_Inputs;
	Packet->len= len;
	Packet->Data=Data;
}

void mb_packet_response_read_holding_registers(mb_packet_s* Packet, uint8_t len,uint8_t *Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Var;
	Packet->func= MB_FUNC_Read_Holding_Registers;
	Packet->len = len;
	Packet->Data=Data;
}

void mb_packet_response_read_input_registers(mb_packet_s* Packet, uint8_t len,uint8_t *Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Var;
	Packet->func= MB_FUNC_Read_Input_Registers;
	Packet->len= len;
	Packet->Data=Data;
}

void mb_packet_response_write_single_coil(mb_packet_s* Packet, uint16_t CAddress,uint16_t Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Fix;
	Packet->func= MB_FUNC_Write_Single_Coil;
	Packet->u16_1=CAddress;
	Packet->u16_2=Data;
}

void mb_packet_response_write_single_register(mb_packet_s* Packet, uint16_t RAddress,uint16_t Data)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Fix;
	Packet->func= MB_FUNC_Write_Single_Register;
	Packet->u16_1=RAddress;
	Packet->u16_2=Data;
}

void mb_packet_response_write_multiple_coils(mb_packet_s* Packet, uint16_t StartAd,uint16_t Quantity)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Fix;
	Packet->func= MB_FUNC_Write_Multiple_Coils;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_response_write_multiple_registers(mb_packet_s* Packet, uint16_t StartAd,uint16_t Quantity)
{
	Packet->type=MB_PACKET_TYPE_Slave_Responce_Fix;
	Packet->func= MB_FUNC_Write_Multiple_Registers;
	Packet->u16_1=StartAd;
	Packet->u16_2=Quantity;
}

void mb_packet_error(mb_packet_s* Packet, mb_functions_e Func,mb_error_e Exeption_Code)
{
	Packet->type=MB_PACKET_TYPE_ERROR;
	Packet->func= 0x80|Func;
	Packet->err= Exeption_Code;
}

#endif
