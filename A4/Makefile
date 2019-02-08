
PORT= 51300
CFLAGS= -D PORT=$(PORT) -g -Wall
all: hcq_server helpcentre

helpcentre: helpcentre.o hcq.o 
	gcc $(CFLAGS) -o helpcentre helpcentre.o hcq.o

helpcentre.o: helpcentre.c hcq.h
	gcc $(CFLAGS) -c helpcentre.c

hcq.o: hcq.c hcq.h
	gcc $(CFLAGS) -c hcq.c

hcq_server.o: hcq_server.c hcq.h
	gcc $(CFLAGS) -c hcq_server.c

hcq_server: hcq_server.o hcq.o
	gcc $(CFLAGS) -std=c99 -o hcq_server hcq_server.o hcq.o 

clean: 
	rm helpcentre *.o
