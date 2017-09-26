#ifndef _SERVER_H_
#define _SERVER_H_


#ifndef _PARSE_H_
#include "parse.h"
#endif


void handle_request_error(int id, Request *request, char *http_response, char *desc);

#endif
