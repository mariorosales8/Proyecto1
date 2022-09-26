#include "include.h"
#include "ClasesCliente.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

using namespace std;

int sock;
bool desconectado;
ControlCliente control;


int main(){
    while(1){
        desconectado = true;
        string ip;
        int puerto, client_fd;
        struct sockaddr_in serv_addr;
        pthread_t hiloRead, hiloSend;

        cout << "IP del servidor: " << flush;
        cin >> ip;
        cout << "Puerto: " << flush;
        cin >> puerto;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0){
            cout << "Error al crear el socket" << endl;
            continue;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(puerto);

        if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0){
            cout << "Error en la dirección IP" << endl;
            continue;
        }

        client_fd = connect(sock, (struct sockaddr*)&serv_addr, 
                            sizeof(serv_addr));
        if(client_fd < 0){
            cout << "No se pudo conectar con el servidor" << endl;
            continue;
        }

        while(desconectado){
            cout << "Nombre de usuario: " << flush;
            string nombreJSON = control.ponNombre();
            if(send(sock, nombreJSON.c_str(),
                    strlen(nombreJSON.c_str()), 0) <= 0){
                cout << "Error al identificarse" << endl;
                continue;
            }
            desconectado = false;
        }

        pthread_create(&hiloRead, NULL, *lee, NULL);
        pthread_create(&hiloSend, NULL, *envia, NULL);
        pthread_join(hiloRead, NULL);
        pthread_join(hiloSend, NULL);
    }
    
    return 0;
}


void *lee(void* args){
    char buffer[1024];
    while(1){
        bzero(buffer, 1024);
        if(read(sock, buffer, 1024) <= 0){
            cout << "Se desconectó el servidor" << endl;
            desconectado = true;
            pthread_exit(NULL);
        }
        cout << buffer << endl;
    }
    return NULL;
}

void *envia(void* args){
    string mensaje;
    //cin.ignore();
    while(1){
        getline(cin, mensaje);
        if(desconectado){
            pthread_exit(NULL);
        }
        if(send(sock, mensaje.c_str(),
                strlen(mensaje.c_str()), 0) <= 0){
            cout << "Error al enviar el mensaje" << endl;
        }
    }
    return NULL;
}
