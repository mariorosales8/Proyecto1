#include "include.h"
#include "ClasesServidor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>
#include <list>

using namespace std;

list<Usuario*> clientes;
ControlServidor control;

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
                Usuario cliente = Usuario(new_socket);
                pthread_t hiloRead;
                pthread_create(&hiloRead, NULL, *lee, (void*)&cliente);
            }
        }
    }
    
    return 0;
}


void *lee(void* args){
    char buffer[1024];
    Usuario u = *(Usuario*)args;
    Usuario *cliente = &u;

    if(read(cliente->getSocket(), buffer, 1024) <= 0){
        pthread_exit(NULL);
    }
    if(control.ponNombre(buffer, cliente)){
        clientes.push_back(cliente);
    }else{
        close(cliente->getSocket());
        pthread_exit(NULL);
    }

    while(1){
        bzero(buffer, 1024);
        if(read(cliente->getSocket(), buffer, 1024) <= 0)
            desconectar(cliente);
        cout << buffer << endl;
    }
    return NULL;
}
void *envia(void* args){
    list<Usuario*>* clientes = (list<Usuario*>*)args;
    string mensaje;
    cin.ignore();

    while(1){
        getline(cin, mensaje);
        for(Usuario *cliente : *clientes){
            if(send(cliente->getSocket(), mensaje.c_str(),
                    strlen(mensaje.c_str()), 0) <= 0){
                cout << "Error al enviar el mensaje" << endl;
            }
        }
    }
    return NULL;
}

void desconectar(Usuario *cliente){
    clientes.remove(cliente);
    pthread_exit(NULL);
}