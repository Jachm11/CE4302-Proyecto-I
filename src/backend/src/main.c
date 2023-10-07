
#include "cJSON.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "serialize.h"

int server_socket, client_socket;
socklen_t client_addr_len;
struct sockaddr_in server_addr, client_addr; 

#define MAX_BUFFER_SIZE 4096

void create_socket(char* ip, int port){

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);
}

char* process_command(char* buffer){

        cJSON *client_command = cJSON_Parse(buffer);
        char* command = cJSON_GetObjectItem(client_command, "command")->valuestring;
        if(strcmp(command, "start") == 0){

            // Start execution
            printf("command %s \n", command);
            char* result = start_execution();
            FILE* file = fopen("/tmp/cache_exec_arqui.txt", "w");
            fwrite(result, 1, strlen(result), file);
            fclose(file);
            return "ok";
        } 
        else {
            // Invalid command
        }
}


int main(){
    srand(time(NULL));

    create_socket("127.0.0.1", 4000);
    char buffer[MAX_BUFFER_SIZE];
    while (1) {
        // Accept client connection
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        ssize_t bytesReceived;
        ssize_t totalBytesReceived = 0;
        memset(buffer, 0, MAX_BUFFER_SIZE);
        
        int bytes_received;
        
        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            // Process received data
            buffer[bytes_received] = '\0';
            printf("Received: %s \n", buffer);
            // You can send a response back to the client using send() here
            break;
        }
        // size_t response_size = 64 * 1024; // 50KB in bytes
        // // Allocate memory for the response buffer
        // char* response = (char*)malloc(response_size);
        // if (response == NULL) {
        //     perror("Memory allocation failed");
        // } else {
        //    char* response = process_command(buffer);
        // }
        char *response = process_command(buffer);

        char* response = process_command(buffer);

        if (send(client_socket, response, strlen(response), 0) < 0) {
                perror("Send failed");
                close(client_socket);
                break; // Close the connection if send fails
        }
        //close(client_socket);
    }

    close(server_socket);



}