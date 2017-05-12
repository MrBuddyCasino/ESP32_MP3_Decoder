/* Created 19 Nov 2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * Uses the RMT peripheral on the ESP32 for very accurate timing of
 * signals sent to the WS2812 LEDs.
 *
 * This code is placed in the public domain (or CC0 licensed, at your option).
 */

#include "ws2812.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <soc/rmt_struct.h>
#include <soc/dport_reg.h>
#include <driver/gpio.h>
#include <soc/gpio_sig_map.h>
#include <esp_intr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ETS_RMT_CTRL_INUM	18
#define ESP_RMT_CTRL_DISABLE	ESP_RMT_CTRL_DIABLE /* Typo in esp_intr.h */

#define DIVIDER		4 /* Above 4, timings start to deviate*/
#define DURATION	12.5 /* minimum time of a single RMT duration
				in nanoseconds based on clock */

#define PULSE_T0H	(  350 / (DURATION * DIVIDER));
#define PULSE_T1H	(  900 / (DURATION * DIVIDER));
#define PULSE_T0L	(  900 / (DURATION * DIVIDER));
#define PULSE_T1L	(  350 / (DURATION * DIVIDER));
#define PULSE_TRS	(50000 / (DURATION * DIVIDER));

#define MAX_PULSES	32

#define RMTCHANNEL	0

typedef union {
  struct {
    uint32_t duration0:15;
    uint32_t level0:1;
    uint32_t duration1:15;
    uint32_t level1:1;
  };
  uint32_t val;
} rmtPulsePair;

static uint8_t *ws2812_buffer = NULL;
static unsigned int ws2812_pos, ws2812_len, ws2812_half;
static xSemaphoreHandle ws2812_sem = NULL;
static intr_handle_t rmt_intr_handle = NULL;
static rmtPulsePair ws2812_bits[2];

void ws2812_initRMTChannel(int rmtChannel)
{
  RMT.apb_conf.fifo_mask = 1;  //enable memory access, instead of FIFO mode.
  RMT.apb_conf.mem_tx_wrap_en = 1; //wrap around when hitting end of buffer
  RMT.conf_ch[rmtChannel].conf0.div_cnt = DIVIDER;
  RMT.conf_ch[rmtChannel].conf0.mem_size = 1;
  RMT.conf_ch[rmtChannel].conf0.carrier_en = 0;
  RMT.conf_ch[rmtChannel].conf0.carrier_out_lv = 1;
  RMT.conf_ch[rmtChannel].conf0.mem_pd = 0;

  RMT.conf_ch[rmtChannel].conf1.rx_en = 0;
  RMT.conf_ch[rmtChannel].conf1.mem_owner = 0;
  RMT.conf_ch[rmtChannel].conf1.tx_conti_mode = 0;    //loop back mode.
  RMT.conf_ch[rmtChannel].conf1.ref_always_on = 1;    // use apb clock: 80M
  RMT.conf_ch[rmtChannel].conf1.idle_out_en = 1;
  RMT.conf_ch[rmtChannel].conf1.idle_out_lv = 0;

  return;
}

void ws2812_copy()
{
  unsigned int i, j, offset, len, bit;


  offset = ws2812_half * MAX_PULSES;
  ws2812_half = !ws2812_half;

  len = ws2812_len - ws2812_pos;
  if (len > (MAX_PULSES / 8))
    len = (MAX_PULSES / 8);

  if (!len) {
    for (i = 0; i < MAX_PULSES; i++)
      RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;
    return;
  }

  for (i = 0; i < len; i++) {
    bit = ws2812_buffer[i + ws2812_pos];
    for (j = 0; j < 8; j++, bit <<= 1) {
      RMTMEM.chan[RMTCHANNEL].data32[j + i * 8 + offset].val =
	ws2812_bits[(bit >> 7) & 0x01].val;
    }
    if (i + ws2812_pos == ws2812_len - 1)
      RMTMEM.chan[RMTCHANNEL].data32[7 + i * 8 + offset].duration1 = PULSE_TRS;
  }

  for (i *= 8; i < MAX_PULSES; i++)
    RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;

  ws2812_pos += len;
  return;
}

void ws2812_handleInterrupt(void *arg)
{
  portBASE_TYPE taskAwoken = 0;


  if (RMT.int_st.ch0_tx_thr_event) {
    ws2812_copy();
    RMT.int_clr.ch0_tx_thr_event = 1;
  }
  else if (RMT.int_st.ch0_tx_end && ws2812_sem) {
    xSemaphoreGiveFromISR(ws2812_sem, &taskAwoken);
    RMT.int_clr.ch0_tx_end = 1;
  }

  return;
}

void ws2812_init(int gpioNum)
{
  DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
  DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);

  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpioNum], 2);
  gpio_matrix_out(gpioNum, RMT_SIG_OUT0_IDX + RMTCHANNEL, 0, 0);
  gpio_set_direction(gpioNum, GPIO_MODE_OUTPUT);

  ws2812_initRMTChannel(RMTCHANNEL);

  RMT.tx_lim_ch[RMTCHANNEL].limit = MAX_PULSES;
  RMT.int_ena.ch0_tx_thr_event = 1;
  RMT.int_ena.ch0_tx_end = 1;

  ws2812_bits[0].level0 = 1;
  ws2812_bits[0].level1 = 0;
  ws2812_bits[0].duration0 = PULSE_T0H;
  ws2812_bits[0].duration1 = PULSE_T0L;
  ws2812_bits[1].level0 = 1;
  ws2812_bits[1].level1 = 0;
  ws2812_bits[1].duration0 = PULSE_T1H;
  ws2812_bits[1].duration1 = PULSE_T1L;

  esp_intr_alloc(ETS_RMT_INTR_SOURCE, 0, ws2812_handleInterrupt, NULL, &rmt_intr_handle);

  return;
}

void ws2812_setColors(unsigned int length, rgbVal *array)
{
  unsigned int i;


  ws2812_len = (length * 3) * sizeof(uint8_t);
  ws2812_buffer = malloc(ws2812_len);

  for (i = 0; i < length; i++) {
    ws2812_buffer[0 + i * 3] = array[i].g;
    ws2812_buffer[1 + i * 3] = array[i].r;
    ws2812_buffer[2 + i * 3] = array[i].b;
  }

  ws2812_pos = 0;
  ws2812_half = 0;

  ws2812_copy();

  if (ws2812_pos < ws2812_len)
    ws2812_copy();

  ws2812_sem = xSemaphoreCreateBinary();

  RMT.conf_ch[RMTCHANNEL].conf1.mem_rd_rst = 1;
  RMT.conf_ch[RMTCHANNEL].conf1.tx_start = 1;

  xSemaphoreTake(ws2812_sem, portMAX_DELAY);
  vSemaphoreDelete(ws2812_sem);
  ws2812_sem = NULL;

  free(ws2812_buffer);

  return;
}
