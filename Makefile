CC = cc
CFLAGS = -Wall -O2
LDFLAGS =

all: tdq tdi tds ab2td sd2td xd2td
.c.o:
	$(CC) -c $(CFLAGS) $<
tdq: tdq.o
	$(CC) -o $@ $^ $(LDFLAGS)
tdi: tdi.o
	$(CC) -o $@ $^ $(LDFLAGS)
tds: tds.o
	$(CC) -o $@ $^ $(LDFLAGS)
xd2td: xd2td.o
	$(CC) -o $@ $^ $(LDFLAGS)
sd2td: sd2td.o
	$(CC) -o $@ $^ $(LDFLAGS)
ab2td: ab2td.o
	$(CC) -o $@ $^ $(LDFLAGS)
clean:
	rm -f *.o tdq tdi tds ab2td sd2td xd2td
