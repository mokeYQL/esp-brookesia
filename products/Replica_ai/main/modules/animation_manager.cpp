/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "animation_manager.hpp"
#include "esp_mmap_assets.h"
#include "bsp/esp-bsp.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

// Include generated asset headers
#include "assets/animations/mmap_generate_anim_boot.h"
#include "assets/animations/mmap_generate_anim_emotion.h"
#include "assets/animations/mmap_generate_anim_icon.h"

static const char *TAG = "AnimManager";

// Animation manager structure
struct animation_manager_t
{
    // Assets handles for each partition
    mmap_assets_handle_t boot_assets;
    mmap_assets_handle_t emotion_assets;
    mmap_assets_handle_t icon_assets;

    // Animation player
    anim_player_handle_t anim_player;

    // LVGL canvas
    lv_obj_t *canvas;
    uint8_t *canvas_buffer;
    int canvas_width;
    int canvas_height;

    // Current animation info
    int anim_width;
    int anim_height;

    // Playback state
    bool is_playing;
    bool is_cycling;

    // Cycling control
    TaskHandle_t cycle_task;
    EventGroupHandle_t cycle_events;
    animation_category_t cycling_category;
    uint32_t cycle_duration_ms;
    int current_category_index;
};

// Event group bits for cycling control
#define CYCLE_STOP_BIT (1 << 0)
#define CYCLE_RUNNING_BIT (1 << 1)

// Forward declarations
static void anim_flush_callback(anim_player_handle_t handle, int x1, int y1, int x2, int y2, const void *data);
static void anim_update_callback(anim_player_handle_t handle, player_event_t event);
static void cycle_task_func(void *param);

