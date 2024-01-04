#include <stdio.h>
#include <time.h>
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
#include <poll.h>
#include <stdbool.h>

bool close_service = false;

#define tcp_port 1035
#define tcp_port_with_dispenser 1036

void compare(int startLine, int arr[])
{
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[1024]; // Adjust the buffer size based on your needs
    ssize_t bytesRead;
    int currentLine = 1; // Track the current line number

    // Move the file pointer to the startLine
    lseek(fd, 0, SEEK_SET);
    while (currentLine < startLine) {
        bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            break; // Error or end of file
        }
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                currentLine++;
            }
        }
    }
    // Read n lines and store in an array
    int linesRead = 0;
    int *lineArray = (int *)malloc(2 * sizeof(int));
    while (linesRead < 2 && (bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';

        char *token = strtok(buffer, " \t\n"); // Assuming columns are space-separated

        while (token != NULL) {
            // Extract the first column and convert to integer
            int value = atoi(token);

            // Store in the array
            lineArray[linesRead++] = value;

            // Move to the next token
            token = strtok(NULL, " \t\n");
        }
    }
    // Print the values stored in the array
    printf("Values read from lines %d to %d:\n", startLine, startLine + 1);
    for (int i = 0; i < linesRead; i++) {
        printf("%d\n", lineArray[i]);
    }
    printf("\nprinting the values from arr: \n");
    for (int i = 0; i < 2; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Close the file
    close(fd);

    // compare
    bool flag = true;
    for (int i = 0; i < 2; i++)
    {
        if (arr[i] != lineArray[i])
        {
            flag = false;
            break;
        }
    }
    if (!flag)
    {
        printf("Arrays are not equal.\n");
        close_service = true;
        // Set the close_service variable if needed
    }
    else
    {
        printf("Arrays are equal.\n");
    }
}

void inform()
{
    // make a tcp connection
    // send request
    int sfd3;
    struct sockaddr_in serv_addr3;
    bzero(&serv_addr3, sizeof(serv_addr3));

    if ((sfd3 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\n socket");
        return;
    }
    else
    {
        printf("\n socket created successfully\n");
    }
    serv_addr3.sin_family = AF_INET;
    serv_addr3.sin_port = htons(tcp_port_with_dispenser);
    serv_addr3.sin_addr.s_addr = INADDR_ANY;

    if (connect(sfd3, (struct sockaddr *)&serv_addr3, sizeof(serv_addr3)) == -1)
    {
        perror("\n connect : ");
        return;
    }
    else
    {
        printf("\nconnect successful");
    }
    close(sfd3);
}

int main()
{
    // tcp connection with
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\n socket ");
        return 1;
    }
    else
    {
        printf("\n socket created successfully");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("\n bind : ");
        return 1;
    }
    else
    {
        printf("\n bind successful ");
    }

    listen(sfd, 10);
    struct pollfd poll_fd[1];
    poll_fd[0].fd = sfd;
    poll_fd[0].events = POLLIN;

    int cntr = 0;
    while (1)
    {
        int vec_no_arr[2];
        for (int i = 0; i < 2; i++)
        {
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
                int vec_no;
                ssize_t bytes_received = recv(nsfd, &vec_no, sizeof(vec_no), 0);
                if (bytes_received == -1)
                {
                    perror("Error receiving buffer");
                    return 1;
                }
                vec_no_arr[i] = vec_no;
                printf("%d\n", vec_no);
                close(nsfd); // Close the connection after sending the file descriptor
            }
        }
        int str = cntr * 2 + 1;
        printf("Observe by camera id is :\n");
        for (int i = 0; i < 2; i++)
        {
            printf("%d\n", vec_no_arr[i]);
        }
        sleep(4);
        compare(str, vec_no_arr);
        // now compare with file value
        if (close_service)
        {
            printf("service closed wrong billing by biller\n");
            break;
        }
        cntr++;
    }
    inform();
    // inform
    return 0;
}
