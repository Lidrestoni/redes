#!/bin/bash
sudo gcc main.c -o main
sudo gcc userserver.c -o userserver -lpthread
sudo gcc router.c -o router
sudo gcc server.c -o server


