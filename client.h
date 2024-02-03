#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
//variable pour stocker le port de serveur
 int server_port;
 //varaible pour le mode de transfére (octet ou netascci)
 char transfer_mode[10];
 //structure de addresse de serveur
 struct sockaddr_in serv_addr;
 //address ip de serveur 
char *server_ip;
//
void erreur_verification(int r, char *message);
//fonction pour envoyer un paket rrq (une requette ) au serveur sc'est une requete pour la demande de fichier
void RRQ(int client_socket, char *nom_fichier);
//fonction qui consiste à envoyer la requette RRQ et récupérer les data du fichier ou les erreur en cas d'erreur et de les écrire sur un fichier  
void get(char *nom_fichier);
//focntion pour l'nvoie d'un packet ACK 
void send_ACK(int client_socket, uint16_t block_number);
void send_WRQ(int client_socket, char *nom_fichier);

#endif /* CLIENT_H */
