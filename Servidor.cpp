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
#include <map>

using namespace std;

list<Usuario*> clientes;

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

    leeIdentificacion(cliente);

    while(1){
        bzero(buffer, 1024);
        if(read(cliente->getSocket(), buffer, 1024) <= 0){
            desconectar(cliente, "Error al leer el mensaje");
            clientes.remove(cliente);
        }
        cout << buffer << endl;
    }
    return NULL;
}



void desconectar(Usuario *cliente, string message){
    Mensaje mensaje;
    mensaje.setTipo("ERROR");
    mensaje.setAtributo("message", message);
    send(cliente->getSocket(), mensaje.toString().c_str(),
         strlen(mensaje.toString().c_str()), 0);
    close(cliente->getSocket());
    pthread_exit(NULL);
}

void leeIdentificacion(Usuario *cliente){
    char buffer[1024];
    map<Mensaje*,list<Usuario*>> envios;

    if(read(cliente->getSocket(), buffer, 1024) <= 0){
        desconectar(cliente, "Error: no se pudo identificar");
    }
    envios = identifica(buffer, cliente);
    for(auto &envio : envios){
        for(Usuario *destinatario : envio.second){
            if(send(destinatario->getSocket(), envio.first->toString().c_str(),
                    strlen(envio.first->toString().c_str()), 0) <= 0){
                close(destinatario->getSocket());
                pthread_exit(NULL);
            }
        }
    }
}
map<Mensaje*,list<Usuario*>> identifica(string json, Usuario *cliente){
    Mensaje mensaje(json);
    list<Usuario*> destinatarios;
    map<Mensaje*,list<Usuario*>> envios;
    if(!mensaje.esValido() || mensaje.getTipo() != "IDENTIFY"){
        desconectar(cliente, "Error: no se pudo identificar");
        envios.insert(pair<Mensaje*,list<Usuario*>>(&mensaje, destinatarios));
        return envios;
    }
    bool nombreUsado = false;
    for(Usuario *usuario : clientes){
        if(usuario->getNombre() == mensaje.getAtributo("username")){
            nombreUsado = true;
            break;
        }
    }
    if(nombreUsado){
        Mensaje warning;
        warning.setTipo("WARNING");
        warning.setAtributo("message", "El usuario '" + mensaje.getAtributo("username") + "' ya existe");
        warning.setAtributo("operation", "IDENTIFY");
        warning.setAtributo("username", mensaje.getAtributo("username"));
        destinatarios.push_back(cliente);
        envios.insert(pair<Mensaje*,list<Usuario*>>(&warning, destinatarios));
        return envios;
    }
    cliente->setNombre(mensaje.getAtributo("username"));
    Mensaje info;
    info.setTipo("INFO");
    info.setAtributo("message", "success");
    info.setAtributo("operation", "IDENTIFY");
    destinatarios.push_back(cliente);
    envios.insert(pair<Mensaje*,list<Usuario*>>(&info, destinatarios));
    Mensaje newUser;
    newUser.setTipo("NEW_USER");
    newUser.setAtributo("username", mensaje.getAtributo("username"));
    envios.insert(pair<Mensaje*,list<Usuario*>>(&newUser, clientes));
    clientes.push_back(cliente);
    return envios;
}