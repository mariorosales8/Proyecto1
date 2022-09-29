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
    void setLista(string llave, list<string> lista){
        for(string s : lista){
            atributos[llave].append(s);
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
        return getTipo() != "";
    }

};