// Animation demo task
void animation_demo_task(void *param)
{
    animation_manager_handle_t anim_mgr = (animation_manager_handle_t)param;

    ESP_LOGI("Main", "Starting animation demo...");

    // Wait for display to be ready
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Demo 1: Play boot animation once
    ESP_LOGI("Main", "=== Demo 1: Boot Animation ===");
    animation_manager_play(anim_mgr, "anim_boot", MMAP_ANIM_BOOT_BOOT_ANIMATION_360_360_AAF, false);
    vTaskDelay(pdMS_TO_TICKS(5000));
    animation_manager_stop(anim_mgr);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Demo 2: Cycle through emotion animations
    ESP_LOGI("Main", "=== Demo 2: Emotion Animation Cycle ===");
    animation_manager_cycle_start(anim_mgr, ANIM_CATEGORY_EMOTION, 3000); // Each emotion plays for 3 seconds

    // Let emotions cycle for 20 seconds
    vTaskDelay(pdMS_TO_TICKS(20000));

    // Stop emotion cycling
    animation_manager_cycle_stop(anim_mgr);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Demo 3: Cycle through icon animations
    ESP_LOGI("Main", "=== Demo 3: Icon Animation Cycle ===");
    animation_manager_cycle_start(anim_mgr, ANIM_CATEGORY_ICON, 2000); // Each icon plays for 2 seconds

    // Let icons cycle for 15 seconds
    vTaskDelay(pdMS_TO_TICKS(15000));

    // Stop icon cycling
    animation_manager_cycle_stop(anim_mgr);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Demo 4: Manual play of specific animations
    ESP_LOGI("Main", "=== Demo 4: Manual Animation Control ===");

    const int emotion_sequence[] = {
        MMAP_ANIM_EMOTION_EMOTION_HAPPY_284_126_AAF,
        MMAP_ANIM_EMOTION_EMOTION_SAD_284_126_AAF,
        MMAP_ANIM_EMOTION_EMOTION_ANGRY_284_126_AAF,
        MMAP_ANIM_EMOTION_EMOTION_SLEEP_284_126_AAF,
    };

    for (int i = 0; i < sizeof(emotion_sequence) / sizeof(emotion_sequence[0]); i++)
    {
        ESP_LOGI("Main", "Playing manual emotion animation %d", i);
        animation_manager_play(anim_mgr, "anim_emotion", emotion_sequence[i], false);
        vTaskDelay(pdMS_TO_TICKS(2500));
        animation_manager_stop(anim_mgr);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Demo 5: Manual play of specific icons
    ESP_LOGI("Main", "=== Demo 5: Manual Icon Animation Control ===");

    const int icon_sequence[] = {
        MMAP_ANIM_ICON_ICON_VOLUME_UP_64_AAF,
        MMAP_ANIM_ICON_ICON_VOLUME_DOWN_64_AAF,
        MMAP_ANIM_ICON_ICON_SERVER_CONNECTED_64_AAF,
    };

    for (int i = 0; i < sizeof(icon_sequence) / sizeof(icon_sequence[0]); i++)
    {
        ESP_LOGI("Main", "Playing manual icon animation %d", i);
        animation_manager_play(anim_mgr, "anim_icon", icon_sequence[i], false);
        vTaskDelay(pdMS_TO_TICKS(2000));
        animation_manager_stop(anim_mgr);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI("Main", "Animation demo completed!");

    // Clean up
    animation_manager_deinit(anim_mgr);

    vTaskDelete(NULL);
}

animation_manager_handle_t animation_manager_init(void)
{
    ESP_LOGD(TAG, "Initializing animation manager");

    // Allocate manager structure
    animation_manager_handle_t manager = (animation_manager_handle_t)heap_caps_calloc(1, sizeof(struct animation_manager_t), MALLOC_CAP_DEFAULT);
    if (!manager)
    {
        ESP_LOGE(TAG, "Failed to allocate manager structure");
        return NULL;
    }

    // Create event group
    manager->cycle_events = xEventGroupCreate();
    if (!manager->cycle_events)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        heap_caps_free(manager);
        return NULL;
    }

    // Initialize boot assets
    const mmap_assets_config_t boot_config = {
        .partition_label = "anim_boot",
        .max_files = MMAP_ANIM_BOOT_FILES,
        .checksum = MMAP_ANIM_BOOT_CHECKSUM,
        .flags = {
            .mmap_enable = true,
            .app_bin_check = true,
            .full_check = true,
        },
    };

    esp_err_t ret = mmap_assets_new(&boot_config, &manager->boot_assets);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize boot assets: %s", esp_err_to_name(ret));
        vEventGroupDelete(manager->cycle_events);
        heap_caps_free(manager);
        return NULL;
    }

    ESP_LOGI(TAG, "Boot assets initialized: %d files", MMAP_ANIM_BOOT_FILES);

    // Initialize emotion assets
    const mmap_assets_config_t emotion_config = {
        .partition_label = "anim_emotion",
        .max_files = MMAP_ANIM_EMOTION_FILES,
        .checksum = MMAP_ANIM_EMOTION_CHECKSUM,
        .flags = {
            .mmap_enable = true,
            .app_bin_check = true,
            .full_check = true,
        },
    };

    ret = mmap_assets_new(&emotion_config, &manager->emotion_assets);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize emotion assets: %s", esp_err_to_name(ret));
        mmap_assets_del(manager->boot_assets);
        vEventGroupDelete(manager->cycle_events);
        heap_caps_free(manager);
        return NULL;
    }

    ESP_LOGI(TAG, "Emotion assets initialized: %d files", MMAP_ANIM_EMOTION_FILES);

    // Initialize icon assets
    const mmap_assets_config_t icon_config = {
        .partition_label = "anim_icon",
        .max_files = MMAP_ANIM_ICON_FILES,
        .checksum = MMAP_ANIM_ICON_CHECKSUM,
        .flags = {
            .mmap_enable = true,
            .app_bin_check = true,
            .full_check = true,
        },
    };

    ret = mmap_assets_new(&icon_config, &manager->icon_assets);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize icon assets: %s", esp_err_to_name(ret));
        mmap_assets_del(manager->emotion_assets);
        mmap_assets_del(manager->boot_assets);
        vEventGroupDelete(manager->cycle_events);
        heap_caps_free(manager);
        return NULL;
    }

    ESP_LOGI(TAG, "Icon assets initialized: %d files", MMAP_ANIM_ICON_FILES);

    // Create LVGL canvas
    manager->canvas_width = BSP_LCD_H_RES;
    manager->canvas_height = BSP_LCD_V_RES;

    bsp_display_lock(0);
    manager->canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(manager->canvas, manager->canvas_width, manager->canvas_height);
    lv_obj_clear_flag(manager->canvas, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(manager->canvas, LV_ALIGN_CENTER, 0, 0);

    // Allocate canvas buffer (using PSRAM for full screen buffer)
    manager->canvas_buffer = (uint8_t *)heap_caps_malloc(
        manager->canvas_width * manager->canvas_height * sizeof(uint16_t),
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!manager->canvas_buffer)
    {
        ESP_LOGE(TAG, "Failed to allocate canvas buffer");
        lv_obj_del(manager->canvas);
        bsp_display_unlock();
        mmap_assets_del(manager->icon_assets);
        mmap_assets_del(manager->emotion_assets);
        mmap_assets_del(manager->boot_assets);
        vEventGroupDelete(manager->cycle_events);
        heap_caps_free(manager);
        return NULL;
    }

    lv_canvas_set_buffer(manager->canvas, manager->canvas_buffer,
                         manager->canvas_width, manager->canvas_height,
                         LV_COLOR_FORMAT_RGB565);
    bsp_display_unlock();

    // Initialize animation player
    anim_player_config_t config = {
        .flush_cb = anim_flush_callback,
        .update_cb = anim_update_callback,
        .user_data = manager,
        .flags = {.swap = false}, // Disable RGB/BGR swap
        .task = ANIM_PLAYER_INIT_CONFIG()};
    config.task.task_stack_caps = MALLOC_CAP_INTERNAL; // Use internal SRAM for task
    config.task.task_affinity = 1;                     // Run on core 1

    manager->anim_player = anim_player_init(&config);
    if (!manager->anim_player)
    {
        ESP_LOGE(TAG, "Failed to initialize animation player");
        heap_caps_free(manager->canvas_buffer);
        bsp_display_lock(0);
        lv_obj_del(manager->canvas);
        bsp_display_unlock();
        mmap_assets_del(manager->icon_assets);
        mmap_assets_del(manager->emotion_assets);
        mmap_assets_del(manager->boot_assets);
        vEventGroupDelete(manager->cycle_events);
        heap_caps_free(manager);
        return NULL;
    }

    manager->is_playing = false;
    manager->is_cycling = false;
    manager->cycle_task = NULL;

    ESP_LOGI(TAG, "Animation manager initialized successfully");
    return manager;
}

