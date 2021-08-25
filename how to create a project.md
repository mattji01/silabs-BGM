1. create a soc ecmpty project and rename it to silabs-BGM
3. add iostream usart(vcom) component
4. add log component
4. add simple button as btn0, selected module PC05
5. copy and overwrite gatt_configuration.btconf to projetc-location\config\btconf
6. comment sl_device_init_lfxo() this line in sl_event_handler.c in autogen folder
7. copy and overwrite sl_bluetooth.c to project-location\auto-gen
8. copy all bgm* files to project-location\ 