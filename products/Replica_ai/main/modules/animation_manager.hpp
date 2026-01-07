/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "anim_player.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void animation_demo_task(void *param);

    /**
     * @brief Animation manager handle type
     */
    typedef struct animation_manager_t *animation_manager_handle_t;

    /**
     * @brief Animation categories
     */
    typedef enum
    {
        ANIM_CATEGORY_BOOT = 0, // Boot animations
        ANIM_CATEGORY_EMOTION,  // Emotion animations
        ANIM_CATEGORY_ICON,     // Icon animations
        ANIM_CATEGORY_ALL,      // All animations
    } animation_category_t;

    /**
     * @brief Initialize animation manager
     *
     * @return animation_manager_handle_t Manager handle, NULL on error
     */
    animation_manager_handle_t animation_manager_init(void);

    /**
     * @brief Deinitialize animation manager
     *
     * @param handle Manager handle
     */
    void animation_manager_deinit(animation_manager_handle_t handle);

    /**
     * @brief Play animation by partition and index
     *
     * @param handle Manager handle
     * @param partition_label Partition label (e.g., "anim_boot", "anim_emotion", "anim_icon")
     * @param anim_index Animation index within the partition
     * @param repeat Repeat playback
     * @return esp_err_t ESP_OK if successful, otherwise an error code
     */
    esp_err_t animation_manager_play(animation_manager_handle_t handle,
                                     const char *partition_label,
                                     int anim_index,
                                     bool repeat);

    /**
     * @brief Stop current animation
     *
     * @param handle Manager handle
     * @return esp_err_t ESP_OK if successful, otherwise an error code
     */
    esp_err_t animation_manager_stop(animation_manager_handle_t handle);

    /**
     * @brief Clear the screen (black)
     *
     * @param handle Manager handle
     * @return esp_err_t ESP_OK if successful, otherwise an error code
     */
    esp_err_t animation_manager_clear(animation_manager_handle_t handle);

    /**
     * @brief Start cycling through animations
     *
     * @param handle Manager handle
     * @param category Category of animations to cycle
     * @param duration_ms Duration for each animation in milliseconds
     * @return esp_err_t ESP_OK if successful, otherwise an error code
     */
    esp_err_t animation_manager_cycle_start(animation_manager_handle_t handle,
                                            animation_category_t category,
                                            uint32_t duration_ms);

    /**
     * @brief Stop animation cycling
     *
     * @param handle Manager handle
     * @return esp_err_t ESP_OK if successful, otherwise an error code
     */
    esp_err_t animation_manager_cycle_stop(animation_manager_handle_t handle);

    /**
     * @brief Check if animation is playing
     *
     * @param handle Manager handle
     * @return true if playing, false otherwise
     */
    bool animation_manager_is_playing(animation_manager_handle_t handle);

    /**
     * @brief Check if cycling is active
     *
     * @param handle Manager handle
     * @return true if cycling, false otherwise
     */
    bool animation_manager_is_cycling(animation_manager_handle_t handle);

#ifdef __cplusplus
}
#endif
