################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          #
#          Wolf Richter <wolf@cs.cmu.edu>                                      #
#                                                                              #
################################################################################

default: echo_server echo_client

echo_server:
	@echo "Compiling echo_server.c"
	@gcc echo_server.c -o echo_server -Wall -Werror

echo_client:
	@echo "Compiling echo_client.c"
	@gcc echo_client.c -o echo_client -Wall -Werror
	@echo "############### Build Successful ############### \n"
clean:
	@rm echo_server echo_client
	@echo "Removed object files: echo_server, echo_client"
	@cp log.txt log_old.txt
	@rm log.txt
	@echo "Removed log.txt"
	@echo "############## Make clean success ############## \n"
