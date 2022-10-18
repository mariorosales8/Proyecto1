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
        if(primeraEntrada){
            cin.ignore();
            primeraEntrada = false;
        }
        getline(cin, s);
        return s;
    }
    string lee(){
        return lee(false);
    }

    void imprime(string s, bool saltoDeLinea){
        if(saltoDeLinea)
            cout << s << endl;
        else
            cout << s;
    }
    void imprime(string s){
        imprime(s, true);
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

    void quitaSala(){
        salaVisible = "";
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
        mensaje.setAtributo("username", interfaz.lee());
        return mensaje.toString();
    }

    void desconectar(){
        desconectado = true;
        primeraEntrada = true;
        tipoSala = '-';
        sala = "";
        salas.clear();
        invitaciones.clear();
        mensajeEnEspera = NINGUNO;
        informacion = "";
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
        mensajeEnEspera = NINGUNO;
        informacion = "";
        interfaz.imprime("--- " + advertencia.getAtributo("message") + " ---");
    }

    void imprime(string s, bool saltoDeLinea){
        interfaz.imprime(s, saltoDeLinea);
    }
    void imprime(string s){
        imprime(s, true);
    }

    string lee(){
        return interfaz.lee();
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
            if(entrada.length() > 2){
                if(tipoSala == 's'){
                    mensaje.setTipo("ROOM_USERS");
                    mensaje.setAtributo("roomname", sala);
                    return mensaje.toString();
                }
                interfaz.imprime("No estás en una sala");
                return "";
            }
            mensaje.setTipo("USERS");
            return mensaje.toString();
        }
        if(entrada[1] == 'v'){
            string salida;
            if(entrada.length() < 3 || entrada[2] != 'i'){
                salida = "Salas a las que te uniste:";
                for(string cuarto : salas){
                    salida += "\n  " + cuarto;
                }
                interfaz.imprime(salida);
            }
            if(entrada.length() < 3 || entrada[2] != 's'){
                salida = "Invitaciones pendientes:";
                for(string invitacion : invitaciones){
                    salida += "\n  " + invitacion;
                }
                interfaz.imprime(salida);
            }
            return "";
        }
        if(entrada[1] == 'a'){
            switch(tipoSala){
                case '-':
                    interfaz.cambiaSala("Chat público");
                    break;
                case 'p':
                    interfaz.cambiaSala(sala + " (Chat privado)");
                    break;
                case 's':
                    interfaz.cambiaSala(sala + " (Sala)");
                    break;
            }
            return "";
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
                interfaz.imprime("Uniéndose a " + informacion + "...");
            }
            mensaje.setAtributo("roomname", informacion);
            invitaciones.remove(informacion);
            return mensaje.toString();
        }
        if(entrada[1] == 'h'){
            string ayuda = "\n";
            ayuda.append("Si escribes un mensaje, se enviará a la sala actual\n")
                .append("Los comandos disponibles son:\n")
                .append("-p:    Cambia al chat público\n")
                .append("-p usuario:   Cambia al chat privado con el ususario especificado\n")
                .append("-s sala:   Cambia a la sala especificada, si no estás en la sala pero si estás invitado, te unes a ella\n")
                .append("-u:    Muestra los usuarios conectados\n")
                .append("-uu:   Si estás en una sala, muestra los usuarios en ésta\n")
                .append("-c nombre:   Crea una sala con el nombre indicado\n")
                .append("-i usuario1, usuario2,...:   Invita a la sala actual a los usuarios indicados\n")
                .append("-j:    Te unes a la última sala a la que te invitaron\n")
                .append("-j sala:  Te unes a la sala especificada si estás invitado\n")
                .append("-q:    Sales de la sala actual\n")
                .append("-q sala:   Sales de la sala especificada\n")
                .append("-ea:   Cambia el estado a ACTIVE\n")
                .append("-ew:   Cambia el estado a AWAY\n")
                .append("-eb:   Cambia el estado a BUSY\n")
                .append("-v:    Muestra las salas a las que te uniste y tus invitaciones pendientes\n")
                .append("-vs:   Muestra las salas a las que te uniste\n")
                .append("-vi:   Muestra tus invitaciones pendientes\n")
                .append("-a:    Muestra la sala actual\n")
                .append("-d:    Desconectarse\n")
                .append("-h:    Muestra esta ayuda");

                interfaz.imprime(ayuda);
                interfaz.quitaSala();
            return "";
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
        if(entrada[1] == 'd'){
            mensaje.setTipo("DISCONNECT");
            desconectar();
            return mensaje.toString();
        }


        interfaz.imprime("Comando no reconocido.\nEscribe -h para mostrar la ayuda");
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

    void disconnected(Mensaje mensaje){
        interfaz.imprime("--- " + mensaje.getAtributo("username") + " se desconectó ---");
    }
};

bool identifica();
void *recibe(void*);
void *envia(void*);
void ejecutaMensaje(string);

