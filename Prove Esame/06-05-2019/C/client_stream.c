/* Nome Cognome Matricola */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF    100
#define LENGTH_NAME 100

int main(int argc, char *argv[]) {
    int                sd,fd, nread, nwrite, port,num_file,count=0;
    char               car, buff[DIM_BUFF], nome_dir[LENGTH_NAME];
    struct hostent    *host;
    struct sockaddr_in servaddr;
    char nome_file[LENGTH_NAME];

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
    printf("Client avviato\n");

    /* PREPARAZIONE INDIRIZZO SERVER ----------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    }

    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

     /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0) {
            perror("apertura socket ");
            exit(3);
        }
        printf("Creata la socket sd=%d\n", sd);

        if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
            perror("Errore in connect");
            exit(4);
        }
        printf("Connect ok\n");

    /* CORPO DEL CLIENT: */
    /* ciclo di accettazione di richieste di file ------- */
    printf("Nome del file da richiedere: ");

    while (gets(nome_dir)) {

        if (write(sd, &nome_dir, (strlen(nome_dir) + 1)) < 0) {
            perror("write");
            close(sd);
            printf("Nome del file da richiedere: ");
            continue;
        }
        printf("Richiesta del file %s inviata... \n", nome_file);

        if(read(sd, &num_file, sizeof(int))<0){
            perror("read");
            close(sd);
            printf("Nome della cartella da richiedere: ");
            continue;
        }
        for(int i=0; i<num_file; i++){
            strcpy(nome_file,"");
            count=0;
            while(read(sd, &car, 1) > 0) {
                if(car!='\0'){
                    nome_file[count]=car;
                    count++;
                }else{
                    nome_file[count]='\0';
                    break;
                }
            }
            fd=open(nome_file, O_CREAT | O_WRONLY, 0777);
            if(fd<0){
                perror("open");
                break;
            }
            while((nread=read(sd, &car, 1))>0){
                if(car!='\0'){
                    write(fd, car, nread);
                }else{
                    break;
                }
            }
            printf("ho scaricato il file con nome: "+ nome_file);
            close(fd);
        }
        

        
        
        
    }

    printf("Chiudo connessione\n");
    shutdown(sd, SHUT_RD);
    shutdown(sd, SHUT_WR);
    close(sd); 

    printf("\nClient: termino...\n");
    exit(0);
}
