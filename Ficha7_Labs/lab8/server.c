#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"

#define MAX_PORT ((1<<16)-1)
int fill_date_time(uint8_t clnt_op,char *S,size_t s_len);

int main(int argc, char *argv[])
{
	struct gengetopt_args_info args;

	// cmdline_parser: deve ser a primeira linha de código no main
	if( cmdline_parser(argc, argv, &args) ){
		fprintf(stderr, "cannot cmdline_parser\n");
		exit(99);
	}

	//Verify Port
	int port = args.porto_arg;
	if (port<=0 || port>MAX_PORT) {
		fprintf(stderr, "port %d out of range, Should be within [1,%d]\n", port, MAX_PORT);
		exit(2);
	}

	//Socket
	int tcp_server_socket;
	if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		ERROR(51, "Can't create tcp_server_socket (IPv4)");
	}

	//Bind
	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); 		// Todas as interfaces de rede
	tcp_server_endpoint.sin_port = htons(port);		// Server port
	int ret_bind = bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(tcp_server_endpoint));
	if (ret_bind == -1) {
		ERROR(52, "Can't bind @tcp_server_endpoint");
	}

	//Listen
	int backlog = 2;
	if(listen(tcp_server_socket, backlog) == -1){
		ERROR(53, "Can't listen @tcp_server_endpoint");
	}

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_len;
	char clnt_IP_S[128];
	size_t num_clnt=0;
	while (1) {
		clnt_addr_len = sizeof(clnt_addr);
		int clnt_sock = accept(tcp_server_socket, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
		if (clnt_sock == -1) {
			fprintf(stderr, "ERROR: problem with accept: %s\n", strerror(errno));
			continue;
		}

		const char *ret_prt = inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, clnt_IP_S, clnt_addr_len);
		if (ret_prt == NULL) {
			fprintf(stderr, "Cannot inet_ntop: %s\n", strerror(errno));
			close(clnt_sock);
			continue;
		}
		num_clnt++;
		printf("Client %zu: IP=%s, port=%u\n",num_clnt, clnt_IP_S, ntohs(clnt_addr.sin_port));

		//Deal with clients
		sleep(2);
		char S[128];
		uint8_t clnt_op;
		ssize_t ret_recv = recv(clnt_sock, &clnt_op, sizeof(clnt_op), 0);
		if (ret_recv == -1) {
			fprintf(stderr, "Cannot recv: %s\n", strerror(errno));
			continue;
		}
		if (ret_recv == 0) {
			fprintf(stderr, "Remote peer has closed\n");
			close(clnt_sock);
			continue;
		}
		fill_date_time(clnt_op,S,sizeof(S));
		printf("S=%s\n",S);
		ssize_t ret_send = send(clnt_sock,S,strlen(S),0);
		if (ret_send == -1) {
			fprintf(stderr, "Cannot send: %s\n", strerror(errno));
			close(clnt_sock);
			continue;
		}

		close(clnt_sock);
	}

	close(tcp_server_socket);

	// libertar recurso (cmdline_parser)
	cmdline_parser_free(&args);

	exit(0);
}

int fill_date_time(uint8_t clnt_op,char *S,size_t s_len){
	time_t now=time(NULL);
	struct tm *tmp;
	tmp=localtime(&now);
	if(tmp==NULL){
		ERROR(EXIT_FAILURE,"Cannot localtime");
	}

	if(clnt_op==0){
		strftime(S, s_len, "%Y%m%d_%Hh%M.%S", tmp);

	}else if(clnt_op==1){
		strftime(S, s_len, "%Y%m%d", tmp);
	}
	else if(clnt_op==2){
		strftime(S, s_len, "%Hh%M.%S", tmp);
	}
	else{
		snprintf(S, s_len, "Invalid code %u\n", clnt_op);
	}
}
