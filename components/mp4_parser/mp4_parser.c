/*
 * mp4.c
 *
 *  Created on: 27.04.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include "esp_log.h"
#include "byteswap.h"
#include "common.h"
#include "mp4_parser.h"

#define TAG "mp4_parser"

_ATOM findMp4Atom(buffer_t *buf, const char *atom, const uint32_t posi,
        const bool loop)
{

    ESP_LOGI(TAG, "%s, line %d.", __func__, __LINE__);
    bool r;
    _ATOM ret;
    _ATOMINFO atomInfo;

    ret.position = posi;
    do {
        r = buf_seek_rel(buf, ret.position);
        // r = fseek(ret.position);

        // fread((uint8_t *) &atomInfo, sizeof(atomInfo));
        buf_read((uint8_t *) &atomInfo, sizeof(atomInfo), 1, buf);

        //ret.size = REV32(atomInfo.size);
        ret.size = __bswap_32(atomInfo.size);
        //ret.size = atomInfo.size;
        if (strncmp(atom, atomInfo.name, 4) == 0) {
            ESP_LOGI(TAG, "%s, line %d, r=%d, ret.size %u, ret.position %u, found %s", __func__, __LINE__, r, ret.size, ret.position, atom);
            return ret;
        }
        ret.position += ret.size;
        ESP_LOGI(TAG, "%s, line %d, r=%d, ret.position %u", __func__, __LINE__, r, ret.position);
    } while (loop && r == 0);

    ret.position = 0;
    ret.size = 0;
    ESP_LOGI(TAG, "%s, line %d.", __func__, __LINE__);
    return ret;
}
