//SCTP client for 2 stream communcations 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netinet/sctp.h>

#define SIZE 1024
char buf[SIZE];
#define TIME_PORT 2013

char *usage_msg = "usage: app ip-addr istreams ostreams";

void usage() {
  fprintf(stderr, "%s\n", usage_msg);
  exit(1);
}


int main(int argc, char *argv[]) {
  int sockfd, client_sockfd;
  int nread, len;
  struct sockaddr_in serv_addr, client_addr;
  time_t t;
  struct sockaddr_in *addresses;
  int addr_size = sizeof(struct sockaddr_in);
  int addr_count = 1;
  int port = TIME_PORT;
  int n;
  struct sctp_initmsg initmsg;
  struct sctp_status status;
  sctp_assoc_t associd;
  struct sctp_sndrcvinfo sinfo;
  struct sctp_event_subscribe events;

  int flags;
int ret;
 // if (argc != 4) usage();

  /* create endpoint */
  sockfd = socket(AF_INET, SOCK_STREAM,IPPROTO_SCTP);
  if (sockfd < 0) {
    perror(NULL);
    exit(2);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(port);

  if (sctp_bindx(sockfd, (struct sockaddr *) &serv_addr, addr_count,SCTP_BINDX_ADD_ADDR) == -1) 
{
    perror("sctp bindx");
    exit(2);
  }

  memset(&initmsg, 0, sizeof(initmsg));
  initmsg.sinit_max_instreams = 2;
  initmsg.sinit_num_ostreams = 2;
  printf("Asking for: input streams: %d, output streams: %d\n",initmsg.sinit_max_instreams,initmsg.sinit_num_ostreams);

  if (setsockopt(sockfd, IPPROTO_SCTP,SCTP_INITMSG, &initmsg, sizeof(initmsg))) {
    perror("set sock opt\n");
  }



  /* specify queue */
  listen(sockfd, 5);
  for (;;) {
    len = sizeof(client_addr);
    client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr,
&len);
    if (client_sockfd == -1) {
      perror(NULL); continue;
    }



    memset(&status, 0, sizeof(status));
    len = sizeof(status);
    status.sstat_assoc_id = 0;

    if (getsockopt(client_sockfd, IPPROTO_SCTP,SCTP_STATUS, &status, &len) == -1) {
      perror("get sock opt\n");
    }

    printf("Got: input streams: %d, output streams: %d\n",status.sstat_instrms,status.sstat_outstrms);



      /* transfer data */
      len = sizeof(client_addr);
      bzero(&sinfo, sizeof(sinfo));
      nread = sctp_recvmsg(client_sockfd, buf, SIZE,(struct sockaddr *) &client_addr, &len,&sinfo, &flags);

      if (nread == 0) {
        break;
      }
printf("stream now is %d\n",sinfo.sinfo_stream);
      printf("read CTime on stream %d\n", sinfo.sinfo_stream);
      printf("sinfo flags: %d\n", sinfo.sinfo_flags);
      write(1, buf, nread);
    

 memset(&events , 0, sizeof(events)); 
 events.sctp_data_io_event = 1; 
 ret = setsockopt(client_sockfd, SOL_SCTP, SCTP_EVENTS, &events, sizeof(events)); 
 memset(&buf, 0, sizeof(buf));
   
      /* transfer data */
      len = sizeof(client_addr);
      bzero(&sinfo, sizeof(sinfo));
      nread = sctp_recvmsg(client_sockfd, buf, SIZE,(struct sockaddr *) &client_addr, &len,&sinfo, &flags);

      if (nread == 0) {
        break;
      }
printf("stream now is %d\n",sinfo.sinfo_stream);
      printf("read GMTime on stream %d\n", sinfo.sinfo_stream);
      printf("sinfo flags: %d\n", sinfo.sinfo_flags);
      write(1, buf, nread);
    
    close(client_sockfd);
  }
}
