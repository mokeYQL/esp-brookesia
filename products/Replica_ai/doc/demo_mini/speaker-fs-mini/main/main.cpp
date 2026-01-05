/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <algorithm>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"

using namespace esp_brookesia::services;

namespace
{

    constexpr const char *TAG = "FSMini";
    constexpr const char *SPIFFS_BASE_PATH = "/spiffs";
    constexpr const char *TEST_FILE_PATH = "/spiffs/test.txt";
    constexpr const char *TEST_FILE_CONTENT = "Hello, ESP-Brookesia! This is a test file.";

    bool mount_spiffs()
    {
        esp_vfs_spiffs_conf_t conf = {
            .base_path = SPIFFS_BASE_PATH,
            .partition_label = "spiffs_data",
            .max_files = 5,
            .format_if_mount_failed = false,
        };
        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "SPIFFS partition not found");
            return false;
        }
        ESP_ERROR_CHECK(ret);

        size_t total = 0;
        size_t used = 0;
        ESP_ERROR_CHECK(esp_spiffs_info(conf.partition_label, &total, &used));
        ESP_LOGI(TAG, "SPIFFS mounted: total=%d KB, used=%d KB, free=%d KB",
                 (int)(total / 1024), (int)(used / 1024), (int)((total - used) / 1024));
        return true;
    }

    bool list_files(const char *path)
    {
        ESP_LOGI(TAG, "Listing files in %s:", path);

        DIR *dir = opendir(path);
        if (!dir)
        {
            ESP_LOGE(TAG, "Failed to open directory %s", path);
            return false;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            char full_path[300];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            struct stat file_stat;
            if (stat(full_path, &file_stat) == 0)
            {
                if (S_ISDIR(file_stat.st_mode))
                {
                    ESP_LOGI(TAG, "  [DIR]  %s", entry->d_name);
                    list_files(full_path); // Recursively list subdirectories
                }
                else
                {
                    ESP_LOGI(TAG, "  [FILE] %s (size: %ld bytes)", entry->d_name, file_stat.st_size);
                }
            }
        }

        closedir(dir);
        return true;
    }

    bool create_test_file()
    {
        ESP_LOGI(TAG, "Creating test file: %s", TEST_FILE_PATH);

        int fd = open(TEST_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return false;
        }

        ssize_t written = write(fd, TEST_FILE_CONTENT, strlen(TEST_FILE_CONTENT));
        close(fd);

        if (written < 0)
        {
            ESP_LOGE(TAG, "Failed to write to file");
            return false;
        }

        ESP_LOGI(TAG, "Successfully wrote %d bytes to %s", (int)written, TEST_FILE_PATH);
        return true;
    }

    bool read_test_file()
    {
        ESP_LOGI(TAG, "Reading test file: %s", TEST_FILE_PATH);

        int fd = open(TEST_FILE_PATH, O_RDONLY);
        if (fd < 0)
        {
            ESP_LOGE(TAG, "Failed to open file for reading");
            return false;
        }

        char buffer[256] = {0};
        ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);

        if (read_bytes < 0)
        {
            ESP_LOGE(TAG, "Failed to read from file");
            return false;
        }

        ESP_LOGI(TAG, "Successfully read %d bytes: %s", (int)read_bytes, buffer);
        return true;
    }

    bool get_file_info(const char *file_path)
    {
        ESP_LOGI(TAG, "Getting info for file: %s", file_path);

        struct stat file_stat;
        if (stat(file_path, &file_stat) != 0)
        {
            ESP_LOGE(TAG, "Failed to get file info");
            return false;
        }

        ESP_LOGI(TAG, "File info:");
        ESP_LOGI(TAG, "  Size: %ld bytes", file_stat.st_size);
        ESP_LOGI(TAG, "  Mode: 0%o", file_stat.st_mode);
        ESP_LOGI(TAG, "  UID: %d", file_stat.st_uid);
        ESP_LOGI(TAG, "  GID: %d", file_stat.st_gid);
        ESP_LOGI(TAG, "  Inode: %ld", file_stat.st_ino);
        ESP_LOGI(TAG, "  Links: %ld", file_stat.st_nlink);
        ESP_LOGI(TAG, "  Access time: %ld", file_stat.st_atime);
        ESP_LOGI(TAG, "  Modification time: %ld", file_stat.st_mtime);
        ESP_LOGI(TAG, "  Change time: %ld", file_stat.st_ctime);

        return true;
    }

    bool delete_test_file()
    {
        ESP_LOGI(TAG, "Deleting test file: %s", TEST_FILE_PATH);

        if (unlink(TEST_FILE_PATH) != 0)
        {
            ESP_LOGE(TAG, "Failed to delete file");
            return false;
        }

        ESP_LOGI(TAG, "Successfully deleted %s", TEST_FILE_PATH);
        return true;
    }

    void fs_demo_task(void *)
    {
        // Wait a bit to ensure system is fully initialized
        vTaskDelay(pdMS_TO_TICKS(1000));

        // List all files in SPIFFS
        list_files(SPIFFS_BASE_PATH);

        // Create a test file
        create_test_file();

        // List files again to see the new file
        list_files(SPIFFS_BASE_PATH);

        // Read the test file
        read_test_file();

        // Get file info
        get_file_info(TEST_FILE_PATH);

        // Delete the test file
        delete_test_file();

        // List files again to confirm deletion
        list_files(SPIFFS_BASE_PATH);

        ESP_LOGI(TAG, "File system demo completed!");

        // Exit the task
        vTaskDelete(nullptr);
    }

} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker file system mini demo");

    /* Initialize power for peripherals */
    ESP_ERROR_CHECK(bsp_power_init(true));

    /* Initialize StorageNVS service */
    auto &storage_service = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage_service.begin() ? ESP_OK : ESP_FAIL);

    /* Mount SPIFFS file system */
    ESP_ERROR_CHECK(mount_spiffs() ? ESP_OK : ESP_FAIL);

    /* Create file system demo task */
    xTaskCreatePinnedToCore(fs_demo_task, "fs_demo", 4096, nullptr, 4, nullptr, tskNO_AFFINITY);

    /* Main loop */
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
