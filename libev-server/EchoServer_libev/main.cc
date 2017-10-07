#include "ev.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLEN 65535
#define PORT 45000
#define ADDR_IP "192.168.19.137"

int socket_init();
static int setnonblock(int);
void accept_callback(struct ev_loop * loop, ev_io * w, int revent);
void recv_callback(struct ev_loop * loop, ev_io * w, int revent);
void write_callback(struct ev_loop * loop, ev_io * w, int revent);

int main(int argc, char ** argv)
{
  do
  {
    printf("hello world.\n");
    continue;
  } while(0);
  int listen;
  ev_io ev_io_watcher;
  listen = socket_init();

  struct ev_loop *loop = EV_DEFAULT;
  ev_io_init(&ev_io_watcher, accept_callback, listen, EV_READ);
  ev_io_start(loop, &ev_io_watcher);
  ev_loop(loop, 0);
}

int socket_init()
{
  struct sockaddr_in my_addr;
  int listener;
  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }
  else
    printf("SOCKET CREATE SUCCESS!\n");

  int so_reuseaddr = 1;
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));
  bzero(&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT);
  my_addr.sin_addr.s_addr = inet_addr(ADDR_IP);

  if (bind(listener, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1)
  {
    perror("bind error!\n");
    exit(1);
  }
  else
    printf("IP BIND SUCCESS, IP:%s\n", ADDR_IP);

  if (listen(listener, 1024) == -1)
  {
    perror("listen error!\n");
    exit(1);
  }
  else
    printf("LISTEN SUCCESS, PORT:%d\n", PORT);
  return listener;
}

void accept_callback(EV_P_ ev_io *w, int revents)
{
  int newfd;
  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  ev_io* accept_watcher = (ev_io*)malloc(sizeof(ev_io));

  while ((newfd = accept(w -> fd, (struct sockaddr *) &sin, &addrlen)) < 0)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      continue;
    else
    {
      printf("accept error.[%s]\n", strerror(errno));
      break;
    }
  }

  setnonblock(newfd);

  ev_io_init(accept_watcher, recv_callback, newfd, EV_READ);
  ev_io_start(loop, accept_watcher);
  printf("accept callback : fd :%d\n", accept_watcher -> fd);
}

void recv_callback(EV_P_ ev_io * w, int revents)
{
  char buffer[MAXLEN - 1] = {0};
  int ret = 0;

  while (true)
  {
    ret = recv(w -> fd, buffer, MAXLEN, 0);
    if (ret > 0)
    {
      printf("recv message: %s \n", buffer);
      write(w -> fd, buffer, strlen(buffer));
      //system("./email");
    }
    else if (ret == 0)
    {
      printf("remote socket closed! socket fd: %d\n", w -> fd);
      close(w -> fd);
      ev_io_stop(loop, w);
      free(w);
      return;
    }
    else
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        continue;
      else
      {
        printf("ret: %d, close socket fd: %d\n", ret, w -> fd);
        close(w -> fd);
        ev_io_stop(loop, w);
        free(w);
        return;
      }
    }
    //int fd = w -> fd;
    //ev_io_stop(loop, w);
    //ev_io_init(w, write_callback, fd, EV_WRITE);
    //ev_io_start(loop, w);
    //printf("socket fd: %d, trun read 2 write loop! ", fd);
    return;
  }
}

void write_callback(EV_P_ ev_io * w, int revents)
{
  char buffer[1024] = {0};
  snprintf(buffer, 1023, "this is a libev server!\n");
  write(w -> fd, buffer, strlen(buffer));
  int fd = w -> fd;
  ev_io_stop(EV_A, w);
  ev_io_init(w, recv_callback, fd, EV_READ);
  ev_io_start(loop, w);
}

static int setnonblock(int fd) {
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0) return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) return -1; 
    return 0;
}
