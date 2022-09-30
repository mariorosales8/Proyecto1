#include "Mensaje.cpp"
#include "tipos.h"
#include <iostream>
#include <list>

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
        salaVisible = sala;
        cout << sala << " <<<<<<<<<<<<" << endl;
    }


};


class ControlCliente{
 private:
    Interfaz interfaz;
    bool desconectado;
    bool primeraEntrada;
    char tipoSala = '-';
    string sala;
    list<string> salas;
    list<string> invitaciones;
    tipo mensajeEnEspera = NINGUNO;
    string informacion;

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

    void info(Mensaje mensaje){
        if(mensaje.getAtributo("message") == "success"){
            switch(mensajeEnEspera){
                case JOIN_ROOM:
                    salas.push_back(informacion);
                    break;

                case JOINED_ROOM:
                    salas.push_back(informacion);
                    invitaciones.remove(informacion);
                    tipoSala = 's';
                    sala = informacion;
                    interfaz.cambiaSala(informacion + " (Sala)");
                    break;

                case NEW_ROOM:
                    salas.push_back(informacion);
                    tipoSala = 's';
                    sala = informacion;
                    interfaz.cambiaSala(informacion + " (Sala)");
                    break;

                default:
                    break;
            }
            mensajeEnEspera = NINGUNO;
            informacion = "";
        }
    }

    void warning(Mensaje advertencia){
        interfaz.imprime("--- " + advertencia.getAtributo("message") + " ---");
        mensajeEnEspera = NINGUNO;
        informacion = "";
    }

    void imprime(string s){
        interfaz.imprime(s);
    }


