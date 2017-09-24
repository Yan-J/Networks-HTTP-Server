#ifndef _HTTP_H_
#define _HTTP_H_


// HTTP status codes
#define HTTP_STATUS_200 "200"	/* OK - respond with requested resource */
#define HTTP_STATUS_400 "400"	/* BAD_REQUEST - incorrect or corrupted */
#define HTTP_STATUS_404 "404"	/* NOT_FOUND - requested resource does not exist */
#define HTTP_STATUS_411 "411"	/* LENGTH_REQUIRED - Content-Length not defined for POST */
#define HTTP_STATUS_500 "500"	/* INTERNAL_SERVER_ERROR - server failure */
#define HTTP_STATUS_501 "501"	/* NOT_IMPLEMENTED - request type not supported */
#define HTTP_STATUS_503 "503"	/* SERVICE_UNAVAILABLE - cannot accept anymore connections */
#define HTTP_STATUS_505 "505"	/* HTTP_VERSION_NOT_SUPPORTED - Does not support http version */


// HTTP message
#define HTTP_RESPONSE_200 "OK"
#define HTTP_RESPONSE_400 "BAD REQUEST"
#define HTTP_RESPONSE_404 "NOT FOUND"
#define HTTP_RESPONSE_411 "LENGTH REQUIRED"
#define HTTP_RESPONSE_500 "INTERNAL SERVER ERROR"
#define HTTP_RESPONSE_501 "NOT IMPLEMENTED"
#define HTTP_RESPONSE_503 "SERVICE UNAVAILABLE"
#define HTTP_RESPONSE_505 "HTTP VERSION NOT SUPPORTED"

#endif
