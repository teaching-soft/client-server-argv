/***************************************************************************
	file				 : server.c
    begin                : 2019
    copyright            : (C) 2019 by Giancarlo Martini and friends
    email                : gm@giancarlomartini.it

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sock_fd, new_sock_fd, port_num, client_len;
	char buffer[256];
	/* sockaddr_in: Contiene le informazioni di un host sulla rete */
	struct sockaddr_in server_addr, client_addr;
	int n;

	/* Controlla che sia stata specificata la porta sulla linea di comando */
	if (argc < 2) {
		fprintf(stderr,
			"ERRORE, non è stata specificata la porta di ascolto, usare:\n%s porta_di_ascolto\n",
			argv[0]);
		exit(1);
	}

	/* Crea il socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	/* Controlla che l'operazione sia riuscita */
	if (sock_fd < 0)
		error("ERRORE nel creare il socket:");

	/* Predispone la struttura che conterrà le informazioni del server */
	bzero((char *)&server_addr, sizeof(server_addr));

	/* Prende il numero di porta passato sulla riga di comando,
	   atoi converte la stringa argv[1] in valore numerico */
	port_num = atoi(argv[1]);

	/* Riempe la struttura con le informazioni della connessione
	   Tipo di socket family = AF_INET,
	   Indirizzi su cui restare in ascolto s_addr = INADDR_ANY, potrebbero essere più di uno
	   Porta con relativa conversione sin_port = htons(port_num);
	   La funzione htonl() converte il valore da host order a network order */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_num);

	/* Collega il socket con informazioni relative alla connessione */
	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
	    < 0)
		error("ERROR on binding");

	/* Si mette in ascolto in attesa di client che si connettano, la coda è lunga 5 unità */
	listen(sock_fd, 1);

	/* Qualcuno si è connesso ... */

	/* Predispone lo spazio per i dati necessari */
	client_len = sizeof(client_addr);

	/* Accetta la connessione prelevando i dati del client */
	new_sock_fd =
	    accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);

	/* Controllo sulla riuscita dell'operazione ... */
	if (new_sock_fd < 0)
		error("ERRORE con la funzione accept");

	/* Predispone il buffer per i dati, azzerandolo */
	bzero(buffer, 256);

	/* Legge i dati arrivati */
	n = read(new_sock_fd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");

	/* Stampa i dati arrivati */
	printf("Questo è il messaggio ricevuto: %s\n", buffer);

	/* Invia il messaggio di avvenuta ricezione */
	char msg[30];
	strcpy(msg, "Messaggio ricevuto");
	n = write(new_sock_fd, msg, strlen(msg));

	if (n < 0)
		error("ERRORE scrivendo nel socket");

	/* Attende per dare tempo al client di terminare la connessione, 100 usec */
	usleep(100);

	/* Chiude la connessione */
	close(sock_fd);
	close(new_sock_fd);
	return 0;
}
