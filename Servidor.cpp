#include "include.h"
#include "Mensaje.cpp"
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

list<int> clientes;

int main(){
    while(1){
        int puerto, server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);

        cout << "Puerto del servidor: " << flush;
        cin >> puerto;

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(server_fd == 0){
            cout << "Error al crear el socket" << endl;
            continue;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(puerto);

        if(bind(server_fd, (struct sockaddr*)&address,
                sizeof(address))  <  0){
            cout << "Error. El número del puerto debe ser al menos 1024" << endl;
            continue;
        }

        if(listen(server_fd, 3) < 0){
            cout << "Error en el listen" << endl;
            continue;
        }

        pthread_t hiloSend;
        pthread_create(&hiloSend, NULL, *envia, (void*)&clientes);

        while(1){
            new_socket = accept(server_fd, (struct sockaddr*)&address,
                                (socklen_t*)&addrlen);
            if(new_socket < 0){
                cout << "No se pudo aceptar al cliente" << endl;
            }else{
                clientes.push_back(new_socket);
                pthread_t hiloRead;
                pthread_create(&hiloRead, NULL, *lee, (void*)&new_socket);   
            }
        }
    }
    
    return 0;
}


void *lee(void* args){
    char buffer[1024];
    int new_socket = *(int*)args;
    while(1){
        bzero(buffer, 1024);
        if(read(new_socket, buffer, 1024) <= 0)
            desconectar(new_socket);
        cout << buffer << endl;
    }
    return NULL;
}
void *envia(void* args){
    list<int>* clientes = (list<int>*)args;
    string mensaje;
    cin.ignore();
    while(1){
        getline(cin, mensaje);
        for(int cliente : *clientes){
            if(send(cliente, mensaje.c_str(),
                    strlen(mensaje.c_str()), 0) <= 0){
                cout << "Error al enviar el mensaje" << endl;
            }
        }
    }
    return NULL;
}

void desconectar(int socket){
    clientes.remove(socket);
    pthread_exit(NULL);
}