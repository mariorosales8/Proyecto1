#include "Mensaje.cpp"
#include "tipos.h"
#include <iostream>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

class Sala{
private:
    string nombre;
    list<Usuario*> usuarios;
    list<Usuario*> invitados;

public:
    Sala(string nombre, Usuario *usuario){
        this->nombre = nombre;
        usuarios.push_back(usuario);
    }

    string getNombre(){
        return nombre;
    }

    list<Usuario*> getUsuarios(){
        return usuarios;
    }

    void agrega(Usuario *usuario){
        usuarios.push_back(usuario);
        invitados.remove(usuario);
    }

    bool elimina(Usuario *usuario){
        usuarios.remove(usuario);
        return usuarios.empty();
    }

    void invita(list<Usuario*> usuarios){
        for(Usuario *usuario : usuarios){
            invitados.push_back(usuario);
        }
    }

    bool estaInvitado(Usuario *usuario){
        return find(invitados.begin(), invitados.end(), usuario) != invitados.end();
    }

    bool estaEnSala(Usuario *usuario){
        return find(usuarios.begin(), usuarios.end(), usuario) != usuarios.end();
    }
};


void *recibe(void* args);
void envia(map<string,list<Usuario*>>);
void desconectarSocket(Usuario*, string);
void recibeIdentificacion(Usuario*);
map<string,list<Usuario*>> ejecutaMensaje(string, Usuario*);
map<string,list<Usuario*>> message(Mensaje mensaje, Usuario *cliente);

map<string,list<Usuario*>> identifica(string, Usuario*);
map<string,list<Usuario*>> publicMessage(Mensaje, Usuario*);
map<string,list<Usuario*>> newRoom(Mensaje, Usuario*);
map<string,list<Usuario*>> invite(Mensaje, Usuario*);
map<string,list<Usuario*>> joinRoom(Mensaje, Usuario*);
map<string,list<Usuario*>> roomMessage(Mensaje, Usuario*);
map<string,list<Usuario*>> status(Mensaje, Usuario*);
map<string,list<Usuario*>> users(Usuario*);
map<string,list<Usuario*>> roomUsers(Mensaje, Usuario*);
map<string,list<Usuario*>> leaveRoom(Mensaje, Usuario*);
map<string,list<Usuario*>> disconnect(Usuario*);