#include "Mensaje.cpp"
#include "tipos.h"
#include <iostream>

using namespace std;

class Interfaz{
private:
string salaVisible = "Chat público";

 public:
    string lee(bool primeraEntrada){
        string s;
        if(primeraEntrada)
            cin.ignore();
        getline(cin, s);
        return s;
    }
    string lee(){
        return lee(false);
    }

    void imprime(string s){
        cout << s << endl;
    }


    void mensaje(string mensaje, string sala){
        if(salaVisible != sala){
            salaVisible = sala;
            cout << "// " << sala << " //" << endl;
        }
        cout << mensaje << endl;
    }

    void cambiaSala(string sala){
        if(salaVisible != sala){
            salaVisible = sala;
            cout << sala << " <<<<<<<<<<<<" << endl;
        }
    }


};


class ControlCliente{
 private:
    Interfaz interfaz;
    bool desconectado;
    bool primeraEntrada;
    char tipoSala = '-';
    string destinatario;

 public:
    string ponNombre(){
        Mensaje mensaje;
        mensaje.setTipo("IDENTIFY");
        mensaje.setAtributo("username", interfaz.lee(primeraEntrada));
        primeraEntrada = false;
        return mensaje.toString();
    }

    void desconectar(){
        desconectado = true;
        primeraEntrada = true;
    }
    void conectar(){
        desconectado = false;
    }
    bool estaDesconectado(){
        return desconectado;
    }


    void warning(string mensaje){
        interfaz.imprime(mensaje);
    }


    string escribeMensaje(){
        String entrada = interfaz.lee();
        Mensaje mensaje;
        if(entrada == "" || entrada.front() != '-'){
            if(tipoSala == '-'){
                mensaje.setTipo("PUBLIC_MESSAGE");
                mensaje.setAtributo("message", entrada);
                return mensaje.toString();
            }
            if(tipoSala == 'p'){
                mensaje.setTipo("MESSAGE");
                mensaje.setAtributo("username", destinatario);
                mensaje.setAtributo("message", entrada);
                return mensaje.toString();
            }

            return "";
        }
        
        if(entrada[1] == 'p'){
            if(entrada.length() > 3){
                tipoSala = 'p';
                destinatario = entrada.substr(3);
                interfaz.cambiaSala(destinatario + " (Chat privado)");
            }else{
                tipoSala = '-';
                interfaz.cambiaSala("Chat público");
            }
            return "";
        }

        
        return "";
    }


    void publicMessageFrom(Mensaje mensaje){
        interfaz.mensaje(mensaje.getAtributo("username") + ": " +
                         mensaje.getAtributo("message"), "Chat público");
    }

    void messageFrom(Mensaje mensaje){
        interfaz.mensaje("> " + mensaje.getAtributo("message"), 
                         mensaje.getAtributo("username") + " (Chat privado)");
    }

    void newUser(Mensaje mensaje){
        interfaz.imprime("--- " + mensaje.getAtributo("username") +  " se ha conectado ---");
    }
};

bool identifica();
void *recibe(void*);
void *envia(void*);
void ejecutaMensaje(string);

