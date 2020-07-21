# your c compiler
CC = gcc

#ld flags
LDFLAGS = -lsodium -ltoxcore 

# your output file
TARGET = pbn

# your project name
TARGET = pbn

CFLAGS = -Ideps -Wall

# all the source files
SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*/*.c)

OBJS = $(SRC:.c=.o)

.PHONY:
all: $(TARGET)

.PHONY:
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

.PHONY:
%.o: %.c
	$(CC) $(DEP_FLAG) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

.PHONY:
clean:
	rm -f $(OBJS)

.PHONY:
install: $(TARGET)
	cp -f $(TARGET) $(PREFIX)

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)


#deff:
	#gcc -static-libgcc -static-libstdc++ --static -o bn bn.c -ltoxcore -lzmq -lpthread -lm -lc -lsodium -lrt -lgcc -lpgm
	#osx build:
	#gcc -o pbn pbn.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -Bdynamic -lc -lm -Ideps
	#linux buid:
	#gcc -o bn bn.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -lrt -lpgm -Bdynamic -lc -lm -lgcc
	#idk this one
	#gcc -o bn bn.c --static -ltoxcore -lzmq -lpthread -lm -lsodium -lrt -lgcc -lpgm  -lc
	#strip pbn
	#upx probably broke
	#upx bn
	#upx --ultra-brute bn
	#file pbn
	#du -hs pbn
	#no static:
	#gcc -o cli cli.c -ltoxcore -lpthread -lsodium
#clean:
#	rm -v bn
