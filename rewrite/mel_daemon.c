/**
 * @file
 * @author  Joshua Ferguson <jwfergus@asu.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright Joshua W. Ferguson 2012
 *
 * @section DESCRIPTION
 *
 * Initial implementation of MEL Node-side
 */

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write / time
#include<cerrno>

#include<iostream>
using namespace std;

void getIP(char* ip){
	//Get our IP address
	FILE *ipPipe;
	int status;

	ipPipe = popen("ifconfig | grep 'inet addr:192.' | awk '{split($2,a,\":\");print a[2]}'", "r");
	if (ipPipe == NULL) // ERROR!
	{
		cout << "Could not get IP address!" << endl;
	}

	if(fgets(ip, 100, ipPipe) == NULL)
		cout << "Problem getting IP address!" << endl;
	status = pclose(ipPipe);
}

int main(int argc , char *argv[])
{
	int socket_desc , new_socket, send_socket, c;
	struct sockaddr_in server , client, to_send;
	char inc_message[2000], message[2000], send_message[2000];
	
	//Create socket FOR LISTENING
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	char ip[100];
	getIP(ip);

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	printf("bind done on ip: %s",ip);
	fflush(stdout);
	
	
	
	//***************************************************************
	
	
	//Create socket FOR SENDING
	
	
	
	send_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (send_socket == -1)
	{
		printf("Could not create socket");
	}

	const char* send_ip = "192.168.1.1";

	//Prepare the sockaddr_in structure
	to_send.sin_family = AF_INET;
	to_send.sin_addr.s_addr = inet_addr(send_ip);
	to_send.sin_port = htons( 8888 );
	
	
	
	//********************************************************
	// 		Main Loop


	int count = 1;
	int recv_return;
	while(1)
	{

		
		//Listen
		listen(socket_desc , 3);
	
		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);

		new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	
		// START EGRESS BLOCK THREAD
		//
		// *************************

		puts("Connected");
		
		
		//Read some data
		if( recv_return =read(new_socket, inc_message , sizeof(inc_message)) && recv_return < 0)
		{
			//FAILURE
			printf("\n**Read Failed**\nrecv_return: %d", recv_return);
			fflush(stdout);
			return 1;
		}
		printf("\n**Data Rcvd**\nmessage = %scount = %d\n",inc_message, count);
		fflush(stdout);
		
		
		sleep(2);
		
		//
		//		SEND INFO BACK TO CENTRAL SERVER
		//
		if (connect(send_socket , (struct sockaddr *)&to_send , sizeof(to_send)) < 0){puts("connect error: %s",strerror(errno));return 1;}
			printf("Connected to %s.\n", send_ip);
		fflush(stdout);

		//	Send message
		strcpy(send_message, "HELLO FROM A NODE \n\r");
		if( send(send_socket , send_message , strlen(send_message) , 0) < 0)
			{
			//FAILURE
			printf("\n**Send Failed**\nCur Message: %s", send_message);
			fflush(stdout);
			return 1;
			}
		printf("\n**Data Sent**\n");
		fflush(stdout);

		close(send_socket);

		count++;
		
		
	}
	printf("after while loop, last message was: %s, recv_return was: %d", inc_message, recv_return);
	
	
	//Reply to the client
//	message = "Hello Client , I have received your connection. But I have to go now, bye\n";
//	write(new_socket , message , strlen(message));
	
	if (new_socket<0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}
