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
ControlCliente control;


int main(){
    while(1){
        control.desconectar();
        string ip;
        int puerto, client_fd;
        struct sockaddr_in serv_addr;

        cout << "IP del servidor: " << flush;
        cin >> ip;
        cout << "Puerto: " << flush;
        cin >> puerto;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0){
            cout << "No se pudo crear el socket" << endl;
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

        if(!identifica()){
            continue;
        }
        control.conectar();

        pthread_t hiloRead, hiloSend;
        pthread_create(&hiloRead, NULL, *recibe, NULL);
        pthread_create(&hiloSend, NULL, *envia, NULL);
        pthread_join(hiloRead, NULL);
        pthread_join(hiloSend, NULL);

    }
    
    return 0;
}


bool identifica(){
    while(1){
        cout << "Nombre de usuario: " << flush;
        string nombreJSON = control.ponNombre();
        if(send(sock, nombreJSON.c_str(),
                strlen(nombreJSON.c_str()), 0) <= 0){
            control.imprime("No se pudo enviar el nombre de usuario");
            continue;
        }
        char buffer[1024];
        if(read(sock, buffer, 1024) <= 0){
            control.imprime("Se desconectó el servidor");
            return false;
        }
        Mensaje mensaje(buffer);
        switch(tipos[mensaje.getTipo()]){
            case INFO:
                return true;
            case WARNING:
                control.imprime(mensaje.getAtributo("message"));
                continue;
            default:
            continue;
        }
    }
    return false;
}

void *recibe(void* args){
    char buffer[1024];
    while(1){
        bzero(buffer, 1024);
        if(read(sock, buffer, 1024) <= 0){
            control.imprime("--- Se desconectó el servidor ---");
            control.desconectar();
            close(sock);
            pthread_exit(NULL);
        }
        ejecutaMensaje(buffer);
    }
    return NULL;
}

void *envia(void* args){
    string mensaje;
    while(1){
        mensaje = control.escribeComando();
        if(control.estaDesconectado()){
            pthread_exit(NULL);
        }
        if(mensaje.empty()){
            continue;
        }

        if(send(sock, mensaje.c_str(),
                strlen(mensaje.c_str()), 0) <= 0){
            control.imprime("--- No se pudo enviar el mensaje ---");
        }
    }
    return NULL;
}

void ejecutaMensaje(string json){
    Mensaje mensaje(json);
    switch(tipos[mensaje.getTipo()]){
        case INFO:
            control.info(mensaje);
            break;
        case WARNING:
            control.warning(mensaje);
            break;
        case ERROR:
            control.imprime("-ERROR: " + mensaje.getAtributo("message"));
            control.desconectar();
            close(sock);
            pthread_exit(NULL);
            break;

        case PUBLIC_MESSAGE_FROM:
            control.publicMessageFrom(mensaje);
            break;

        case MESSAGE_FROM:
            control.messageFrom(mensaje);
            break;

        case ROOM_MESSAGE_FROM:
            control.roomMessageFrom(mensaje);
            break;

        case NEW_USER:
            control.newUser(mensaje);
            break;

        case USER_LIST:
            control.userList(mensaje);
            break;

        case ROOM_USER_LIST:
            control.roomUserList(mensaje);
            break;

        case JOINED_ROOM:
            control.joinedRoom(mensaje);
            break;

        case INVITATION:
            control.invitation(mensaje);
            break;

        case NEW_STATUS:
            control.newStatus(mensaje);
            break;

        case LEFT_ROOM:
            control.leftRoom(mensaje);
            break;

        default:
            control.imprime("--- Mensaje del servidor no reconocido ---");
            break;
    }
}