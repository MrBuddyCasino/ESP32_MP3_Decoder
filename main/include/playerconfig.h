#ifndef _PLAYER_CONFIG_H_
#define _PLAYER_CONFIG_H_


// defined via 'make menuconfig'
#define WIFI_AP_NAME CONFIG_WIFI_SSID
#define WIFI_AP_PASS CONFIG_WIFI_PASSWORD


/* Define stream URL here.*/
#if 1
#define PLAY_URL "http://ice1.somafm.com/illstreet-128-mp3"
#endif

/* You can use something like this to connect to a local mpd server which has a configured
mp3 output: */
#if 0
#define PLAY_URL "http://192.168.33.128:8000/"
#endif

/* You can also play a non-streaming mp3 file that's hosted somewhere. WARNING: If you do this,
make sure to comment out the ADD_DEL_SAMPLES define below, or you'll get too fast a playback
rate! */
#if 0
#define PLAY_URL "http://meuk.spritesserver.nl/Ii.Romanzeandante.mp3"
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


/* Connecting an I2S codec to the ESP is the best way to get nice
16-bit sounds out of the ESP, but it is possible to run this code without the codec. For
this to work, instead of outputting a 2x16bit PCM sample the DAC can decode, we use the built-in
8-Bit DAC.*/
#define OUTPUT_MODE I2S // possible values: I2S, DAC_BUILT_IN

/* there is currently a bug in the SDK when using the DAC - this is a temporary
   workaround until the issue is fixed */
// #define DAC_BUG_WORKAROUND


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
