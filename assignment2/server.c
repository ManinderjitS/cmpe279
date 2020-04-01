// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/wait.h>
#include <string.h>
#define PORT 80

void handle_client_connections(int*, const char*);
int convert_str_to_int(const char*);
char* convert_int_to_str(int);
char *strrev_custom(char *);

int main(int argc, char const *argv[]) 
{ 
	if(argc > 1 && argv[1] == "execed"){
		printf("This is the execed version of the program\n.");
		
		if(setuid(1)){ //CHange to a non root user
			perror("Setuid");
		}
		int exec_new_sock = 0;
		exec_new_sock = convert_str_to_int(argv[2]);
		
		handle_client_connections(&exec_new_sock, argv[3]);
	}
	
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    
    int forked = 0;
    if((forked = fork()) < 0){
		perror("fork");
	}
	
	if(forked == 0){
		perror("Child");
		printf("THe set uid will b set to: %i\n", 1);
		
		char* str_new_sock;
		
		str_new_sock = convert_int_to_str(new_socket);
		char * argv_list[] = {"server","execed", str_new_sock, hello, NULL}; 
		//~ handle_client_connections(&new_socket, hello);
	}
	else{
		perror("Parent");
		close(new_socket);
	}
     
    int status;
	// try to wait for any children while there exists at least one
	while ((forked=waitpid(-1,&status,0))!=-1) {
	  printf("Process %d terminated\n",forked);
	}
     
    close(server_fd);
    return 0; 
} 


void handle_client_connections(int *new_socket, const char* hello){
	int valread;
	char buffer[1024] = {0}; 
	perror("Handle_client_connection");
	valread = read(*new_socket, buffer, 1024); 
    printf("%s\n",buffer ); 
    send(*new_socket , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n");
    if(setuid(0)){
		perror("Setuid back to root");
	}
}

int convert_str_to_int(const char *str){
	int str_len = strlen(str);
	int result = 0;
	
	for(int i = 0; i < str_len; i++){
		result = result * 10 + ( str[i] - '0' );
	}
	
	printf("This is the passed socket fd: %i\n", result);
	return result;
}

char* convert_int_to_str(int new_sock){
	int temp_val_holder = new_sock;
	int remainder = 0, divider = 10;
	char temp = '0';
	char str_new_sock[64];
	
	while(temp_val_holder >= 10){
		remainder = temp_val_holder % 10;
		char char_remainder = remainder + temp;
		strncat(str_new_sock, &char_remainder, 1); 
		temp_val_holder = temp_val_holder / 10;
	}
	
	char char_remainder = temp_val_holder + temp;
	strncat(str_new_sock, &char_remainder, 1);
	char *rev_str_new_sock = strrev_custom(str_new_sock);	
	return rev_str_new_sock;
}


char *strrev_custom(char *str){
	char *p1, *p2;
	if (! str || ! *str)  return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2){
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}
