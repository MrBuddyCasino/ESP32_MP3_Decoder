/*
 * url_parser.c
 *
 * This is a wrapper around http_parser_url to simplify usage.
 *
 *  Created on: 29.03.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"

#include "http_parser.h"
#include "url_parser.h"

#define TAG "url_parser"


static char* url_get_scheme(struct http_parser_url *url, char *uri)
{
    return strndup(&uri[url->field_data[UF_SCHEMA].off], url->field_data[UF_SCHEMA].len);
}

static char* url_get_host(struct http_parser_url *url, char *uri)
{
    return strndup(&uri[url->field_data[UF_HOST].off], url->field_data[UF_HOST].len);
}

static uint16_t url_get_port(struct http_parser_url *url)
{
    uint16_t port;

    if (url->field_set & (1 << UF_PORT)) {
        port = url->port;
    } else {
        // assume: 4 = http, 5 = https
        port = (url->field_data[UF_SCHEMA].len == 5) ? 443 : 80;
    }

    return port;
}

static char* url_get_authority(struct http_parser_url *url, char *uri)
{
    char *authority;

    /* MAX 5 digits (max 65535) + 1 ':' + 1 NULL (because of snprintf) */
    size_t extra = 7;

    uint16_t authoritylen = url->field_data[UF_HOST].len;
    authority = calloc(authoritylen + extra, sizeof(char));
    memcpy(authority,
            &uri[url->field_data[UF_HOST].off],
            url->field_data[UF_HOST].len);
    /* maybe add port */
    if (url->field_set & (1 << UF_PORT)) {
        authoritylen += (size_t) snprintf(authority + authoritylen,
                extra, ":%u", url->port);
    }
    authority[authoritylen] = '\0';

    return authority;
}

static char* url_get_path(struct http_parser_url *url, char *uri)
{
    /* path */
    char *path;

    /* If we don't have path in URI, we use "/" as path. */
    uint16_t pathlen = 1;
    if (url->field_set & (1 << UF_PATH)) {
        pathlen = url->field_data[UF_PATH].len;
    }
    if (url->field_set & (1 << UF_QUERY)) {
        /* +1 for '?' character */
        pathlen += (size_t) (url->field_data[UF_QUERY].len + 1);
    }

    /* +1 for \0 */
    path = malloc(pathlen + 1);
    if (url->field_set & (1 << UF_PATH)) {
        memcpy(path, &uri[url->field_data[UF_PATH].off],
                url->field_data[UF_PATH].len);
    } else {
        path[0] = '/';
    }

    if (url->field_set & (1 << UF_QUERY)) {
        path[pathlen - url->field_data[UF_QUERY].len - 1] = '?';
        memcpy(
                path + pathlen - url->field_data[UF_QUERY].len,
                &uri[url->field_data[UF_QUERY].off], url->field_data[UF_QUERY].len);
    }
    path[pathlen] = '\0';

    return path;
}


url_t *url_parse(char *uri)
{
    struct http_parser_url *url_parser = calloc(1, sizeof(struct http_parser_url));
    if(url_parser == NULL) {
        ESP_LOGE(TAG, "could not allocate http_parser_url");
        return NULL;
    }

    int ret = http_parser_parse_url(uri, strlen(uri), 0, url_parser);
    if (ret != 0) {
        ESP_LOGE(TAG, "Could not parse URI %s", uri);
        return NULL;
    }

    url_t *url = calloc(1, sizeof(url_t));
    if(url_parser == NULL) {
        ESP_LOGE(TAG, "could not allocate url_t");
        return NULL;
    }

    url->scheme = url_get_scheme(url_parser, uri);
    url->host = url_get_host(url_parser, uri);
    url->port = url_get_port(url_parser);
    url->authority = url_get_authority(url_parser, uri);
    url->path = url_get_path(url_parser, uri);

    free(url_parser);

    return url;
}

void url_free(url_t *url)
{
    if(url->scheme != NULL) free(url->scheme);
    if(url->host != NULL) free(url->host);
    if(url->authority != NULL) free(url->authority);
    if(url->path != NULL) free(url->path);

    free(url);
}
