/***************************************************************************//**
 * @file
 * @brief BGM measurement characteristic
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

#include "sl_simple_button_instances.h"
#include "bgm.h"

// Glucose measurement characteristic notification enable
bool measure_enabled = false;
static bool bgm_in_process = false;
// Glucose measurement data
#if SL_BT_BGM_STATUS_ANNUNCIATION_SUPPORTED
uint8_t sl_glucose_records[MAX_RECORD_NUM][17] = {0};
#else
uint8_t sl_glucose_records[MAX_RECORD_NUM][15] = {0};
#endif

/**************************************************************************//**
 * add a glucose measurement record
 *****************************************************************************/
void bgm_generate_record(uint16_t index)
{
  seq_num ++;

  uint8_t temp[15] = {0};
 //flag 0000 0111
  //bit0: timeoffset
  //bit1: Glucose Concentration Field and Type-Sample Location Field
  //bit2: the unit is in base units of mol/L (typically displayed in units of mmol/L)
  //and bit 2 of the Flags field is set to 1.
  //bit3:Sensor Status Annunciation Field
  //bit4: context
 temp[0] = 0x07;
  //kg/L
  //temp[0] = 0x03;
#if SL_BT_BGM_STATUS_ANNUNCIATION_SUPPORTED
 temp[0] = temp[0] | SL_BT_BGM_SENSOR_STATUS_ANNUNCIATION;
#endif
// temp[0] = temp[0] | SL_BT_BGM_MEASURE_CONTEXT;
 //seq num
 temp[1] = seq_num, temp[2] = 0;
 //time
 temp[3] = 0xE5, temp[4] = 7, temp[5] = 7, temp[6] = 5,temp[7] = 12, temp[8] = 30, temp[9] = 5;
 //time offset
 temp[10] = 0, temp[11] = 0;
 //measure
 temp[12] = 50 + seq_num; temp[13] = 192;
 //kg/L
 //temp[12] = 60 + seq_num; temp[13] = 176;
 //location + type
 temp[14] = 0x11;
 //for(uint8_t i = 0; i<15; i++)
 //  app_log("0x%02x ", temp[i]);
 //app_log("\n");
 //Annunciation uint16_t
 memcpy(sl_glucose_records[index],temp,sizeof(temp));
#if SL_BT_BGM_STATUS_ANNUNCIATION_SUPPORTED
#endif
 records_num ++;
}


/**@brief Glucose measurement context flags */
#define SL_BT_BGM_CONTEXT_CARB         0x01
#define SL_BT_BGM_CONTEXT_MEAL         0x02
#define SL_BT_BGM_CONTEXT_TESTER       0x04
#define SL_BT_BGM_CONTEXT_EXERCISE     0x08
#define SL_BT_BGM_CONTEXT_MED          0x10
#define SL_BT_BGM_CONTEXT_MED_KG       0x00  // used with SL_BT_BGM_CONTEXT_MED
#define SL_BT_BGM_CONTEXT_MED_L        0x20  // used with SL_BT_BGM_CONTEXT_MED
#define SL_BT_BGM_CONTEXT_HBA1C        0x40

/**************************************************************************//**
 * 4.9.1 GLS/SEN/SPR/BV-01-C [Report Stored Records - All records]
 *****************************************************************************/
