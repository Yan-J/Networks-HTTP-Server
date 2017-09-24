/******************************************************************************
* server.c                                                                    *
*                                                                             *
* Description: This file contains the C source code for an http server. The   *
*              server runs on a hard-coded port and serves responses to       *
*              to connected clients.        				      *
*      			                                                      *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>                          *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*	   Nish Kumar <nk595@cornell.edu>				      *
*                                                                             *
*******************************************************************************/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "parse.h"
#include "http.h"


/******************************************************************************
 *                              Server settings                               *
 ******************************************************************************/
#define WWW_PATH		"./static_site/"
#define ECHO_PORT		9999
#define TYPE_LEN		64
#define BUF_SIZE		8192
#define RESPONSE_HEADER_SIZE	8192
#define RESPONSE_BODY_SIZE	8192
#define LOG_FILE		"log.txt"
#define MAX_CONN		1250
#define MAX_HEADER_SIZE 	4096
#define LOG_BUF_SIZE		1024


/******************************************************************************
 *				Global variables			      *
 ******************************************************************************/
int client_sockets[MAX_CONN];	/* Array to track all connections */
int max_sd;			/* Max socket descriptor */
fd_set readfds;			/* Read file descriptors for incoming conn */
char log_buf[LOG_BUF_SIZE];

void log_write(char* msg)
{
    FILE* file_ptr = fopen(LOG_FILE, "a+");

    if(file_ptr != NULL)
    {
	fputs(msg, file_ptr);
	fclose(file_ptr);
    }

    memset(log_buf, 0, LOG_BUF_SIZE);
}



/*
 * Function to close a server socket
 */
int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}


/*
 * Function to close a client connection
 */
int close_connection(int index)
{
    int sd = client_sockets[index];
    client_sockets[index] = 0;
    return close_socket(sd);
}


/*
 * Creates response header
 */
int create_response_header(int sd, char *buf)
{
    return 0;
}


/*
 * Creates response body
 */
int create_response_body(int sd, char *buf)
{
    return 0;
}


/*
 * Handle head request
 */
void handle_head_request(int id,  Request *request)
{

}


/*
 * Handle get request
 */
void handle_get_request(int id,  Request *request)
{

}


/*
 * Handle post request
 */
void handle_post_request(int id,  Request *request)
{

}



void handle_request_error(int id, char *status_code, char *msg, char *desc)
{
    struct tm tm;
    time_t now;
    char header[RESPONSE_HEADER_SIZE], body[RESPONSE_BODY_SIZE], date_time[TYPE_LEN];

    now = time(0);
    tm = *gmtime(&now);
    strftime(date_time, TYPE_LEN, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    // response header
    sprintf(header, "HTTP/1.1 %s %s\r\n", status_code, msg);
    sprintf(header, "%sDate: %s\r\n", header, date_time);
    sprintf(header, "%sServer: Liso/1.0\r\n", header);
    if (client_sockets[id] != 0) 
        sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sContent-type: text/html\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, (int)strlen(body));
    
    // response body
    sprintf(body, "<html>\r\n");
    sprintf(body, "%s<body>\r\n", body);
    sprintf(body, "%s<h4> ERROR - %s </h4>\r\n", body, msg);
    sprintf(body, "%s<br><p>%s</p>\r\n", body, desc);
    sprintf(body, "%s</body>\r\n", body);
    sprintf(body, "%s</html>\r\n", body);

    send(client_sockets[id], header, strlen(header), 0);
    send(client_sockets[id], body, strlen(body), 0);
}


/*
 * Parse client http request
 */
void parse_request(int index, int sd, char *buf, ssize_t read_ret)
{
    int i = 0;
    //Parse the buffer to the parse function. You will need to pass the socket fd and the buffer would need to
    //be read from that fd
    Request *request = parse(buf, read_ret, sd);
    printf("parsing request for sd: %d \n", sd);
	
    // Check if request header is valid
    if(request == NULL)
     {
	char * description = "Invalid HTTP header!";
	handle_request_error(index, HTTP_STATUS_505, HTTP_RESPONSE_505, description);
	return;
     }

    // Just printing everything
    printf("###################################### \n");
    printf("Http Method: %s\n",request->http_method);
    printf("Http Version: %s\n",request->http_version);
    printf("Http Uri: %s\n",request->http_uri);


    printf("----------- Request Header -----------\n");
    for(i = 0; i < request->header_count; i++)
    {
        printf("name: %s  |  ",request->headers[i].header_name);
        printf("value: %s \n",request->headers[i].header_value);
    }

    printf("###################################### \n");
   	

    // Check http version 
    if(strcasecmp(request->http_version, "HTTP/1.1") != 0)
    {
	char * description = "Liso server only supports HTTP/1.1";
        handle_request_error(index, HTTP_STATUS_505, HTTP_RESPONSE_505, description);
        free(request->headers);
        free(request);
	printf(" Invalid HTTP version \n");
	return;
    }

    // Check request type and handle request
    if(strcasecmp(request->http_method, "GET") && 
	strcasecmp(request->http_method, "HEAD") && 
	strcasecmp(request->http_method, "POST"))
     {
	char * desc = "Requested method is not supported by liso server.";
        handle_request_error(index, HTTP_STATUS_501, HTTP_RESPONSE_501, desc);
        free(request->headers);
        free(request);
	printf("Method not supported \n");
	return;
    }

    if(strcasecmp(request->http_method, "HEAD") == 0)
    {	printf(" HEAD request \n");
        handle_head_request(index, request);
    }
    else if(strcasecmp(request->http_method, "GET") == 0)
    {
	printf(" GET request \n");
        handle_get_request(index, request);
    }
    else if(strcasecmp(request->http_method, "POST") == 0)
    {
	printf(" POST  request \n");
        handle_post_request(index, request);
    }

    // Free memory
    //free(request->headers);
    //free(request);
    printf(" Done parsing request for sd: %d \n", sd);
    return;
}

