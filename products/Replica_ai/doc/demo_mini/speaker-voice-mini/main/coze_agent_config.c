/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_check.h"
#include "coze_agent_config.h"
#include "coze_agent_config_default.h"

static const char *TAG = "coze_agent_config";

static char *strdup_or_empty(const char *s)
{
    return s ? strdup(s) : strdup("");
}

esp_err_t coze_agent_config_read(coze_agent_config_t *config)
{

    return ESP_OK;
}

esp_err_t coze_agent_config_release(coze_agent_config_t *config)
{
    if (!config)
    {
        return ESP_ERR_INVALID_ARG;
    }

    free(config->public_key);
    free(config->private_key);
    free(config->appid);
    free(config->custom_consumer);

    for (int i = 0; i < config->bot_num; i++)
    {
        free(config->bot[i].bot_id);
        free(config->bot[i].voice_id);
        free(config->bot[i].bot_name);
        free(config->bot[i].bot_description);
    }

    return ESP_OK;
}