void bgm_report_all_records(uint8_t connection)
{
  if(  bgm_in_process == false){
      bgm_in_process = true;
  }else{
      app_log("procedure already in process.\n");
      bgm_send_racp_indication(connection, 0x01, 0x80);
      return;
  }
  bgm_abort_operation_flag = false;
  sl_status_t sc = SL_STATUS_FAIL;
  uint8_t context[11] = {0};


  if(measure_enabled == true && records_num > 0){
      for(uint16_t i = 0; i < records_num; i++){
          if(bgm_abort_operation_flag == true){
              bgm_abort_operation_flag = false;
              app_log("set bgm_abort_operation_flag back to false\n");
              bgm_in_process = false;
              return;
          }
/*
        context[0] = context[0] | SL_BT_BGM_CONTEXT_CARB;
        context[1] = sl_glucose_records[i][1];  // seq num
        context[2] = sl_glucose_records[i][2];
        context[3] = 1;  //carb ID
        context[4] = 50; //carb value float
        context[5] = 208;

          context[0] = context[0] | SL_BT_BGM_CONTEXT_MEAL;
          context[1] = sl_glucose_records[i][1];
          context[2] = sl_glucose_records[i][2];
          context[3] = 1; //meal breakfast

          context[0] = context[0] | SL_BT_BGM_CONTEXT_TESTER;
          context[1] = sl_glucose_records[i][1];
          context[2] = sl_glucose_records[i][2];
          context[3] = 3; //lab test
          */
          context[0] = context[0] | SL_BT_BGM_CONTEXT_EXERCISE;
          context[0] = context[0] | SL_BT_BGM_CONTEXT_MED;
          context[0] = context[0] | SL_BT_BGM_CONTEXT_MED_L;
          context[0] = context[0] | SL_BT_BGM_CONTEXT_HBA1C;

          context[1] = sl_glucose_records[i][1];
          context[2] = sl_glucose_records[i][2];

          context[3] = 1;  //exercise duration
          context[4] = 0;
          context[6] = 5; //exercise intensity

          context[7] = 50; // 50 milligrams or 50 milliliters
          // context[8] = 160; // SFLOAT -6
           context[8] = 208; // SFLOAT -3

           context[9] = 50; // HbA1c 50 percent
           context[10] = 0; //sfloat 0

        sc = sl_bt_gatt_server_send_notification(
          connection,
          gattdb_glucose_measurement,
          sizeof(sl_glucose_records[0]),
          sl_glucose_records[i]
          );
        sc = sl_bt_gatt_server_send_notification(
                  connection,
                  gattdb_glucose_measurement_context,
                  sizeof(context),
                  context
                  );
        if(sc){
          app_log_warning("Failed to send all records 0x%04X\n", sc);
          bgm_in_process = false;
          return;
        }

      }
  }
  bgm_send_racp_indication(connection, 0x01, 0x01);
  bgm_in_process = false;
}

/**************************************************************************//**
 * 4.9.2 GLS/SEN/SPR/BV-02-C [Report Stored Records - Less than or equal to Sequence Number]
 *****************************************************************************/
void bgm_report_less_record(uint8_t connection, uint16_t high)
{
  sl_status_t sc = SL_STATUS_FAIL;
  if(high > records_num){
      high = records_num;
  }
  if(records_num > 0){
      for(uint8_t i = 0; i < high; i++){
        sc = sl_bt_gatt_server_send_notification(
          connection,
          gattdb_glucose_measurement,
          sizeof(sl_glucose_records[0]),
          sl_glucose_records[i]
          );
        app_log("send one record, high is %d, connect is %d\n", high, connection);
      }
  }
  if (sc) {
    app_log_warning("Failed to send less than records 0x%04X\n", sc);
  }else{
      bgm_send_racp_indication(connection, 0x01, 0x01);
  }
}

/**************************************************************************//**
 * 4.9.3 GLS/SEN/SPR/BV-03-C [Report Stored Records - Greater than or equal to Sequence Number]
 *****************************************************************************/
void bgm_report_greater(uint8_t connection, uint8_t filter_type, uint16_t low)
{
  sl_status_t sc = SL_STATUS_FAIL;
  // sequence number filter type
  if(filter_type == 0x01){
    if(records_num > 0){
        for(uint16_t i = low - 1 ; i < records_num; i++){
          sc = sl_bt_gatt_server_send_notification(
            connection,
            gattdb_glucose_measurement,
            sizeof(sl_glucose_records[0]),
            sl_glucose_records[i]
            );
        }
    }
  }
  // user facing time filter
  else if(filter_type == 0x02){
      //bgm_unsupported_operand(connection);
      if(records_num > 0){
          for(uint16_t i = 0 ; i < 2; i++){
            sc = sl_bt_gatt_server_send_notification(
              connection,
              gattdb_glucose_measurement,
              sizeof(sl_glucose_records[0]),
              sl_glucose_records[i]
              );
          }
      }
  }
  // unsupported operand
  else{
      bgm_unsupported_operand(connection);
      return;
  }

  if (sc) {
    app_log_warning("Failed to report greater than records\n");
  }else{
      bgm_send_racp_indication(connection, 0x01, 0x01);
  }
}


/**************************************************************************//**
 * 4.9.4 GLS/SEN/SPR/BV-04-C [Report Stored Records - Greater than or equal to User Facing Time]
 *****************************************************************************/




/**************************************************************************//**
 * 4.9.5 GLS/SEN/SPR/BV-05-C [Report Stored Records - Within range of Sequence Number value pair]
 *****************************************************************************/
