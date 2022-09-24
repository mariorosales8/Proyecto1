#include "include.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <list>

using namespace std;
static int puerto, server_fd, new_socket;
static struct sockaddr_in address;
static int addrlen = sizeof(address);
static char buffer[1024];
static string mensaje;
sem_t semaforo;

int main(){
    pthread_t hiloRead, hiloSend;
    list<int> clientes;
    
    cout << "Puerto del servidor: " << flush;
    cin >> puerto;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        error("Error al crear el socket");
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puerto);
    
    if(bind(server_fd, (struct sockaddr*)&address,
            sizeof(address))  <  0){
        error("Error. El número del puerto debe ser al menos 1024");
    }
    
    if(listen(server_fd, 3) < 0){
        error("Error en el listen");
    }

    pthread_create(&hiloSend, NULL, *envia, (void*)&clientes);

    while(1){
        new_socket = accept(server_fd, (struct sockaddr*)&address,
                            (socklen_t*)&addrlen);
        if(new_socket < 0){
            error("Error al aceptar al cliente");
        }else{
            clientes.push_back(new_socket);
            pthread_t hiloRead;
            pthread_create(&hiloRead, NULL, *lee, (void*)&new_socket);   
        }
    }

    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);
    
    
    return 0;
}


void error(const char* error){
    cout << error << endl;
    main();
    exit(0);
}

void *lee(void* args){
    int new_socket = *(int*)args;
    while(1){
        bzero(buffer, 1024);
        if(read(new_socket, buffer, 1024) < 0)
            error("Error al leer");
        cout << buffer << endl;
    }
    return NULL;
}
void *envia(void* args){
    list<int>* clientes = (list<int>*)args;
    cin.ignore();
    while(1){
        getline(cin, mensaje);
        cout << (*clientes).size() << endl;
        for(int cliente : *clientes){
            if(send(cliente, mensaje.c_str(),
                    strlen(mensaje.c_str()), 0) < 0){
                error("Error al escribir");
            }
        }
    }
    return NULL;
}