void animation_manager_deinit(animation_manager_handle_t manager)
{
    if (!manager)
    {
        return;
    }

    ESP_LOGD(TAG, "Deinitializing animation manager");

    // Stop cycling if running
    animation_manager_cycle_stop(manager);

    // Stop current animation
    if (manager->is_playing)
    {
        animation_manager_stop(manager);
    }

    if (manager->anim_player)
    {
        anim_player_deinit(manager->anim_player);
    }

    if (manager->canvas)
    {
        bsp_display_lock(0);
        lv_obj_del(manager->canvas);
        bsp_display_unlock();
    }

    if (manager->canvas_buffer)
    {
        heap_caps_free(manager->canvas_buffer);
    }

    if (manager->icon_assets)
    {
        mmap_assets_del(manager->icon_assets);
    }

    if (manager->emotion_assets)
    {
        mmap_assets_del(manager->emotion_assets);
    }

    if (manager->boot_assets)
    {
        mmap_assets_del(manager->boot_assets);
    }

    if (manager->cycle_events)
    {
        vEventGroupDelete(manager->cycle_events);
    }

    heap_caps_free(manager);
    ESP_LOGI(TAG, "Animation manager deinitialized");
}

esp_err_t animation_manager_play(animation_manager_handle_t manager,
                                 const char *partition_label,
                                 int anim_index,
                                 bool repeat)
{
    if (!manager || !manager->anim_player)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Playing animation: partition=%s, index=%d", partition_label, anim_index);

    // Stop current animation if playing
    if (manager->is_playing)
    {
        animation_manager_stop(manager);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Get appropriate assets handle
    mmap_assets_handle_t assets_handle = NULL;
    if (strcmp(partition_label, "anim_boot") == 0)
    {
        assets_handle = manager->boot_assets;
    }
    else if (strcmp(partition_label, "anim_emotion") == 0)
    {
        assets_handle = manager->emotion_assets;
    }
    else if (strcmp(partition_label, "anim_icon") == 0)
    {
        assets_handle = manager->icon_assets;
    }
    else
    {
        ESP_LOGE(TAG, "Unknown partition label: %s", partition_label);
        return ESP_ERR_INVALID_ARG;
    }

    if (!assets_handle)
    {
        ESP_LOGE(TAG, "Assets handle is NULL");
        return ESP_ERR_INVALID_STATE;
    }

    // Get animation data
    const void *anim_data = mmap_assets_get_mem(assets_handle, anim_index);
    size_t anim_size = mmap_assets_get_size(assets_handle, anim_index);
    const char *anim_name = mmap_assets_get_name(assets_handle, anim_index);

    if (!anim_data)
    {
        ESP_LOGE(TAG, "Failed to get animation data (index %d)", anim_index);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "Playing animation: %s (size: %d bytes)", anim_name, anim_size);

    // Set animation source
    esp_err_t ret = anim_player_set_src_data(manager->anim_player, anim_data, anim_size);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set animation source: %s", esp_err_to_name(ret));
        return ret;
    }

    // Get animation segment and configure playback
    uint32_t start, end;
    anim_player_get_segment(manager->anim_player, &start, &end);

    ESP_LOGD(TAG, "Animation segment: start=%lu, end=%lu", start, end);

    // Set playback parameters (30 FPS)
    anim_player_set_segment(manager->anim_player, start, end, 30, repeat);

    // Start playback
    anim_player_update(manager->anim_player, PLAYER_ACTION_START);

    manager->is_playing = true;
    ESP_LOGI(TAG, "Animation started");
    return ESP_OK;
}

