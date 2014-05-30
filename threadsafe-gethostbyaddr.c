// THIS IS THE THREAD-SAFE IMPLEMENTATION OF GETHOSTBYADDR FUNCTION, USED IN tcp SOCKET I/O
//Implemented using mutex locks. semaphores and dynamic allocation for return types of gethostbyaddr to avoid re-entrancy issue
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
static sem_t mutex; /* protects calls to gethostbyname */
/* $begin unixerror */
void unix_error(char *msg) /* unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
void *Malloc(size_t size) 
{
    void *p;

    if ((p  = malloc(size)) == NULL)
	unix_error("Malloc error");
    return p;
}
//self defined wrapper for sem_init
void Sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    if (sem_init(sem, pshared, value) < 0)
	unix_error("Sem_init error");
}
//P() is a wrapper defined for sem_wait()
void P(sem_t *sem) 
{
    if (sem_wait(sem) < 0)
	unix_error("P error");
}
//P() is a wrapper defined for sem_post()
void V(sem_t *sem) 
{
    if (sem_post(sem) < 0)
	unix_error("V error");
}
static void init_gethostbyaddr_ts(void)
{
    Sem_init(&mutex, 0, 1);
}


 /* *Gethostbyaddr_ts - thread safe implementation of gethostbyaddr*/
struct hostent *Gethostbyaddr_ts(const char *addr, int len, int type)
{
  struct hostent *sharedp, *unsharedp;

  unsharedp = Malloc(sizeof(struct hostent));
  P(&mutex);
  sharedp = gethostbyaddr(addr,len,type);
  *unsharedp = *sharedp;
  V(&mutex);
  return unsharedp;
}

   
void main()
{
	pthread_t t1, t2; /* two threads */
 void *getta(void *);
// Each thread calls same function of "getta"
//"getta" performs as gethostbyaddr() with argument passed is the IP address
  pthread_create(&t1, NULL, getta, (void *)"127.0.0.1");
//Thread 1 queries with IP = 127.0.0.1
  pthread_create(&t2, NULL, getta, (void *)"10.0.2.15");
//Thread 2 queries with IP= 10.0.2.15
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
	
}
void *getta (void *m)
{ char **cp;
char *buffer= (char *) m;
	struct in_addr *addr;
	struct hostent *hp;
	char **p;
    init_gethostbyaddr_ts();
	
	if ((int)(addr = inet_addr(m)) == -1) {
	    (void) printf("IP-address must be of the form a.b.c.d\n");
	    exit (2);
	}

	hp = Gethostbyaddr_ts((char *)&addr, sizeof (addr), AF_INET);
	if (hp == NULL) {
	    (void) printf("host information for %s not found\n", m);
	    exit (3);
	}
  printf("nom = %s\n", hp->h_name);
   printf("type = %d, long = %d\n", hp->h_addrtype,hp->h_length); 
   for (cp=hp->h_aliases;cp != NULL && *cp != NULL;){
        if (cp == hp->h_aliases)
            printf("aliases = ");
        printf ("%s",*cp);
        if (*(++cp) != NULL) printf (",");
   }
   if (cp != hp->h_aliases) printf("\n");
   for (cp=hp->h_addr_list;cp != NULL && *cp != NULL;){
        if (cp == hp->h_addr_list)
            printf("addresses = ");
        printf ("%s",inet_ntoa(*((struct in_addr *)*cp)));
        if (*(++cp) != NULL) 
printf (",");
   }
   if (cp != hp->h_addr_list) 
printf("\n");}
