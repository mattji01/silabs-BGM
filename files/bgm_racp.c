/***************************************************************************//**
 * @file
 * @brief BGM Record Access Control Point characteristic
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


#include "bgm.h"

bool racp_enabled = false;

void bgm_send_racp_indication(uint8_t connection, uint8_t opcode, uint8_t error)
{
  sl_status_t sc = SL_STATUS_FAIL;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x01};
  buf[2] = opcode;
  buf[3] = error;
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send indication in RACP 0x%04X\n", sc);
  }
}


/**************************************************************************//**
 * 4.10.1 GLS/SEN/SPD/BV-01-C [Delete Stored Records - All records]
 *****************************************************************************/
void bgm_delete_all_records(uint8_t connection)
{
  const uint8_t data[]={2,1};
  sl_bt_gatt_server_send_indication(
      connection,
      gattdb_record_access_control_point,
      2, data
      );
  records_num = 0;
  //memset(sl_glucose_records, 0, sizeof(sl_glucose_records));

}

/**************************************************************************//**
 * 4.10.2 GLS/SEN/SPD/BV-02-C [Delete Stored Records - Within range of Sequence Number value pair]
 *****************************************************************************/
void bgm_delete_within_range_seq(uint8_t connection, uint16_t low, uint16_t high)
{
 /* sl_bt_gatt_server_send_indication(
      connection,
      gattdb_record_access_control_point,
      sizeof(delete_within_reply_data), delete_within_reply_data
      );*/
  uint8_t reply[] = {0x06, 0x00, 0x02, 0x01};
  sl_bt_gatt_server_send_indication(
        connection,
        gattdb_record_access_control_point,
        sizeof(reply), reply
        );
  records_num = records_num - (high - low + 1);
}


uint8_t bgm_report_all_num_records_data[] = {0x05, 0x00, 0x00, 0x00};
/**************************************************************************//**
 * 4.12.1 GLS/SEN/SPN/BV-01-C [Report Number of Stored Records - All records]
 * 4.12.3 GLS/SEN/SPN/BV-03-C [Report Number of Stored Records – No records found]
 *****************************************************************************/
void bgm_report_all_num_records(uint8_t connection)
{
  sl_status_t sc = SL_STATUS_FAIL;
  bgm_report_all_num_records_data[2] = records_num & 0xff;
  bgm_report_all_num_records_data[3] = records_num >> 8;
  sc = sl_bt_gatt_server_send_indication(
      connection,
      gattdb_record_access_control_point,
      sizeof(bgm_report_all_num_records_data), bgm_report_all_num_records_data
      );
  if (sc) {
    app_log_warning("Failed to report all records number\n");
  }
}
/**************************************************************************//**
 * 4.12.2 GLS/SEN/SPN/BV-01-C [Report Number of Stored Records - Greater than or equal to Sequence Number]
 *****************************************************************************/
void bgm_report_greater_num_records(uint8_t connection, uint16_t high)
{
  sl_status_t sc = SL_STATUS_FAIL;
  uint16_t temp = records_num - high + 1;
  bgm_report_all_num_records_data[2] = temp & 0xff;
  bgm_report_all_num_records_data[3] = temp >> 8;
  sc = sl_bt_gatt_server_send_indication(
      connection,
      gattdb_record_access_control_point,
      4, bgm_report_all_num_records_data
      );
  if (sc) {
    app_log_warning("Failed to report greater or equal sequence number records\n");
  }
}
/**************************************************************************//**
 * can be used in GLS/SEN/SPD/BV-02-C
 * [Delete Stored Records - Within range of Sequence Number value pair]
 *****************************************************************************/
void bgm_report_within_range(sl_bt_msg_t *evt)
{
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  //uint8_t operator = evt->data.evt_gatt_server_attribute_value.value.data[1];
  //uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  //uint8_t filter_type = 0;
 // uint16_t operand2 = 0;
 // filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
  uint16_t low = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] >> 8;
  uint16_t high = evt->data.evt_gatt_server_attribute_value.value.data[5] | evt->data.evt_gatt_server_attribute_value.value.data[6] >> 8;
  sl_status_t sc = SL_STATUS_FAIL;
  uint16_t temp = high - low;
  bgm_report_all_num_records_data[2] = temp & 0xff;
  bgm_report_all_num_records_data[3] = temp >> 8;
  sc = sl_bt_gatt_server_send_indication(
      connection,
      gattdb_record_access_control_point,
      sizeof(bgm_report_all_num_records_data), bgm_report_all_num_records_data
      );
  if (sc) {
    app_log_warning("Failed to report all records number\n");
  }
}
/**************************************************************************//**
 * BGM - BGM Record Access Control Point characteristic
 * indication changed callback
 *
 * Called when indication of BGM RCAP is enabled/disabled by
 * the client.
 *****************************************************************************/

void sl_bt_bgm_racp_indication_handler(sl_bt_msg_t *evt)
{
  // client characteristic configuration changed by remote GATT client
  if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
      if (sl_bt_gatt_server_indication == (sl_bt_gatt_server_client_configuration_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
          app_log("RACP enable indicate\n");
          racp_enabled = true;
      }
      else if(sl_bt_gatt_server_disable == (sl_bt_gatt_server_client_configuration_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
          app_log("RACP disable indicate\n");
          racp_enabled = false;
      }
   }
  // confirmation of indication received from remove GATT client
  else if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
    app_log("RACP receive indication confirm\n");
  }
}


void sl_bt_bgm_racp_handler(sl_bt_msg_t *evt)
{

  uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  for(int i = 0; i < len; i++){
      app_log("0x%02x ", evt->data.evt_gatt_server_attribute_value.value.data[i]);
  }
  app_log("\n");

  uint8_t opcode = evt->data.evt_gatt_server_attribute_value.value.data[0];
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  if(measure_enabled == false || racp_enabled == false){
      sl_bt_gatt_server_send_user_write_response(connection, gattdb_record_access_control_point, 0x81);
      return;
  }else if(security_level == 1){
      sl_bt_gatt_server_send_user_write_response(connection, gattdb_record_access_control_point, 0x0F);
      return;
  }else{
      sl_bt_gatt_server_send_user_write_response(connection, gattdb_record_access_control_point, 0);
  }

  switch(opcode)
  {
    case 0x01:
      bgm_report_record(evt);
      break;
    case 0x02:
      bgm_delete_record(evt);
      break;
    case 0x03:
      bgm_abort_operation(evt);
      break;
    case 0x04:
      bgm_report_num_records(evt);
      break;
    default:
      app_log("unsupported opcode\n");
      bgm_unsupported_opcode(connection, opcode);
      break;
  }
}