esp_err_t animation_manager_stop(animation_manager_handle_t manager)
{
    if (!manager || !manager->anim_player)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Stopping animation");

    anim_player_update(manager->anim_player, PLAYER_ACTION_STOP);

    // Clear canvas to black screen
    bsp_display_lock(0);
    memset(manager->canvas_buffer, 0, manager->canvas_width * manager->canvas_height * sizeof(uint16_t));
    lv_obj_invalidate(manager->canvas);
    bsp_display_unlock();

    manager->is_playing = false;
    ESP_LOGI(TAG, "Animation stopped and screen cleared");
    return ESP_OK;
}

esp_err_t animation_manager_clear(animation_manager_handle_t manager)
{
    if (!manager || !manager->canvas)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Clearing screen");

    bsp_display_lock(0);
    memset(manager->canvas_buffer, 0, manager->canvas_width * manager->canvas_height * sizeof(uint16_t));
    lv_obj_invalidate(manager->canvas);
    bsp_display_unlock();

    ESP_LOGI(TAG, "Screen cleared");
    return ESP_OK;
}

bool animation_manager_is_playing(animation_manager_handle_t manager)
{
    return manager ? manager->is_playing : false;
}

bool animation_manager_is_cycling(animation_manager_handle_t manager)
{
    return manager ? manager->is_cycling : false;
}

esp_err_t animation_manager_cycle_start(animation_manager_handle_t manager,
                                        animation_category_t category,
                                        uint32_t duration_ms)
{
    if (!manager)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Starting cycle: category=%d, duration=%lu ms", category, duration_ms);

    // Check if cycling is already running
    if (manager->is_cycling)
    {
        ESP_LOGW(TAG, "Cycling already in progress, stopping first");
        animation_manager_cycle_stop(manager);
    }

    manager->cycling_category = category;
    manager->cycle_duration_ms = duration_ms;
    manager->current_category_index = 0;

    // Start cycle task
    BaseType_t ret = xTaskCreate(
        cycle_task_func,
        "anim_cycle",
        4096,
        manager,
        5,
        &manager->cycle_task);

    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create cycle task");
        return ESP_ERR_NO_MEM;
    }

    manager->is_cycling = true;
    ESP_LOGI(TAG, "Started cycling through category %d, duration: %lu ms",
             category, duration_ms);

    return ESP_OK;
}

esp_err_t animation_manager_cycle_stop(animation_manager_handle_t manager)
{
    if (!manager || !manager->is_cycling)
    {
        return ESP_OK; // Already stopped
    }

    ESP_LOGD(TAG, "Stopping cycle");

    // Signal stop
    xEventGroupSetBits(manager->cycle_events, CYCLE_STOP_BIT);

    // Wait for task to finish
    if (manager->cycle_task)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (eTaskGetState(manager->cycle_task) == eDeleted ||
            eTaskGetState(manager->cycle_task) == eInvalid)
        {
            manager->cycle_task = NULL;
        }
    }

    // Stop current animation
    animation_manager_stop(manager);

    manager->is_cycling = false;
    ESP_LOGI(TAG, "Cycling stopped");
    return ESP_OK;
}

