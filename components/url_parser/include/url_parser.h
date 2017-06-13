/*
 * url_parser.h
 *
 *  Created on: 29.03.2017
 *      Author: michaelboeckling
 */

#ifndef _URL_PARSER_H_
#define _URL_PARSER_H_


typedef struct
{
    /* The host portion of the |uri|, NULL-terminated */
    char *host;

    /* The schema portion of the |uri|, NULL-terminated */
    char *scheme;

    /* The port portion of the |uri|, or the schema's default port */
    uint16_t port;

    /* The authority portion of the |uri|, NULL-terminated */
    char *authority;

    /* The path portion of the |uri|, including query, NULL-terminated */
    char *path;

} url_t;


url_t *url_parse(char *uri);

void url_free(url_t *url);


#endif /* _URL_PARSER_H_ */