void bgm_report_within(uint8_t connection, uint8_t filter_type, uint16_t low, uint16_t high)
{
  (void)filter_type;
  sl_status_t sc = SL_STATUS_FAIL;
  if(records_num == 0){
      return;
  }
  for(uint16_t i = low; i <= high; i++){
    sc = sl_bt_gatt_server_send_notification(
      connection,
      gattdb_glucose_measurement,
      sizeof(sl_glucose_records[0]),
      sl_glucose_records[i]
      );
  }

  if (sc) {
    app_log_warning("Failed to send within records\n");
  }else{
      bgm_send_racp_indication(connection, 0x01, 0x01);
  }
}

/**************************************************************************//**
 * 4.9.6 GLS/SEN/SPR/BV-06-C [Report Stored Records – First record]
 *****************************************************************************/
void bgm_report_first_record(uint8_t connection)
{
  sl_status_t sc = SL_STATUS_FAIL;
  if(measure_enabled == true && records_num > 0){
    sc = sl_bt_gatt_server_send_notification(
      connection,
      gattdb_glucose_measurement,
      sizeof(sl_glucose_records[0]),
      sl_glucose_records[0]
      );
  }
  if (sc) {
    app_log_warning("Failed to send first record.\n");
  }else{
      bgm_send_racp_indication(connection, 0x01, 0x01);
  }
}

/**************************************************************************//**
 * 4.9.7 GLS/SEN/SPR/BV-07-C [Report Stored Records – Last record]
 *****************************************************************************/
void bgm_report_last_record(uint8_t connection)
{
  sl_status_t sc = SL_STATUS_FAIL;
  if(measure_enabled == true && records_num > 0){
    sc = sl_bt_gatt_server_send_notification(
      connection,
      gattdb_glucose_measurement,
      sizeof(sl_glucose_records[0]),
      sl_glucose_records[records_num-1]
      );
  }
  if (sc) {
    app_log_warning("Failed to send last record\n");
  }else{
      bgm_send_racp_indication(connection, 0x01, 0x01);
  }
}

/**************************************************************************//**
 * BGM - BGM Measurement
 * notification changed callback
 *
 * Called when notification of BGM measurement is enabled/disabled by
 * the client.
 *****************************************************************************/

void sl_bt_bgm_measurement_notification_handler(sl_bt_msg_t *evt)
{
    // client characteristic configuration changed by remote GATT client
    if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
        if (sl_bt_gatt_server_notification == (sl_bt_gatt_server_client_configuration_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
            app_log("measurement enable notification\n");
            measure_enabled = true;
        }  // notification disabled.
        else if(sl_bt_gatt_server_disable == (sl_bt_gatt_server_client_configuration_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
            app_log("measurement disable notification\n");
            measure_enabled = false;
        }
    }
}

void bgm_add_measurement_record(void)
{
  app_log("add one measurement record\n");
  if(records_num == MAX_RECORD_NUM){
    app_log("reach most record num %d, need to delete all\n", MAX_RECORD_NUM);
  }
  else{
      bgm_generate_record(records_num);
  }
}

void bgm_add_200_measurement_records(void)
{
  for(uint16_t i = records_num; i < MAX_RECORD_NUM; i++){
      app_log("add one record in loop\n");
      bgm_generate_record(i);
  }
}


static volatile bool app_btn0_pressed = false;

/**************************************************************************//**
 * Simple Button
 * Button state changed callback
 * @param[in] handle Button event handle
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  app_log("button pressed\n");
  static uint32_t timestamp = 0;
  uint32_t t_diff;
  // Button pressed.
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      app_btn0_pressed = true;
      timestamp = sl_sleeptimer_get_tick_count();
    }
  }
  // Button released.
  else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED &&  (&sl_button_btn0 == handle)) {
    t_diff = sl_sleeptimer_get_tick_count() - timestamp;
    if(t_diff < sl_sleeptimer_ms_to_tick(SHORT_BUTTON_PRESS_DURATION)){
      app_btn0_pressed = false;
      app_log("add one record\n");
      bgm_add_measurement_record();
    }else if (t_diff < sl_sleeptimer_ms_to_tick(MEDIUM_BUTTON_PRESS_DURATION)) {
        app_log("add %d records\n", MAX_RECORD_NUM);
      bgm_add_200_measurement_records();
    }
  }
}
