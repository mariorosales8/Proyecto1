#include "Usuario.cpp"
#include <json/json.h>
#include <map>
#include<list>
#include <iostream>

using namespace Json;
using namespace std;

class Mensaje{
private:
    Value atributos;

public:
    /**
     * @brief Constructor por defecto
     */
    Mensaje(){}
    /**
     * @brief Constructor por JSON
     * @param jsonString Un string en formato JSON
     */
    Mensaje(string jsonString){
        Reader reader;
        reader.parse(jsonString, atributos);
    }

    string getTipo(){
        return atributos["type"].asString();
    }
    void setTipo(string tipo){
        atributos["type"] = tipo;
    }

    /**
     * @brief Obtiene un atributo del mensaje
     * @param llave La llave del atributo que se quiere obtener
     * @return string El atributo del mensaje
     */
    string getAtributo(string llave){
        return atributos[llave].asString();
    }
    /**
     * @brief Obtiene la lista de elementos de un atributo
     * @param llave La llave del atributo que se quiere obtener
     * @return list<string> La lista de elementos si el atributo es una lista, una lista vacía si no lo es
     */
    list<string> getLista(string llave){
        list<string> lista;
        if(atributos[llave].isArray()){
            for (auto &elemento : atributos[llave]){
                lista.push_back(elemento.asString());
            }
        }
        return lista;
    }

    /**
     * @brief Agrega un atributo al mensaje
     * @param llave La llave en la que se quiere agregar el atributo
     * @param valor El valor del atributo que se quiere agregar
     */
    void setAtributo(string llave, string valor){
        atributos[llave] = valor;
    }

    /**
     * @brief Agrega una lista al mensaje
     * @param llave La llave en la que se quiere agregar la lista
     * @param lista La lista de strings que se quiere agregar
     */
    void setLista(string llave, list<string> lista){
        for(string s : lista){
            atributos[llave].append(s);
        }
    }
    /**
     * @brief Agrega una lista al mensaje
     * @param llave La llave en la que se quiere agregar la lista
     * @param lista La lista de usuarios que se quiere agregar
     */
    void setLista(string llave, list<Usuario*> lista){
        for(Usuario *usuario : lista){
            atributos[llave].append(usuario->getNombre());
        }
    }

    /**
     * @brief Convierte el mensaje a un string
     * @return string El mensaje en formato json
     */
    string toString(){
        FastWriter writer;
        return writer.write(atributos);
    }

    /**
     * @brief Comprueba que el mensaje sea válido
     * @return true si el tipo es distinto de vacío, false en otro caso
     */
    bool esValido(){
        return !getTipo().empty();
    }

};
