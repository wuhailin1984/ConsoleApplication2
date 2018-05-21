#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TEMP_FILE_PATH_NAME  "/sys/bus/w1/devices/28-05170052edff/w1_slave"

int main() {
	//creat socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		printf("Create Socket Failed!\\n");
		exit(1);
	}

	//Send request to server
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //initialize
	serv_addr.sin_family = AF_INET;  //use IPv4 address
	//serv_addr.sin_addr.s_addr = inet_addr("10.46.40.123");  //server IP
	serv_addr.sin_addr.s_addr = inet_addr("35.204.196.49");  //GCP server IP
	serv_addr.sin_port = htons(3001);  //port

	if ( connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0)
	{
		printf("Connect to server Failed!\\n");
		exit(1);
	}
	bool flag = true;

	while (flag)
	{
		FILE * fp = fopen(TEMP_FILE_PATH_NAME, "r");
		if (NULL == fp)
		{
			printf("Can not read the temperature file!\\n");
			sleep(1);
			break;
		}
		char TempDataLine[50];
		fgets(TempDataLine, 50, fp);    //read the first line

		fclose(fp);

		char tempTwoByte[7] ;
		tempTwoByte[0] = '0';
		tempTwoByte[1] = 'x';
		tempTwoByte[2] = TempDataLine[3];
		tempTwoByte[3] = TempDataLine[4];
		tempTwoByte[4] = TempDataLine[0];
		tempTwoByte[5] = TempDataLine[1];
		tempTwoByte[6] = '\0';
		printf("%s", tempTwoByte);
		int data =(int) strtol(tempTwoByte, NULL, 16);
		printf("%x", data);

		//write data to Server
		char TempData[5];
		bzero(TempData, 5);
		TempData[0] = 0x88;
		TempData[1] = (char)(data & 0xff);
		TempData[2] = (char)((data >> 8) & 0xff);
		TempData[3] = (char)((data >> 16) & 0xff);
		TempData[4] = (char)(data >> 24);
		write(sock, TempData, sizeof(TempData));

		//read data from server
		char buffer[50];
		read(sock, buffer, sizeof(buffer) - 1);

		printf("Message from server: %s\n", buffer);

		sleep(3);
	}
	

	//close socket
	close(sock);
	return 0;
}