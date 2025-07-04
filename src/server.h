#ifndef SERVER_H
#define SERVER_H

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builder.h"
#include "platform.h"
#include "search.h"

void Serve(const char *dir);
void SendFile(int client_fd, const char *path, const char *status);

#endif  // SERVER_H