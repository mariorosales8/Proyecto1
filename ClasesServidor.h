#include "Mensaje.cpp"
#include <iostream>
#include <list>
#include <map>

using namespace std;

class Usuario{
private:
    string nombre;
    string estado;
    int socket;

public:
    Usuario(int socket){
        this->socket = socket;
    }
    
    int getSocket(){
        return socket;
    }
    string getNombre(){
        return nombre;
    }
    void setNombre(string nombre){
        this->nombre = nombre;
    }
};


class Sala{
private:
    list<Usuario> usuarios;
};


class ControlServidor{

public:
    bool ponNombre(string json, Usuario *usuario){
        Mensaje mensaje(json);
        if(mensaje.esValido() && mensaje.getAtributo("username") != ""){
            usuario->setNombre(mensaje.getAtributo("username"));
            return true;
        }
        return false;
    }
};

void desconectar(Usuario*, string);
void leeIdentificacion(Usuario*);
map<Mensaje*,list<Usuario*>> identifica(string, Usuario*);

enum tipo{ERROR,WARNING,INFO,STATUS,USERS,MENSAJE,PUBLIC_MESSAGE,
          NEW_ROOM,INVITE,JOIN_ROOM,ROOM_USERS,ROOM_MESSAGE,
          LEAVE_ROOM,DISCONNECT,NEW_USER,NES_STATUS,USER_LIST,
          MESSAGE_FROM,PUBLIC_MESSAGE_FROM,JOINED_ROOM,
          ROOM_USER_LIST,ROOM_MESSAGE_FROM,LEFT_ROOM,DISCONNECTED};

map<string, tipo> tipos = {{"ERROR",ERROR},{"WARNING",WARNING},
                           {"INFO",INFO},{"STATUS",STATUS},
                           {"USERS",USERS},{"MENSAJE",MENSAJE},
                           {"PUBLIC_MESSAGE",PUBLIC_MESSAGE},
                           {"NEW_ROOM",NEW_ROOM},{"INVITE",INVITE},
                           {"JOIN_ROOM",JOIN_ROOM},{"ROOM_USERS",ROOM_USERS},
                           {"ROOM_MESSAGE",ROOM_MESSAGE},
                           {"LEAVE_ROOM",LEAVE_ROOM},
                           {"DISCONNECT",DISCONNECT},
                           {"NEW_USER",NEW_USER},{"NES_STATUS",NES_STATUS},
                           {"USER_LIST",USER_LIST},
                           {"MESSAGE_FROM",MESSAGE_FROM},
                           {"PUBLIC_MESSAGE_FROM",PUBLIC_MESSAGE_FROM},
                           {"JOINED_ROOM",JOINED_ROOM},
                           {"ROOM_USER_LIST",ROOM_USER_LIST},
                           {"ROOM_MESSAGE_FROM",ROOM_MESSAGE_FROM},
                           {"LEFT_ROOM",LEFT_ROOM},
                           {"DISCONNECTED",DISCONNECTED}};