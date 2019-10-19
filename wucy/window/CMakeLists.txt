idf_component_register(SRCS "AdafruitGFX/Adafruit_GFX.cpp" 
                            "AdafruitGFX/glcdfont.c"
                            "window.cpp"
                            
                       SRC_DIRS "./" 
                       
                       INCLUDE_DIRS "include" 
                                    "AdafruitGFX/" 
                                    "AdafruitGFX/Fonts" 
                                    
                       REQUIRES freertos)