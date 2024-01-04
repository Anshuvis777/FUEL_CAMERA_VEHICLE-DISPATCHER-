#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
// #include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<poll.h>


#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
#define ADDRESS "mysocket"

int turn = 1;

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

    if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
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

void biller1(int nnusfd)
{
    printf("\nBiller1 is active \n");
    int fd = open("file.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("Error opening file");
        return;
    }
    int vec_no;
    ssize_t bytes_received = recv(nnusfd, &vec_no, sizeof(vec_no), 0);
    if (bytes_received == -1)
    {
        perror("Error receiving buffer");
        close(fd);
        return;
    }
    char buffer[20];
    sprintf(buffer, "%d\n", vec_no);  // Append a newline character at the end
    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        perror("Error writing to file");
    }
    printf("Data appended to file.txt: %d\n", vec_no);
    close(fd);
    turn = 2;
}

void biller2(int nnusfd)
{
    printf("\nBiller2 is active \n");
    int fd = open("file.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("Error opening file");
        return;
    }
    int vec_no;
    ssize_t bytes_received = recv(nnusfd, &vec_no, sizeof(vec_no), 0);
    if (bytes_received == -1)
    {
        perror("Error receiving buffer");
        close(fd);
        return;
    }
    char buffer[20];
    sprintf(buffer, "%d\n", vec_no);  // Append a newline character at the end
    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        perror("Error writing to file");
    }
    printf("Data appended to file.txt: %d\n", vec_no);
    close(fd);
    turn = 1;
}

int main()
{
    int usfd;
    struct sockaddr_un userv_addr, ucli_addr;
    int userv_len, ucli_len;

    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    perror("socket");
    bzero(&userv_addr, sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, ADDRESS);
    unlink(ADDRESS);
    userv_len = sizeof(userv_addr);
    
    if (bind(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
        perror("server: bind");
    listen(usfd, 1);
    ucli_len = sizeof(ucli_addr);
    int nusfd;
    nusfd = accept(usfd, (struct sockaddr *)&ucli_addr, &ucli_len);
    
    if (nusfd == -1)
    {
        perror("server: accept");
    }
    else
    {
        printf("Connection established successfully!\n");
    }
    struct pollfd poll_fd[1];
    poll_fd[0].fd = nusfd;
    poll_fd[0].events = POLLIN;
    int i = 0;
    while (1)
    {
        int poll_result = poll(poll_fd, 1, -1);
        if (poll_result == -1)
        {
            perror("poll");
            break;
        }
        if (poll_fd[0].revents & POLLIN)
        {
            int nnusfd = recv_fd(nusfd);
            if (nnusfd == -1)
            {
                perror("error in receiving fds\n");
                continue;
            }
            if (turn == 1)
            {
                biller1(nnusfd);
            }
            else if (turn == 2)
            {
                biller2(nnusfd);
            }
            i++;
            printf("bill is Done\n");
            printf("you can go now\n");
            if (i == 3)
            {
                printf("Reached maximum_______________\n");
                break;
            }
        }
    }
    close(usfd);
    close(nusfd);

    return 0;
}
