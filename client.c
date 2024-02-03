#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "client.h" // Ajout de l'en-tête client.h
#define BUFSIZE 1024 //TAILLE maximale DE TOMPON

/* j'ai mis toute les déclaration des variable globale dans client.h*/
void erreur_verification(int r,char *message)
{
  if (r < 0) {
        // affichage de l'erreur
        perror(message);
       
        exit(-1);
    }
}
//fonction pour la demande de lecture de fichier  "nom_fichier" depuis le serveur à traver la socket
void send_RRQ(int client_socket, char *nom_fichier)
{
        int len = 0;//longueur de buffer
        char buffer[BUFSIZE];//le buffer va etre notre trame à transférer ,
                             //il contient [opcode(2bit),nomfichier,0(fin de nomfichier),MOde de transfére,0]
        uint16_t opcode = htons(1);//l'opcode de RRQ est 1 (d'aprés le RFC 135)
        uint8_t end_string = 0;
        memcpy(buffer, &opcode, 2); //copier l'opcode de RRQ d+ans le buffer 
        len+=2;//on ajout la longueur d'opcode au longueur final de buffer
        strcpy(buffer + len, nom_fichier);//on ajoute le nom fichier dans notre buffer(trame)
        len += strlen(nom_fichier);
        memcpy(buffer + len, &end_string, 1);//on ajoute la fin de nom fichier
        len++;
        strcpy(buffer + len, transfer_mode); //on ajoute le mode de transfére dans notre trame de donnée(buffer)    
        len += strlen(transfer_mode);
        memcpy(buffer + len, &end_string, 1);
        len++;
        /*dans la ligne suivante on utilise la fonction sendto de la bib socket.h pour envoyer
        la requette RRQ (buffer) à la serveur en utilisant la socket client 
        et l'addresse de serveur est défini
        dans la structure sockaddr */
        int sent_len = sendto(client_socket, buffer,len,MSG_CONFIRM,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
        erreur_verification(sent_len, "erreur d'envoi de RRQ");
}
//la fonction suivante est la fonction qui va créer le packet WRQ d'opcode 2 et l'envoyer 
//au serveur en utilisant la socket client
void send_WRQ(int client_socket, char *nom_fichier)
{
        int len = 0;//longueur de buffer
        char buffer[BUFSIZE];//le buffer va etre notre trame à transférer ,
                             //il contient [opcode(2bit),nomfichier,0(fin de nomfichier),MOde de transfére,0]
        uint16_t opcode = htons(2);//l'opcode de WRQ est 2 (d'aprés le RFC 135)
        uint8_t end_string = 0;
        memcpy(buffer, &opcode, 2); //copier l'opcode de WRQ d+ans le buffer 
        len+=2;//on ajout la longueur d'opcode au longueur final de buffer
        strcpy(buffer + len, nom_fichier);//on ajoute le nom fichier dans notre buffer(trame)
        len += strlen(nom_fichier);
        memcpy(buffer + len, &end_string, 1);//on ajoute la fin de nom fichier
        len++;
        strcpy(buffer + len, transfer_mode); //on ajoute le mode de transfére dans notre trame de donnée(buffer)    
        len += strlen(transfer_mode);
        memcpy(buffer + len, &end_string, 1);
        len++;
        /*dans la ligne suivante on utilise la fonction sendto de la bib socket.h pour envoyer
        la requette RRQ (buffer) à la serveur en utilisant la socket client 
        et l'addresse de serveur est défini
        dans la structure sockaddr */
        int sent_len = sendto(client_socket, buffer,len,MSG_CONFIRM,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
        erreur_verification(sent_len, "erreur d'envoi de RRQ");
}
void send_ACK(int client_socket, uint16_t numero_block)
{
    int longueur;
    char buffer[BUFSIZE];
    //l'ocpcode de packet ACK est 4
    uint16_t opcode = htons(4);
    //on affecte l(opcode au tompon de l'indice 0 à 1 (2bit)
    memcpy(buffer, &opcode, 2);
    longueur+=2;
    //par la suite on copie le numéro de block qu'est 2 bits
    numero_block = htons(numero_block);
    memcpy(buffer +longueur,&numero_block,2);
    longueur+=2;
    //
    int sent_len = sendto(client_socket, buffer,longueur,MSG_CONFIRM,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
    erreur_verification(sent_len, "erreur d'envoi de RRQ");
}

void get(char *nom_fichier)
{
        //on va initialiser l'addresse de serveur avec la famille ipv4
    	serv_addr.sin_family = AF_INET;
        serv_addr.sin_port=server_port;
        inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);
        //maintenant on va créer la socket client 
        int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
        //on déclare deux variable pour stocker le nom de fichier source (fichier serveur) 
        //et l'autre ppour stocker la fichier destinataire source>>destinataire
        char nom_fichier_serveur[256];
        strcpy(nom_fichier_serveur,nom_fichier);
	    char nom_fichier_client[256];
        //on va utiliser la variable opcode pour tester sur la réponse serveur par la suite
        int opcode;
        send_RRQ(client_socket, nom_fichier_serveur);
        //buffer est un tampon pour stocker la réponse du serveur
        char buffer[BUFSIZE];

        int longueur_addresse = sizeof(serv_addr);//on va utiliser longueur_addresse par la suite dans la fonction 
        //recvfrom est une fonction prédefinit dans la bib socket.h pour récuperer la reponse de serveur 
        int recv_len = recvfrom(client_socket,(char *)buffer,BUFSIZE,MSG_WAITALL,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);
        erreur_verification(recv_len,"erreur de recevoi un message aprés qu'on a envoyé RRQ");
        /*ces deux ligne copie l'opcode qui contient dans le buffer 
        dans la variable  opcode en utilisant la méthode opcode*/
        memcpy(&opcode, (uint16_t *) & buffer, 2);
	    opcode = ntohs(opcode);
        /*maintenant on va tester sur l'opcode sachant que
         1:RRQ message
         2 :WRQ
         3:DATA
         4:ACK
         5:ERROR
         */
         if (opcode == 5) //cas de packet d'erreur
	{
        /* je vais par la suite afficher le message d'erreur qui contient dans le tompon à partie*/
    fprintf(stderr, "Error: %s.\n", buffer + 2);		
	} 
    else if (opcode == 3)	//cas de packet de data
    {
        uint16_t nombre_block; //onva  recupérer le numéro de block  dans cette variable
        memcpy(&nombre_block, (uint16_t *) & buffer[2], 2);
        nombre_block = ntohs(nombre_block);
        FILE *fichier_client = fopen(nom_fichier_serveur, "w");/*ouvrir un fichier en mode ecriture pour stocker le data
                                           de tompon dans ce fichier */
     if (fichier_client == NULL) 
    {
    fprintf(stderr, "Error while opening the destination file: errno ");
    exit(-1);
    
    }
    /* la packet de  data d'opcode 3 est constitué selon cette structure 
    [opcode(2bit)|blck_number(2bit)|data]
    donc pour récupérer les donnée envoyé par le serveur on doit commencer par l'indice 4
    donc la boucle for par la suite récupére les donné de packet data dans la fichier client
    */
    int i = 0;
		for (i = 0; i < recv_len - 4; i++) {
			// 
			fputc(buffer[i + 4], fichier_client);
		}
             //à chaque fois qu'un packet data arrive on doit l'acciter par un ACK 
    send_ACK(client_socket, nombre_block);
    /* la taille de packet data est 512 octer or que il ya un entete de 4 octet 
    donc on va récuperer à chaque fois le packet et une fois qu'on a un packet <512 
    c'est à dire c'est la derniére packet donc la boucle while va sortir
    */
    while (recv_len == 516) {
        recv_len = recvfrom(client_socket,
					    (char *)buffer,//tompon 
					    BUFSIZE,//longueur de buffer
					    MSG_WAITALL,
					    (struct sockaddr *)&serv_addr,//addresse se serveur qui est notre structure
					    (socklen_t *) & longueur_addresse);
        erreur_verification(recv_len,"erreur de recevour de packet data");
        memcpy(&nombre_block, (uint16_t *) & buffer[2], 2);// on copie le numéro de blocl dans la variable nombre block
		nombre_block = ntohs(nombre_block);//convertit des nim"ro de packer réseau en  locales.
     //on a expliquer ce bloc précédement 
     int i = 0;
			for (i = 0; i < recv_len - 4; i++) {
				fputc(buffer[i + 4], fichier_client);
			}
			send_ACK(client_socket, nombre_block);
     }
     /*shutdown en C est utilisée pour arrêter l'envoi et/ou la 
     réception de données sur un socket.*/
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
	fclose(fichier_client);
    }
}

int main(int argc, char *argv[])
{
    if (argc !=5)
    {
        printf("nombre d'argument invalide \n usage : .\a.out get|put [server_ip] [server_port]");
        return -1;
    }
	 server_ip = argv[3]; //recuperation de l'addresse ip de serveur dans variable sever_ip
     server_port = atoi(argv[4]);//converssion de port en entier et recuperer son valeur dans server_port  
   
    strcpy(transfer_mode, "netascii"); // Mode netascii pour les fichiers .txt
    
    char *filename =argv[2];

     if (strcmp(argv[1], "get") == 0)
     {
        //appeller la méthode get 
     }
     else if (strcmp(argv[1], "put") == 0)
     {
        //appeller la méthode put
     }
     else 
     {
        printf("commande introuvable get | put");
     }
    return 0;
}