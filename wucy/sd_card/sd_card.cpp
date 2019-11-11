///*
// * sd_card.cpp
// *
// *  Created on: 2019-11-09
// *      Author: Lukas
// */
///*
//	Software for project WUCY - wearable open source personal computer.
//	based on ESP32 running FreeRTOS on custom Therram kernel.
//	<https://github.com/therram/wucy>
//
//	License: GPL 3.0
//
//	Copyright (C) 2019 Lukas Neverauskis
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.
//*/
//
//#include "sd_card.hpp"
//
//
///* SD card and FAT filesystem example.
//   This example code is in the Public Domain (or CC0 licensed, at your option.)
//
//   Unless required by applicable law or agreed to in writing, this
//   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//   CONDITIONS OF ANY KIND, either express or implied.
//*/
//
//#include <stdio.h>
//#include <string.h>
//#include <sys/unistd.h>
//#include <sys/stat.h>
//#include "esp_err.h"
//#include "esp_log.h"
//#include "esp_vfs_fat.h"
//#include "driver/sdmmc_host.h"
//#include "driver/sdspi_host.h"
//#include "sdmmc_cmd.h"
//
//static const char *TAG = "example";
//
//// This example can use SDMMC and SPI peripherals to communicate with SD card.
//// By default, SDMMC peripheral is used.
//// To enable SPI mode, uncomment the following line:
//
//// #define USE_SPI_MODE
//
//// When testing SD and SPI modes, keep in mind that once the card has been
//// initialized in SPI mode, it can not be reinitialized in SD mode without
//// toggling power to the card.
//
//#ifdef USE_SPI_MODE
//// Pin mapping when using SPI mode.
//// With this mapping, SD card can be used both in SPI and 1-line SD mode.
//// Note that a pull-up on CS line is required in SD mode.
//#define PIN_NUM_MISO 2
//#define PIN_NUM_MOSI 15
//#define PIN_NUM_CLK  14
//#define PIN_NUM_CS   13
//#endif //USE_SPI_MODE
//
//void sd_test(void)
//{
//    ESP_LOGI(TAG, "Initializing SD card");
//
//#ifndef USE_SPI_MODE
//    ESP_LOGI(TAG, "Using SDMMC peripheral");
//    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
//
//    // This initializes the slot without card detect (CD) and write protect (WP) signals.
//    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
//    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
//
//    // To use 1-line SD mode, uncomment the following line:
//    // slot_config.width = 1;
//
//    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
//    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
//    // does make a difference some boards, so we do that here.
//    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
//    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
//    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
//    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
//    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes
//
//#else
//    ESP_LOGI(TAG, "Using SPI peripheral");
//
//    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
//    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
//    slot_config.gpio_miso = PIN_NUM_MISO;
//    slot_config.gpio_mosi = PIN_NUM_MOSI;
//    slot_config.gpio_sck  = PIN_NUM_CLK;
//    slot_config.gpio_cs   = PIN_NUM_CS;
//    // This initializes the slot without card detect (CD) and write protect (WP) signals.
//    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
//#endif //USE_SPI_MODE
//
//    // Options for mounting the filesystem.
//    // If format_if_mount_failed is set to true, SD card will be partitioned and
//    // formatted in case when mounting fails.
//    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
//        .format_if_mount_failed = false,
//        .max_files = 5,
//        .allocation_unit_size = 16 * 1024
//    };
//
//    // Use settings defined above to initialize SD card and mount FAT filesystem.
//    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
//    // Please check its source code and implement error recovery when developing
//    // production applications.
//    sdmmc_card_t* card;
//    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
//
//    if (ret != ESP_OK) {
//        if (ret == ESP_FAIL) {
//            ESP_LOGE(TAG, "Failed to mount filesystem. "
//                "If you want the card to be formatted, set format_if_mount_failed = true.");
//        } else {
//            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
//                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
//        }
//        return;
//    }
//
//    // Card has been initialized, print its properties
//    sdmmc_card_print_info(stdout, card);
//
//    // Use POSIX and C standard library functions to work with files.
//    // First create a file.
//    ESP_LOGI(TAG, "Opening file");
//    FILE* f = fopen("/sdcard/hello.txt", "w");
//    if (f == NULL) {
//        ESP_LOGE(TAG, "Failed to open file for writing");
//        return;
//    }
//    fprintf(f, "Hello %s!\n", card->cid.name);
//    fclose(f);
//    ESP_LOGI(TAG, "File written");
//
//    // Check if destination file exists before renaming
//    struct stat st;
//    if (stat("/sdcard/foo.txt", &st) == 0) {
//        // Delete it if it exists
//        unlink("/sdcard/foo.txt");
//    }
//
//    // Rename original file
//    ESP_LOGI(TAG, "Renaming file");
//    if (rename("/sdcard/hello.txt", "/sdcard/foo.txt") != 0) {
//        ESP_LOGE(TAG, "Rename failed");
//        return;
//    }
//
//    // Open renamed file for reading
//    ESP_LOGI(TAG, "Reading file");
//    f = fopen("/sdcard/foo.txt", "r");
//    if (f == NULL) {
//        ESP_LOGE(TAG, "Failed to open file for reading");
//        return;
//    }
//    char line[64];
//    fgets(line, sizeof(line), f);
//    fclose(f);
//    // strip newline
//    char* pos = strchr(line, '\n');
//    if (pos) {
//        *pos = '\0';
//    }
//    ESP_LOGI(TAG, "Read from file: '%s'", line);
//
//    // All done, unmount partition and disable SDMMC or SPI peripheral
//    esp_vfs_fat_sdmmc_unmount();
//    ESP_LOGI(TAG, "Card unmounted");
//}
//
//
//
