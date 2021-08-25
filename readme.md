# Health Care - Blood Glucose Meters  #

## Overview ##

This project shows an example of **Blood Glucose Meters** using the **Silicon Labs demo board.**

The demo board captured ADC data and use Bluetooth Glucose Profile to transmit all the data.

## Simplicity Studio and Gecko SDK Suite version ##

Simplicity Studio SV5.2.1.1 and GSDK v3.2.1

## Hardware Required ##

- EFR32BG22C112F352GM32 Silicon Labs demo board

## Software Required

LightBlue app/EFR connect app


## How It Works ##

### setup

1. get a silicon labs demo board, connect it with WSTK
2. git clone https://github.com/mattji01/silabs-BGM
3. flash bluetooth_bgm_iadc.s37
4. if the phone bonded with the device before, 
  please remove the device from the bluetooth setting in the phone

### Test

open EFR connect app, filter silabs-BGM, connect it.

there are two major service, Glucose and the IADC service, the IADC service UUID is fd5e3ed9-58c1-4064-bda9-02aa5116a0ef

<img src=".\files\service.jpg" style="zoom:67%;" />

##### Get IADC data

tap More Info under the Unknown Service(IADC service), there are 7 characteristics in this service

1. internal ADC result(mV), UUID 4b41881a-cb05-4112-a913-42b34dac6204

   ![](\files\interval_adc.jpg)

2. external ADC result(mV), UUID 85577c9f-c9fe-4821-b0e6-bbc731890301

   ![](\files\external_adc.jpg)

3. DAC adjust, set DAC output voltage level, in range of 0-1.25v, UUID edfd1260-c69d-4dec-8996-3a898e9f8b84, write 0.625v to DAC

   <img src="\files\set_dac.jpg" style="zoom: 67%;" />

4. Die Temperature, UUID b97a8a77-f1fd-4a13-a492-05360de078cd, read chipset die temperature

   ![](\files\temperature.jpg)

5. offset of the internal ADC(mV), UUID 7489487f-3519-455e-8e1b-1e9dc56c3ba9, 

   ![](\files\offset.jpg)

6. Gain error of the internal ADC, UUID 4a9e0629-48a0-4a89-9314-6f6c70879c6d

   ![](\files\gain_error.jpg)

##### Get Glucose data

1. the device have 3 records inside by default, 

2. open lightblue app, find internal ADC result characteristic(UUID 4b41881a-cb05-4112-a913-42b34dac6204) , tap read 2 times to simulate generating 2 BGM measurement records,

3. in Glucose service, set Notify of 0x2A18 characteristic(Glucose Measurement characteristic), 

   set Notify of 0x2A34 characteristic(Glucose Measurement Context characteristic), 

   set indicate of 0x2A52 characteristic(Record Access Control Point characteristic),

![](\files\set_notify.jpg)

4. write "0101" to 0x2A52, it means read all records, you can find this test case in Glucose.service.TS.1.0.9 section 4.9.1: GLS/SEN/SPR/BV-01-C [Report Stored Records - All records]), 

![](\files\read_all_records.jpg)

5. there will be 5 notifications in 0x2A18, it means 5 Glucose Measurement records

<img src="\files\measure.jpg" style="zoom:67%;" />

6. there will be 5 notifications in 0x2A34, it means 5 Glucose Measurement Context values

   <img src="\files\context.jpg" style="zoom:67%;" />

7. and finally 1 indication in 0x2A52 to show the process is completed.

<img src="\files\racp.jpg" style="zoom:67%;" />

# Create an example application #

1. create a soc ecmpty project and rename it to silabs-BGM
2. add iostream usart(vcom) component
3. add log component
4. add simple button as btn0, selected module PC05
5. copy and overwrite gatt_configuration.btconf to projetc-location\config\btconf
6. comment sl_device_init_lfxo() this line in sl_event_handler.c in autogen folder
7. copy and overwrite sl_bluetooth.c to project-location\auto-gen
8. copy all bgm* files to project-location\ 

## .sls Projects Used ##

silabs-BGM.sls - This is the project. 

Also precompiled binaries in S-Record format (.s37) are included for the projects above test the applications instantly. The files can be programmed using for example _Simplicity Studio Flash Programmer_ tool or _Simplicity Commander_ application. Remember to flash also the bootloader at least once.

# More information #

## PTS test ##

You can download BGM spec and test case in https://www.bluetooth.com/specifications/specs/, or in specifications folder in this repository.

## Pending test case ##

GLS/SEN/SPT/BV-01-C [Service Procedure - Time Update]

