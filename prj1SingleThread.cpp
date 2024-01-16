#include<iostream>
#include<fstream>
#include<stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class Server_socket{
    fstream file;

    int PORT;
    
    int general_socket_descriptor;
    int new_socket_descriptor;
    struct sockaddr_in address;
    int address_length;

    public:
    string fileName = "";
    bool fileTypePDF = false;
    bool fileTypeHTML = false;
    bool NotFound = false;
    std::string contentToSendHTML = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
    std::string contentToSendPDF = "HTTP/1.0 200 OK\r\nContent-Type: application/pdf\r\n\r\n";
    std::string contentToSend404 = "HTTP/1.0 404 Not Found\r\n\r\n404 Not Found";
        Server_socket(){
            create_socket();
            PORT = 1234;

            address.sin_family = AF_INET; 
            address.sin_addr.s_addr = INADDR_ANY; 
            address.sin_port = htons( PORT );
            address_length = sizeof(address);

            bind_socket();
            set_listen_set();
            accept_connection();
            read_request();
            openFile();
            //file.open(".//Data//Server//server_text.txt", ios::in | ios::binary);
        }

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
                perror("[ERROR] : Socket failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void bind_socket(){
            if (bind(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address))<0) {
                perror("[ERROR] : Bind failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Bind Successful.\n";
        }

        void set_listen_set(){
            if (listen(general_socket_descriptor, 3) < 0) {
                perror("[ERROR] : Listen");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket in Listen State (Max Connection Queue: 3)\n";
        }

        void accept_connection(){
            if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) { 
                perror("[ERROR] : Accept");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connected to Client.\n";
        }

        void read_request() {
            fileName = "";
            int buffer_size = 1024;
            char buffer[buffer_size];
            int request = read(new_socket_descriptor, buffer, buffer_size);
            string receivedMessage(buffer, request);
            string m = receivedMessage;
            //cout << "Received request: " << receivedMessage << endl;
            
            int length = receivedMessage.length();
            std::string token = receivedMessage.substr(receivedMessage.find(' '), buffer_size);
            token = receivedMessage.substr(0,receivedMessage.find(" "));
            //cout << "TOKEN: " << token << endl;
            for (int i = 0; i < length; i++) {
                if (m[i] == '/') {
                    i++;
                    while(m[i] != ' ') {
                        fileName.append(1, m[i]);
                        //cout << "e" << endl;
                        i++;
                        //fileName.append(m[i].c_str());
                    }
                    break;
                }
            }
            cout << "FILENAME: " << fileName << endl; // NOW THAT WE HAVE FILENAME TRY TO OPEN. IF CANT, RETURN 404; OTHERWISE CONSTRUCT RIGHT MESSAGE.
            if (fileName.back() == 'l') {
                fileTypeHTML = true;
            }
        }

        void openFile() {
            file.open(fileName, ios::in | ios::binary);
            if(file.is_open()){
                cout<<"[LOG] : File is ready to Transmit.\n";
            }
            else{
                NotFound = true;
                cout<<"[ERROR] : File loading failed, Exititng.\n";
            }
        }

        void transmit_file(){
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            //cout << contents << endl;
            cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

            cout<<"[LOG] : Sending...\n";
            cout << new_socket_descriptor << endl;
            int bytes_sent = 0;
            if (NotFound == true) {
                cout << "404 error called" << endl;
                bytes_sent = send(new_socket_descriptor , contentToSend404.c_str() , contentToSend404.length() , 0 );
            }
            else if (fileTypeHTML) {
                cout << "HTML FILE FOUND" << endl;
                contentToSendHTML.append(contents);
                bytes_sent = send(new_socket_descriptor , contentToSendHTML.c_str() , contentToSendHTML.length() , 0 );
            }
            else{
                cout << "PDF FILE FOUND" << endl;
                contentToSendPDF.append(contents);
                bytes_sent = send(new_socket_descriptor , contentToSendPDF.c_str() , contentToSendPDF.length() , 0 );
            }

            cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

            cout<<"[LOG] : File Transfer Complete.\n";
            fileTypeHTML = false;
            fileTypePDF = false;
            NotFound = false;
            file.close();
            close(new_socket_descriptor);
        }
};

int main(){
    Server_socket S;
    S.transmit_file();
    while(true) {
    S.set_listen_set();
    S.accept_connection();
    S.read_request();
    S.openFile();
    S.transmit_file();
    }
    return 0;
}