static void anim_flush_callback(anim_player_handle_t handle, int x1, int y1, int x2, int y2, const void *data)
{
    animation_manager_handle_t manager = (animation_manager_handle_t)anim_player_get_user_data(handle);

    if (!manager || !manager->canvas)
    {
        return;
    }

    // Calculate animation size and centered position
    int anim_width = x2 - x1;
    int anim_height = y2 - y1;

    int center_x = (manager->canvas_width - anim_width) / 2;
    int center_y = (manager->canvas_height - anim_height) / 2;

    ESP_LOGD(TAG, "Animation size: %dx%d, Center pos: (%d,%d)",
             anim_width, anim_height, center_x, center_y);

    // Copy decoded frame data to LVGL canvas (centered)
    bsp_display_lock(0);

    // First clear the canvas (black)
    memset(manager->canvas_buffer, 0, manager->canvas_width * manager->canvas_height * sizeof(uint16_t));

    // Calculate the offset in the canvas buffer for centered position
    int stride = manager->canvas_width * sizeof(uint16_t);
    uint8_t *canvas_buf_start = manager->canvas_buffer + center_y * stride + center_x * sizeof(uint16_t);
    uint8_t *data_ptr = (uint8_t *)data;

    // Copy line by line
    for (int y = 0; y < anim_height; y++)
    {
        uint8_t *dest_line = canvas_buf_start + y * stride;
        uint8_t *src_line = data_ptr + y * anim_width * sizeof(uint16_t);
        memcpy(dest_line, src_line, anim_width * sizeof(uint16_t));
    }

    lv_obj_invalidate(manager->canvas);
    bsp_display_unlock();

    // Notify animation player that flush is complete
    anim_player_flush_ready(handle);
}

static void anim_update_callback(anim_player_handle_t handle, player_event_t event)
{
    switch (event)
    {
    case PLAYER_EVENT_IDLE:
        ESP_LOGD(TAG, "Animation idle");
        break;

    case PLAYER_EVENT_ONE_FRAME_DONE:
        // Frame decoded and displayed, next frame will be decoded automatically
        break;

    case PLAYER_EVENT_ALL_FRAME_DONE:
        ESP_LOGD(TAG, "All frames completed");
        {
            animation_manager_handle_t manager = (animation_manager_handle_t)anim_player_get_user_data(handle);
            if (manager && !manager->is_cycling)
            {
                manager->is_playing = false;
            }
        }
        break;

    default:
        ESP_LOGW(TAG, "Unknown event: %d", event);
        break;
    }
}

