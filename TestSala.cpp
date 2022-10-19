#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "ClasesServidor.h"
#include <list>

using namespace std;

TEST_CASE(){
    Usuario *u1 = new Usuario(1);
    Sala s("Sala 1", u1);
    REQUIRE(s.getUsuarios().front() == u1);
    REQUIRE(s.getUsuarios().size() == 1);
    
    list<Usuario*> lista;
    lista.push_back(new Usuario(1));
    lista.push_back(new Usuario(2));
    for(Usuario *u : lista){
        REQUIRE(!s.estaInvitado(u));
        REQUIRE(!s.estaEnSala(u));
    }
    s.invita(lista);
    for(Usuario *u : lista){
        REQUIRE(s.estaInvitado(u));
        REQUIRE(!s.estaEnSala(u));
        s.agrega(u);
        REQUIRE(s.getUsuarios().back() == u);
        REQUIRE(!s.estaInvitado(u));
        REQUIRE(s.estaEnSala(u));
    }
    REQUIRE(s.getUsuarios().size() == 3);

    for(Usuario *u : lista){
        REQUIRE(!s.elimina(u));
    }
    REQUIRE(s.getUsuarios().size() == 1);
    REQUIRE(s.elimina(u1));

    REQUIRE(s.getNombre() == "Sala 1");
}
