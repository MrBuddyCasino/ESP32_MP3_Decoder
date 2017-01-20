#ifndef _PLAYER_CONFIG_H_
#define _PLAYER_CONFIG_H_


/* If you don't want to enter your wifi credentials in this file, you can set them
   in the shell as environment variables like this:
   export AP_NAME=wifi_name
   export AP_PASS=mypasswd */

#ifndef WIFI_AP_NAME
#define WIFI_AP_NAME "cc-wlan"
#endif

#ifndef WIFI_AP_PASS
#define WIFI_AP_PASS "****"
#endif


/* Define stream URL here. For example, the URL to the MP3 stream of a certain Dutch radio station
is http://icecast.omroep.nl/3fm-sb-mp3 . This translates of a server name of "icecast.omroep.nl"
and a path of "/3fm-sb-mp3". The port usually is 80 (the standard HTTP port) */
#if 0
#define PLAY_SERVER "icecast.omroep.nl"
#define PLAY_PATH "/3fm-sb-mp3"
#define PLAY_PORT 80
#endif
/*
Here's a DI.fm stream
*/
#if 0
#define PLAY_SERVER "pub7.di.fm"
#define PLAY_PATH "/di_classiceurodance"
#define PLAY_PORT 80
#endif

/* You can use something like this to connect to a local mpd server which has a configured
mp3 output: */
#if 0
#define PLAY_SERVER "192.168.33.128"
#define PLAY_PATH "/"
#define PLAY_PORT 8000
#endif

/* You can also play a non-streaming mp3 file that's hosted somewhere. WARNING: If you do this,
make sure to comment out the ADD_DEL_SAMPLES define below, or you'll get too fast a playback
rate! */
#if 0
#define PLAY_SERVER "meuk.spritesserver.nl"
#define PLAY_PATH "/Ii.Romanzeandante.mp3"
#define PLAY_PORT 80
#endif

#if 0
#define PLAY_SERVER "mp3ad.egofm.c.nmdn.net"
#define PLAY_PATH "/ps-egofmsoul_192/livestream.mp3?token=c3RhcnRUaW1lPTIwMTYwNjIzMTkwNjEzJmVuZFRpbWU9MjAxNjA2MjMxOTE2MTMmZGlnZXN0PTY5MmM1ZjRmMWI3NjhlNGY2NTkzMmUyMjQ5MWZkZjBk"
#define PLAY_PORT "80"
#endif

#if 1
#define PLAY_SERVER "ice1.somafm.com"
#define PLAY_PATH "/illstreet-128-mp3"
#define PLAY_PORT "80"
#endif



/*Playing a real-time MP3 stream has the added complication of clock differences: if the sample
clock of the server is a bit faster than our sample clock, it will send out mp3 data faster
than we process it and our buffer will fill up. Conversely, if the server clock is slower, we'll
eat up samples quicker than the server provides them and we end up with an empty buffer.
To fix this, the mp3 logic can insert/delete some samples to modify the speed of playback.
If our buffers are filling up too fast (presumably due to a quick sample clock on the other side)
we will increase our playout speed; if our buffers empty too quickly, we will decrease it a bit.
Unfortunately, adding or deleting samples isn't very good for the audio quality. If you
want better quality, turn this off and/or feel free to implement a better algorithm.
WARNING: Don't use this define if you play non-stream files. It will presume the sample clock
on the server side is waaay too fast and will default to playing back the stream too fast.*/
// #define ADD_DEL_SAMPLES

/*ADD_DEL_SAMPLES parameter:
Size of the cumulative buffer offset before we are going to add or remove a sample
The higher this number, the more aggressive we're adjusting the sample rate. Higher numbers give
better resistance to buffer over/underflows due to clock differences, but also can result in
the music sounding higher/lower due to network issues.*/
#define ADD_DEL_BUFFPERSAMP (1000)

/*ADD_DEL_SAMPLES parameter:
Same as ADD_DEL_BUFFPERSAMP but for systems without a big SPI RAM chip to buffer mp3 data in.*/
#define ADD_DEL_BUFFPERSAMP_NOSPIRAM (1500)


/*While connecting an I2S codec to the I2S port of the ESP is obviously the best way to get nice
16-bit sounds out of the ESP, it is possible to run this code without the codec. For
this to work, instead of outputting a 2x16bit PCM sample the DAC can decode, we use the built-in
8-Bit DAC.*/
// #define USE_DAC


/*While a large (tens to hundreds of K) buffer is necessary for Internet streams, on a
quiet network and with a direct connection to the stream server, you can get away with
a much smaller buffer. Enabling the following switch will disable accesses to the
23LC1024 code and use a much smaller but internal buffer instead. You want to enable
this if you don't have a 23LC1024 chip connected to the ESP but still want to try
the MP3 decoder. Be warned, if your network isn't 100% quiet and latency-free and/or
the server isn't very close to your ESP, this _will_ lead to stutters in the played
MP3 stream! */
#define FAKE_SPI_BUFF

/** prints the amount of unused stack memory for an freertos task in words */
//#define PRINT_TASK_MEMORY
//#define PRINT_HEAP_MEMORY

#endif
