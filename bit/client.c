/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"

int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
	    exit(ERR_ARGS);
    }

    int tcp_client_socket;
    if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      ERROR(41, "Can't create tcp_client_socket (IPv4): %s\n", strerror(errno));

    // TCP IPv4: connect ao IP/porto do servidor
    struct sockaddr_in tcp_server_endpoint;
    memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
    tcp_server_endpoint.sin_family = AF_INET;
      if (inet_pton(AF_INET, args_info.ip_arg, &tcp_server_endpoint.sin_addr) <= 0)	// Server IP
      ERROR(42, "Can't convert IP address: %s\n", strerror(errno));
    tcp_server_endpoint.sin_port = htons(args_info.port_arg);						// Server port

    printf("a ligar ao servidor... "); fflush(stdout);
    if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
      ERROR(43, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
    printf("ok. \n");

    // TCP IPv4: informação sobre IP/porto do cliente (porto atribuído automaticamente)
    struct sockaddr_in tcp_client_endpoint;
    socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
    char tcp_client_string_ip[20];
    if (getsockname(tcp_client_socket, (struct sockaddr *)&tcp_client_endpoint, &tcp_client_endpoint_length) == -1)
      ERROR(44, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
    printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));



    // aqui... a comunicação com o servidor


    // TCP IPv4: fecha socket (client)






    //----- TCP cliente :: comunicação -  (** não copiar este comentário **)

    // TCP IPv4: variáveis auxiliares para send() / recv()
    ssize_t tcp_read_bytes, tcp_sent_bytes;
    //char buffer[???];
    //...

    // TCP IPv4: "send" para o servidor
    printf("a enviar dados para o servidor... "); fflush(stdout);
    char domain[]="www.google.pt";
    if ((tcp_sent_bytes = send(tcp_client_socket, domain, strlen(domain), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    if (close(tcp_client_socket) == -1)
      ERROR(45, "Can't close tcp_client_socket (IPv4): %s\n", strerror(errno));
    printf("ligação fechada. ok. \n");

    cmdline_parser_free(&args_info);

    return 0;
}
