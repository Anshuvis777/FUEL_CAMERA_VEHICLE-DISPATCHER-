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
// #include<pcap.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <poll.h>
#include <pthread.h>
#include <stdbool.h>
#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
#define ADDRESS "mysocket"
#define tcp_port 1034

bool close_serv = false;
int send_fd(int socket, int fd_to_send)
{
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;

    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;

    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;

    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(control_message)) = fd_to_send;

    return sendmsg(socket, &socket_message, 0);
}

int recv_fd(int socket)
{
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

    if (recvmsg(socket, &socket_message, 0) < 0)
        return -1;

    if (message_buffer[0] != 'F')
    {
        return -1;
    }

    if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
    {
        return -1;
    }

    for (control_message = CMSG_FIRSTHDR(&socket_message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&socket_message, control_message))
    {
        if ((control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS))
        {
            sent_fd = *((int *)CMSG_DATA(control_message));
            return sent_fd;
        }
    }
    return -1;
}
void *closing_service(void *args)
{
    int sfd2;
    struct sockaddr_in serv_addr2, cli_addr2;
    socklen_t cli_len2;

    if ((sfd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket ");
    else
        printf("\n socket created successfully");

    bzero(&serv_addr2, sizeof(serv_addr2));

    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(1036);
    serv_addr2.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");

    listen(sfd2, 1);
    int nsfd;
    if ((nsfd = accept(sfd2, (struct sockaddr *)&cli_addr2, &cli_len2)) == -1)
    {
        perror("\n accept ");
    }
    else
    {
        printf("\n closing connection successful TIME TO CLOSE THE SERVICE !!!!");
        close_serv = true;
    }
    close(sfd2); // Close the server socket
    pthread_exit(NULL);
}
void *no_fuel_inform(void* args){
    printf("sending msg to all the consumer with no fuel\n");
    int raw_socket = socket(AF_INET, SOCK_RAW, 100);
    if (raw_socket == -1) {
        perror("Socket creation error");
        exit(1);
    }
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    char packet[] = "NULL fuel\n";
    // Send the packet using sendto
    if (sendto(raw_socket, packet, sizeof(packet), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("Packet send error");
    } 
    else {
        printf("Packet sent successfully.\n");
    }
    close(raw_socket);
    pthread_exit(NULL);
}
int main()
{
    int d = 2;
    int petrol = 1;
    int gas = 2;
    int usfd;
    struct sockaddr_un userv_addr;
    int userv_len, ucli_len;
    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1)
        perror("\nsocket");
    bzero(&userv_addr, sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, ADDRESS);
    userv_len = sizeof(userv_addr);
    if (connect(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
        perror("\n connect ");
    else
        printf("\nconnected with biller successful");
    printf("Time to come vehicle for fuel\n");
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket ");
    else
        printf("\n socket created successfully");

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");
    listen(sfd, 2);
    struct pollfd poll_fd[1];
    poll_fd[0].fd = sfd;
    poll_fd[0].events = POLLIN;
    int i = 1;
    pthread_t request_for_terminate;
    pthread_create(&request_for_terminate,NULL,closing_service,NULL);
    while (1)
    {
        if(close_serv){
            printf("BILLING IN WRONG");
            break;
        }
        int poll_result = poll(poll_fd, 1, -1);
        if (poll_result == -1)
        {
            perror("poll");
            break;
        }
        if (poll_fd[0].revents & POLLIN)
        {
            int nsfd;
            if ((nsfd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
            {
                perror("\n accept ");
                return 1;
            }
            else
            {
                printf("\n new vehicle connected successfully");
            }
            int arr[3];
            arr[0] = petrol;
            arr[1] = d;
            arr[2] = gas;
            if (send(nsfd, arr, sizeof(arr), 0) == -1)
            {
                perror("\n send array to nsfd");
            }
            else
            {
                printf("\n Array sent successfully to nsfd");
            }
            int opt;
            ssize_t bytes_received = recv(nsfd, &opt, sizeof(opt), 0);
            if (bytes_received == -1)
            {
                perror("\n receive buffer from sfd");
            }
            printf("choose option is %d",opt);
            if (opt == 1){
                petrol--;
            }
            if (opt == 2)
                d--;
            if (opt == 3)
                gas--;
            if(petrol==0||gas==0||d==0){
                pthread_t pid ;
                pthread_create(&pid,NULL,no_fuel_inform,NULL);
                pthread_join(pid,NULL);
            }
            if (send_fd(usfd, nsfd) == -1)
            {
                perror("\n send_fd failed");
            }
            printf("File descriptor successfully passed to the server \n");
            printf("I am send to biller the vehicle bill it : \n");
            close(nsfd); // Close the connection after sending the file descriptor
        }
    }

    pthread_join(request_for_terminate,NULL);
    // End
    close(sfd);
    close(usfd);

    return 0;
}
