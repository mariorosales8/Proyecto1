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
#include <algorithm>

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
            cout << "No se pudo crear el socket" << endl;
            continue;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(puerto);

        if(bind(server_fd, (struct sockaddr*)&address,
                sizeof(address))  <  0){
            cout << "Error con el número de puerto" << endl;
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
                pthread_t hiloCliente;
                pthread_create(&hiloCliente, NULL, *recibe, (void*)&cliente);
            }
        }
    }
    
    return 0;
}


void *recibe(void* args){
    Usuario u = *(Usuario*)args;
    Usuario *cliente = &u;

    while(find(clientes.begin(), clientes.end(), cliente) == clientes.end()){
        recibeIdentificacion(cliente);
    }

    char buffer[1024];
    while(1){
        bzero(buffer, 1024);
        if(read(cliente->getSocket(), buffer, 1024) <= 0){
            clientes.remove(cliente);
            desconectar(cliente, "No se pudo recibir el mensaje");
        }

        map<string,list<Usuario*>> envios;
        envios = ejecutaMensaje(buffer, cliente);
        for(auto &envio : envios){
            for(Usuario *destinatario : envio.second){
                if(send(destinatario->getSocket(), envio.first.c_str(),
                        strlen(envio.first.c_str()), 0) <= 0){
                    close(destinatario->getSocket());
                    clientes.remove(destinatario);
                    pthread_exit(NULL);
                }
            }
        }
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

void recibeIdentificacion(Usuario *cliente){
    char buffer[1024];
    map<string,list<Usuario*>> envios;

    if(read(cliente->getSocket(), buffer, 1024) <= 0){
        desconectar(cliente, "No se pudo identificar");
    }
    envios = identifica(buffer, cliente);
    for(auto &envio : envios){
        for(Usuario *destinatario : envio.second){
            if(send(destinatario->getSocket(), envio.first.c_str(),
                    strlen(envio.first.c_str()), 0) <= 0){
                close(destinatario->getSocket());
                pthread_exit(NULL);
            }
        }
    }
}
map<string,list<Usuario*>> identifica(string json, Usuario *cliente){
    Mensaje mensaje(json);
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    if(!mensaje.esValido() || mensaje.getTipo() != "IDENTIFY"){
        desconectar(cliente, "No se pudo identificar");
        envios.insert(pair<string,list<Usuario*>>(mensaje.toString(), destinatarios));
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
        envios.insert(pair<string,list<Usuario*>>(warning.toString(), destinatarios));
        return envios;
    }
    cliente->setNombre(mensaje.getAtributo("username"));
    Mensaje info;
    info.setTipo("INFO");
    info.setAtributo("message", "success");
    info.setAtributo("operation", "IDENTIFY");
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(info.toString(), destinatarios));
    Mensaje newUser;
    newUser.setTipo("NEW_USER");
    newUser.setAtributo("username", mensaje.getAtributo("username"));
    envios.insert(pair<string,list<Usuario*>>(newUser.toString(), clientes));
    clientes.push_back(cliente);
    return envios;
}

map<string,list<Usuario*>> ejecutaMensaje(string json, Usuario *cliente){
    Mensaje mensaje(json);
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    if(!mensaje.esValido()){
        clientes.remove(cliente);
        desconectar(cliente, "Mensaje inválido");
        envios.insert(pair<string,list<Usuario*>>(mensaje.toString(), destinatarios));
        return envios;
    }
    switch(tipos[mensaje.getTipo()]){
        case PUBLIC_MESSAGE:
            envios = publicMessage(mensaje, cliente);
            break;

        case MESSAGE:
            envios = message(mensaje, cliente);
            break;
        
        default:
            clientes.remove(cliente);
            desconectar(cliente, "Mensaje no reconocido");
            envios.insert(pair<string,list<Usuario*>>(mensaje.toString(), destinatarios));
            break;
    }
    return envios;
}

map<string,list<Usuario*>> publicMessage(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;

    Mensaje respuesta;
    respuesta.setTipo("PUBLIC_MESSAGE_FROM");
    respuesta.setAtributo("username", cliente->getNombre());
    respuesta.setAtributo("message", mensaje.getAtributo("message"));

    for(Usuario *usuario : clientes){
        if(usuario->getNombre() != cliente->getNombre()){
            destinatarios.push_back(usuario);
        }
    }
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> message(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    for(Usuario *usuario : clientes){
        if(usuario->getNombre() == mensaje.getAtributo("username")){
            destinatarios.push_back(usuario);
            break;
        }
    }
    if(destinatarios.empty()){
        Mensaje warning;
        warning.setTipo("WARNING");
        warning.setAtributo("message", "El usuario '" + mensaje.getAtributo("username") + "' no existe");
        warning.setAtributo("operation", "MESSAGE");
        warning.setAtributo("username", mensaje.getAtributo("username"));
        destinatarios.push_back(cliente);
        envios.insert(pair<string,list<Usuario*>>(warning.toString(), destinatarios));
        return envios;
    }

    Mensaje respuesta;
    respuesta.setTipo("MESSAGE_FROM");
    respuesta.setAtributo("username", cliente->getNombre());
    respuesta.setAtributo("message", mensaje.getAtributo("message"));

    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}