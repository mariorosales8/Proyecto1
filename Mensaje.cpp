#include <json/json.h>
#include <map>
#include <iostream>

using namespace Json;
using namespace std;

class Mensaje{
private:
    string tipo;
    map<string,string> atributos;

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
        Value json;
        reader.parse(jsonString, json);
        tipo = json["type"].asString();
        for(auto &llave : json.getMemberNames()){
           if(llave != "type"){
                atributos[llave] = json[llave].asString();
           }
        }
    }

    string getTipo(){
        return tipo;
    }
    void setTipo(string tipo){
        this->tipo = tipo;
    }

    /**
     * @brief Obtiene un atributo del mensaje
     * @param llave La llave del atributo que se quiere obtener
     * @return string El atributo del mensaje
     */
    string getAtributo(string llave){
        return atributos[llave];
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
     * @brief Convierte el mensaje a un string
     * @return string El mensaje en formato json
     */
    string toString(){
        FastWriter writer;
        Value json;
        json["type"] = tipo;
        for(auto &[llave, atributo] : atributos){
            json[llave] = atributo;
        }
        return writer.write(json);
    }

};
