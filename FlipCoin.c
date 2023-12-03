#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>


int main() {
    int choice_buffer;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Gagal inisialisasi Winsock\n");
        return 1;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Gagal membuat socket\n");
        WSACleanup();
        return 1;
    }

    puts("Apakah anda bermain pertama: \n1. Iya\n2.Tidak\n");
    scanf("%d", &choice_buffer);

    if(choice_buffer < 1 || choice_buffer > 2) {
        printf("Input invalid!");
    }

    if(choice_buffer == 2) {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(12345);  // Ganti dengan port yang sesuai
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Gagal binding\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        if (listen(sockfd, 1) == SOCKET_ERROR) {
            printf("Gagal mendengarkan\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        printf("Menunggu koneksi...\n");

        struct sockaddr_in new_addr;
        int addr_size = sizeof(new_addr);
        SOCKET new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
        if (new_sock == INVALID_SOCKET) {
            printf("Gagal menerima koneksi\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        // Menerima pilihan dari sender (pemain A) tanpa menampilkan
        char sender_sign[1024];
        int bytes_received = recv(new_sock, sender_sign, sizeof(sender_sign), 0);

        // Menerima pilihan dari pemain B
        printf("Pemain 2, pilih salah satu: (1) Head, (2) Tail,");
        int choice;
        scanf("%d", &choice);

        if (choice < 1 || choice > 2) {
            printf("Pilihan tidak valid\n");
            closesocket(new_sock);
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        const char* sign[] = { "Head", "Tail" };
        const char* receiver_sign = sign[choice - 1];

    
        srand((unsigned) time(NULL));
        int rand_num_buffer = rand() % 2;
        const char* coin;
        coin = sign[rand_num_buffer];

        // kirim recv sign
        

        send(new_sock, receiver_sign, (int)strlen(receiver_sign), 0);
        // Kirim hasil permainan kepada pemain A (sender)
        send(new_sock, coin, (int)strlen(coin), 0);
        
        printf("Hasil permainan: %s\n", coin);
        printf("Pemain 1 menebak : %s\n", sender_sign);
        printf("Pemain 2 menebak : %s\n\n", receiver_sign);


        if(strcmp(receiver_sign, sender_sign) == 0) printf("Draw!");
        else if(coin == receiver_sign) printf("You Win!");
        else printf("You Lose!");


        closesocket(new_sock);
        closesocket(sockfd);
        WSACleanup();
    
    } else if (choice_buffer == 1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(12345);
        dest_addr.sin_addr.s_addr = inet_addr("192.168.110.146");

        if (connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
            printf("Gagal terhubung\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        
        printf("Pemain 1, Pilih salah satu: (1) Head, (2) Tail");
        int choice;
        scanf("%d", &choice);

        if (choice < 1 || choice > 2) {
            printf("Pilihan tidak valid\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        const char* choices[] = { "Head", "Tail" };
        const char* message = choices[choice - 1];
        send(sockfd, message, (int)strlen(message), 0);
        printf("Pilihan terkirim: %s\n", message);

        char recvsign_buffer[1024];
        int bytes_received = recv(sockfd, recvsign_buffer, sizeof(recvsign_buffer), 0);
        


        char buffer[1024];
        bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if(bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Hasil permainan: %s\n", buffer);
        }
        
        printf("Pemain 1 menebak : %s\n", message);
        printf("Pemain 2 menebak : %s\n\n", recvsign_buffer);


        if(strcmp(message, recvsign_buffer) == 0) printf("Draw!");
        else if(strcmp(buffer, message) == 0) printf("You Win!");
        else printf("You Lose!");

        closesocket(sockfd);
        WSACleanup();
    }

    return 0;
}
