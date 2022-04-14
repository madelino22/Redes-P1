#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <time.h>
#include <string.h>
#include <unistd.h>

//compilar
//   gcc -o redes_P2-2 redes_P2-2.cc
 
 //ejecutar 
 //  ./redes_P2-2 0.0.0.0 3000

 //nc -u 127.0.0.1 3000




//Estructura de un socket:
// struct sockaddr_in {
//       sa_family_t    sin_family; // fijar a AF_INET
//       in_port_t      sin_port;   // puerto
//       struct in_addr sin_addr;   // dirección IP
//     };
//     struct in_addr {
//       uint32_t       s_addr;     // 32 bits dirección IP
//     };


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("No se ha introducido la dirección o el puerto");
        return 1;
    }

    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    // hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;//De base viene SOCK_STREAM, que no paran de llegar y sería TCP, con DGRAM es UDP

    int check = getaddrinfo(argv[1], argv[2], &hints, &info);

    if (check != 0)
    {
        fprintf(stderr, "Error using getaddrinfo: %s\n", gai_strerror(check));
        return -1;
    }

    int udp_socket = socket(info->ai_family, info->ai_socktype, 0);

    if (udp_socket == -1)
    {
        fprintf(stderr, "Error getting the socket %s\n", gai_strerror(udp_socket));
        return -1;
    }

    bind(udp_socket, info->ai_addr, info->ai_addrlen);

    freeaddrinfo(info);

    const int bufferLen = 80; 

    while (true)
    {
        char buffer[buferLen] = {};

        struct sockaddr client;
        socklen_t clientLen = sizeof(sockaddr);

        int bytes = recvfrom(udp_socket, buffer, bufferLen, 0, &client, &clientLen);

        char puerto[NI_MAXSERV];
        char nombreHost[NI_MAXHOST];
        //
        check = getnameinfo(&client, clientLen, nombreHost, NI_MAXHOST, puerto, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if (check != 0)
        {
            fprintf(stderr, "Error using getnameinfo: %s\n", gai_strerror(check));
            continue;
        }

        printf("%d bytes de %s:%s\n", bytes, nombreHost, puerto);
        time_t rawTime;
        struct tm *timeInfo;

        time(&rawTime);

        timeInfo = localtime(&rawTime);

        char *format;

        switch (buffer[0])
        {
        case 't':
            format = "%r";
            break;
        case 'd':
         format = "%F";
            break;
        case 'q':
            printf("Saliendo...\n");
            close(udp_socket);
            return 0;
        default:
            printf("Comando no soportado %s\n", buffer);
            sendto(udp_socket, "Comando no soportado\n", 21, 0, &client, clientLen);
            continue;
        }

        char send[80];
        int timeBytes = strftime(send, 80, format, timeInfo);
        send[timeBytes] = '\n';

        sendto(udp_socket, send, timeBytes + 1, 0, &client, clientLen);
    }

    close(udp_socket);

    return 0;
}