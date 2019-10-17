idf_component_register(SRCS "AdafruitGFX/Adafruit_GFX.cpp" 
                            "AdafruitGFX/glcdfont.c"
                            
                       SRC_DIRS "./" 
                       
                       INCLUDE_DIRS "include" 
                                    "AdafruitGFX/" 
                                    "AdafruitGFX/Fonts" 
                                    
                       REQUIRES freertos kernel)