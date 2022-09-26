#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Mensaje.cpp"
#include <json/json.h>

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
    REQUIRE(m.getAtributo("ninguno") == "");
    
    m = Mensaje(m.toString());
    REQUIRE(m.esValido());
    REQUIRE(m.getTipo() == "Prueba");
    REQUIRE(m.getAtributo("nombre") == "Juan");
    REQUIRE(m.getAtributo("ninguno") == "");

    m = Mensaje("Juan, 20 años");
    REQUIRE(!m.esValido());
}