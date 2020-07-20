def:
	#gcc -static-libgcc -static-libstdc++ --static -o bn bn.c -ltoxcore -lzmq -lpthread -lm -lc -lsodium -lrt -lgcc -lpgm
	#osx build:
	gcc -o pbn pb.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -Bdynamic -lc -lm 
	#linux buid:
	#gcc -o bn bn.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -lrt -lpgm -Bdynamic -lc -lm -lgcc
	#idk this one
	#gcc -o bn bn.c --static -ltoxcore -lzmq -lpthread -lm -lsodium -lrt -lgcc -lpgm  -lc
	strip bn
	#upx probably broke
	#upx bn
	#upx --ultra-brute bn
	file bn
	du -hs bn
	#no static:
	#gcc -o cli cli.c -ltoxcore -lpthread -lsodium
clean:

	rm -v bn
