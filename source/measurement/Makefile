CC = g++
JSONCPP = #-I/usr/local/include
LIBCURL = -lcurl
LFLAGS = $(LIBCURL) $(JSONCPP) -std=c++11  -Wall -pedantic
OBJS = main.o jsoncpp.o UlrRequester.o Response.o TestData.o ResponseScan.o

all: run

run: compile
	#./program.o
	time ./program.o http://87.190.239.41

compile: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o program.o

main.o: main.cpp Request_thr.h
	$(CC) $(LFLAGS) -c main.cpp -o main.o

TestData.o: TestData.cpp TestData.hpp
	$(CC) $(LFLAGS) -c TestData.cpp -o TestData.o

UlrRequester.o: UlrRequester.cpp UlrRequester.hpp
	$(CC) $(LFLAGS) -c UlrRequester.cpp -o UlrRequester.o

Response.o: Response.cpp Response.hpp
	$(CC) $(LFLAGS) -c Response.cpp -o Response.o

ResponseScan.o: ResponseScan.cpp ResponseScan.hpp
	$(CC) $(LFLAGS) -c ResponseScan.cpp -o ResponseScan.o

jsoncpp.o: ./jsoncpp/jsoncpp.cpp ./jsoncpp/json/json.h  ./jsoncpp/json/json-forwards.h
	$(CC) $(LFLAGS) -c ./jsoncpp/jsoncpp.cpp -o jsoncpp.o


clean:
	rm $(OBJS) program.o