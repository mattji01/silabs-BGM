/***************************************************************************//**
 * @file
 * @brief 4.14 Service Procedures – General Error Handling
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

/**************************************************************************//**
 * 4.14.1 GLS/SEN/SPE/BI-01-C [Unsupported Op Code]
 *****************************************************************************/
void bgm_unsupported_opcode(uint8_t connection, uint8_t opcode)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x00, 0x02};
  buf[2] = opcode;
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send unsupported opcode handling\n");
  }else{
      app_log("unsupported opcode\n");
  }
}

/**************************************************************************//**
 * 4.14.2 GLS/SEN/SPE/BI-02-C [Invalid Operator]
 *****************************************************************************/

void bgm_invalid_operator(uint8_t connection)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x03};
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send invalid operator error handling\n");
  }else{
      app_log("invalid operator\n");
  }
}

/**************************************************************************//**
 * 4.14.3 GLS/SEN/SPE/BI-03-C [Unsupported Operator]
 *****************************************************************************/
void bgm_unsupported_operator(uint8_t connection)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x04};
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send unsupported operator error handling\n");
  }else{
      app_log("unsupported operator\n");
  }
}

/**************************************************************************//**
 * 4.14.4 GLS/SEN/SPE/BI-04-C [Invalid Operand – Type 1]
 *****************************************************************************/
void bgm_invalid_operand_type1(uint8_t connection)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x05};
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send invalid operand error handling\n");
  }else{
      app_log("invalid operand type1\n");
  }
}

/**************************************************************************//**
 * 4.14.5 GLS/SEN/SPE/BI-05-C [Invalid Operand – Type 2]
 *****************************************************************************/
void bgm_invalid_operand_type2(uint8_t connection)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x05};
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send invalid operand error handling\n");
  }else{
      app_log("invalid operand type2\n");
  }
}

/**************************************************************************//**
 * 4.14.6 GLS/SEN/SPE/BI-06-C [Unsupported Operand]
 *****************************************************************************/
void bgm_unsupported_operand(uint8_t connection)
{
  sl_status_t sc;
  uint8_t buf[4] = {0x06, 0x00, 0x01, 0x09};
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_record_access_control_point,
    sizeof(buf),
    buf);
  if (sc) {
    app_log_warning("Failed to send invalid operand error handling\n");
  }else{
      app_log("unsupported operand\n");
  }
}
