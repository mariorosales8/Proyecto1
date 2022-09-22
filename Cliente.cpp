#include "include.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<iostream>

using namespace std;

void error(const char*);

static string ip, mensaje;
static int puerto, sock, client_fd;
static struct sockaddr_in serv_addr;
static char buffer[1024];

int main(){
    pthread_t hiloRead, hiloSend;
    
    cout << "IP del servidor: ";
    cin >> ip;
    cout << "Puerto: ";
    cin >> puerto;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        error("Error al crear el socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto);

    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
        error("Error con la dirección del servidor");

    client_fd = connect(sock, (struct sockaddr*)&serv_addr, 
                        sizeof(serv_addr));
    if(client_fd < 0)
        error("No se pudo conectar al servidor");

    pthread_create(&hiloRead, NULL, *recibe, NULL);
    pthread_create(&hiloSend, NULL, *envia, NULL);
    pthread_join(hiloRead, NULL);
    pthread_join(hiloSend, NULL);
    


    close(client_fd);


    
    return 0;
}


void error(const char* error){
    cout << error << endl;
    main();
    exit(0);
}

void *recibe(void* args){
    for(int i=0; i < 3; i++){
        bzero(buffer, 1024);
        if(read(sock, buffer, 1024) < 0)
            error("Error al leer");
        cout << buffer << endl;
    }
}
void *envia(void* args){
    cin.ignore();
    for(int i=0; i < 3; i++){
        getline(cin, mensaje);
        if(send(sock, mensaje.c_str(),
                strlen(mensaje.c_str()), 0) < 0){
            error("Error al escribir");
        }
    }
}
