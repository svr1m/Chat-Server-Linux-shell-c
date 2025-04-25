#!bin/bash

 gcc -o chat_server chat_server.c -lpthread
 echo "Enter Port: "
 read hlo;
 ./chat_server $hlo
