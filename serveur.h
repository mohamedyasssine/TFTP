
/*STRUCTURE DE RRQ ET WRQ PACKET [OPCODE(2bit)|nomfichier|0(fin de string|mode|0)]*/
  /*opcode de chaque packet
         1:RRQ message
         2 :WRQ
         3:DATA
         4:ACK
         5:ERROR
         */

#ifndef SERVEUR_H
#define SERVEUR_H

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

#define BUFSIZE 1024
char *base_dir;
int listener;
void erreur_verification(int r, char *message);
void ecouter_packet();
int creer_socket_UDP(int port);

#endif /* SERVEUR_H */
