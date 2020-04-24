#include <string>

using namespace std;

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <hashlib++/hashlibpp.h>
#define PORT 8080


Json::Value toJson(string message){
    Json::Value val;
    Json::Reader reader;
    bool b = reader.parse(message, val);
    if (!b)
        cout << "Error: " << reader.getFormattedErrorMessages();

    return val;
}

void manageLogin(string message, string *userName) {
    string line;
    ifstream MyReadFile("users.txt");

    int pos = message.find_first_of(";");
    string clientUser = message.substr(0, pos);
    message.erase(0, pos + 1);
    string clientPassword = message;

    string user;
    string password;

    if (password == clientPassword) {
        bool userFound = false;
        while (getline(MyReadFile, line)) {
            pos = line.find_first_of(";");
            user = line.substr(0, pos);

            if (user == clientUser) {
                userFound = true;
                *userName = user;
                break;
            }
        }
        MyReadFile.close();
        if (!userFound) {
            std::ofstream outfile;

            outfile.open("users.txt", std::ios_base::app);
            outfile << clientUser;
            *userName = clientUser;
        }
    } else {

    }

}

void manageCalls(int sd, char buffer[1024], string *user) {
    string message;
    string command;
    int pos;
    message = buffer;
    pos = message.find_first_of(";");
    command = message.substr(0, pos);
    message.erase(0, pos + 1);


    pos = message.find_first_of(";");
    string clientUser = message.substr(0, pos);

    if (command == "log-in") {
        manageLogin(message, user);
    } else if (!user->empty()) {
        if (command == "new-vs") {
            Json::Value data = toJson(message);
            data["type"];
            data["data"];
        } else if (command == "new-ref") {

        } else if (command == "delete-ref") {

        }
    }

}



//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE   1
#define FALSE  0
#define PORT 8888

string getmd5(string pass){
    md5wrapper md5=md5wrapper();
    pass=md5.getHashFromString(pass);
    return pass;
}

int main(int argc, char *argv[]) {
    string password="myserverpassword";
    password=getmd5(password);
    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[30],
            max_clients = 30, activity, i, valread, sd;
    string user[30];
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];
    fd_set readfds;
    char *message = "ECHO Daemon v1.0 \r\n";

    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (TRUE) {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket,
                   inet_ntoa(address.sin_addr), ntohs
                           (address.sin_port));

            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    getpeername(sd, (struct sockaddr *) &address, \
                        (socklen_t *) &addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                }
                else {
                    manageCalls(sd, buffer, &user[i]);
                }
            }
        }
    }

    return 0;
}
