/***************************************************************************//**
 * @file
 * @brief BGM service procedures, include feature read
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

#define SL_BT_BGM_FEATURE_MULTI_BOND_SUPPORT true
#define SL_BT_BGM_FEATURE_MULTI_BOND        0x04 /**< Multiple-Bond Supported */

void sl_bt_bgm_read_feature(sl_bt_msg_t *evt)
{
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  uint16_t sent_len;
  size_t len = 2;
  uint8_t att_errorcode = 0;
  uint8_t value[] = {0x00, 0x00};
#if SL_BT_BGM_FEATURE_MULTI_BOND_SUPPORT
  value[1] |= SL_BT_BGM_FEATURE_MULTI_BOND;
#endif
  sl_bt_gatt_server_send_user_read_response(connection,
                                            gattdb_glucose_feature,
                                            att_errorcode,
                                            len,
                                            value,
                                            &sent_len);
}
/**************************************************************************//**
 * 4.9 Service Procedures – Report Stored Records
 *****************************************************************************/
void bgm_report_record(sl_bt_msg_t *evt)
{
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  uint8_t operator = evt->data.evt_gatt_server_attribute_value.value.data[1];
  uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  uint8_t filter_type = 0;
  uint16_t operand1 =0, operand2 = 0;

  switch(operator)
  {
    case 0x00:
      bgm_invalid_operator(connection);
      return;
      //break;
    case 0x01:
      app_log("report all records\n");
      if(len != 2){
        bgm_invalid_operand_type1(connection);
        return;
      }
      bgm_report_all_records(connection);
      break;
    case 0x02:
      app_log("report less than record\n");
      if(len != 5){
          bgm_invalid_operand_type1(connection);
          return;
      }
      filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
      operand1 = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] << 8;
      bgm_report_less_record(connection, operand1);
      break;
    case 0x03:
      app_log("report greater than record\n");
      filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
      if(filter_type == 0x01 && len != 5){
          bgm_invalid_operand_type1(connection);
          return;
      }
      operand1 = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] << 8;
      if(filter_type == 0x01 && operand1 > seq_num){
          bgm_send_racp_indication(connection, 0x01, 0x06);
          return;
      }
      if(filter_type != 0x01 && filter_type != 0x02){
          bgm_send_racp_indication(connection, 0x01, 0x09);
          return;
      }

      bgm_report_greater(connection, filter_type, operand1);
      break;
    case 0x04:
      app_log("report within range of sequence number value pair\n");
      if(len != 7){
          bgm_invalid_operand_type1(connection);
          return;
      }
      filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
      operand1 = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] << 8;
      operand2 = evt->data.evt_gatt_server_attribute_value.value.data[5] | evt->data.evt_gatt_server_attribute_value.value.data[6] << 8;
      if(operand2 < operand1){
          bgm_invalid_operand_type2(connection);
          return;
      }
      if(operand2 > records_num || operand1 > records_num){
          bgm_invalid_operand_type2(connection);
          return;
      }
      //bgm_unsupported_operator(connection);
      //return;
      bgm_report_within(connection, filter_type, operand1, operand2);
      break;
    case 0x05:
      app_log("read first record\n");
      bgm_report_first_record(connection);
      break;
    case 0x06:
      app_log("read last record\n");
      bgm_report_last_record(connection);
      break;

    default:
      app_log("read record unsupported operator\n");
      bgm_unsupported_operator(connection);
      break;
  }
}


/**************************************************************************//**
 * 4.10 Service Procedures – Delete Stored Records
 *****************************************************************************/
void bgm_delete_record(sl_bt_msg_t *evt)
{
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  if(len < 2){
      bgm_invalid_operator(connection);
  }
  uint8_t operator = evt->data.evt_gatt_server_attribute_value.value.data[1];
  uint8_t filter_type = 0, operand1 = 0, operand2 = 0;
  switch(operator)
  {
    case 0x01:
      app_log("delete all records\n");
      bgm_delete_all_records(evt->data.evt_gatt_server_attribute_value.connection);
      break;
    case 0x04:
      app_log("delete records within range\n");
      if(len != 7){
          bgm_invalid_operand_type1(connection);
          return;
      }
      filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
      operand1 = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] << 8;
      operand2 = evt->data.evt_gatt_server_attribute_value.value.data[5] | evt->data.evt_gatt_server_attribute_value.value.data[6] << 8;
      if(filter_type != 0x01){
          app_log("unsupported operand\n");
          bgm_unsupported_operand(connection);
      }
      if(operand2 < operand1){
          bgm_invalid_operand_type2(connection);
          return;
      }
      if(operand2 > seq_num - 1){
          bgm_invalid_operand_type2(connection);
          return;
      }
      bgm_delete_within_range_seq(connection, operand1, operand2);
      break;
    default:
      app_log("unknown report record opcode\n");
      bgm_unsupported_operator(connection);
      break;
  }
}

/**************************************************************************//**
 * 4.11 Service Procedures – Abort Operation
 * 4.11.1 GLS/SEN/SPA/BV-01-C [Abort Operation – Report Stored Records]
 *****************************************************************************/
void bgm_abort_operation(sl_bt_msg_t *evt)
{
  uint8_t opcode = evt->data.evt_gatt_server_attribute_value.value.data[0];
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  if(len != 2){
      bgm_invalid_operator(connection);
  }
  uint8_t operator = evt->data.evt_gatt_server_attribute_value.value.data[1];
  if(operator != 0x00){
      bgm_unsupported_operator(connection);
  }else{
      bgm_abort_operation_flag = true;
      app_log("set bgm_abort_operation_flag true\n");
  }
  bgm_send_racp_indication(connection, opcode, 0x01);

}


/**************************************************************************//**
 * 4.12 Service Procedures – Report Number of Stored Records
 *****************************************************************************/
void bgm_report_num_records(sl_bt_msg_t *evt)
{
  uint8_t connection = evt->data.evt_gatt_server_attribute_value.connection;
  uint8_t operator = evt->data.evt_gatt_server_attribute_value.value.data[1];
  uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
  uint8_t filter_type = 0;
  uint16_t operand2 = 0;

  switch (operator)
  {
    // report all records
    case 0x01:
      if(len != 2){
          bgm_invalid_operand_type1(connection);
          return;
      }
      bgm_report_all_num_records(connection);
      break;
    case 0x03:
      if(len != 5){
          bgm_invalid_operand_type1(connection);
          return;
      }
      filter_type = evt->data.evt_gatt_server_attribute_value.value.data[2];
      operand2 = evt->data.evt_gatt_server_attribute_value.value.data[3] | evt->data.evt_gatt_server_attribute_value.value.data[4] >> 8;
      if(filter_type != 0x01){
          bgm_unsupported_operand(connection);
          return;
      }
      if(operand2 > records_num){
          bgm_unsupported_operand(connection);
      }
      bgm_report_greater_num_records(connection, operand2);
      break;
    case 0x04:
      app_log("report records number within range of\n");
      bgm_report_within_range(evt);
      break;
    default:
      bgm_unsupported_operator(connection);
      break;
  }

}
