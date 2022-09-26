#include "Mensaje.cpp"
#include <iostream>
#include <list>

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
    void setNombre(string nombre){
        this->nombre = nombre;
    }
    string getNombre(){
        return nombre;
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


void desconectar(Usuario *socket);

