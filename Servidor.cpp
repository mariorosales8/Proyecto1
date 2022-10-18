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
#include <semaphore.h>

using namespace std;

list<Usuario*> clientes;
list <Sala*> salas;
sem_t semaforo;

int main(int argc, char *argv[]){
    if(argc <= 1){
        cout << "No se especificó el puerto" << endl;
        return 0;
    }
    if(string(argv[1]).size() > 9){
        cout << "El puerto es demasiado largo" << endl;
        return 0;
    }
    for(char c : string(argv[1])){
        if(isdigit(c) == 0){
            cout << "El puerto debe ser un número" << endl;
            return 0;
        }
    }

    int server_fd, new_socket;
    int puerto = stoi(argv[1]);
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        cout << "No se pudo crear el socket" << endl;
        return 0;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puerto);
    if(bind(server_fd, (struct sockaddr*)&address,
            sizeof(address))  <  0){
        cout << "Puerto no disponible" << endl;
        return 0;
    }
    if(listen(server_fd, 3) < 0){
        cout << "Error en el listen" << endl;
        return 0;
    }
    
    sem_init(&semaforo, 0, 1);
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
            envia(disconnect(cliente));
            desconectarSocket(cliente, "No se pudo recibir el mensaje");
            cout << buffer << endl;
        }

        envia(ejecutaMensaje(buffer, cliente));
        cout << buffer << endl;
    }
    return NULL;
}

void envia(map<string,list<Usuario*>> envios){
    for(auto &envio : envios){
            for(Usuario *destinatario : envio.second){
                if(send(destinatario->getSocket(), envio.first.c_str(),
                        strlen(envio.first.c_str()), 0) <= 0){
                    close(destinatario->getSocket());
                    disconnect(destinatario);
                    pthread_exit(NULL);
                }
            }
        }
}


