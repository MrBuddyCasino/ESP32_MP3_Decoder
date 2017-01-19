
/**
  * @brief  Application specified event callback function
  *
  * @param  char *recv_buf : buffer
  * @param  ssize_t bytes_read : bytes read
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
typedef esp_err_t (*stream_reader_cb)(char *recv_buf, ssize_t bytes_read);

esp_err_t http_client_get(char *host, char *port, char *path, stream_reader_cb callback);
