#include "Mensaje.cpp"
#include "tipos.h"
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


void *recibe(void* args);
void desconectar(Usuario*, string);
void recibeIdentificacion(Usuario*);
map<string,list<Usuario*>> ejecutaMensaje(string, Usuario*);
map<string,list<Usuario*>> message(Mensaje mensaje, Usuario *cliente);

map<string,list<Usuario*>> identifica(string, Usuario*);
map<string,list<Usuario*>> publicMessage(Mensaje, Usuario*);