noinst_LIBRARIES = libluasocket.a
libluasocket_a_SOURCES = \
	auxiliar.h auxiliar.c \
	buffer.h buffer.c \
	except.h except.c \
	inet.h inet.c \
	io.h io.c \
	luasocket.h luasocket.c \
	mime.h mime.c \
	options.h options.c \
	select.h select.c \
	socket.h \
	tcp.h tcp.c \
	timeout.h timeout.c \
	udp.h udp.c
EXTRA_libluasocket_a_SOURCES = \
	usocket.h usocket.c \
	wsocket.h wsocket.c
libluasocket_a_LIBADD = $(LUASOCKET_SYSTEM)
libluasocket_a_DEPENDENCIES = $(LUASOCKET_SYSTEM)
EXTRA_DIST = LICENSE NEW
