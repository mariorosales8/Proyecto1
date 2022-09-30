#include <list>
#include <iostream>

using namespace std;

class Usuario{
private:
    string nombre;
    string estado = "ACTIVE";
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
    string getEstado(){
        return estado;
    }
    void setEstado(string estado){
        this->estado = estado;
    }
};