void desconectarSocket(Usuario *cliente, string message){
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
        desconectarSocket(cliente, "No se pudo identificar");
        cout << buffer << endl;
    }
    sem_wait(&semaforo);
    envios = identifica(buffer, cliente);
    sem_post(&semaforo);
    for(auto &envio : envios){
        for(Usuario *destinatario : envio.second){
            if(send(destinatario->getSocket(), envio.first.c_str(),
                    strlen(envio.first.c_str()), 0) <= 0){
                close(destinatario->getSocket());
                pthread_exit(NULL);
            }
        }
    }
    cout << buffer << endl;
}
map<string,list<Usuario*>> identifica(string json, Usuario *cliente){
    Mensaje mensaje(json);
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    if(!mensaje.esValido() || mensaje.getTipo() != "IDENTIFY"){
        desconectarSocket(cliente, "No se pudo identificar");
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
        disconnect(cliente);
        desconectarSocket(cliente, "Mensaje inválido");
        envios.insert(pair<string,list<Usuario*>>(mensaje.toString(), destinatarios));
        return envios;
    }
    sem_wait(&semaforo);
    switch(tipos[mensaje.getTipo()]){
        case PUBLIC_MESSAGE:
            envios = publicMessage(mensaje, cliente);
            break;

        case MESSAGE:
            envios = message(mensaje, cliente);
            break;

        case ROOM_MESSAGE:
            envios = roomMessage(mensaje, cliente);
            break;

        case USERS:
            envios = users(cliente);
            break;

        case ROOM_USERS:
            envios = roomUsers(mensaje, cliente);
            break;

        case INVITE:
            envios = invite(mensaje, cliente);
            break;

        case JOIN_ROOM:
            envios = joinRoom(mensaje, cliente);
            break;

        case NEW_ROOM:
            envios = newRoom(mensaje, cliente);
            break;

        case STATUS:
            envios = status(mensaje, cliente);
            break;

        case LEAVE_ROOM:
            envios = leaveRoom(mensaje, cliente);
            break;

        case DISCONNECT:
            envios = disconnect(cliente);
            break;
        
        default:
            disconnect(cliente);
            desconectarSocket(cliente, "Mensaje no reconocido");
            envios.insert(pair<string,list<Usuario*>>(mensaje.toString(), destinatarios));
            break;
    }
    sem_post(&semaforo);
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

map<string,list<Usuario*>> newRoom(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;
    destinatarios.push_back(cliente);
    for(Sala *sala : salas){
        if(sala->getNombre() == mensaje.getAtributo("roomname")){
            respuesta.setTipo("WARNING");
            respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' ya existe");
            respuesta.setAtributo("operation", "NEW_ROOM");
            respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
            envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
            return envios;
        }
    }
    salas.push_back(new Sala(mensaje.getAtributo("roomname"), cliente));
    respuesta.setTipo("INFO");
    respuesta.setAtributo("message", "success");
    respuesta.setAtributo("operation", "NEW_ROOM");
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> invite(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;
    Sala *sala = NULL;
    
    for(Sala *cuarto : salas){
        if(cuarto->getNombre() == mensaje.getAtributo("roomname")){
            sala = cuarto;
            break;
        }
    }
    if(sala == NULL){
        respuesta.setTipo("WARNING");
        respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' no existe");
        respuesta.setAtributo("operation", "INVITE");
        respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
        destinatarios.push_back(cliente);
        envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
        return envios;
    }

    list<string> nombres = mensaje.getLista("usernames");
    string noExiste = "";
    for(string nombre : nombres){
        noExiste = nombre;
        for(Usuario *usuario : clientes){
            if(usuario->getNombre() == nombre){
                destinatarios.push_back(usuario);
                noExiste = "";
                break;
            }
        }
        if(noExiste != ""){
            break;
        }
    }
    if(noExiste != ""){
        respuesta.setTipo("WARNING");
        respuesta.setAtributo("message", "El usuario '" + noExiste + "' no existe");
        respuesta.setAtributo("operation", "INVITE");
        respuesta.setAtributo("username", noExiste);
        destinatarios.clear();
        destinatarios.push_back(cliente);
        envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
        return envios;
    }

    sala->invita(destinatarios);

    Mensaje invitacion;
    invitacion.setTipo("INVITATION");
    invitacion.setAtributo("message", cliente->getNombre() + " te invita al cuarto '" 
                                     + sala->getNombre() + "'");
    invitacion.setAtributo("username", cliente->getNombre());
    invitacion.setAtributo("roomname", sala->getNombre());
    envios.insert(pair<string,list<Usuario*>>(invitacion.toString(), destinatarios));

    destinatarios.clear();
    respuesta.setTipo("INFO");
    respuesta.setAtributo("message", "success");
    respuesta.setAtributo("operation", "INVITE");
    respuesta.setAtributo("roomname", sala->getNombre());
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> joinRoom(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;

    for(Sala *sala : salas){
        if(sala->getNombre() == mensaje.getAtributo("roomname")){
            if(sala->estaEnSala(cliente)){
                respuesta.setTipo("WARNING");
                respuesta.setAtributo("message", "El usuario ya se unió al cuarto '" + sala->getNombre() + "'");
                respuesta.setAtributo("operation", "JOIN_ROOM");
                respuesta.setAtributo("roomname", sala->getNombre());
                destinatarios.push_back(cliente);
                envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
                return envios;
            }
            if(!sala->estaInvitado(cliente)){
                respuesta.setTipo("WARNING");
                respuesta.setAtributo("message", "El usuario no ha sido invitado al cuarto '" + sala->getNombre() + "'");
                respuesta.setAtributo("operation", "JOIN_ROOM");
                respuesta.setAtributo("roomname", sala->getNombre());
                destinatarios.push_back(cliente);
                envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
                return envios;
            }
            Mensaje joinedRoom;
            joinedRoom.setTipo("JOINED_ROOM");
            joinedRoom.setAtributo("roomname", sala->getNombre());
            joinedRoom.setAtributo("username", cliente->getNombre());
            for(Usuario *usuario : sala->getUsuarios()){
                destinatarios.push_back(usuario);
            }
            envios.insert(pair<string,list<Usuario*>>(joinedRoom.toString(), destinatarios));

            sala->agrega(cliente);
            respuesta.setTipo("INFO");
            respuesta.setAtributo("message", "success");
            respuesta.setAtributo("operation", "JOIN_ROOM");
            respuesta.setAtributo("roomname", sala->getNombre());
            destinatarios.clear();
            destinatarios.push_back(cliente);
            envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
            return envios;
        }
    }
    respuesta.setTipo("WARNING");
    respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' no existe");
    respuesta.setAtributo("operation", "JOIN_ROOM");
    respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> roomMessage(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;

    for(Sala *sala : salas){
        if(sala->getNombre() == mensaje.getAtributo("roomname")){
            if(!sala->estaEnSala(cliente)){
                respuesta.setTipo("WARNING");
                respuesta.setAtributo("message", "El usuario no se ha unido al cuarto '" + sala->getNombre() + "'");
                respuesta.setAtributo("operation", "ROOM_MESSAGE");
                respuesta.setAtributo("roomname", sala->getNombre());
                destinatarios.push_back(cliente);
                envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
                return envios;
            }
            respuesta.setTipo("ROOM_MESSAGE_FROM");
            respuesta.setAtributo("roomname", sala->getNombre());
            respuesta.setAtributo("username", cliente->getNombre());
            respuesta.setAtributo("message", mensaje.getAtributo("message"));
            for(Usuario *usuario : sala->getUsuarios()){
                if(usuario != cliente){
                    destinatarios.push_back(usuario);
                }
            }
            envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
            return envios;
        }
    }
    respuesta.setTipo("WARNING");
    respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' no existe");
    respuesta.setAtributo("operation", "ROOM_MESSAGE");
    respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> status(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;

    if(mensaje.getAtributo("status") == cliente->getEstado()){
        respuesta.setTipo("WARNING");
        respuesta.setAtributo("message", "El estado ya es '" + cliente->getEstado() + "'");
        respuesta.setAtributo("operation", "STATUS");
        respuesta.setAtributo("status", mensaje.getAtributo("status"));
        destinatarios.push_back(cliente);
        envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
        return envios;
    }
    cliente->setEstado(mensaje.getAtributo("status"));
    respuesta.setTipo("INFO");
    respuesta.setAtributo("message", "success");
    respuesta.setAtributo("operation", "STATUS");
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));

    Mensaje newStatus;
    newStatus.setTipo("NEW_STATUS");
    newStatus.setAtributo("username", cliente->getNombre());
    newStatus.setAtributo("status", mensaje.getAtributo("status"));
    destinatarios.clear();
    for(Usuario *usuario : clientes){
        if(usuario != cliente){
            destinatarios.push_back(usuario);
        }
    }
    envios.insert(pair<string,list<Usuario*>>(newStatus.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> users(Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;

    respuesta.setTipo("USER_LIST");
    respuesta.setLista("usernames", clientes);
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> roomUsers(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;
    destinatarios.push_back(cliente);

    for(Sala *sala : salas){
        if(sala->getNombre() == mensaje.getAtributo("roomname")){
            if(!sala->estaEnSala(cliente)){
                respuesta.setTipo("WARNING");
                respuesta.setAtributo("message", "El usuario no se ha unido al cuarto '" + sala->getNombre() + "'");
                respuesta.setAtributo("operation", "ROOM_USERS");
                respuesta.setAtributo("roomname", sala->getNombre());
                envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
                return envios;
            }
            respuesta.setTipo("ROOM_USER_LIST");
            respuesta.setLista("usernames", sala->getUsuarios());
            envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
            return envios;
        }
    }
    respuesta.setTipo("WARNING");
    respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' no existe");
    respuesta.setAtributo("operation", "ROOM_USERS");
    respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> leaveRoom(Mensaje mensaje, Usuario *cliente){
    list<Usuario*> destinatarios;
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;

    for(Sala *sala : salas){
        if(sala->getNombre() == mensaje.getAtributo("roomname")){
            if(!sala->estaEnSala(cliente)){
                respuesta.setTipo("WARNING");
                respuesta.setAtributo("message", "El usuario no se ha unido al cuarto '" + sala->getNombre() + "'");
                respuesta.setAtributo("operation", "LEAVE_ROOM");
                respuesta.setAtributo("roomname", sala->getNombre());
                destinatarios.push_back(cliente);
                envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
                return envios;
            }
            if(sala->elimina(cliente)){
                salas.remove(sala);
            }
            Mensaje leftRoom;
            leftRoom.setTipo("LEFT_ROOM");
            leftRoom.setAtributo("roomname", sala->getNombre());
            leftRoom.setAtributo("username", cliente->getNombre());
            destinatarios = sala->getUsuarios();
            destinatarios.remove(cliente);
            envios.insert(pair<string,list<Usuario*>>(leftRoom.toString(), destinatarios));

            respuesta.setTipo("INFO");
            respuesta.setAtributo("message", "success");
            respuesta.setAtributo("operation", "LEAVE_ROOM");
            respuesta.setAtributo("roomname", sala->getNombre());
            destinatarios.clear();
            destinatarios.push_back(cliente);
            envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
            return envios;
        }
    }
    respuesta.setTipo("WARNING");
    respuesta.setAtributo("message", "El cuarto '" + mensaje.getAtributo("roomname") + "' no existe");
    respuesta.setAtributo("operation", "LEAVE_ROOM");
    respuesta.setAtributo("roomname", mensaje.getAtributo("roomname"));
    destinatarios.push_back(cliente);
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), destinatarios));
    return envios;
}

map<string,list<Usuario*>> disconnect(Usuario *cliente){
    map<string,list<Usuario*>> envios;
    Mensaje respuesta;
    list<Sala*> vacias;

    clientes.remove(cliente);
    respuesta.setTipo("DISCONNECTED");
    respuesta.setAtributo("username", cliente->getNombre());
    envios.insert(pair<string,list<Usuario*>>(respuesta.toString(), clientes));

    for(Sala *sala : salas){
        if(sala->estaEnSala(cliente)){
            Mensaje leftRoom;
            leftRoom.setTipo("LEFT_ROOM");
            leftRoom.setAtributo("roomname", sala->getNombre());
            leftRoom.setAtributo("username", cliente->getNombre());
            if(sala->elimina(cliente)){
                vacias.push_back(sala);
            }
            envios.insert(pair<string,list<Usuario*>>(leftRoom.toString(), sala->getUsuarios()));
        }
    }
    for(Sala *sala : vacias){
        salas.remove(sala);
    }

    close(cliente->getSocket());
    return envios;
}