
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


#endif
