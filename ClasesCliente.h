#include "Mensaje.cpp"
#include <iostream>

using namespace std;

class Interfaz{

 public:
    string lee(){
        string s;
        cin.ignore();
        getline(cin, s);
        return s;
    }
};


class ControlCliente{
 private:
    Interfaz interfaz;

 public:
    string ponNombre(){
        Mensaje mensaje;
        mensaje.setTipo("IDENTIFY");
        mensaje.setAtributo("username", interfaz.lee());
        return mensaje.toString();
    }
};
