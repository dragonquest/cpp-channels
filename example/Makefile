
build:
	clang++ -Wall -stdlib=libc++ -std=c++1z -o main main.cc ../waitgroup.cc -ggdb -O0
	clang++ -Wall -stdlib=libc++ -std=c++1z -o main2 main2.cc ../waitgroup.cc -ggdb -O0

clean:
	rm main
	rm -rf main.dSYM/
	rm main2
	rm -rf main2.dSYM/
