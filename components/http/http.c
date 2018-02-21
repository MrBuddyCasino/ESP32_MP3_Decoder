#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "errno.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "url_parser.h"
#include "http.h"


#define TAG "http_client"
int icymeta,httpmeta;
char icymeta_text[64];
/**
 * @brief simple http_get
 * see https://github.com/nodejs/http-parser for callback usage
 */
int http_client_get(char *uri, http_parser_settings *callbacks, void *user_data)
{
    url_t *url = url_parse(uri);

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    char port_str[6]; // stack allocated
    snprintf(port_str, 6, "%d", url->port);

    int err = getaddrinfo(url->host, port_str, &hints, &res);
    if(err != ESP_OK || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return err;
    }

    // print resolved IP
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    // allocate socket
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if(sock < 0) {
        ESP_LOGE(TAG, "... Failed to allocate socket.");
        freeaddrinfo(res);
    }
    ESP_LOGI(TAG, "... allocated socket");


    // connect, retrying a few times
    char retries = 0;
    while(connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
        retries++;
        ESP_LOGE(TAG, "... socket connect attempt %d failed, errno=%d", retries, errno);

        if(retries > 5) {
            ESP_LOGE(TAG, "giving up");
            close(sock);
            freeaddrinfo(res);
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "... connected");
    freeaddrinfo(res);

    // write http request
    char *request;
//    if(asprintf(&request, "GET %s HTTP/1.0\r\nHost: %s:%d\r\nUser-Agent: ESP32\r\nAccept: */*\r\n\r\n", url->path, url->host, url->port) < 0)

	    if(asprintf(&request, "%s%s%s%s%s%s%s%s%s", "GET ", url->path, " HTTP/1.0\r\nHost: ", url->host, "\r\n","User-Agent: ESP32\r\n","Range: bytes=0-\r\n","Connection: close\r\n","Icy-MetaData: 1\r\n\r\n") < 0)

    {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "requesting %s", request);

    if (write(sock, request, strlen(request)) < 0) {
        ESP_LOGE(TAG, "... socket send failed");
        close(sock);
    }

    free(request);
    ESP_LOGI(TAG, "... socket send success");


    /* Read HTTP response */
    int i,k=0,k1=0,k2=0;httpmeta=1;icymeta=0;//icy-metaint Parser
    char recv_buf[64];
    bzero(recv_buf, sizeof(recv_buf));
    ssize_t recved;

    /* intercept on_headers_complete() */

    /* parse response */
    http_parser parser;
    http_parser_init(&parser, HTTP_RESPONSE);
    parser.data = user_data;

    esp_err_t nparsed = 0;
    do {
        recved = read(sock, recv_buf, sizeof(recv_buf)-1);

    //icy-metaint Parser dedect icy-metaint: for spiram_fifo.c    
    if(k2<1000) {
       k2++;
         if(k!=0)  { 
            i=k;
            if (recv_buf[k-i] == 0x69&&k==1)  k++;   //i
            if (recv_buf[k-i] == 0x6e&&k==2)  k++;   //n
            if (recv_buf[k-i] == 0x74&&k==3)  k++;   //t
            if (recv_buf[k-i] == 0x3a&&k==4)  k++;    //:
            if (k==5) {k1=recv_buf[k-i]-0x30; k++;} //Zahl
            k1=k1*10000+(recv_buf[k-i]-0x30)*1000;  //Zahl
               if(k<5)   
                  k1=0;
                  k=0;
         };    


         if(k1==0)  {
            for (i=0;i<recved;i++) {
               if (recv_buf[i] == 0x61){k++;if(i>=recved) break;      //a
               if (recv_buf[i+k] == 0x69){k++;if(i+k>=recved) break;  //i
               if (recv_buf[i+k] == 0x6e){k++;if(i+k>=recved) break;  //n
               if (recv_buf[i+k] == 0x74){k++;if(i+k>=recved) break;  //t
               if (recv_buf[i+k] == 0x3a){k++;if(i+k>=recved) break;  //:
               k1=recv_buf[i+k]-0x30;k++;if(i+k>=recved) break;       //Zahl
               k1=k1*10000+(recv_buf[i+k]-0x30)*1000;k=0;
               icymeta=k1; //icy-metaint: found
               printf("icy-metaint: k1=%d\n",k1);
               k2=1000;
               break;}
               }}}} k=0;
            }
         };

       };

        // using http parser causes stack overflow somtimes - disable for now
        nparsed = http_parser_execute(&parser, callbacks, recv_buf, recved);

        // invoke on_body cb directly
        // nparsed = callbacks->on_body(&parser, recv_buf, recved);
    } while(recved > 0 && nparsed >= 0);

    free(url);

    ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d", recved, errno);
    close(sock);
    ESP_LOGI(TAG, "socket closed");

    return 0;
}
