
#ifndef _HTTP_H_
#define _HTTP_H_

#include "http_parser.h"

/**
  * @brief  Application specified event callback function
  *
  * @param  char *recv_buf : buffer
  * @param  ssize_t bytes_read : bytes read
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
typedef esp_err_t (*stream_reader_cb)(char *recv_buf, ssize_t bytes_read, void *user_data);

int http_client_get(char *uri, http_parser_settings *callbacks, void *user_data);


// icy-metaint-parser

#define min(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a < _b ? _a : _b;                                                     \
    })

#define ICY_META_BUFF_LEN 128

extern char icymeta_text[ICY_META_BUFF_LEN];

extern bool newHttpRequest;
extern int icymeta_interval;

#endif
