/***************************************************************************//**
 * @file
 * @brief BGM header file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef BGM_H_
#define BGM_H_

#include "stdbool.h"
#include "gatt_db.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "app_log.h"

// <o SHORT_BUTTON_PRESS_DURATION> Duration of Short button presses
// <i> Default: 250
// <i> Any button press shorter than this value will be considered SHORT_BUTTON_PRESS.
#define SHORT_BUTTON_PRESS_DURATION   (250)

// <o MEDIUM_BUTTON_PRESS_DURATION> Duration of MEDIUM button presses
// <i> Default: 1000
// <i> Any button press shorter than this value and longer SHORT_BUTTON_PRESS than will
// be considered MEDIUM_BUTTON_PRESS.
#define MEDIUM_BUTTON_PRESS_DURATION   (1000)

// Glucose measurement records max number
#define MAX_RECORD_NUM 500
#define SL_BT_BGM_STATUS_ANNUNCIATION_SUPPORTED false
#define SL_BT_BGM_SENSOR_STATUS_ANNUNCIATION 0x08
#define SL_BT_BGM_MEASURE_CONTEXT 0x10

extern bool measure_enabled;
extern bool racp_enabled;
extern uint8_t security_level;

// Glucose measurement current record number
extern uint16_t records_num;

// Glucose sequence number
extern uint8_t seq_num;

// GLusose abort operation - Report Stored Records
extern bool bgm_abort_operation_flag;

// Glucose measurement data
#if SL_BT_BGM_STATUS_ANNUNCIATION_SUPPORTED
extern uint8_t sl_glucose_records[MAX_RECORD_NUM][17];
#else
extern uint8_t sl_glucose_records[MAX_RECORD_NUM][15];
#endif

//measurement characteristic functions
void sl_bt_bgm_measurement_notification_handler(sl_bt_msg_t *evt);
void sl_bt_bgm_measurement_context_handler(sl_bt_msg_t *evt);
void bgm_report_all_records(uint8_t connection);
void bgm_report_less_record(uint8_t connection, uint16_t maximum);
void bgm_report_greater(uint8_t connection, uint8_t filter_type, uint16_t low);
void bgm_report_within(uint8_t connection, uint8_t filter_type, uint16_t low, uint16_t high);
void bgm_report_first_record(uint8_t connection);
void bgm_report_last_record(uint8_t connection);


// error handling functions
void bgm_unsupported_opcode(uint8_t connection, uint8_t opcode);
void bgm_invalid_operator(uint8_t connection);
void bgm_unsupported_operator(uint8_t connection);
void bgm_invalid_operand_type1(uint8_t connection);
void bgm_invalid_operand_type2(uint8_t connection);
void bgm_unsupported_operand(uint8_t connection);

// RACP functions
void sl_bt_bgm_racp_indication_handler(sl_bt_msg_t *evt);
void sl_bt_bgm_racp_handler(sl_bt_msg_t *evt);
void bgm_send_racp_indication(uint8_t connection, uint8_t opcode, uint8_t error);
void bgm_delete_all_records(uint8_t connection);
void bgm_delete_within_range_seq(uint8_t connection, uint16_t low, uint16_t high);
void bgm_report_all_num_records(uint8_t connection);
void bgm_report_greater_num_records(uint8_t connection, uint16_t high);

// Report records
void bgm_report_record(sl_bt_msg_t *evt);
void bgm_delete_record(sl_bt_msg_t *evt);
void bgm_abort_operation(sl_bt_msg_t *evt);
void bgm_report_num_records(sl_bt_msg_t *evt);

void bgm_add_measurement_record(void);
void sl_bt_bgm_set_dac(sl_bt_msg_t *evt);
void sl_bt_bgm_get_internal_adc(sl_bt_msg_t *evt);
void sl_bt_bgm_get_external_adc(sl_bt_msg_t *evt);
void sl_bt_bgm_get_temp(sl_bt_msg_t *evt);
void sl_bt_bgm_get_enob(sl_bt_msg_t *evt);
void sl_bt_bgm_get_dac_level(sl_bt_msg_t *evt);
void sl_bt_bgm_read_feature(sl_bt_msg_t *evt);
void bgm_report_within_range(sl_bt_msg_t *evt);




#endif /* BGM_H_ */
