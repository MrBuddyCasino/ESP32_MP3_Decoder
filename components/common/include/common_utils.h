/*
 * common_utils.h
 *
 *  Created on: 13.06.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_COMMON_UTILS_H_
#define _INCLUDE_COMMON_UTILS_H_

int starts_with(const char *str, const char *pre)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

#endif /* _INCLUDE_COMMON_UTILS_H_ */
