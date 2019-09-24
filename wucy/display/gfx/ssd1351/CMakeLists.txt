idf_component_register(SRCS "user_hal.c" "hal.c" "ssd1351.c"
                       INCLUDE_DIRS "include"
                       REQUIRES freertos driver)