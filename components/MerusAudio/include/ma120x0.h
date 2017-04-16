/*---------------------------------------------------------------------------------------*/
/*   Merus Audio MA120x0 register map                                                    */
/*                                                                                       */
/*   Provides : Defines for registers and symbols in merus audio amplifiers              */
/*              Macros for set and get content of symbols                                */
/*              Read-modify-write on write to symbols less then 8 bits                   */
/*                                                                                       */
/*   Symbols is a collection of 1 to 8 bits within a single address                      */
/*                                                                                       */
/*   Provided as is - free to use and share                                              */
/*                                                                                       */
/*   Timestamp                        :                         Thu Feb 23 23:06:51 2017 */
/*   Created from database            :                     ma12070_register_spec_rev6_1 */
/*   Errors and updates please contact:       Jorgen Kragh Jakobsen, jkj@merus-audio.com */
/*---------------------------------------------------------------------------------------*/

#include <stdint.h>


#ifndef _MA120X0_H_ 
#define _MA120X0_H_ 


//------------------------------------------------------------------------------manualPM---
// Select Manual PowerMode control 
#define MA_manualPM__a 0
#define MA_manualPM__len 1
#define MA_manualPM__mask 0x40
#define MA_manualPM__shift 0x06
#define MA_manualPM__reset 0x00
#define set_obj_MA_manualPM(o,y) ({ uint8_t __ret = o.read(0); o.write(0,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_manualPM(y) ({ uint8_t __ret = ma_read_byte(0); ma_write_byte(0,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_manualPM(o) (o.read(0) & 0x40)>>6 
#define get_MA_manualPM() ( ma_read_byte(0) & 0x40)>>6 
//------------------------------------------------------------------------------PM_man---
// Manual selected power mode 
#define MA_PM_man__a 0
#define MA_PM_man__len 2
#define MA_PM_man__mask 0x30
#define MA_PM_man__shift 0x04
#define MA_PM_man__reset 0x03
#define set_obj_MA_PM_man(o,y) ({ uint8_t __ret = o.read(0); o.write(0,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define set_MA_PM_man(y) ({ uint8_t __ret = ma_read_byte(0); ma_write_byte(0,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define get_obj_MA_PM_man(o) (o.read(0) & 0x30)>>4 
#define get_MA_PM_man() ( ma_read_byte(0) & 0x30)>>4 
//------------------------------------------------------------------------------Mthr_1to2---
// Mod. index threshold value for PM1=>PM2 change. 
#define MA_Mthr_1to2__a 1
#define MA_Mthr_1to2__len 8
#define MA_Mthr_1to2__mask 0xff
#define MA_Mthr_1to2__shift 0x00
#define MA_Mthr_1to2__reset 0x3c
#define set_obj_MA_Mthr_1to2(o,y) o.write(1,y);
#define set_MA_Mthr_1to2(y) ma_write_byte(1,y);
#define get_obj_MA_Mthr_1to2(o) (o.read(1) & 0xff)>>0 
#define get_MA_Mthr_1to2() ( ma_read_byte(1) & 0xff)>>0 
//------------------------------------------------------------------------------Mthr_2to1---
// Mod. index threshold value for PM2=>PM1 change. 
#define MA_Mthr_2to1__a 2
#define MA_Mthr_2to1__len 8
#define MA_Mthr_2to1__mask 0xff
#define MA_Mthr_2to1__shift 0x00
#define MA_Mthr_2to1__reset 0x32
#define set_obj_MA_Mthr_2to1(o,y) o.write(2,y);
#define set_MA_Mthr_2to1(y) ma_write_byte(2,y);
#define get_obj_MA_Mthr_2to1(o) (o.read(2) & 0xff)>>0 
#define get_MA_Mthr_2to1() ( ma_read_byte(2) & 0xff)>>0 
//------------------------------------------------------------------------------Mthr_2to3---
// Mod. index threshold value for PM2=>PM3 change. 
#define MA_Mthr_2to3__a 3
#define MA_Mthr_2to3__len 8
#define MA_Mthr_2to3__mask 0xff
#define MA_Mthr_2to3__shift 0x00
#define MA_Mthr_2to3__reset 0x5a
#define set_obj_MA_Mthr_2to3(o,y) o.write(3,y);
#define set_MA_Mthr_2to3(y) ma_write_byte(3,y);
#define get_obj_MA_Mthr_2to3(o) (o.read(3) & 0xff)>>0 
#define get_MA_Mthr_2to3() ( ma_read_byte(3) & 0xff)>>0 
//------------------------------------------------------------------------------Mthr_3to2---
// Mod. index threshold value for PM3=>PM2 change. 
#define MA_Mthr_3to2__a 4
#define MA_Mthr_3to2__len 8
#define MA_Mthr_3to2__mask 0xff
#define MA_Mthr_3to2__shift 0x00
#define MA_Mthr_3to2__reset 0x50
#define set_obj_MA_Mthr_3to2(o,y) o.write(4,y);
#define set_MA_Mthr_3to2(y) ma_write_byte(4,y);
#define get_obj_MA_Mthr_3to2(o) (o.read(4) & 0xff)>>0 
#define get_MA_Mthr_3to2() ( ma_read_byte(4) & 0xff)>>0 
//------------------------------------------------------------------------------pwmClkDiv_nom---
// PWM default clock divider value 
#define MA_pwmClkDiv_nom__a 8
#define MA_pwmClkDiv_nom__len 8
#define MA_pwmClkDiv_nom__mask 0xff
#define MA_pwmClkDiv_nom__shift 0x00
#define MA_pwmClkDiv_nom__reset 0x26
#define set_obj_MA_pwmClkDiv_nom(o,y) o.write(8,y);
#define set_MA_pwmClkDiv_nom(y) ma_write_byte(8,y);
#define get_obj_MA_pwmClkDiv_nom(o) (o.read(8) & 0xff)>>0 
#define get_MA_pwmClkDiv_nom() ( ma_read_byte(8) & 0xff)>>0 
//------------------------------------------------------------------------------ocp_latch_en---
// High to use permanently latching level-2 OCP 
#define MA_ocp_latch_en__a 10
#define MA_ocp_latch_en__len 1
#define MA_ocp_latch_en__mask 0x02
#define MA_ocp_latch_en__shift 0x01
#define MA_ocp_latch_en__reset 0x00
#define set_obj_MA_ocp_latch_en(o,y) ({ uint8_t __ret = o.read(10); o.write(10,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define set_MA_ocp_latch_en(y) ({ uint8_t __ret = ma_read_byte(10); ma_write_byte(10,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define get_obj_MA_ocp_latch_en(o) (o.read(10) & 0x02)>>1 
#define get_MA_ocp_latch_en() ( ma_read_byte(10) & 0x02)>>1 
//------------------------------------------------------------------------------lf_clamp_en---
// High (default) to enable LF int2+3 clamping on clip 
#define MA_lf_clamp_en__a 10
#define MA_lf_clamp_en__len 1
#define MA_lf_clamp_en__mask 0x80
#define MA_lf_clamp_en__shift 0x07
#define MA_lf_clamp_en__reset 0x00
#define set_obj_MA_lf_clamp_en(o,y) ({ uint8_t __ret = o.read(10); o.write(10,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_lf_clamp_en(y) ({ uint8_t __ret = ma_read_byte(10); ma_write_byte(10,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_lf_clamp_en(o) (o.read(10) & 0x80)>>7 
#define get_MA_lf_clamp_en() ( ma_read_byte(10) & 0x80)>>7 
//------------------------------------------------------------------------------PMcfg_BTL_B.modType---
//  
#define MA_PMcfg_BTL_B__modType__a 18
#define MA_PMcfg_BTL_B__modType__len 2
#define MA_PMcfg_BTL_B__modType__mask 0x18
#define MA_PMcfg_BTL_B__modType__shift 0x03
#define MA_PMcfg_BTL_B__modType__reset 0x02
#define set_obj_MA_PMcfg_BTL_B__modType(o,y) ({ uint8_t __ret = o.read(18); o.write(18,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_PMcfg_BTL_B__modType(y) ({ uint8_t __ret = ma_read_byte(18); ma_write_byte(18,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_PMcfg_BTL_B__modType(o) (o.read(18) & 0x18)>>3 
#define get_MA_PMcfg_BTL_B__modType() ( ma_read_byte(18) & 0x18)>>3 
//------------------------------------------------------------------------------PMcfg_BTL_B.freqDiv---
//  
#define MA_PMcfg_BTL_B__freqDiv__a 18
#define MA_PMcfg_BTL_B__freqDiv__len 2
#define MA_PMcfg_BTL_B__freqDiv__mask 0x06
#define MA_PMcfg_BTL_B__freqDiv__shift 0x01
#define MA_PMcfg_BTL_B__freqDiv__reset 0x01
#define set_obj_MA_PMcfg_BTL_B__freqDiv(o,y) ({ uint8_t __ret = o.read(18); o.write(18,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define set_MA_PMcfg_BTL_B__freqDiv(y) ({ uint8_t __ret = ma_read_byte(18); ma_write_byte(18,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define get_obj_MA_PMcfg_BTL_B__freqDiv(o) (o.read(18) & 0x06)>>1 
#define get_MA_PMcfg_BTL_B__freqDiv() ( ma_read_byte(18) & 0x06)>>1 
//------------------------------------------------------------------------------PMcfg_BTL_B.lf_gain_ol---
//  
#define MA_PMcfg_BTL_B__lf_gain_ol__a 18
#define MA_PMcfg_BTL_B__lf_gain_ol__len 1
#define MA_PMcfg_BTL_B__lf_gain_ol__mask 0x01
#define MA_PMcfg_BTL_B__lf_gain_ol__shift 0x00
#define MA_PMcfg_BTL_B__lf_gain_ol__reset 0x01
#define set_obj_MA_PMcfg_BTL_B__lf_gain_ol(o,y) ({ uint8_t __ret = o.read(18); o.write(18,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_PMcfg_BTL_B__lf_gain_ol(y) ({ uint8_t __ret = ma_read_byte(18); ma_write_byte(18,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_PMcfg_BTL_B__lf_gain_ol(o) (o.read(18) & 0x01)>>0 
#define get_MA_PMcfg_BTL_B__lf_gain_ol() ( ma_read_byte(18) & 0x01)>>0 
//------------------------------------------------------------------------------PMcfg_BTL_C.freqDiv---
//  
#define MA_PMcfg_BTL_C__freqDiv__a 19
#define MA_PMcfg_BTL_C__freqDiv__len 2
#define MA_PMcfg_BTL_C__freqDiv__mask 0x06
#define MA_PMcfg_BTL_C__freqDiv__shift 0x01
#define MA_PMcfg_BTL_C__freqDiv__reset 0x01
#define set_obj_MA_PMcfg_BTL_C__freqDiv(o,y) ({ uint8_t __ret = o.read(19); o.write(19,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define set_MA_PMcfg_BTL_C__freqDiv(y) ({ uint8_t __ret = ma_read_byte(19); ma_write_byte(19,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define get_obj_MA_PMcfg_BTL_C__freqDiv(o) (o.read(19) & 0x06)>>1 
#define get_MA_PMcfg_BTL_C__freqDiv() ( ma_read_byte(19) & 0x06)>>1 
//------------------------------------------------------------------------------PMcfg_BTL_C.modType---
//  
#define MA_PMcfg_BTL_C__modType__a 19
#define MA_PMcfg_BTL_C__modType__len 2
#define MA_PMcfg_BTL_C__modType__mask 0x18
#define MA_PMcfg_BTL_C__modType__shift 0x03
#define MA_PMcfg_BTL_C__modType__reset 0x01
#define set_obj_MA_PMcfg_BTL_C__modType(o,y) ({ uint8_t __ret = o.read(19); o.write(19,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_PMcfg_BTL_C__modType(y) ({ uint8_t __ret = ma_read_byte(19); ma_write_byte(19,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_PMcfg_BTL_C__modType(o) (o.read(19) & 0x18)>>3 
#define get_MA_PMcfg_BTL_C__modType() ( ma_read_byte(19) & 0x18)>>3 
//------------------------------------------------------------------------------PMcfg_BTL_C.lf_gain_ol---
//  
#define MA_PMcfg_BTL_C__lf_gain_ol__a 19
#define MA_PMcfg_BTL_C__lf_gain_ol__len 1
#define MA_PMcfg_BTL_C__lf_gain_ol__mask 0x01
#define MA_PMcfg_BTL_C__lf_gain_ol__shift 0x00
#define MA_PMcfg_BTL_C__lf_gain_ol__reset 0x00
#define set_obj_MA_PMcfg_BTL_C__lf_gain_ol(o,y) ({ uint8_t __ret = o.read(19); o.write(19,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_PMcfg_BTL_C__lf_gain_ol(y) ({ uint8_t __ret = ma_read_byte(19); ma_write_byte(19,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_PMcfg_BTL_C__lf_gain_ol(o) (o.read(19) & 0x01)>>0 
#define get_MA_PMcfg_BTL_C__lf_gain_ol() ( ma_read_byte(19) & 0x01)>>0 
//------------------------------------------------------------------------------PMcfg_BTL_D.modType---
//  
#define MA_PMcfg_BTL_D__modType__a 20
#define MA_PMcfg_BTL_D__modType__len 2
#define MA_PMcfg_BTL_D__modType__mask 0x18
#define MA_PMcfg_BTL_D__modType__shift 0x03
#define MA_PMcfg_BTL_D__modType__reset 0x02
#define set_obj_MA_PMcfg_BTL_D__modType(o,y) ({ uint8_t __ret = o.read(20); o.write(20,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_PMcfg_BTL_D__modType(y) ({ uint8_t __ret = ma_read_byte(20); ma_write_byte(20,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_PMcfg_BTL_D__modType(o) (o.read(20) & 0x18)>>3 
#define get_MA_PMcfg_BTL_D__modType() ( ma_read_byte(20) & 0x18)>>3 
//------------------------------------------------------------------------------PMcfg_BTL_D.freqDiv---
//  
#define MA_PMcfg_BTL_D__freqDiv__a 20
#define MA_PMcfg_BTL_D__freqDiv__len 2
#define MA_PMcfg_BTL_D__freqDiv__mask 0x06
#define MA_PMcfg_BTL_D__freqDiv__shift 0x01
#define MA_PMcfg_BTL_D__freqDiv__reset 0x02
#define set_obj_MA_PMcfg_BTL_D__freqDiv(o,y) ({ uint8_t __ret = o.read(20); o.write(20,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define set_MA_PMcfg_BTL_D__freqDiv(y) ({ uint8_t __ret = ma_read_byte(20); ma_write_byte(20,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define get_obj_MA_PMcfg_BTL_D__freqDiv(o) (o.read(20) & 0x06)>>1 
#define get_MA_PMcfg_BTL_D__freqDiv() ( ma_read_byte(20) & 0x06)>>1 
//------------------------------------------------------------------------------PMcfg_BTL_D.lf_gain_ol---
//  
#define MA_PMcfg_BTL_D__lf_gain_ol__a 20
#define MA_PMcfg_BTL_D__lf_gain_ol__len 1
#define MA_PMcfg_BTL_D__lf_gain_ol__mask 0x01
#define MA_PMcfg_BTL_D__lf_gain_ol__shift 0x00
#define MA_PMcfg_BTL_D__lf_gain_ol__reset 0x00
#define set_obj_MA_PMcfg_BTL_D__lf_gain_ol(o,y) ({ uint8_t __ret = o.read(20); o.write(20,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_PMcfg_BTL_D__lf_gain_ol(y) ({ uint8_t __ret = ma_read_byte(20); ma_write_byte(20,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_PMcfg_BTL_D__lf_gain_ol(o) (o.read(20) & 0x01)>>0 
#define get_MA_PMcfg_BTL_D__lf_gain_ol() ( ma_read_byte(20) & 0x01)>>0 
//------------------------------------------------------------------------------PMcfg_SE_A.modType---
//  
#define MA_PMcfg_SE_A__modType__a 21
#define MA_PMcfg_SE_A__modType__len 2
#define MA_PMcfg_SE_A__modType__mask 0x18
#define MA_PMcfg_SE_A__modType__shift 0x03
#define MA_PMcfg_SE_A__modType__reset 0x01
#define set_obj_MA_PMcfg_SE_A__modType(o,y) ({ uint8_t __ret = o.read(21); o.write(21,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_PMcfg_SE_A__modType(y) ({ uint8_t __ret = ma_read_byte(21); ma_write_byte(21,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_PMcfg_SE_A__modType(o) (o.read(21) & 0x18)>>3 
#define get_MA_PMcfg_SE_A__modType() ( ma_read_byte(21) & 0x18)>>3 
//------------------------------------------------------------------------------PMcfg_SE_A.freqDiv---
//  
#define MA_PMcfg_SE_A__freqDiv__a 21
#define MA_PMcfg_SE_A__freqDiv__len 2
#define MA_PMcfg_SE_A__freqDiv__mask 0x06
#define MA_PMcfg_SE_A__freqDiv__shift 0x01
#define MA_PMcfg_SE_A__freqDiv__reset 0x00
#define set_obj_MA_PMcfg_SE_A__freqDiv(o,y) ({ uint8_t __ret = o.read(21); o.write(21,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define set_MA_PMcfg_SE_A__freqDiv(y) ({ uint8_t __ret = ma_read_byte(21); ma_write_byte(21,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define get_obj_MA_PMcfg_SE_A__freqDiv(o) (o.read(21) & 0x06)>>1 
#define get_MA_PMcfg_SE_A__freqDiv() ( ma_read_byte(21) & 0x06)>>1 
//------------------------------------------------------------------------------PMcfg_SE_A.lf_gain_ol---
//  
#define MA_PMcfg_SE_A__lf_gain_ol__a 21
#define MA_PMcfg_SE_A__lf_gain_ol__len 1
#define MA_PMcfg_SE_A__lf_gain_ol__mask 0x01
#define MA_PMcfg_SE_A__lf_gain_ol__shift 0x00
#define MA_PMcfg_SE_A__lf_gain_ol__reset 0x01
#define set_obj_MA_PMcfg_SE_A__lf_gain_ol(o,y) ({ uint8_t __ret = o.read(21); o.write(21,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_PMcfg_SE_A__lf_gain_ol(y) ({ uint8_t __ret = ma_read_byte(21); ma_write_byte(21,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_PMcfg_SE_A__lf_gain_ol(o) (o.read(21) & 0x01)>>0 
#define get_MA_PMcfg_SE_A__lf_gain_ol() ( ma_read_byte(21) & 0x01)>>0 
//------------------------------------------------------------------------------PMcfg_SE_B.lf_gain_ol---
//  
#define MA_PMcfg_SE_B__lf_gain_ol__a 22
#define MA_PMcfg_SE_B__lf_gain_ol__len 1
#define MA_PMcfg_SE_B__lf_gain_ol__mask 0x01
#define MA_PMcfg_SE_B__lf_gain_ol__shift 0x00
#define MA_PMcfg_SE_B__lf_gain_ol__reset 0x00
#define set_obj_MA_PMcfg_SE_B__lf_gain_ol(o,y) ({ uint8_t __ret = o.read(22); o.write(22,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_PMcfg_SE_B__lf_gain_ol(y) ({ uint8_t __ret = ma_read_byte(22); ma_write_byte(22,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_PMcfg_SE_B__lf_gain_ol(o) (o.read(22) & 0x01)>>0 
#define get_MA_PMcfg_SE_B__lf_gain_ol() ( ma_read_byte(22) & 0x01)>>0 
//------------------------------------------------------------------------------PMcfg_SE_B.freqDiv---
//  
#define MA_PMcfg_SE_B__freqDiv__a 22
#define MA_PMcfg_SE_B__freqDiv__len 2
#define MA_PMcfg_SE_B__freqDiv__mask 0x06
#define MA_PMcfg_SE_B__freqDiv__shift 0x01
#define MA_PMcfg_SE_B__freqDiv__reset 0x01
#define set_obj_MA_PMcfg_SE_B__freqDiv(o,y) ({ uint8_t __ret = o.read(22); o.write(22,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define set_MA_PMcfg_SE_B__freqDiv(y) ({ uint8_t __ret = ma_read_byte(22); ma_write_byte(22,(__ret&0xf9)|((y<<1)&0x06)); }) 
#define get_obj_MA_PMcfg_SE_B__freqDiv(o) (o.read(22) & 0x06)>>1 
#define get_MA_PMcfg_SE_B__freqDiv() ( ma_read_byte(22) & 0x06)>>1 
//------------------------------------------------------------------------------PMcfg_SE_B.modType---
//  
#define MA_PMcfg_SE_B__modType__a 22
#define MA_PMcfg_SE_B__modType__len 2
#define MA_PMcfg_SE_B__modType__mask 0x18
#define MA_PMcfg_SE_B__modType__shift 0x03
#define MA_PMcfg_SE_B__modType__reset 0x01
#define set_obj_MA_PMcfg_SE_B__modType(o,y) ({ uint8_t __ret = o.read(22); o.write(22,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_PMcfg_SE_B__modType(y) ({ uint8_t __ret = ma_read_byte(22); ma_write_byte(22,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_PMcfg_SE_B__modType(o) (o.read(22) & 0x18)>>3 
#define get_MA_PMcfg_SE_B__modType() ( ma_read_byte(22) & 0x18)>>3 
//------------------------------------------------------------------------------balWaitCount_PM1---
// PM1 balancing period. 
#define MA_balWaitCount_PM1__a 23
#define MA_balWaitCount_PM1__len 8
#define MA_balWaitCount_PM1__mask 0xff
#define MA_balWaitCount_PM1__shift 0x00
#define MA_balWaitCount_PM1__reset 0x14
#define set_obj_MA_balWaitCount_PM1(o,y) o.write(23,y);
#define set_MA_balWaitCount_PM1(y) ma_write_byte(23,y);
#define get_obj_MA_balWaitCount_PM1(o) (o.read(23) & 0xff)>>0 
#define get_MA_balWaitCount_PM1() ( ma_read_byte(23) & 0xff)>>0 
//------------------------------------------------------------------------------balWaitCount_PM2---
// PM2 balancing period. 
#define MA_balWaitCount_PM2__a 24
#define MA_balWaitCount_PM2__len 8
#define MA_balWaitCount_PM2__mask 0xff
#define MA_balWaitCount_PM2__shift 0x00
#define MA_balWaitCount_PM2__reset 0x14
#define set_obj_MA_balWaitCount_PM2(o,y) o.write(24,y);
#define set_MA_balWaitCount_PM2(y) ma_write_byte(24,y);
#define get_obj_MA_balWaitCount_PM2(o) (o.read(24) & 0xff)>>0 
#define get_MA_balWaitCount_PM2() ( ma_read_byte(24) & 0xff)>>0 
//------------------------------------------------------------------------------balWaitCount_PM3---
// PM3 balancing period. 
#define MA_balWaitCount_PM3__a 25
#define MA_balWaitCount_PM3__len 8
#define MA_balWaitCount_PM3__mask 0xff
#define MA_balWaitCount_PM3__shift 0x00
#define MA_balWaitCount_PM3__reset 0x1a
#define set_obj_MA_balWaitCount_PM3(o,y) o.write(25,y);
#define set_MA_balWaitCount_PM3(y) ma_write_byte(25,y);
#define get_obj_MA_balWaitCount_PM3(o) (o.read(25) & 0xff)>>0 
#define get_MA_balWaitCount_PM3() ( ma_read_byte(25) & 0xff)>>0 
//------------------------------------------------------------------------------useSpread_PM1---
// PM1 PWM spread-spectrum mode on/off. 
#define MA_useSpread_PM1__a 26
#define MA_useSpread_PM1__len 1
#define MA_useSpread_PM1__mask 0x40
#define MA_useSpread_PM1__shift 0x06
#define MA_useSpread_PM1__reset 0x00
#define set_obj_MA_useSpread_PM1(o,y) ({ uint8_t __ret = o.read(26); o.write(26,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_useSpread_PM1(y) ({ uint8_t __ret = ma_read_byte(26); ma_write_byte(26,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_useSpread_PM1(o) (o.read(26) & 0x40)>>6 
#define get_MA_useSpread_PM1() ( ma_read_byte(26) & 0x40)>>6 
//------------------------------------------------------------------------------DTsteps_PM1---
// PM1 dead time setting [10ns steps]. 
#define MA_DTsteps_PM1__a 26
#define MA_DTsteps_PM1__len 3
#define MA_DTsteps_PM1__mask 0x38
#define MA_DTsteps_PM1__shift 0x03
#define MA_DTsteps_PM1__reset 0x04
#define set_obj_MA_DTsteps_PM1(o,y) ({ uint8_t __ret = o.read(26); o.write(26,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define set_MA_DTsteps_PM1(y) ({ uint8_t __ret = ma_read_byte(26); ma_write_byte(26,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define get_obj_MA_DTsteps_PM1(o) (o.read(26) & 0x38)>>3 
#define get_MA_DTsteps_PM1() ( ma_read_byte(26) & 0x38)>>3 
//------------------------------------------------------------------------------balType_PM1---
// PM1 balancing sensor scheme. 
#define MA_balType_PM1__a 26
#define MA_balType_PM1__len 3
#define MA_balType_PM1__mask 0x07
#define MA_balType_PM1__shift 0x00
#define MA_balType_PM1__reset 0x00
#define set_obj_MA_balType_PM1(o,y) ({ uint8_t __ret = o.read(26); o.write(26,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_balType_PM1(y) ({ uint8_t __ret = ma_read_byte(26); ma_write_byte(26,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_balType_PM1(o) (o.read(26) & 0x07)>>0 
#define get_MA_balType_PM1() ( ma_read_byte(26) & 0x07)>>0 
//------------------------------------------------------------------------------useSpread_PM2---
// PM2 PWM spread-spectrum mode on/off. 
#define MA_useSpread_PM2__a 27
#define MA_useSpread_PM2__len 1
#define MA_useSpread_PM2__mask 0x40
#define MA_useSpread_PM2__shift 0x06
#define MA_useSpread_PM2__reset 0x00
#define set_obj_MA_useSpread_PM2(o,y) ({ uint8_t __ret = o.read(27); o.write(27,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_useSpread_PM2(y) ({ uint8_t __ret = ma_read_byte(27); ma_write_byte(27,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_useSpread_PM2(o) (o.read(27) & 0x40)>>6 
#define get_MA_useSpread_PM2() ( ma_read_byte(27) & 0x40)>>6 
//------------------------------------------------------------------------------DTsteps_PM2---
// PM2 dead time setting [10ns steps]. 
#define MA_DTsteps_PM2__a 27
#define MA_DTsteps_PM2__len 3
#define MA_DTsteps_PM2__mask 0x38
#define MA_DTsteps_PM2__shift 0x03
#define MA_DTsteps_PM2__reset 0x03
#define set_obj_MA_DTsteps_PM2(o,y) ({ uint8_t __ret = o.read(27); o.write(27,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define set_MA_DTsteps_PM2(y) ({ uint8_t __ret = ma_read_byte(27); ma_write_byte(27,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define get_obj_MA_DTsteps_PM2(o) (o.read(27) & 0x38)>>3 
#define get_MA_DTsteps_PM2() ( ma_read_byte(27) & 0x38)>>3 
//------------------------------------------------------------------------------balType_PM2---
// PM2 balancing sensor scheme. 
#define MA_balType_PM2__a 27
#define MA_balType_PM2__len 3
#define MA_balType_PM2__mask 0x07
#define MA_balType_PM2__shift 0x00
#define MA_balType_PM2__reset 0x01
#define set_obj_MA_balType_PM2(o,y) ({ uint8_t __ret = o.read(27); o.write(27,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_balType_PM2(y) ({ uint8_t __ret = ma_read_byte(27); ma_write_byte(27,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_balType_PM2(o) (o.read(27) & 0x07)>>0 
#define get_MA_balType_PM2() ( ma_read_byte(27) & 0x07)>>0 
//------------------------------------------------------------------------------useSpread_PM3---
// PM3 PWM spread-spectrum mode on/off. 
#define MA_useSpread_PM3__a 28
#define MA_useSpread_PM3__len 1
#define MA_useSpread_PM3__mask 0x40
#define MA_useSpread_PM3__shift 0x06
#define MA_useSpread_PM3__reset 0x00
#define set_obj_MA_useSpread_PM3(o,y) ({ uint8_t __ret = o.read(28); o.write(28,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_useSpread_PM3(y) ({ uint8_t __ret = ma_read_byte(28); ma_write_byte(28,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_useSpread_PM3(o) (o.read(28) & 0x40)>>6 
#define get_MA_useSpread_PM3() ( ma_read_byte(28) & 0x40)>>6 
//------------------------------------------------------------------------------DTsteps_PM3---
// PM3 dead time setting [10ns steps]. 
#define MA_DTsteps_PM3__a 28
#define MA_DTsteps_PM3__len 3
#define MA_DTsteps_PM3__mask 0x38
#define MA_DTsteps_PM3__shift 0x03
#define MA_DTsteps_PM3__reset 0x01
#define set_obj_MA_DTsteps_PM3(o,y) ({ uint8_t __ret = o.read(28); o.write(28,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define set_MA_DTsteps_PM3(y) ({ uint8_t __ret = ma_read_byte(28); ma_write_byte(28,(__ret&0xc7)|((y<<3)&0x38)); }) 
#define get_obj_MA_DTsteps_PM3(o) (o.read(28) & 0x38)>>3 
#define get_MA_DTsteps_PM3() ( ma_read_byte(28) & 0x38)>>3 
//------------------------------------------------------------------------------balType_PM3---
// PM3 balancing sensor scheme. 
#define MA_balType_PM3__a 28
#define MA_balType_PM3__len 3
#define MA_balType_PM3__mask 0x07
#define MA_balType_PM3__shift 0x00
#define MA_balType_PM3__reset 0x03
#define set_obj_MA_balType_PM3(o,y) ({ uint8_t __ret = o.read(28); o.write(28,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_balType_PM3(y) ({ uint8_t __ret = ma_read_byte(28); ma_write_byte(28,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_balType_PM3(o) (o.read(28) & 0x07)>>0 
#define get_MA_balType_PM3() ( ma_read_byte(28) & 0x07)>>0 
//------------------------------------------------------------------------------PMprofile---
// PM profile select. Valid presets: 0-1-2-3-4. 5=> custom profile. 
#define MA_PMprofile__a 29
#define MA_PMprofile__len 3
#define MA_PMprofile__mask 0x07
#define MA_PMprofile__shift 0x00
#define MA_PMprofile__reset 0x00
#define set_obj_MA_PMprofile(o,y) ({ uint8_t __ret = o.read(29); o.write(29,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_PMprofile(y) ({ uint8_t __ret = ma_read_byte(29); ma_write_byte(29,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_PMprofile(o) (o.read(29) & 0x07)>>0 
#define get_MA_PMprofile() ( ma_read_byte(29) & 0x07)>>0 
//------------------------------------------------------------------------------PM3_man---
// Custom profile PM3 contents. 0=>A, 1=>B, 2=>C, 3=>D 
#define MA_PM3_man__a 30
#define MA_PM3_man__len 2
#define MA_PM3_man__mask 0x30
#define MA_PM3_man__shift 0x04
#define MA_PM3_man__reset 0x02
#define set_obj_MA_PM3_man(o,y) ({ uint8_t __ret = o.read(30); o.write(30,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define set_MA_PM3_man(y) ({ uint8_t __ret = ma_read_byte(30); ma_write_byte(30,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define get_obj_MA_PM3_man(o) (o.read(30) & 0x30)>>4 
#define get_MA_PM3_man() ( ma_read_byte(30) & 0x30)>>4 
//------------------------------------------------------------------------------PM2_man---
// Custom profile PM2 contents. 0=>A, 1=>B, 2=>C, 3=>D 
#define MA_PM2_man__a 30
#define MA_PM2_man__len 2
#define MA_PM2_man__mask 0x0c
#define MA_PM2_man__shift 0x02
#define MA_PM2_man__reset 0x03
#define set_obj_MA_PM2_man(o,y) ({ uint8_t __ret = o.read(30); o.write(30,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define set_MA_PM2_man(y) ({ uint8_t __ret = ma_read_byte(30); ma_write_byte(30,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define get_obj_MA_PM2_man(o) (o.read(30) & 0x0c)>>2 
#define get_MA_PM2_man() ( ma_read_byte(30) & 0x0c)>>2 
//------------------------------------------------------------------------------PM1_man---
// Custom profile PM1 contents. 0=>A, 1=>B, 2=>C, 3=>D 
#define MA_PM1_man__a 30
#define MA_PM1_man__len 2
#define MA_PM1_man__mask 0x03
#define MA_PM1_man__shift 0x00
#define MA_PM1_man__reset 0x03
#define set_obj_MA_PM1_man(o,y) ({ uint8_t __ret = o.read(30); o.write(30,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_PM1_man(y) ({ uint8_t __ret = ma_read_byte(30); ma_write_byte(30,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_PM1_man(o) (o.read(30) & 0x03)>>0 
#define get_MA_PM1_man() ( ma_read_byte(30) & 0x03)>>0 
//------------------------------------------------------------------------------ocp_latch_clear---
// Low-high clears current OCP latched condition. 
#define MA_ocp_latch_clear__a 32
#define MA_ocp_latch_clear__len 1
#define MA_ocp_latch_clear__mask 0x80
#define MA_ocp_latch_clear__shift 0x07
#define MA_ocp_latch_clear__reset 0x00
#define set_obj_MA_ocp_latch_clear(o,y) ({ uint8_t __ret = o.read(32); o.write(32,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_ocp_latch_clear(y) ({ uint8_t __ret = ma_read_byte(32); ma_write_byte(32,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_ocp_latch_clear(o) (o.read(32) & 0x80)>>7 
#define get_MA_ocp_latch_clear() ( ma_read_byte(32) & 0x80)>>7 
//------------------------------------------------------------------------------audio_in_mode---
// Audio input mode; 0-1-2-3-4-5 
#define MA_audio_in_mode__a 37
#define MA_audio_in_mode__len 3
#define MA_audio_in_mode__mask 0xe0
#define MA_audio_in_mode__shift 0x05
#define MA_audio_in_mode__reset 0x00
#define set_obj_MA_audio_in_mode(o,y) ({ uint8_t __ret = o.read(37); o.write(37,(__ret&0x1f)|((y<<5)&0xe0)); }) 
#define set_MA_audio_in_mode(y) ({ uint8_t __ret = ma_read_byte(37); ma_write_byte(37,(__ret&0x1f)|((y<<5)&0xe0)); }) 
#define get_obj_MA_audio_in_mode(o) (o.read(37) & 0xe0)>>5 
#define get_MA_audio_in_mode() ( ma_read_byte(37) & 0xe0)>>5 
//------------------------------------------------------------------------------eh_dcShdn---
// High to enable DC protection 
#define MA_eh_dcShdn__a 38
#define MA_eh_dcShdn__len 1
#define MA_eh_dcShdn__mask 0x04
#define MA_eh_dcShdn__shift 0x02
#define MA_eh_dcShdn__reset 0x01
#define set_obj_MA_eh_dcShdn(o,y) ({ uint8_t __ret = o.read(38); o.write(38,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define set_MA_eh_dcShdn(y) ({ uint8_t __ret = ma_read_byte(38); ma_write_byte(38,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define get_obj_MA_eh_dcShdn(o) (o.read(38) & 0x04)>>2 
#define get_MA_eh_dcShdn() ( ma_read_byte(38) & 0x04)>>2 
//------------------------------------------------------------------------------audio_in_mode_ext---
// If set, audio_in_mode is controlled from audio_in_mode register. If not set  audio_in_mode is set from fuse bank setting 
#define MA_audio_in_mode_ext__a 39
#define MA_audio_in_mode_ext__len 1
#define MA_audio_in_mode_ext__mask 0x20
#define MA_audio_in_mode_ext__shift 0x05
#define MA_audio_in_mode_ext__reset 0x00
#define set_obj_MA_audio_in_mode_ext(o,y) ({ uint8_t __ret = o.read(39); o.write(39,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define set_MA_audio_in_mode_ext(y) ({ uint8_t __ret = ma_read_byte(39); ma_write_byte(39,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define get_obj_MA_audio_in_mode_ext(o) (o.read(39) & 0x20)>>5 
#define get_MA_audio_in_mode_ext() ( ma_read_byte(39) & 0x20)>>5 
//------------------------------------------------------------------------------eh_clear---
// Flip to clear error registers 
#define MA_eh_clear__a 45
#define MA_eh_clear__len 1
#define MA_eh_clear__mask 0x04
#define MA_eh_clear__shift 0x02
#define MA_eh_clear__reset 0x00
#define set_obj_MA_eh_clear(o,y) ({ uint8_t __ret = o.read(45); o.write(45,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define set_MA_eh_clear(y) ({ uint8_t __ret = ma_read_byte(45); ma_write_byte(45,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define get_obj_MA_eh_clear(o) (o.read(45) & 0x04)>>2 
#define get_MA_eh_clear() ( ma_read_byte(45) & 0x04)>>2 
//------------------------------------------------------------------------------thermal_compr_en---
// Enable otw-contr.  input compression? 
#define MA_thermal_compr_en__a 45
#define MA_thermal_compr_en__len 1
#define MA_thermal_compr_en__mask 0x20
#define MA_thermal_compr_en__shift 0x05
#define MA_thermal_compr_en__reset 0x01
#define set_obj_MA_thermal_compr_en(o,y) ({ uint8_t __ret = o.read(45); o.write(45,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define set_MA_thermal_compr_en(y) ({ uint8_t __ret = ma_read_byte(45); ma_write_byte(45,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define get_obj_MA_thermal_compr_en(o) (o.read(45) & 0x20)>>5 
#define get_MA_thermal_compr_en() ( ma_read_byte(45) & 0x20)>>5 
//------------------------------------------------------------------------------system_mute---
// 1 = mute system, 0 = normal operation 
#define MA_system_mute__a 45
#define MA_system_mute__len 1
#define MA_system_mute__mask 0x40
#define MA_system_mute__shift 0x06
#define MA_system_mute__reset 0x00
#define set_obj_MA_system_mute(o,y) ({ uint8_t __ret = o.read(45); o.write(45,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_system_mute(y) ({ uint8_t __ret = ma_read_byte(45); ma_write_byte(45,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_system_mute(o) (o.read(45) & 0x40)>>6 
#define get_MA_system_mute() ( ma_read_byte(45) & 0x40)>>6 
//------------------------------------------------------------------------------thermal_compr_max_db---
// Audio limiter max thermal reduction 
#define MA_thermal_compr_max_db__a 46
#define MA_thermal_compr_max_db__len 3
#define MA_thermal_compr_max_db__mask 0x07
#define MA_thermal_compr_max_db__shift 0x00
#define MA_thermal_compr_max_db__reset 0x04
#define set_obj_MA_thermal_compr_max_db(o,y) ({ uint8_t __ret = o.read(46); o.write(46,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_thermal_compr_max_db(y) ({ uint8_t __ret = ma_read_byte(46); ma_write_byte(46,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_thermal_compr_max_db(o) (o.read(46) & 0x07)>>0 
#define get_MA_thermal_compr_max_db() ( ma_read_byte(46) & 0x07)>>0 
//------------------------------------------------------------------------------audio_proc_enable---
// Enable Audio proc, bypass if not enabled 
#define MA_audio_proc_enable__a 53
#define MA_audio_proc_enable__len 1
#define MA_audio_proc_enable__mask 0x08
#define MA_audio_proc_enable__shift 0x03
#define MA_audio_proc_enable__reset 0x00
#define set_obj_MA_audio_proc_enable(o,y) ({ uint8_t __ret = o.read(53); o.write(53,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define set_MA_audio_proc_enable(y) ({ uint8_t __ret = ma_read_byte(53); ma_write_byte(53,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define get_obj_MA_audio_proc_enable(o) (o.read(53) & 0x08)>>3 
#define get_MA_audio_proc_enable() ( ma_read_byte(53) & 0x08)>>3 
//------------------------------------------------------------------------------audio_proc_release---
// 00:slow, 01:normal, 10:fast 
#define MA_audio_proc_release__a 53
#define MA_audio_proc_release__len 2
#define MA_audio_proc_release__mask 0x30
#define MA_audio_proc_release__shift 0x04
#define MA_audio_proc_release__reset 0x00
#define set_obj_MA_audio_proc_release(o,y) ({ uint8_t __ret = o.read(53); o.write(53,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define set_MA_audio_proc_release(y) ({ uint8_t __ret = ma_read_byte(53); ma_write_byte(53,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define get_obj_MA_audio_proc_release(o) (o.read(53) & 0x30)>>4 
#define get_MA_audio_proc_release() ( ma_read_byte(53) & 0x30)>>4 
//------------------------------------------------------------------------------audio_proc_attack---
// 00:slow, 01:normal, 10:fast  
#define MA_audio_proc_attack__a 53
#define MA_audio_proc_attack__len 2
#define MA_audio_proc_attack__mask 0xc0
#define MA_audio_proc_attack__shift 0x06
#define MA_audio_proc_attack__reset 0x00
#define set_obj_MA_audio_proc_attack(o,y) ({ uint8_t __ret = o.read(53); o.write(53,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define set_MA_audio_proc_attack(y) ({ uint8_t __ret = ma_read_byte(53); ma_write_byte(53,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define get_obj_MA_audio_proc_attack(o) (o.read(53) & 0xc0)>>6 
#define get_MA_audio_proc_attack() ( ma_read_byte(53) & 0xc0)>>6 
//------------------------------------------------------------------------------i2s_format---
// i2s basic data format, 000 = std. i2s, 001 = left justified (default) 
#define MA_i2s_format__a 53
#define MA_i2s_format__len 3
#define MA_i2s_format__mask 0x07
#define MA_i2s_format__shift 0x00
#define MA_i2s_format__reset 0x01
#define set_obj_MA_i2s_format(o,y) ({ uint8_t __ret = o.read(53); o.write(53,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_i2s_format(y) ({ uint8_t __ret = ma_read_byte(53); ma_write_byte(53,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_i2s_format(o) (o.read(53) & 0x07)>>0 
#define get_MA_i2s_format() ( ma_read_byte(53) & 0x07)>>0 
//------------------------------------------------------------------------------audio_proc_limiterEnable---
// 1: enable limiter, 0: disable limiter 
#define MA_audio_proc_limiterEnable__a 54
#define MA_audio_proc_limiterEnable__len 1
#define MA_audio_proc_limiterEnable__mask 0x40
#define MA_audio_proc_limiterEnable__shift 0x06
#define MA_audio_proc_limiterEnable__reset 0x00
#define set_obj_MA_audio_proc_limiterEnable(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_audio_proc_limiterEnable(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_audio_proc_limiterEnable(o) (o.read(54) & 0x40)>>6 
#define get_MA_audio_proc_limiterEnable() ( ma_read_byte(54) & 0x40)>>6 
//------------------------------------------------------------------------------audio_proc_mute---
// 1: mute, 0: unmute 
#define MA_audio_proc_mute__a 54
#define MA_audio_proc_mute__len 1
#define MA_audio_proc_mute__mask 0x80
#define MA_audio_proc_mute__shift 0x07
#define MA_audio_proc_mute__reset 0x00
#define set_obj_MA_audio_proc_mute(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_audio_proc_mute(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_audio_proc_mute(o) (o.read(54) & 0x80)>>7 
#define get_MA_audio_proc_mute() ( ma_read_byte(54) & 0x80)>>7 
//------------------------------------------------------------------------------i2s_sck_pol---
// i2s sck polarity cfg. 0 = rising edge data change 
#define MA_i2s_sck_pol__a 54
#define MA_i2s_sck_pol__len 1
#define MA_i2s_sck_pol__mask 0x01
#define MA_i2s_sck_pol__shift 0x00
#define MA_i2s_sck_pol__reset 0x01
#define set_obj_MA_i2s_sck_pol(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_i2s_sck_pol(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_i2s_sck_pol(o) (o.read(54) & 0x01)>>0 
#define get_MA_i2s_sck_pol() ( ma_read_byte(54) & 0x01)>>0 
//------------------------------------------------------------------------------i2s_framesize---
// i2s word length. 00 = 32bit, 01 = 24bit 
#define MA_i2s_framesize__a 54
#define MA_i2s_framesize__len 2
#define MA_i2s_framesize__mask 0x18
#define MA_i2s_framesize__shift 0x03
#define MA_i2s_framesize__reset 0x00
#define set_obj_MA_i2s_framesize(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define set_MA_i2s_framesize(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xe7)|((y<<3)&0x18)); }) 
#define get_obj_MA_i2s_framesize(o) (o.read(54) & 0x18)>>3 
#define get_MA_i2s_framesize() ( ma_read_byte(54) & 0x18)>>3 
//------------------------------------------------------------------------------i2s_ws_pol---
// i2s ws polarity. 0 = low first 
#define MA_i2s_ws_pol__a 54
#define MA_i2s_ws_pol__len 1
#define MA_i2s_ws_pol__mask 0x02
#define MA_i2s_ws_pol__shift 0x01
#define MA_i2s_ws_pol__reset 0x00
#define set_obj_MA_i2s_ws_pol(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define set_MA_i2s_ws_pol(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define get_obj_MA_i2s_ws_pol(o) (o.read(54) & 0x02)>>1 
#define get_MA_i2s_ws_pol() ( ma_read_byte(54) & 0x02)>>1 
//------------------------------------------------------------------------------i2s_order---
// i2s word bit order. 0 = MSB first 
#define MA_i2s_order__a 54
#define MA_i2s_order__len 1
#define MA_i2s_order__mask 0x04
#define MA_i2s_order__shift 0x02
#define MA_i2s_order__reset 0x00
#define set_obj_MA_i2s_order(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define set_MA_i2s_order(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define get_obj_MA_i2s_order(o) (o.read(54) & 0x04)>>2 
#define get_MA_i2s_order() ( ma_read_byte(54) & 0x04)>>2 
//------------------------------------------------------------------------------i2s_rightfirst---
// i2s L/R word order; 0 = left first 
#define MA_i2s_rightfirst__a 54
#define MA_i2s_rightfirst__len 1
#define MA_i2s_rightfirst__mask 0x20
#define MA_i2s_rightfirst__shift 0x05
#define MA_i2s_rightfirst__reset 0x00
#define set_obj_MA_i2s_rightfirst(o,y) ({ uint8_t __ret = o.read(54); o.write(54,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define set_MA_i2s_rightfirst(y) ({ uint8_t __ret = ma_read_byte(54); ma_write_byte(54,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define get_obj_MA_i2s_rightfirst(o) (o.read(54) & 0x20)>>5 
#define get_MA_i2s_rightfirst() ( ma_read_byte(54) & 0x20)>>5 
//------------------------------------------------------------------------------vol_db_master---
// Master volume db 
#define MA_vol_db_master__a 64
#define MA_vol_db_master__len 8
#define MA_vol_db_master__mask 0xff
#define MA_vol_db_master__shift 0x00
#define MA_vol_db_master__reset 0x18
#define set_obj_MA_vol_db_master(o,y) o.write(64,y);
#define set_MA_vol_db_master(y) ma_write_byte(64,y);
#define get_obj_MA_vol_db_master(o) (o.read(64) & 0xff)>>0 
#define get_MA_vol_db_master() ( ma_read_byte(64) & 0xff)>>0 
//------------------------------------------------------------------------------vol_lsb_master---
// Master volume lsb 1/4 steps 
#define MA_vol_lsb_master__a 65
#define MA_vol_lsb_master__len 2
#define MA_vol_lsb_master__mask 0x03
#define MA_vol_lsb_master__shift 0x00
#define MA_vol_lsb_master__reset 0x00
#define set_obj_MA_vol_lsb_master(o,y) ({ uint8_t __ret = o.read(65); o.write(65,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_vol_lsb_master(y) ({ uint8_t __ret = ma_read_byte(65); ma_write_byte(65,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_vol_lsb_master(o) (o.read(65) & 0x03)>>0 
#define get_MA_vol_lsb_master() ( ma_read_byte(65) & 0x03)>>0 
//------------------------------------------------------------------------------vol_db_ch0---
// Volume channel 0 
#define MA_vol_db_ch0__a 66
#define MA_vol_db_ch0__len 8
#define MA_vol_db_ch0__mask 0xff
#define MA_vol_db_ch0__shift 0x00
#define MA_vol_db_ch0__reset 0x18
#define set_obj_MA_vol_db_ch0(o,y) o.write(66,y);
#define set_MA_vol_db_ch0(y) ma_write_byte(66,y);
#define get_obj_MA_vol_db_ch0(o) (o.read(66) & 0xff)>>0 
#define get_MA_vol_db_ch0() ( ma_read_byte(66) & 0xff)>>0 
//------------------------------------------------------------------------------vol_db_ch1---
// Volume channel 1 
#define MA_vol_db_ch1__a 67
#define MA_vol_db_ch1__len 8
#define MA_vol_db_ch1__mask 0xff
#define MA_vol_db_ch1__shift 0x00
#define MA_vol_db_ch1__reset 0x18
#define set_obj_MA_vol_db_ch1(o,y) o.write(67,y);
#define set_MA_vol_db_ch1(y) ma_write_byte(67,y);
#define get_obj_MA_vol_db_ch1(o) (o.read(67) & 0xff)>>0 
#define get_MA_vol_db_ch1() ( ma_read_byte(67) & 0xff)>>0 
//------------------------------------------------------------------------------vol_db_ch2---
// Volume channel 2 
#define MA_vol_db_ch2__a 68
#define MA_vol_db_ch2__len 8
#define MA_vol_db_ch2__mask 0xff
#define MA_vol_db_ch2__shift 0x00
#define MA_vol_db_ch2__reset 0x18
#define set_obj_MA_vol_db_ch2(o,y) o.write(68,y);
#define set_MA_vol_db_ch2(y) ma_write_byte(68,y);
#define get_obj_MA_vol_db_ch2(o) (o.read(68) & 0xff)>>0 
#define get_MA_vol_db_ch2() ( ma_read_byte(68) & 0xff)>>0 
//------------------------------------------------------------------------------vol_db_ch3---
// Volume channel 3 
#define MA_vol_db_ch3__a 69
#define MA_vol_db_ch3__len 8
#define MA_vol_db_ch3__mask 0xff
#define MA_vol_db_ch3__shift 0x00
#define MA_vol_db_ch3__reset 0x18
#define set_obj_MA_vol_db_ch3(o,y) o.write(69,y);
#define set_MA_vol_db_ch3(y) ma_write_byte(69,y);
#define get_obj_MA_vol_db_ch3(o) (o.read(69) & 0xff)>>0 
#define get_MA_vol_db_ch3() ( ma_read_byte(69) & 0xff)>>0 
//------------------------------------------------------------------------------vol_lsb_ch0---
// volume channel 1 - 1/4 steps 
#define MA_vol_lsb_ch0__a 70
#define MA_vol_lsb_ch0__len 2
#define MA_vol_lsb_ch0__mask 0x03
#define MA_vol_lsb_ch0__shift 0x00
#define MA_vol_lsb_ch0__reset 0x00
#define set_obj_MA_vol_lsb_ch0(o,y) ({ uint8_t __ret = o.read(70); o.write(70,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_vol_lsb_ch0(y) ({ uint8_t __ret = ma_read_byte(70); ma_write_byte(70,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_vol_lsb_ch0(o) (o.read(70) & 0x03)>>0 
#define get_MA_vol_lsb_ch0() ( ma_read_byte(70) & 0x03)>>0 
//------------------------------------------------------------------------------vol_lsb_ch1---
// volume channel 3 - 1/4 steps 
#define MA_vol_lsb_ch1__a 70
#define MA_vol_lsb_ch1__len 2
#define MA_vol_lsb_ch1__mask 0x0c
#define MA_vol_lsb_ch1__shift 0x02
#define MA_vol_lsb_ch1__reset 0x00
#define set_obj_MA_vol_lsb_ch1(o,y) ({ uint8_t __ret = o.read(70); o.write(70,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define set_MA_vol_lsb_ch1(y) ({ uint8_t __ret = ma_read_byte(70); ma_write_byte(70,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define get_obj_MA_vol_lsb_ch1(o) (o.read(70) & 0x0c)>>2 
#define get_MA_vol_lsb_ch1() ( ma_read_byte(70) & 0x0c)>>2 
//------------------------------------------------------------------------------vol_lsb_ch2---
// volume channel 2 - 1/4 steps 
#define MA_vol_lsb_ch2__a 70
#define MA_vol_lsb_ch2__len 2
#define MA_vol_lsb_ch2__mask 0x30
#define MA_vol_lsb_ch2__shift 0x04
#define MA_vol_lsb_ch2__reset 0x00
#define set_obj_MA_vol_lsb_ch2(o,y) ({ uint8_t __ret = o.read(70); o.write(70,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define set_MA_vol_lsb_ch2(y) ({ uint8_t __ret = ma_read_byte(70); ma_write_byte(70,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define get_obj_MA_vol_lsb_ch2(o) (o.read(70) & 0x30)>>4 
#define get_MA_vol_lsb_ch2() ( ma_read_byte(70) & 0x30)>>4 
//------------------------------------------------------------------------------vol_lsb_ch3---
// volume channel 3 - 1/4 steps 
#define MA_vol_lsb_ch3__a 70
#define MA_vol_lsb_ch3__len 2
#define MA_vol_lsb_ch3__mask 0xc0
#define MA_vol_lsb_ch3__shift 0x06
#define MA_vol_lsb_ch3__reset 0x00
#define set_obj_MA_vol_lsb_ch3(o,y) ({ uint8_t __ret = o.read(70); o.write(70,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define set_MA_vol_lsb_ch3(y) ({ uint8_t __ret = ma_read_byte(70); ma_write_byte(70,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define get_obj_MA_vol_lsb_ch3(o) (o.read(70) & 0xc0)>>6 
#define get_MA_vol_lsb_ch3() ( ma_read_byte(70) & 0xc0)>>6 
//------------------------------------------------------------------------------thr_db_ch0---
// thr_db channel 0 
#define MA_thr_db_ch0__a 71
#define MA_thr_db_ch0__len 8
#define MA_thr_db_ch0__mask 0xff
#define MA_thr_db_ch0__shift 0x00
#define MA_thr_db_ch0__reset 0x18
#define set_obj_MA_thr_db_ch0(o,y) o.write(71,y);
#define set_MA_thr_db_ch0(y) ma_write_byte(71,y);
#define get_obj_MA_thr_db_ch0(o) (o.read(71) & 0xff)>>0 
#define get_MA_thr_db_ch0() ( ma_read_byte(71) & 0xff)>>0 
//------------------------------------------------------------------------------thr_db_ch1---
// Thr db ch1 
#define MA_thr_db_ch1__a 72
#define MA_thr_db_ch1__len 8
#define MA_thr_db_ch1__mask 0xff
#define MA_thr_db_ch1__shift 0x00
#define MA_thr_db_ch1__reset 0x18
#define set_obj_MA_thr_db_ch1(o,y) o.write(72,y);
#define set_MA_thr_db_ch1(y) ma_write_byte(72,y);
#define get_obj_MA_thr_db_ch1(o) (o.read(72) & 0xff)>>0 
#define get_MA_thr_db_ch1() ( ma_read_byte(72) & 0xff)>>0 
//------------------------------------------------------------------------------thr_db_ch2---
// thr db ch2 
#define MA_thr_db_ch2__a 73
#define MA_thr_db_ch2__len 8
#define MA_thr_db_ch2__mask 0xff
#define MA_thr_db_ch2__shift 0x00
#define MA_thr_db_ch2__reset 0x18
#define set_obj_MA_thr_db_ch2(o,y) o.write(73,y);
#define set_MA_thr_db_ch2(y) ma_write_byte(73,y);
#define get_obj_MA_thr_db_ch2(o) (o.read(73) & 0xff)>>0 
#define get_MA_thr_db_ch2() ( ma_read_byte(73) & 0xff)>>0 
//------------------------------------------------------------------------------thr_db_ch3---
// Threshold db ch3   
#define MA_thr_db_ch3__a 74
#define MA_thr_db_ch3__len 8
#define MA_thr_db_ch3__mask 0xff
#define MA_thr_db_ch3__shift 0x00
#define MA_thr_db_ch3__reset 0x18
#define set_obj_MA_thr_db_ch3(o,y) o.write(74,y);
#define set_MA_thr_db_ch3(y) ma_write_byte(74,y);
#define get_obj_MA_thr_db_ch3(o) (o.read(74) & 0xff)>>0 
#define get_MA_thr_db_ch3() ( ma_read_byte(74) & 0xff)>>0 
//------------------------------------------------------------------------------thr_lsb_ch0---
// Thr lsb ch0 
#define MA_thr_lsb_ch0__a 75
#define MA_thr_lsb_ch0__len 2
#define MA_thr_lsb_ch0__mask 0x03
#define MA_thr_lsb_ch0__shift 0x00
#define MA_thr_lsb_ch0__reset 0x00
#define set_obj_MA_thr_lsb_ch0(o,y) ({ uint8_t __ret = o.read(75); o.write(75,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_thr_lsb_ch0(y) ({ uint8_t __ret = ma_read_byte(75); ma_write_byte(75,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_thr_lsb_ch0(o) (o.read(75) & 0x03)>>0 
#define get_MA_thr_lsb_ch0() ( ma_read_byte(75) & 0x03)>>0 
//------------------------------------------------------------------------------thr_lsb_ch1---
// thr lsb ch1 
#define MA_thr_lsb_ch1__a 75
#define MA_thr_lsb_ch1__len 2
#define MA_thr_lsb_ch1__mask 0x0c
#define MA_thr_lsb_ch1__shift 0x02
#define MA_thr_lsb_ch1__reset 0x00
#define set_obj_MA_thr_lsb_ch1(o,y) ({ uint8_t __ret = o.read(75); o.write(75,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define set_MA_thr_lsb_ch1(y) ({ uint8_t __ret = ma_read_byte(75); ma_write_byte(75,(__ret&0xf3)|((y<<2)&0x0c)); }) 
#define get_obj_MA_thr_lsb_ch1(o) (o.read(75) & 0x0c)>>2 
#define get_MA_thr_lsb_ch1() ( ma_read_byte(75) & 0x0c)>>2 
//------------------------------------------------------------------------------thr_lsb_ch2---
// thr lsb ch2 1/4 db step 
#define MA_thr_lsb_ch2__a 75
#define MA_thr_lsb_ch2__len 2
#define MA_thr_lsb_ch2__mask 0x30
#define MA_thr_lsb_ch2__shift 0x04
#define MA_thr_lsb_ch2__reset 0x00
#define set_obj_MA_thr_lsb_ch2(o,y) ({ uint8_t __ret = o.read(75); o.write(75,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define set_MA_thr_lsb_ch2(y) ({ uint8_t __ret = ma_read_byte(75); ma_write_byte(75,(__ret&0xcf)|((y<<4)&0x30)); }) 
#define get_obj_MA_thr_lsb_ch2(o) (o.read(75) & 0x30)>>4 
#define get_MA_thr_lsb_ch2() ( ma_read_byte(75) & 0x30)>>4 
//------------------------------------------------------------------------------thr_lsb_ch3---
// threshold lsb ch3 
#define MA_thr_lsb_ch3__a 75
#define MA_thr_lsb_ch3__len 2
#define MA_thr_lsb_ch3__mask 0xc0
#define MA_thr_lsb_ch3__shift 0x06
#define MA_thr_lsb_ch3__reset 0x00
#define set_obj_MA_thr_lsb_ch3(o,y) ({ uint8_t __ret = o.read(75); o.write(75,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define set_MA_thr_lsb_ch3(y) ({ uint8_t __ret = ma_read_byte(75); ma_write_byte(75,(__ret&0x3f)|((y<<6)&0xc0)); }) 
#define get_obj_MA_thr_lsb_ch3(o) (o.read(75) & 0xc0)>>6 
#define get_MA_thr_lsb_ch3() ( ma_read_byte(75) & 0xc0)>>6 
//------------------------------------------------------------------------------dcu_mon0.PM_mon---
// Power mode monitor channel 0 
#define MA_dcu_mon0__PM_mon__a 96
#define MA_dcu_mon0__PM_mon__len 2
#define MA_dcu_mon0__PM_mon__mask 0x03
#define MA_dcu_mon0__PM_mon__shift 0x00
#define MA_dcu_mon0__PM_mon__reset 0x00
#define set_obj_MA_dcu_mon0__PM_mon(o,y) ({ uint8_t __ret = o.read(96); o.write(96,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_dcu_mon0__PM_mon(y) ({ uint8_t __ret = ma_read_byte(96); ma_write_byte(96,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_dcu_mon0__PM_mon(o) (o.read(96) & 0x03)>>0 
#define get_MA_dcu_mon0__PM_mon() ( ma_read_byte(96) & 0x03)>>0 
//------------------------------------------------------------------------------dcu_mon0.freqMode_mon---
// Frequence mode monitor channel 0 
#define MA_dcu_mon0__freqMode_mon__a 96
#define MA_dcu_mon0__freqMode_mon__len 3
#define MA_dcu_mon0__freqMode_mon__mask 0x70
#define MA_dcu_mon0__freqMode_mon__shift 0x04
#define MA_dcu_mon0__freqMode_mon__reset 0x00
#define set_obj_MA_dcu_mon0__freqMode_mon(o,y) ({ uint8_t __ret = o.read(96); o.write(96,(__ret&0x8f)|((y<<4)&0x70)); }) 
#define set_MA_dcu_mon0__freqMode_mon(y) ({ uint8_t __ret = ma_read_byte(96); ma_write_byte(96,(__ret&0x8f)|((y<<4)&0x70)); }) 
#define get_obj_MA_dcu_mon0__freqMode_mon(o) (o.read(96) & 0x70)>>4 
#define get_MA_dcu_mon0__freqMode_mon() ( ma_read_byte(96) & 0x70)>>4 
//------------------------------------------------------------------------------dcu_mon0.pps_passed---
// DCU0 PPS completion indicator 
#define MA_dcu_mon0__pps_passed__a 96
#define MA_dcu_mon0__pps_passed__len 1
#define MA_dcu_mon0__pps_passed__mask 0x80
#define MA_dcu_mon0__pps_passed__shift 0x07
#define MA_dcu_mon0__pps_passed__reset 0x00
#define set_obj_MA_dcu_mon0__pps_passed(o,y) ({ uint8_t __ret = o.read(96); o.write(96,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_dcu_mon0__pps_passed(y) ({ uint8_t __ret = ma_read_byte(96); ma_write_byte(96,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_dcu_mon0__pps_passed(o) (o.read(96) & 0x80)>>7 
#define get_MA_dcu_mon0__pps_passed() ( ma_read_byte(96) & 0x80)>>7 
//------------------------------------------------------------------------------dcu_mon0.OCP_mon---
// OCP Monitor channel 0 
#define MA_dcu_mon0__OCP_mon__a 97
#define MA_dcu_mon0__OCP_mon__len 1
#define MA_dcu_mon0__OCP_mon__mask 0x01
#define MA_dcu_mon0__OCP_mon__shift 0x00
#define MA_dcu_mon0__OCP_mon__reset 0x00
#define set_obj_MA_dcu_mon0__OCP_mon(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_dcu_mon0__OCP_mon(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_dcu_mon0__OCP_mon(o) (o.read(97) & 0x01)>>0 
#define get_MA_dcu_mon0__OCP_mon() ( ma_read_byte(97) & 0x01)>>0 
//------------------------------------------------------------------------------dcu_mon0.Vcfly1_ok---
// Cfly1 protection monitor channel 0. 
#define MA_dcu_mon0__Vcfly1_ok__a 97
#define MA_dcu_mon0__Vcfly1_ok__len 1
#define MA_dcu_mon0__Vcfly1_ok__mask 0x02
#define MA_dcu_mon0__Vcfly1_ok__shift 0x01
#define MA_dcu_mon0__Vcfly1_ok__reset 0x00
#define set_obj_MA_dcu_mon0__Vcfly1_ok(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define set_MA_dcu_mon0__Vcfly1_ok(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define get_obj_MA_dcu_mon0__Vcfly1_ok(o) (o.read(97) & 0x02)>>1 
#define get_MA_dcu_mon0__Vcfly1_ok() ( ma_read_byte(97) & 0x02)>>1 
//------------------------------------------------------------------------------dcu_mon0.Vcfly2_ok---
// Cfly2 protection monitor channel 0. 
#define MA_dcu_mon0__Vcfly2_ok__a 97
#define MA_dcu_mon0__Vcfly2_ok__len 1
#define MA_dcu_mon0__Vcfly2_ok__mask 0x04
#define MA_dcu_mon0__Vcfly2_ok__shift 0x02
#define MA_dcu_mon0__Vcfly2_ok__reset 0x00
#define set_obj_MA_dcu_mon0__Vcfly2_ok(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define set_MA_dcu_mon0__Vcfly2_ok(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define get_obj_MA_dcu_mon0__Vcfly2_ok(o) (o.read(97) & 0x04)>>2 
#define get_MA_dcu_mon0__Vcfly2_ok() ( ma_read_byte(97) & 0x04)>>2 
//------------------------------------------------------------------------------dcu_mon0.pvdd_ok---
// DCU0 PVDD monitor 
#define MA_dcu_mon0__pvdd_ok__a 97
#define MA_dcu_mon0__pvdd_ok__len 1
#define MA_dcu_mon0__pvdd_ok__mask 0x08
#define MA_dcu_mon0__pvdd_ok__shift 0x03
#define MA_dcu_mon0__pvdd_ok__reset 0x00
#define set_obj_MA_dcu_mon0__pvdd_ok(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define set_MA_dcu_mon0__pvdd_ok(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define get_obj_MA_dcu_mon0__pvdd_ok(o) (o.read(97) & 0x08)>>3 
#define get_MA_dcu_mon0__pvdd_ok() ( ma_read_byte(97) & 0x08)>>3 
//------------------------------------------------------------------------------dcu_mon0.vdd_ok---
// DCU0 VDD monitor 
#define MA_dcu_mon0__vdd_ok__a 97
#define MA_dcu_mon0__vdd_ok__len 1
#define MA_dcu_mon0__vdd_ok__mask 0x10
#define MA_dcu_mon0__vdd_ok__shift 0x04
#define MA_dcu_mon0__vdd_ok__reset 0x00
#define set_obj_MA_dcu_mon0__vdd_ok(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xef)|((y<<4)&0x10)); }) 
#define set_MA_dcu_mon0__vdd_ok(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xef)|((y<<4)&0x10)); }) 
#define get_obj_MA_dcu_mon0__vdd_ok(o) (o.read(97) & 0x10)>>4 
#define get_MA_dcu_mon0__vdd_ok() ( ma_read_byte(97) & 0x10)>>4 
//------------------------------------------------------------------------------dcu_mon0.mute---
// DCU0 mute monitor 
#define MA_dcu_mon0__mute__a 97
#define MA_dcu_mon0__mute__len 1
#define MA_dcu_mon0__mute__mask 0x20
#define MA_dcu_mon0__mute__shift 0x05
#define MA_dcu_mon0__mute__reset 0x00
#define set_obj_MA_dcu_mon0__mute(o,y) ({ uint8_t __ret = o.read(97); o.write(97,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define set_MA_dcu_mon0__mute(y) ({ uint8_t __ret = ma_read_byte(97); ma_write_byte(97,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define get_obj_MA_dcu_mon0__mute(o) (o.read(97) & 0x20)>>5 
#define get_MA_dcu_mon0__mute() ( ma_read_byte(97) & 0x20)>>5 
//------------------------------------------------------------------------------dcu_mon0.M_mon---
// M sense monitor channel 0 
#define MA_dcu_mon0__M_mon__a 98
#define MA_dcu_mon0__M_mon__len 8
#define MA_dcu_mon0__M_mon__mask 0xff
#define MA_dcu_mon0__M_mon__shift 0x00
#define MA_dcu_mon0__M_mon__reset 0x00
#define set_obj_MA_dcu_mon0__M_mon(o,y) o.write(98,y);
#define set_MA_dcu_mon0__M_mon(y) ma_write_byte(98,y);
#define get_obj_MA_dcu_mon0__M_mon(o) (o.read(98) & 0xff)>>0 
#define get_MA_dcu_mon0__M_mon() ( ma_read_byte(98) & 0xff)>>0 
//------------------------------------------------------------------------------dcu_mon1.PM_mon---
// Power mode monitor channel 1 
#define MA_dcu_mon1__PM_mon__a 100
#define MA_dcu_mon1__PM_mon__len 2
#define MA_dcu_mon1__PM_mon__mask 0x03
#define MA_dcu_mon1__PM_mon__shift 0x00
#define MA_dcu_mon1__PM_mon__reset 0x00
#define set_obj_MA_dcu_mon1__PM_mon(o,y) ({ uint8_t __ret = o.read(100); o.write(100,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_dcu_mon1__PM_mon(y) ({ uint8_t __ret = ma_read_byte(100); ma_write_byte(100,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_dcu_mon1__PM_mon(o) (o.read(100) & 0x03)>>0 
#define get_MA_dcu_mon1__PM_mon() ( ma_read_byte(100) & 0x03)>>0 
//------------------------------------------------------------------------------dcu_mon1.freqMode_mon---
// Frequence mode monitor channel 1 
#define MA_dcu_mon1__freqMode_mon__a 100
#define MA_dcu_mon1__freqMode_mon__len 3
#define MA_dcu_mon1__freqMode_mon__mask 0x70
#define MA_dcu_mon1__freqMode_mon__shift 0x04
#define MA_dcu_mon1__freqMode_mon__reset 0x00
#define set_obj_MA_dcu_mon1__freqMode_mon(o,y) ({ uint8_t __ret = o.read(100); o.write(100,(__ret&0x8f)|((y<<4)&0x70)); }) 
#define set_MA_dcu_mon1__freqMode_mon(y) ({ uint8_t __ret = ma_read_byte(100); ma_write_byte(100,(__ret&0x8f)|((y<<4)&0x70)); }) 
#define get_obj_MA_dcu_mon1__freqMode_mon(o) (o.read(100) & 0x70)>>4 
#define get_MA_dcu_mon1__freqMode_mon() ( ma_read_byte(100) & 0x70)>>4 
//------------------------------------------------------------------------------dcu_mon1.pps_passed---
// DCU1 PPS completion indicator 
#define MA_dcu_mon1__pps_passed__a 100
#define MA_dcu_mon1__pps_passed__len 1
#define MA_dcu_mon1__pps_passed__mask 0x80
#define MA_dcu_mon1__pps_passed__shift 0x07
#define MA_dcu_mon1__pps_passed__reset 0x00
#define set_obj_MA_dcu_mon1__pps_passed(o,y) ({ uint8_t __ret = o.read(100); o.write(100,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_dcu_mon1__pps_passed(y) ({ uint8_t __ret = ma_read_byte(100); ma_write_byte(100,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_dcu_mon1__pps_passed(o) (o.read(100) & 0x80)>>7 
#define get_MA_dcu_mon1__pps_passed() ( ma_read_byte(100) & 0x80)>>7 
//------------------------------------------------------------------------------dcu_mon1.OCP_mon---
// OCP Monitor channel 1 
#define MA_dcu_mon1__OCP_mon__a 101
#define MA_dcu_mon1__OCP_mon__len 1
#define MA_dcu_mon1__OCP_mon__mask 0x01
#define MA_dcu_mon1__OCP_mon__shift 0x00
#define MA_dcu_mon1__OCP_mon__reset 0x00
#define set_obj_MA_dcu_mon1__OCP_mon(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define set_MA_dcu_mon1__OCP_mon(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xfe)|((y<<0)&0x01)); }) 
#define get_obj_MA_dcu_mon1__OCP_mon(o) (o.read(101) & 0x01)>>0 
#define get_MA_dcu_mon1__OCP_mon() ( ma_read_byte(101) & 0x01)>>0 
//------------------------------------------------------------------------------dcu_mon1.Vcfly1_ok---
// Cfly1 protcetion monitor channel 1 
#define MA_dcu_mon1__Vcfly1_ok__a 101
#define MA_dcu_mon1__Vcfly1_ok__len 1
#define MA_dcu_mon1__Vcfly1_ok__mask 0x02
#define MA_dcu_mon1__Vcfly1_ok__shift 0x01
#define MA_dcu_mon1__Vcfly1_ok__reset 0x00
#define set_obj_MA_dcu_mon1__Vcfly1_ok(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define set_MA_dcu_mon1__Vcfly1_ok(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xfd)|((y<<1)&0x02)); }) 
#define get_obj_MA_dcu_mon1__Vcfly1_ok(o) (o.read(101) & 0x02)>>1 
#define get_MA_dcu_mon1__Vcfly1_ok() ( ma_read_byte(101) & 0x02)>>1 
//------------------------------------------------------------------------------dcu_mon1.Vcfly2_ok---
// Cfly2 protection monitor channel 1 
#define MA_dcu_mon1__Vcfly2_ok__a 101
#define MA_dcu_mon1__Vcfly2_ok__len 1
#define MA_dcu_mon1__Vcfly2_ok__mask 0x04
#define MA_dcu_mon1__Vcfly2_ok__shift 0x02
#define MA_dcu_mon1__Vcfly2_ok__reset 0x00
#define set_obj_MA_dcu_mon1__Vcfly2_ok(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define set_MA_dcu_mon1__Vcfly2_ok(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xfb)|((y<<2)&0x04)); }) 
#define get_obj_MA_dcu_mon1__Vcfly2_ok(o) (o.read(101) & 0x04)>>2 
#define get_MA_dcu_mon1__Vcfly2_ok() ( ma_read_byte(101) & 0x04)>>2 
//------------------------------------------------------------------------------dcu_mon1.pvdd_ok---
// DCU1 PVDD monitor 
#define MA_dcu_mon1__pvdd_ok__a 101
#define MA_dcu_mon1__pvdd_ok__len 1
#define MA_dcu_mon1__pvdd_ok__mask 0x08
#define MA_dcu_mon1__pvdd_ok__shift 0x03
#define MA_dcu_mon1__pvdd_ok__reset 0x00
#define set_obj_MA_dcu_mon1__pvdd_ok(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define set_MA_dcu_mon1__pvdd_ok(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xf7)|((y<<3)&0x08)); }) 
#define get_obj_MA_dcu_mon1__pvdd_ok(o) (o.read(101) & 0x08)>>3 
#define get_MA_dcu_mon1__pvdd_ok() ( ma_read_byte(101) & 0x08)>>3 
//------------------------------------------------------------------------------dcu_mon1.vdd_ok---
// DCU1 VDD monitor 
#define MA_dcu_mon1__vdd_ok__a 101
#define MA_dcu_mon1__vdd_ok__len 1
#define MA_dcu_mon1__vdd_ok__mask 0x10
#define MA_dcu_mon1__vdd_ok__shift 0x04
#define MA_dcu_mon1__vdd_ok__reset 0x00
#define set_obj_MA_dcu_mon1__vdd_ok(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xef)|((y<<4)&0x10)); }) 
#define set_MA_dcu_mon1__vdd_ok(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xef)|((y<<4)&0x10)); }) 
#define get_obj_MA_dcu_mon1__vdd_ok(o) (o.read(101) & 0x10)>>4 
#define get_MA_dcu_mon1__vdd_ok() ( ma_read_byte(101) & 0x10)>>4 
//------------------------------------------------------------------------------dcu_mon1.mute---
// DCU1 mute monitor 
#define MA_dcu_mon1__mute__a 101
#define MA_dcu_mon1__mute__len 1
#define MA_dcu_mon1__mute__mask 0x20
#define MA_dcu_mon1__mute__shift 0x05
#define MA_dcu_mon1__mute__reset 0x00
#define set_obj_MA_dcu_mon1__mute(o,y) ({ uint8_t __ret = o.read(101); o.write(101,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define set_MA_dcu_mon1__mute(y) ({ uint8_t __ret = ma_read_byte(101); ma_write_byte(101,(__ret&0xdf)|((y<<5)&0x20)); }) 
#define get_obj_MA_dcu_mon1__mute(o) (o.read(101) & 0x20)>>5 
#define get_MA_dcu_mon1__mute() ( ma_read_byte(101) & 0x20)>>5 
//------------------------------------------------------------------------------dcu_mon1.M_mon---
// M sense monitor channel 1 
#define MA_dcu_mon1__M_mon__a 102
#define MA_dcu_mon1__M_mon__len 8
#define MA_dcu_mon1__M_mon__mask 0xff
#define MA_dcu_mon1__M_mon__shift 0x00
#define MA_dcu_mon1__M_mon__reset 0x00
#define set_obj_MA_dcu_mon1__M_mon(o,y) o.write(102,y);
#define set_MA_dcu_mon1__M_mon(y) ma_write_byte(102,y);
#define get_obj_MA_dcu_mon1__M_mon(o) (o.read(102) & 0xff)>>0 
#define get_MA_dcu_mon1__M_mon() ( ma_read_byte(102) & 0xff)>>0 
//------------------------------------------------------------------------------dcu_mon0.sw_enable---
// DCU0 Switch enable monitor 
#define MA_dcu_mon0__sw_enable__a 104
#define MA_dcu_mon0__sw_enable__len 1
#define MA_dcu_mon0__sw_enable__mask 0x40
#define MA_dcu_mon0__sw_enable__shift 0x06
#define MA_dcu_mon0__sw_enable__reset 0x00
#define set_obj_MA_dcu_mon0__sw_enable(o,y) ({ uint8_t __ret = o.read(104); o.write(104,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_dcu_mon0__sw_enable(y) ({ uint8_t __ret = ma_read_byte(104); ma_write_byte(104,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_dcu_mon0__sw_enable(o) (o.read(104) & 0x40)>>6 
#define get_MA_dcu_mon0__sw_enable() ( ma_read_byte(104) & 0x40)>>6 
//------------------------------------------------------------------------------dcu_mon1.sw_enable---
// DCU1 Switch enable monitor 
#define MA_dcu_mon1__sw_enable__a 104
#define MA_dcu_mon1__sw_enable__len 1
#define MA_dcu_mon1__sw_enable__mask 0x80
#define MA_dcu_mon1__sw_enable__shift 0x07
#define MA_dcu_mon1__sw_enable__reset 0x00
#define set_obj_MA_dcu_mon1__sw_enable(o,y) ({ uint8_t __ret = o.read(104); o.write(104,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_dcu_mon1__sw_enable(y) ({ uint8_t __ret = ma_read_byte(104); ma_write_byte(104,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_dcu_mon1__sw_enable(o) (o.read(104) & 0x80)>>7 
#define get_MA_dcu_mon1__sw_enable() ( ma_read_byte(104) & 0x80)>>7 
//------------------------------------------------------------------------------hvboot0_ok_mon---
// HVboot0_ok for test/debug 
#define MA_hvboot0_ok_mon__a 105
#define MA_hvboot0_ok_mon__len 1
#define MA_hvboot0_ok_mon__mask 0x40
#define MA_hvboot0_ok_mon__shift 0x06
#define MA_hvboot0_ok_mon__reset 0x00
#define set_obj_MA_hvboot0_ok_mon(o,y) ({ uint8_t __ret = o.read(105); o.write(105,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define set_MA_hvboot0_ok_mon(y) ({ uint8_t __ret = ma_read_byte(105); ma_write_byte(105,(__ret&0xbf)|((y<<6)&0x40)); }) 
#define get_obj_MA_hvboot0_ok_mon(o) (o.read(105) & 0x40)>>6 
#define get_MA_hvboot0_ok_mon() ( ma_read_byte(105) & 0x40)>>6 
//------------------------------------------------------------------------------hvboot1_ok_mon---
// HVboot1_ok for test/debug 
#define MA_hvboot1_ok_mon__a 105
#define MA_hvboot1_ok_mon__len 1
#define MA_hvboot1_ok_mon__mask 0x80
#define MA_hvboot1_ok_mon__shift 0x07
#define MA_hvboot1_ok_mon__reset 0x00
#define set_obj_MA_hvboot1_ok_mon(o,y) ({ uint8_t __ret = o.read(105); o.write(105,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define set_MA_hvboot1_ok_mon(y) ({ uint8_t __ret = ma_read_byte(105); ma_write_byte(105,(__ret&0x7f)|((y<<7)&0x80)); }) 
#define get_obj_MA_hvboot1_ok_mon(o) (o.read(105) & 0x80)>>7 
#define get_MA_hvboot1_ok_mon() ( ma_read_byte(105) & 0x80)>>7 
//------------------------------------------------------------------------------error_acc---
// Accumulated errors, at and after triggering 
#define MA_error_acc__a 109
#define MA_error_acc__len 8
#define MA_error_acc__mask 0xff
#define MA_error_acc__shift 0x00
#define MA_error_acc__reset 0x00
#define set_obj_MA_error_acc(o,y) o.write(109,y);
#define set_MA_error_acc(y) ma_write_byte(109,y);
#define get_obj_MA_error_acc(o) (o.read(109) & 0xff)>>0 
#define get_MA_error_acc() ( ma_read_byte(109) & 0xff)>>0 
//------------------------------------------------------------------------------i2s_data_rate---
// Detected i2s data rate: 00/01/10 = x1/x2/x4 
#define MA_i2s_data_rate__a 116
#define MA_i2s_data_rate__len 2
#define MA_i2s_data_rate__mask 0x03
#define MA_i2s_data_rate__shift 0x00
#define MA_i2s_data_rate__reset 0x00
#define set_obj_MA_i2s_data_rate(o,y) ({ uint8_t __ret = o.read(116); o.write(116,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define set_MA_i2s_data_rate(y) ({ uint8_t __ret = ma_read_byte(116); ma_write_byte(116,(__ret&0xfc)|((y<<0)&0x03)); }) 
#define get_obj_MA_i2s_data_rate(o) (o.read(116) & 0x03)>>0 
#define get_MA_i2s_data_rate() ( ma_read_byte(116) & 0x03)>>0 
//------------------------------------------------------------------------------audio_in_mode_mon---
// Audio input mode monitor 
#define MA_audio_in_mode_mon__a 116
#define MA_audio_in_mode_mon__len 3
#define MA_audio_in_mode_mon__mask 0x1c
#define MA_audio_in_mode_mon__shift 0x02
#define MA_audio_in_mode_mon__reset 0x00
#define set_obj_MA_audio_in_mode_mon(o,y) ({ uint8_t __ret = o.read(116); o.write(116,(__ret&0xe3)|((y<<2)&0x1c)); }) 
#define set_MA_audio_in_mode_mon(y) ({ uint8_t __ret = ma_read_byte(116); ma_write_byte(116,(__ret&0xe3)|((y<<2)&0x1c)); }) 
#define get_obj_MA_audio_in_mode_mon(o) (o.read(116) & 0x1c)>>2 
#define get_MA_audio_in_mode_mon() ( ma_read_byte(116) & 0x1c)>>2 
//------------------------------------------------------------------------------msel_mon---
// MSEL[2:0] monitor register 
#define MA_msel_mon__a 117
#define MA_msel_mon__len 3
#define MA_msel_mon__mask 0x07
#define MA_msel_mon__shift 0x00
#define MA_msel_mon__reset 0x00
#define set_obj_MA_msel_mon(o,y) ({ uint8_t __ret = o.read(117); o.write(117,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define set_MA_msel_mon(y) ({ uint8_t __ret = ma_read_byte(117); ma_write_byte(117,(__ret&0xf8)|((y<<0)&0x07)); }) 
#define get_obj_MA_msel_mon(o) (o.read(117) & 0x07)>>0 
#define get_MA_msel_mon() ( ma_read_byte(117) & 0x07)>>0 
//------------------------------------------------------------------------------error---
// Current error flag monitor reg - for app. ctrl. 
#define MA_error__a 124
#define MA_error__len 8
#define MA_error__mask 0xff
#define MA_error__shift 0x00
#define MA_error__reset 0x00
#define set_obj_MA_error(o,y) o.write(124,y);
#define set_MA_error(y) ma_write_byte(124,y);
#define get_obj_MA_error(o) (o.read(124) & 0xff)>>0 
#define get_MA_error() ( ma_read_byte(124) & 0xff)>>0 
//------------------------------------------------------------------------------audio_proc_limiter_mon---
// b7-b4: Channel 3-0 limiter active  
#define MA_audio_proc_limiter_mon__a 126
#define MA_audio_proc_limiter_mon__len 4
#define MA_audio_proc_limiter_mon__mask 0xf0
#define MA_audio_proc_limiter_mon__shift 0x04
#define MA_audio_proc_limiter_mon__reset 0x00
#define set_obj_MA_audio_proc_limiter_mon(o,y) ({ uint8_t __ret = o.read(126); o.write(126,(__ret&0x0f)|((y<<4)&0xf0)); }) 
#define set_MA_audio_proc_limiter_mon(y) ({ uint8_t __ret = ma_read_byte(126); ma_write_byte(126,(__ret&0x0f)|((y<<4)&0xf0)); }) 
#define get_obj_MA_audio_proc_limiter_mon(o) (o.read(126) & 0xf0)>>4 
#define get_MA_audio_proc_limiter_mon() ( ma_read_byte(126) & 0xf0)>>4 
//------------------------------------------------------------------------------audio_proc_clip_mon---
// b3-b0: Channel 3-0 clipping monitor 
#define MA_audio_proc_clip_mon__a 126
#define MA_audio_proc_clip_mon__len 4
#define MA_audio_proc_clip_mon__mask 0x0f
#define MA_audio_proc_clip_mon__shift 0x00
#define MA_audio_proc_clip_mon__reset 0x00
#define set_obj_MA_audio_proc_clip_mon(o,y) ({ uint8_t __ret = o.read(126); o.write(126,(__ret&0xf0)|((y<<0)&0x0f)); }) 
#define set_MA_audio_proc_clip_mon(y) ({ uint8_t __ret = ma_read_byte(126); ma_write_byte(126,(__ret&0xf0)|((y<<0)&0x0f)); }) 
#define get_obj_MA_audio_proc_clip_mon(o) (o.read(126) & 0x0f)>>0 
#define get_MA_audio_proc_clip_mon() ( ma_read_byte(126) & 0x0f)>>0 
#endif   /* _MA120X0_H_ */
