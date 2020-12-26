#include "stdio.h"
#include "compress.h"
#include "libgohooks.h"
#include "loadsym.h"
#include <netinet/tcp.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#define INIT() init_lib_wrapper(__FUNCTION__)

#define SETUP_SYM(X)                                                           \
  do {                                                                         \
    if (!pointer_##X)                                                             \
      pointer_##X = load_sym(#X);                                              \
  } while (0)


pthread_once_t init_once = PTHREAD_ONCE_INIT;
static void setup_hooks(void);

static void do_init(void) {

  srand(time(NULL));
  setup_hooks();
}


void init_lib_wrapper(const char *caller) {
#ifndef DEBUG
  (void)caller;
#endif
  // if(!init_l) PDEBUG("%s called from %s\n", __FUNCTION__,  caller);
  pthread_once(&init_once, do_init);
}

void *load_sym(char *symname) {
    void *funcptr = dlsym(RTLD_NEXT, symname);

  if (!funcptr) {
    fprintf(stderr, "Cannot load symbol '%s' %s\n", symname, dlerror());
    exit(1);
  } else {
    //fprintf(stderr,
            //"loaded symbol '%s'"
            //" real addr %p \n ",
            //symname, funcptr);
  }
  return funcptr;
}




static void setup_hooks(void) {
  SETUP_SYM(send);
  SETUP_SYM(sendto);
  SETUP_SYM(write);
  SETUP_SYM(read);
  SETUP_SYM(socket);
  SETUP_SYM(connect);
}

int g_proxy_fd=0;


//wrap for sendto
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {
  INIT();
  return 0;
}
ssize_t true_sendto(int sockfd, const void *buf, size_t len, int flags,
                            const struct sockaddr *dest_addr,
                            socklen_t addrlen){
    return pointer_sendto(sockfd,buf,len,flags,dest_addr,addrlen);
}

//wrap for socket
int socket(int domain, int type, int protocol){
    INIT();
    int res=0;
    if((type&0xF) & SOCK_STREAM){
        g_proxy_fd = true_socket(domain,type,protocol);
        printf("open socket: %d\n",g_proxy_fd);
        res=g_proxy_fd;
    } else{
        res = true_socket(domain,type,protocol);
    }
    return res;
}

int true_socket(int domain, int type, int protocol){
    return pointer_socket(domain,type,protocol);
}



//wrap for write
ssize_t write(int fd, const void *buf, size_t count) {
   static char ibuf  [2<<20];
  INIT();
  if((fd==g_proxy_fd)&&(count>10000)){
      size_t len = compress_buffer((char *)buf,count,ibuf,2<<20);
      true_write(fd,(void *)ibuf,len);
      return  count;
  }
  return true_write(fd,(void *)buf,count);
}
ssize_t true_write(int fd, const void *buf, size_t count){
    return pointer_write(fd,buf,count);
}

//wrap for oend
ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
  return true_send(sockfd, buf, len, flags);
}

ssize_t true_send(int sockfd, const void *buf, size_t len, int flags){
    return pointer_send(sockfd,buf,len,flags);
}

ssize_t true_read(int fd, void *buf, size_t count){
    return pointer_read(fd,buf,count);
}


void debug(const char * cstring){
    printf("%s",cstring);
}

