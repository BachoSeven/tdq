CC = cc
CFLAGS = -Wall -O2
LDFLAGS =
BIN = ab2td sd2td tdi tdq tds xd2td
PREFIX=/usr/local

all: ${BIN}
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

install: all
	@cp -f ${BIN} "${DESTDIR}${PREFIX}/bin"

uninstall:
	@rm -f "${DESTDIR}${PREFIX}/bin/ab2td"
	@rm -f "${DESTDIR}${PREFIX}/bin/sd2td"
	@rm -f "${DESTDIR}${PREFIX}/bin/tdi"
	@rm -f "${DESTDIR}${PREFIX}/bin/tdq"
	@rm -f "${DESTDIR}${PREFIX}/bin/tds"
	@rm -f "${DESTDIR}${PREFIX}/bin/xd2td"
