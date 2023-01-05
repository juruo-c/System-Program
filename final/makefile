all: Server Client

Server: ./server/server.c ./server/config.c ./common/fifo.c ./common/utils.c ./server/daemon.c ./server/log.c ./server/userlist.c ./server/pool.c ./server/register.c ./server/login.c ./server/logout.c ./server/message.c ./server/getuserlist.c
	gcc -o ./bin/Server ./server/server.c ./server/config.c ./common/fifo.c ./common/utils.c ./server/daemon.c ./server/log.c ./server/userlist.c ./server/pool.c ./server/register.c ./server/login.c ./server/logout.c ./server/message.c ./server/getuserlist.c -lpthread -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient 

Client: ./client/client.c ./common/fifo.c ./common/utils.c ./client/register.c ./server/log.c ./server/config.c ./server/pool.c ./server/userlist.c ./client/login.c ./client/chat.c
	gcc -o ./bin/Client ./client/client.c ./common/fifo.c ./common/utils.c ./client/register.c ./server/log.c ./server/config.c ./server/pool.c ./server/userlist.c ./client/login.c ./client/chat.c