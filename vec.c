#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>

#define tcp_port_for_dispenser 1034
#define tcp_port_for_camera 1035

void *thread_runner(void *args)
{
	printf("thread is running \n");
	int rsfd = socket(AF_INET, SOCK_RAW, 100);
	if (rsfd < 0)
	{
		printf("Error in creating socket\n");
		exit(0);
	}
	char buffer[4096];
	struct sockaddr_in client;
	int len = sizeof(client);
	int n = recvfrom(rsfd, buffer, 4096, 0, (struct sockaddr *)&client, (socklen_t *)&len);
	if (n < 0)
	{
		printf("Error in receiving packet\n");
		exit(0);
	}
	struct iphdr *ip = (struct iphdr *)buffer;
	for (int i = ip->ihl * 4; i < n; i++)
	{
		printf("%c", buffer[i]);
	}
	printf("\n");
	pthread_exit(NULL);
}
int main()
{
	int sfd;
	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("\n socket");
	else
		printf("\n socket created successfully\n");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(tcp_port_for_dispenser);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("\n connect : ");
		return 1;
	}
	else
		printf("\nconnected from dispenser succesful");
	// connection   with camera
	// ****************
	int sfd1;
	struct sockaddr_in serv_addr1;
	bzero(&serv_addr1, sizeof(serv_addr1));

	if ((sfd1 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("\n socket");
	else
		printf("\n socket created successfully\n");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(tcp_port_for_camera);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (connect(sfd1, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("\n connect : ");
		return 1;
	}
	else
		printf("\nconnected from camera  succesful");
	int pid = getpid();
	if (send(sfd1, &pid, sizeof(pid), 0) == -1)
	{
		perror("error in sending pid (vechile number )\n");
	}
	else
	{
		printf("send succesfull to camera pid\n");
	}
	// *********
	int received_arr[3];
	ssize_t bytes_received = recv(sfd, received_arr, sizeof(received_arr), 0);
	if (bytes_received == -1)
	{
		perror("\n receive array from nsfd");
	}
	else
	{
		printf("\n Array received successfully avialable : petrol  %d diesel \n, %d, gas %d\n", received_arr[0], received_arr[1], received_arr[2]);
		printf("please  select option : ");
		int opt = rand() % 3 + 1;
		// scanf("%d", &opt);
		if (send(sfd, &opt, sizeof(opt), 0) == -1)
		{
			perror("\n send opt to nsfd");
		}
		printf("printing the process id : %d\n", pid);
		if (send(sfd, &pid, sizeof(pid), 0) == -1)
		{
			perror("error in sending pid (vechile number )\n");
		}
		else
		{
			printf("send succesfull to biller pid\n");
		}
		// sleep(10);
	}
	pthread_t id ;
	pthread_create(&id,NULL,thread_runner,NULL);
	pthread_join(id,NULL);
}