static void cycle_task_func(void *param)
{
    animation_manager_handle_t manager = (animation_manager_handle_t)param;

    ESP_LOGI(TAG, "Cycle task started");

    // Determine start and end indices based on category
    int start_anim = 0;
    int end_anim = 0;
    mmap_assets_handle_t assets_handle = NULL;

    switch (manager->cycling_category)
    {
    case ANIM_CATEGORY_BOOT:
        start_anim = 0;
        end_anim = MMAP_ANIM_BOOT_FILES - 1;
        assets_handle = manager->boot_assets;
        break;

    case ANIM_CATEGORY_EMOTION:
        start_anim = 0;
        end_anim = MMAP_ANIM_EMOTION_FILES - 1;
        assets_handle = manager->emotion_assets;
        break;

    case ANIM_CATEGORY_ICON:
        start_anim = 0;
        end_anim = MMAP_ANIM_ICON_FILES - 1;
        assets_handle = manager->icon_assets;
        break;

    case ANIM_CATEGORY_ALL:
        // Special case: cycle through all partitions sequentially
        ESP_LOGI(TAG, "Cycling through ALL animations");
        // This is handled in the loop below
        break;

    default:
        ESP_LOGE(TAG, "Invalid category: %d", manager->cycling_category);
        vTaskDelete(NULL);
        return;
    }

    xEventGroupClearBits(manager->cycle_events, CYCLE_STOP_BIT);
    xEventGroupSetBits(manager->cycle_events, CYCLE_RUNNING_BIT);

    if (manager->cycling_category == ANIM_CATEGORY_ALL)
    {
        // Cycle through all partitions: boot -> emotion -> icon -> repeat
        const char *partitions[] = {"anim_boot", "anim_emotion", "anim_icon"};
        const int files_count[] = {MMAP_ANIM_BOOT_FILES, MMAP_ANIM_EMOTION_FILES, MMAP_ANIM_ICON_FILES};
        const int total_partitions = 3;

        int current_partition = 0;
        int current_anim = 0;

        while (1)
        {
            // Check for stop signal
            EventBits_t bits = xEventGroupWaitBits(manager->cycle_events, CYCLE_STOP_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(100));
            if (bits & CYCLE_STOP_BIT)
            {
                ESP_LOGI(TAG, "Cycle task received stop signal");
                break;
            }

            // Get current partition assets handle
            if (current_partition == 0)
            {
                assets_handle = manager->boot_assets;
            }
            else if (current_partition == 1)
            {
                assets_handle = manager->emotion_assets;
            }
            else
            {
                assets_handle = manager->icon_assets;
            }

            // Play current animation
            if (current_anim < files_count[current_partition])
            {
                ESP_LOGI(TAG, "[ALL] Playing partition %d, animation %d", current_partition, current_anim);

                esp_err_t ret = animation_manager_play(manager,
                                                       partitions[current_partition],
                                                       current_anim,
                                                       false);
                if (ret == ESP_OK)
                {
                    // Wait for duration
                    vTaskDelay(pdMS_TO_TICKS(manager->cycle_duration_ms));

                    // Stop animation
                    animation_manager_stop(manager);
                    vTaskDelay(pdMS_TO_TICKS(200));

                    current_anim++;
                }
                else
                {
                    ESP_LOGW(TAG, "Failed to play animation, skipping");
                    current_anim++;
                }

                // Check for stop signal
                bits = xEventGroupWaitBits(manager->cycle_events, CYCLE_STOP_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(10));
                if (bits & CYCLE_STOP_BIT)
                {
                    break;
                }
            }
            else
            {
                // Move to next partition
                current_anim = 0;
                current_partition = (current_partition + 1) % total_partitions;
                ESP_LOGI(TAG, "[ALL] Moving to partition %d", current_partition);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
    }
    else
    {
        // Normal cycling within a single category
        while (1)
        {
            // Check for stop signal
            EventBits_t bits = xEventGroupWaitBits(manager->cycle_events, CYCLE_STOP_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(100));
            if (bits & CYCLE_STOP_BIT)
            {
                ESP_LOGI(TAG, "Cycle task received stop signal");
                break;
            }

            // Play next animation in category
            for (int anim_index = start_anim; anim_index <= end_anim; anim_index++)
            {
                ESP_LOGI(TAG, "Playing animation index %d in category", anim_index);

                esp_err_t ret = animation_manager_play(manager,
                                                       (manager->cycling_category == ANIM_CATEGORY_BOOT) ? "anim_boot" : (manager->cycling_category == ANIM_CATEGORY_EMOTION) ? "anim_emotion"
                                                                                                                                                                              : "anim_icon",
                                                       anim_index,
                                                       false);
                if (ret == ESP_OK)
                {
                    // Wait for duration
                    vTaskDelay(pdMS_TO_TICKS(manager->cycle_duration_ms));

                    // Stop animation
                    animation_manager_stop(manager);
                    vTaskDelay(pdMS_TO_TICKS(200)); // Brief pause between animations

                    // Check for stop signal
                    bits = xEventGroupWaitBits(manager->cycle_events, CYCLE_STOP_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(10));
                    if (bits & CYCLE_STOP_BIT)
                    {
                        break;
                    }
                }
                else
                {
                    ESP_LOGW(TAG, "Failed to play animation, skipping");
                }
            }

            // If stop was signaled during loop
            bits = xEventGroupGetBits(manager->cycle_events);
            if (bits & CYCLE_STOP_BIT)
            {
                break;
            }

            // Small delay before restarting cycle
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    xEventGroupClearBits(manager->cycle_events, CYCLE_RUNNING_BIT);
    ESP_LOGI(TAG, "Cycle task exiting");

    vTaskDelete(NULL);
}
