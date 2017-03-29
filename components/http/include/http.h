
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

int http_client_get(char *host, uint16_t port, char *path, stream_reader_cb callback, void *user_data);
