//SCTP server program 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <netinet/sctp.h>
#include<netdb.h> 



#define SIZE 1024
char buf[SIZE];
#define ECHO_PORT 2013

char *usage_msg = "usage: streamsend_echo_client ip-addr istreams ostreams stream";

void usage() {
        fprintf(stderr, "%s\n", usage_msg);
        exit(1);
}


int main(int argc, char *argv[]) {
        int sockfd;
        int len;
        struct sockaddr_in serv_addr;
        struct sockaddr_in *addresses;
        int addr_size = sizeof(struct sockaddr_in);
        int addr_count = argc - 1;
        int port = ECHO_PORT;

        char *message = "hello\n";
        struct sctp_initmsg initmsg;
        struct sctp_status status;
        struct sctp_sndrcvinfo sinfo;
        int ochannel;
  char *strr;
        //if (argc != 2) usage();

        /* create endpoint */
        sockfd = socket(AF_INET, SOCK_STREAM,IPPROTO_SCTP);

        if (sockfd < 0) {
                perror(NULL);
                exit(2); }
        /* connect to server */
        addresses = malloc(addr_size);
        if (addresses == NULL) {
                exit(1);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(port);

        memcpy(addresses, &serv_addr, addr_size);

        memset(&initmsg, 0, sizeof(initmsg));
        initmsg.sinit_max_instreams =2;
        initmsg.sinit_num_ostreams = 2;
        printf("Asking for: input streams: %d, output streams: %d\n",initmsg.sinit_max_instreams,initmsg.sinit_num_ostreams);

        if (setsockopt(sockfd, IPPROTO_SCTP,SCTP_INITMSG, &initmsg, sizeof(initmsg))) {
                perror("set sock opt\n");
        }

        if (sctp_connectx(sockfd, (struct sockaddr *) addresses, 1,0) < 0)
{
                perror("connectx");
                exit(3);
        }

        memset(&status, 0, sizeof(status));
        len = sizeof(status);
        status.sstat_assoc_id = 1;

        if (getsockopt(sockfd, IPPROTO_SCTP,SCTP_STATUS, &status, &len) == -1) {
                perror("get sock opt\n");
        }
        printf("Got: input streams: %d, output streams: %d\n",status.sstat_instrms,status.sstat_outstrms);

        /* sanity check channel */
        ochannel = 0;
        if (ochannel >= status.sstat_outstrms)
                printf("Writing on illegal channel %d\n", ochannel);
time_t ctime_t; 
 struct tm *gtime; 
 time_t gmtime_t; 
time_t curtime;

   time(&curtime);

   printf("Current time = %s", ctime(&curtime));
    // transfer data 
strr=ctime(&curtime);
        bzero(&sinfo, sizeof(sinfo));
        sinfo.sinfo_stream = ochannel;
        sctp_send(sockfd, strr,strlen(strr),
                  &sinfo, 0);

        sinfo.sinfo_flags = SCTP_EOF;
        sctp_send(sockfd, NULL, 0,
                  &sinfo, 0);
time_t ltime;
   time(&ltime);
   printf ("Coordinated Universal Time is %s\n",asctime(gmtime(&ltime)));
 //transfer data 
strr= asctime(gmtime(&ltime));
        bzero(&sinfo, sizeof(sinfo));
        sinfo.sinfo_stream = 1;
        sctp_send(sockfd, strr, strlen(strr),
                  &sinfo, 0);

        sinfo.sinfo_flags = SCTP_EOF;
        sctp_send(sockfd, NULL, 0,
                  &sinfo, 0);
        close(sockfd);
        exit(0);
}
