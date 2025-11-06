#!/bin/bash
gcc -Wall -Wextra -o sync_thread sync_thread.c -lpthread
gcc -Wall -Wextra -o async_thread async_thread.c -lpthread