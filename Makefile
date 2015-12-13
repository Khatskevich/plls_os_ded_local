all:
	g++ main.cpp OSDPlib.c OSDPlib.h log.h log.c -lssl -lcrypto -g
	mkdir base_dir
	mkdir base_dir/info
	mkdir base_dir/data
test:
	cat a.out >> a.for_test
	cat a.out>> a.for_test
	cat a.for_test >> a.for_test2
	cat a.for_test >> a.for_test2
	./a.out store a.out
	./a.out store a.for_test
	./a.out store a.for_test2
	./a.out restore a.out a.out.restored
	./a.out restore a.for_test a.for_test.restored
	./a.out restore a.for_test2 a.for_test2.restored

clean:
	rm -rf base_dir
	rm a.*
