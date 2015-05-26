ALL: server_counter client_counter experiment_counter
CC = g++
CFLAGS = -g -O2 -Wfatal-errors -Wshadow -Wall -I/usr/local/include
GMP = -lgmp
HEdir = /home/jesus/HElib/src
HElib = $(HEdir)/fhe.a

LDLIBS = -L/usr/local/lib $(HElib) -lntl $(GMP) -lm
INCDIR = -I$(HEdir)

#HEcalc: HEcalc.cpp
#	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)

client_counter: client_counter.cpp
	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)

server_counter: server_counter.cpp
	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)
experiment_counter: experiment_counter.cpp
	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)

clean:
	rm -f HEcalc counter server_counter client_counter experiment_counter
