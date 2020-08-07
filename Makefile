def:
	#gcc -static-libgcc -static-libstdc++ --static -o bn bn.c -ltoxcore -lzmq -lpthread -lm -lc -lsodium -lrt -lgcc -lpgm
	#osx build:
	#gcc -o pbn pb.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -Bdynamic -lc -lm 
	#linux buid:
	#gcc -o bn bn.c -Bstatic /usr/local/lib/libtoxcore.a /usr/local/lib/libsodium.a  -lpthread -lrt -lpgm -Bdynamic -lc -lm -lgcc
	#idk this one
	#gcc -o bn bn.c --static -ltoxcore -lzmq -lpthread -lm -lsodium -lrt -lgcc -lpgm  -lc 
	#gcc -o pbn pbn.c -I tox/include -Bstatic -lpthread ./tox/lib/libtoxcore.a /usr/lib64/libsodium.a -lrt -lc -lm -lgcc
	gcc -o pbn pbn.c -I tox/include -I deps/file2str -lpthread -Bstatic ./tox/lib/libtoxcore.a /usr/lib64/libsodium.a -lrt -Bdynamic -lc -lm -lgcc
	strip pbn
	#upx probably broke
	#upx bn
	#upx --ultra-brute bn
	file pbn
	du -hs pbn
	#no static:
	#gcc -o cli cli.c -ltoxcore -lpthread -lsodium
clean:

	rm -v pbn