/*
 * Handle http request
 */
int handle_request(int sd, char *buf)
{
    return 0;   
}



/******************************************************************************
 *                                Server Core                                 *
 ******************************************************************************/
int main(int argc, char* argv[])
{
    int sock, new_socket;
    int i, count;
    int sd, selected_fd;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    /* Initialize client connections */
    for(i=0; i<MAX_CONN; i++){
	client_sockets[i] = 0;
    }

    fprintf(stdout, "############# Starting server ############# \n");
    char* msg = "############# Starting server ############# \n";
    log_write(msg);

    /* Primary server socket - creates end point for binding */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
	sprintf(log_buf,"Failed creating socket.\n");
	log_write(log_buf);
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))){
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
	sprintf(log_buf,"Failed binding socket.\n");
        log_write(log_buf);
        return EXIT_FAILURE;
    }


    /* Server listen for incoming connections */
    if (listen(sock, 5)){
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
	sprintf(log_buf,"Error listening on socket.\n");
        log_write(log_buf);
        return EXIT_FAILURE;
    }

    /* finally, loop waiting for input and then write it back */
    while(1){
       /* clear the socket set */
       FD_ZERO(&readfds);

       /* add master socket to set */
       FD_SET(sock, &readfds);
       max_sd = sock;

       count = 0;
       for(i=0; i<MAX_CONN; i++) {
            sd = client_sockets[i];

            // if socket descriptor is valid then add to read list
            if(sd > 0){
		count++;
                FD_SET(sd , &readfds);
	    }

            // save highest file descriptor number, for use in select function
            if(sd > max_sd)
                max_sd = sd;
        }

	printf("Active connections: %d \n", count);
	sprintf(log_buf, "Active connections: %d \n", count);
	log_write(log_buf);


	/* wait for select to return, timeout is NULL - so wait indefinitely */
	selected_fd = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

	if(selected_fd < 0)
	{
	    msg = "Select() error \n";
	    log_write(msg);
	}

	cli_size = sizeof(cli_addr);
        // If master socket is set, then there is a new incoming connection
        if(FD_ISSET(sock, &readfds))
	{
            if((new_socket = accept(sock, (struct sockaddr *)&cli_addr, (socklen_t*)&cli_size))<0)
	    {
                printf("Failed to accept incoming connection! \n");
		sprintf(log_buf, "Failed to accept incoming connection! \n");
        	log_write(log_buf);
                exit(EXIT_FAILURE);
            }

	    // inform user of socket number - used in send and receive command
	    printf("New connection::  socket fd=%d , ip: %s , port: %d \n", new_socket,
		inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port));
	    sprintf(log_buf, "New connection::  socket fd=%d , ip: %s , port: %d \n", new_socket,
                inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port));
            log_write(log_buf);


	/*
	    msg = "Simple Web server version 1.0 \n";
	    // send new connection greeting message
	    if( send(new_socket, msg, strlen(msg), 0) != strlen(msg) ){
		printf("send error");
	    }

	    puts("Welcome message sent!");
	*/

            // add new socket to the first free position in client sockets array
	    for (i = 0; i < MAX_CONN; i++){
		// if position is empty
		if( client_sockets[i] == 0 ){
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

	/* Handle all concurrent connections in the client array */
        for(i = 0; i < MAX_CONN; i++){
	    sd = client_sockets[i];

            if(FD_ISSET(sd, &readfds)){
                // Check if it was for closing, and also read the incoming message
                if ((readret = read(sd, buf, BUF_SIZE)) <= 0){
                    // Somebody disconnected, get his details and print
                    getpeername(sd , (struct sockaddr*)&cli_addr , (socklen_t*)&cli_size);

                    printf("Closing connection: ip %s , port %d \n",
                           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                    sprintf(log_buf, "Closing connection: ip %s , port %d \n",
                           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            	    log_write(log_buf);

		    //close(sd);
                    //client_sockets[i] = 0;
		    // Close connection
		    close_connection(i);
                }else{
		    printf("Reading client message size: %ld  \n", readret);
		    //parse_request(i, sd, buf, readret);
		    //close_connection(i);
                    send(sd , buf , readret , 0);
		    // handle_request(sd, buf)
                }

		// Clear buffer after message was sent back to client
		memset(buf, 0, BUF_SIZE);
            }
        }


    }

    close_socket(sock);

    return EXIT_SUCCESS;
}
