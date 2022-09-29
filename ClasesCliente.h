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
            if(tipoSala == 'p'){
                mensaje.setTipo("MESSAGE");
                mensaje.setAtributo("username", destinatario);
                mensaje.setAtributo("message", entrada);
                return mensaje.toString();
            }
            mensaje.setTipo("PUBLIC_MESSAGE");
            mensaje.setAtributo("message", entrada);
            return mensaje.toString();
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
        if(entrada[1] == 's'){
            if(entrada.length() > 3){
                tipoSala = 's';
                destinatario = entrada.substr(3);
                interfaz.cambiaSala(destinatario + " (Sala)");
            }else{
                warning("No se especificó el nombre de la sala");
            }
            return "";
        }
        if(entrada[1] == 'c'){
            if(entrada.length() > 3){
                mensaje.setTipo("NEW_ROOM");
                mensaje.setAtributo("roomname", entrada.substr(3));
                tipoSala = 's';
                interfaz.cambiaSala(entrada.substr(3) + " (Sala)");
                return mensaje.toString();
            }else{
                warning("No se especificó el nombre de la sala a crear");
            }
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

