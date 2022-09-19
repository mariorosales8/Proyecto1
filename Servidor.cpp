#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

using namespace std;

void error(const char*);

int main(){
    int puerto, server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];
    string mensaje;
    
    cout << "Puerto del servidor: ";
    cin >> puerto;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        error("Error al crear el socket");
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puerto);
    
    if(bind(server_fd, (struct sockaddr*)&address,
            sizeof(address))  <  0){
        error("Error. El número del puerto debe ser al menos 1024");
    }
    
    if(listen(server_fd, 3) < 0){
        error("Error en el listen");
    }
    
    new_socket = accept(server_fd, (struct sockaddr*)&address,
                        (socklen_t*)&addrlen);
    if(new_socket < 0){
        error("Error al aceptar al cliente");
    }

    for(int i=0; i < 3; i++){
        if(read(new_socket, buffer, 1024) < 0)
            error("Error al leer");
        cout << buffer << endl;
        cin >> mensaje;
        if(send(new_socket, mensaje.c_str(), strlen(mensaje.c_str()), 0) < 0){
            error("Error al escribir");
        }
    }

    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);

    
    
    return 0;
}


void error(const char* error){
    cout << error << endl;
    main();
}
