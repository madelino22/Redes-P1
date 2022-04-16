#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <time.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // Gestion de errores
    if (argc != 3)
    {
        printf("Numero de argumentos no valido\n");
        printf("Formato debe ser \"./program host port\"\n");
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *info;

    memset(&hints, 0, sizeof(struct addrinfo));


    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &info);

    if (rc != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    int sd = socket(info->ai_family, info->ai_socktype, 0);

    bind(sd, info->ai_addr, info->ai_addrlen);
    

    freeaddrinfo(info);

    //5--> como máximo hay 5 conexiones pendientes
    listen(sd, 5);
    


    struct sockaddr cliente_addr;
    socklen_t cliente_len = sizeof(struct sockaddr);
    int cliente_sd = accept(sd, (struct sockaddr *) &cliente_addr, &cliente_len);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    getnameinfo((struct sockaddr *) &cliente_addr, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
    printf("Conexión desde Host:%s Puerto:%s\n",host, serv);

    int c = -1;
    while (c != 0) {
        
        //Ejemplo: recepción en buffer de aplicación (no leer de byte en byte)
        int bufferLen = 80;
        char buffer[bufferLen];
        memset(&buffer, '\0', sizeof(char) * bufferLen);
       
        c = recv(cliente_sd, buffer, (bufferLen - 1) * sizeof(char), 0);
        
        //recv devuelve 0 si se cierra la conexión
        if(c == 0){
            printf("Conexión terminada\n");
        }
        send(cliente_sd, buffer, bufferLen, 0);
    }


    return 0;
}