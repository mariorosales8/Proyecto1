#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Mensaje.cpp"
#include <json/json.h>
#include <list>

using namespace Json;
using namespace std;

TEST_CASE("Mensaje", "Mensaje"){
    Mensaje m;
    m.setTipo("Prueba");
    m.setAtributo("nombre", "Juan");
    m.setAtributo("edad", "20");
    REQUIRE(m.esValido());
    REQUIRE(m.getTipo() == "Prueba");
    REQUIRE(m.getAtributo("nombre") == "Juan");
    REQUIRE(m.getAtributo("ninguno").empty());

    list<string> lista;
    lista.push_back("1");
    lista.push_back("2");
    lista.push_back("elemento 3");
    m.setLista("lista", lista);
    REQUIRE(m.getLista("lista") == lista);
    REQUIRE(m.getLista("edad") == list<string>());
    
    m = Mensaje(m.toString());
    REQUIRE(m.esValido());
    REQUIRE(m.getTipo() == "Prueba");
    REQUIRE(m.getAtributo("nombre") == "Juan");
    REQUIRE(m.getAtributo("ninguno").empty());
    REQUIRE(m.getLista("lista") == lista);

    m = Mensaje("Juan, 20 años");
    REQUIRE(!m.esValido());
}