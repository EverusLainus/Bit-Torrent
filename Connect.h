#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <iostream>
#include <string>
#include "./cpp-bencoding/include/bencoding.h"
int Connect(std::string& ip, int port);