    string escribeComando(){
        String entrada = interfaz.lee();
        Mensaje mensaje;
        if(entrada.empty() || entrada.front() != '-'){
            if(tipoSala == 'p'){
                mensaje.setTipo("MESSAGE");
                mensaje.setAtributo("username", sala);
                mensaje.setAtributo("message", entrada);
                return mensaje.toString();
            }
            if(tipoSala == 's'){
                mensaje.setTipo("ROOM_MESSAGE");
                mensaje.setAtributo("roomname", sala);
                mensaje.setAtributo("message", entrada);
                return mensaje.toString();
            }
            mensaje.setTipo("PUBLIC_MESSAGE");
            mensaje.setAtributo("message", entrada);
            return mensaje.toString();
        }
        if(entrada[1] == 'u'){
            if(tipoSala == 's' && entrada.length() > 2){
                mensaje.setTipo("ROOM_USERS");
                mensaje.setAtributo("roomname", sala);
                return mensaje.toString();
            }
            mensaje.setTipo("USERS");
            return mensaje.toString();
        }
        if(entrada[1] == 'p'){
            if(entrada.length() > 3){
                tipoSala = 'p';
                sala = entrada.substr(3);
                interfaz.cambiaSala(sala + " (Chat privado)");
            }else{
                tipoSala = '-';
                sala = "";
                interfaz.cambiaSala("Chat público");
            }
            return "";
        }
        if(entrada[1] == 's'){
            if(entrada.length() > 3){
                for(string cuarto : salas){
                    if(cuarto == entrada.substr(3)){
                        tipoSala = 's';
                        sala = cuarto;
                        interfaz.cambiaSala(sala + " (Sala)");
                        return "";
                    }
                }
                for(string invitacion : invitaciones){
                    if(invitacion == entrada.substr(3)){
                        mensaje.setTipo("JOIN_ROOM");
                        mensajeEnEspera = JOINED_ROOM;
                        informacion = entrada.substr(3);
                        mensaje.setAtributo("roomname", informacion);
                        return mensaje.toString();
                    }
                }
                interfaz.imprime("No estás en la sala " + entrada.substr(3));
            }else{
                interfaz.imprime("No se especificó el nombre de la sala");
            }
            return "";
        }
        if(entrada[1] == 'c'){
            if(entrada.length() > 3){
                mensaje.setTipo("NEW_ROOM");
                mensaje.setAtributo("roomname", entrada.substr(3));
                mensajeEnEspera = NEW_ROOM;
                informacion = entrada.substr(3);
                return mensaje.toString();
            }else{
                interfaz.imprime("No se especificó el nombre de la sala a crear");
            }
        }
        if(entrada[1] == 'i'){
            if(tipoSala == 's'){
                if(entrada.length() > 3){
                    mensaje.setTipo("INVITE");
                    mensaje.setAtributo("roomname", sala);
                    list<string> invitados;
                    string invitado;
                    for(unsigned int i = 3; i < entrada.length(); i++){
                        if(entrada[i] == ','){
                            invitados.push_back(invitado);
                            invitado = "";
                            if(entrada[i+1] == ' '){
                                i++;
                            }
                        }else{
                            invitado += entrada[i];
                        }
                    }
                    invitados.push_back(invitado);
                    mensaje.setLista("usernames", invitados);
                    return mensaje.toString();
                }
                interfaz.imprime("No se especificaron los usuarios a invitar");
                return "";
            }
            interfaz.imprime("Debes estar en una sala para invitar a alguien");
            return "";
        }
        if(entrada[1] == 'j'){
            if(invitaciones.empty()){
                interfaz.imprime("No tienes invitaciones pendientes");
                return "";
            }
            mensaje.setTipo("JOIN_ROOM");
            mensajeEnEspera = JOIN_ROOM;
            if(entrada.length() > 3){
                informacion = entrada.substr(3);
            }else{
                informacion = invitaciones.back();
                interfaz.imprime("Entrando a la sala " + informacion + "...");
            }
            mensaje.setAtributo("roomname", informacion);
            invitaciones.remove(informacion);
            return mensaje.toString();
        }
        if(entrada[1] == 'e'){
            if(entrada.length() > 2){
                switch(entrada[2]){
                    case 'a':
                        mensaje.setAtributo("status", "ACTIVE");
                        break;
                    case 'w':
                        mensaje.setAtributo("status", "AWAY");
                        break;
                    case 'b':
                        mensaje.setAtributo("status", "BUSY");
                        break;
                    default:
                        interfaz.imprime("No es un estado válido");
                        return "";
                }
                mensaje.setTipo("STATUS");
                return mensaje.toString();
            }
            interfaz.imprime("No se especificó el estado");
            return "";
        }
        if(entrada[1] == 'q'){
            if(entrada.length() > 3){
                mensaje.setTipo("LEAVE_ROOM");
                mensaje.setAtributo("roomname", entrada.substr(3));
                salas.remove(entrada.substr(3));
                if(sala == entrada.substr(3)){
                    tipoSala = '-';
                    sala = "";
                    interfaz.cambiaSala("Chat público");
                }
                return mensaje.toString();
            }
            if(tipoSala == 's'){
                mensaje.setTipo("LEAVE_ROOM");
                mensaje.setAtributo("roomname", sala);
                salas.remove(sala);
                tipoSala = '-';
                sala = "";
                interfaz.cambiaSala("Chat público");
                return mensaje.toString();
            }
            interfaz.imprime("No se especificó la sala");
            return "";
        }


        interfaz.imprime("Comando no reconocido");
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

    void invitation(Mensaje mensaje){
        invitaciones.push_back(mensaje.getAtributo("roomname"));
        interfaz.imprime("+++++ " + mensaje.getAtributo("message"));
    }

    void joinedRoom(Mensaje mensaje){
        interfaz.imprime("--- " + mensaje.getAtributo("username") + " se ha unido a " +
                         mensaje.getAtributo("roomname") + " ---");
    }

    void roomMessageFrom(Mensaje mensaje){
        interfaz.mensaje(mensaje.getAtributo("username") + ": " +
                         mensaje.getAtributo("message"), 
                         mensaje.getAtributo("roomname") + " (Sala)");
    }

    void newStatus(Mensaje mensaje){
        interfaz.imprime("--- " + mensaje.getAtributo("username") + " ahora está " +
                         mensaje.getAtributo("status") + " ---");
    }

    void userList(Mensaje mensaje){
        list<string> nombres = mensaje.getLista("usernames");
        string salida = "Usuarios conectados: \n";
        for(string nombre : nombres){
            salida += nombre + "\n";
        }
        interfaz.imprime(salida.substr(0, salida.length()-1));
    }

    void roomUserList(Mensaje mensaje){
        list<string> nombres = mensaje.getLista("usernames");
        string salida = "Usuarios en la sala " + sala + ":\n";
        for(string nombre : nombres){
            salida += nombre + "\n";
        }
        interfaz.imprime(salida.substr(0, salida.length()-1));
    }

    void leftRoom(Mensaje mensaje){
        interfaz.imprime("--- " + mensaje.getAtributo("username") + " ha dejado la sala " +
                         mensaje.getAtributo("roomname") + " ---");
    }
};

bool identifica();
void *recibe(void*);
void *envia(void*);
void ejecutaMensaje(string);

