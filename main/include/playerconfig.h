#ifndef _PLAYER_CONFIG_H_
#define _PLAYER_CONFIG_H_


/* MP3 stream */
#if 1
#define PLAY_URL "http://ice1.somafm.com/illstreet-128-mp3"
#endif

/* AAC LC stream */
#if 0
#define PLAY_URL "http://ice1.somafm.com/illstreet-128-aac"
#endif

/* AAC+ stream */
#if 0
#define PLAY_URL "http://81.201.157.218:80/a1stg/livestream1.aac"
#endif

/* AAC in a MP4 container */
#if 0
#define PLAY_URL "http://yourhost/some_file.m4a"
#endif

#if 0
#define PLAY_URL "http://mp3ad.egofm.c.nmdn.net/ps-egofmraw_192/livestream.mp3"
#endif


// defined via 'make menuconfig'
#define WIFI_AP_NAME CONFIG_WIFI_SSID
#define WIFI_AP_PASS CONFIG_WIFI_PASSWORD


// defined via 'make menuconfig'
#define AUDIO_OUTPUT_MODE CONFIG_AUDIO_OUTPUT_MODE


#endif
