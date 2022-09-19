#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<iostream>

using namespace std;

void error(const char*);

int main(){
    string ip, mensaje;
    int puerto, sock, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    
    cout << "IP del servidor: ";
    cin >> ip;
    cout << "Puerto: ";
    cin >> puerto;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        error("Error al crear el socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto);

    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
        error("Error con la dirección del servidor");

    client_fd = connect(sock, (struct sockaddr*)&serv_addr, 
                        sizeof(serv_addr));
    if(client_fd < 0)
        error("No se pudo conectar al servidor");

    cin.ignore();
    for(int i=0; i < 3; i++){
        getline(cin, mensaje);
        send(sock, mensaje.c_str(), strlen(mensaje.c_str()), 0);
        bzero(buffer, 1024);
        read(sock, buffer, 1024);
        cout << buffer << endl;
    }

    close(client_fd);


    
    return 0;
}


void error(const char* error){
    cout << error << endl;
    main();
}
