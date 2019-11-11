idf_component_register(SRC_DIRS "." 
                       INCLUDE_DIRS "include" 
                       REQUIRES  wucy_api freertos  
                       PRIV_REQUIRES window RotaryEncoder bouncer sd_card button)