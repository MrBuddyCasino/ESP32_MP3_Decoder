/*
 * abs_xtensa.h
 *
 *  Created on: 13.05.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_XTENSA_ABS_XTENSA_H_
#define _INCLUDE_XTENSA_ABS_XTENSA_H_


#if defined(__disabled__)

    //#include "xtensa/tie/xt_core.h"
    // extern int _TIE_xt_core_ABS(int t);

    inline INT fixabs_D(INT x)
    {
        INT result;
        __asm__ ("ABS %0, %1" : "=r" (result) : "r" (x));
        return result;
    }

    #define fixabs_S(x) fixabs_D(x)
    #define fixabs_I(x) fixabs_D(x)

    #define FUNCTION_fixabs_S
    #define FUNCTION_fixabs_D
    #define FUNCTION_fixabs_I

#endif /*__xtensa__ */


#endif /* _INCLUDE_XTENSA_ABS_XTENSA_H_ */
