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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "server.h"
#include "http.h"


/******************************************************************************
 *                              Server settings                               *
 ******************************************************************************/
#define DEBUG			1
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
#define FILE_NAME_SIZE		512
#define FILE_PATH_SIZE		1024


/******************************************************************************
 *				Global variables			      *
 ******************************************************************************/
int client_sockets[MAX_CONN];	/* Array to track all connections */
int max_sd;			/* Max socket descriptor */
fd_set readfds;			/* Read file descriptors for incoming conn */
char log_buf[LOG_BUF_SIZE];



/******************************************************************************
 *                              Helper functions                              *
 ******************************************************************************/
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
 * Helper function to get requested file type
 */
void parse_file_type(char *filename, char *file_type)
{
    
    if (strstr(filename, ".html"))
        strcpy(file_type, "text/html");
    else if (strstr(filename, ".css"))
        strcpy(file_type, "text/css");
    else if (strstr(filename, ".js"))
        strcpy(file_type, "application/javascript");
    else if (strstr(filename, ".gif"))
        strcpy(file_type, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(file_type, "image/png");
    else if (strstr(filename, ".jpg") || strstr(filename, "jpeg"))
        strcpy(file_type, "image/jpeg");
    else if (strstr(filename, ".wav"))
        strcpy(file_type, "audio/x-wav");
    else
        strcpy(file_type, "text/plain");
}


/*
 * Helper function to get requested file path
 */
int parse_request_URI(Request *request, char *filename)
{
    // path of www dir
    strcpy(filename, WWW_PATH);

    if (!strstr(request->http_uri, "cgi-bin"))
    {
        //static res
        strcat(filename, request->http_uri);
        if (request->http_uri[strlen(request->http_uri)-1] == '/')
            strcat(filename, "index.html");
        return 0;
    }
    else{
        //dynamic res
        return 1;
    }
    return 0;
}


/*
 * Helper function to check if requested file is valid
 */
int validate_request_file(int id, Request *request)
{
    struct stat sbuf;
    char filename[FILE_PATH_SIZE];

    parse_request_URI(request, filename);

    // check if file exist
    if (stat(filename, &sbuf) < 0)
    {
	char * desc = "Requested file could not be found.";
        handle_request_error(id, request, HTTP_RESPONSE_404, desc);
        return 0;
    }

    // check we have permission
    if ((!S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    {
	char * desc = "You do not have permission to access this file.";
        handle_request_error(id, request, HTTP_RESPONSE_403, desc);
        return 0;
    }

    return 1;
}



/******************************************************************************
 *                              HTTP functions                                *
 ******************************************************************************/
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
int  handle_head_request(int id,  Request *request)
{
    struct tm tm;
    struct stat sbuf;
    time_t now;
    char filename[FILE_PATH_SIZE], content[BUF_SIZE];
    char filetype[TYPE_LEN], tbuf[TYPE_LEN], dbuf[TYPE_LEN]; 
 
   /*
    if (validate_request_file(id, request) == 0) 
    {
	printf("handle_head: FORBIDDEN or NOT FOUND");
	return 1;
    }
   */
    
    parse_request_URI(request, filename);
    stat(filename, &sbuf);
    parse_file_type(filename, filetype);
     
    tm = *gmtime(&sbuf.st_mtime);
    strftime(tbuf, TYPE_LEN, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    now = time(0);
    tm = *gmtime(&now);
    strftime(dbuf, TYPE_LEN, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    sprintf(content, "HTTP/1.1 200 OK\r\n");
    if (client_sockets[id] > 0)
        sprintf(content, "%sConnection: close\r\n", content);
    else
        sprintf(content, "%sConnection: keep-alive\r\n", content);

    sprintf(content, "%sContent-Length: %ld\r\n", content, sbuf.st_size);
    sprintf(content, "%sContent-Type: %s\r\n", content, filetype);
    sprintf(content, "%sDate: %s\r\n", content, dbuf);
    sprintf(content, "%sLast-Modified: %s\r\n", content, tbuf);
    sprintf(content, "%sServer: Liso/1.0\r\n\r\n", content);
    send(client_sockets[id], content, strlen(content), 0);

#ifdef DEBUG    
    printf("Sending response for HEAD: \n  %s\n",content);
#endif
    return 0;
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



void handle_request_error(int id, Request *request, char *http_response, char *desc)
{
    struct tm tm;
    time_t now;
    char header[RESPONSE_HEADER_SIZE], body[RESPONSE_BODY_SIZE], date_time[TYPE_LEN];

    now = time(0);
    tm = *gmtime(&now);
    strftime(date_time, TYPE_LEN, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    // response header
    sprintf(header, "HTTP/1.1 %s \r\n", http_response);
    sprintf(header, "%sDate: %s\r\n", header, date_time);
    sprintf(header, "%sServer: Liso/1.0\r\n", header);
    if (client_sockets[id] != 0) 
        sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sContent-type: text/html\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, (int)strlen(body));
    
    if(request == NULL)
    {
#ifdef DEBUG
        printf(" BAD request: response header \n %s", header);
#endif
	send(client_sockets[id], header, strlen(header), 0);
	return;
    }
    
    if(strcasecmp(request->http_method, "HEAD") == 0)
    {   
	send(client_sockets[id], header, strlen(header), 0);
#ifdef DEBUG
	printf(" HEAD request: response header \n %s", header);
#endif
	return;
    }
  
 
    // response body
    sprintf(body, "<html>\r\n");
    sprintf(body, "%s<body>\r\n", body);
    sprintf(body, "%s<h4> ERROR - %s </h4>\r\n", body, http_response);
    sprintf(body, "%s<br><p>%s</p>\r\n", body, desc);
    sprintf(body, "%s</body>\r\n", body);
    sprintf(body, "%s</html>\r\n", body);

    send(client_sockets[id], header, strlen(header), 0);
    send(client_sockets[id], body, strlen(body), 0);
#ifdef DEBUG
    printf("Sending header: %s", header);
    printf("Sending body: %s", body);
#endif
}


/*
 * Parse client http request
 */
void parse_request(int index, int sd, char *buf, ssize_t read_ret)
{
    int i = 0;

    //Parse the request
    Request *request = parse(buf, read_ret, sd);
	
    // Check if request header is valid
    if(request == NULL)
     {
	char * description = "Invalid HTTP header!";
	handle_request_error(index, request, HTTP_STATUS_505, description);
	free(request->headers);
    	free(request);
	return;
     }

    /* Just printing everything
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
   */	

    // Check http version 
    if(strcasecmp(request->http_version, "HTTP/1.1") != 0)
    {
	char * description = "Liso server only supports HTTP/1.1";
        handle_request_error(index, request, HTTP_STATUS_505, description);
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
        handle_request_error(index, request, HTTP_STATUS_501, desc);
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
	printf(" POST request \n");
        handle_post_request(index, request);
    }

    // Free memory
    free(request->headers);
    free(request);
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
		    parse_request(i, sd, buf, readret);
		    //close_connection(i);
                    // send(sd , buf , readret , 0);
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
