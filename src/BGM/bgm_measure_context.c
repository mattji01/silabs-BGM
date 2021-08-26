/***************************************************************************//**
 * @file
 * @brief BGM measure context characteristic
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

bool measure_context_enabled = false;

/**************************************************************************//**
 * BGM Measurement characteristic CCCD changed.
 *****************************************************************************/
void sl_bt_bgm_measurement_context_handler(sl_bt_msg_t *evt)
{
  // client characteristic configuration changed by remote GATT client
  if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
      if (sl_bt_gatt_server_notification == (sl_bt_gatt_server_client_configuration_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
          if (gatt_disable != evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
              app_log("measure context enable notification\n");
              measure_context_enabled = true;
          }
          else {
              app_log("disable measure context notification\n");
              measure_context_enabled = false;
          }
      }
  }
}

