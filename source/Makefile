CC = g++
JSONCPP = #-I/usr/local/include
LIBCURL = -lcurl
LFLAGS = $(LIBCURL) $(JSONCPP) -std=c++11  -Wall -pedantic
SOURCE_DIR = measurement
OBJS = $(SOURCE_DIR)/main.o $(SOURCE_DIR)/jsoncpp.o $(SOURCE_DIR)/UlrRequester.o $(SOURCE_DIR)/Response.o $(SOURCE_DIR)/TestData.o $(SOURCE_DIR)/ResponseScan.o

all: compile

install:
	pip install pycurl
	pip install commentjson
	pip install python-novaclient

run:
	time $(SOURCE_DIR)/program.o password_1234 tmp_data.json config.json

compile: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(SOURCE_DIR)/program.o

$(SOURCE_DIR)/main.o: $(SOURCE_DIR)/main.cpp $(SOURCE_DIR)/Request_thr.h
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/main.cpp -o $(SOURCE_DIR)/main.o

$(SOURCE_DIR)/TestData.o: $(SOURCE_DIR)/TestData.cpp $(SOURCE_DIR)/TestData.hpp
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/TestData.cpp -o $(SOURCE_DIR)/TestData.o

$(SOURCE_DIR)/UlrRequester.o: $(SOURCE_DIR)/UlrRequester.cpp $(SOURCE_DIR)/UlrRequester.hpp $(SOURCE_DIR)/Request_thr.h
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/UlrRequester.cpp -o $(SOURCE_DIR)/UlrRequester.o

$(SOURCE_DIR)/Response.o: $(SOURCE_DIR)/Response.cpp $(SOURCE_DIR)/Response.hpp
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/Response.cpp -o $(SOURCE_DIR)/Response.o

$(SOURCE_DIR)/ResponseScan.o: $(SOURCE_DIR)/ResponseScan.cpp $(SOURCE_DIR)/ResponseScan.hpp
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/ResponseScan.cpp -o $(SOURCE_DIR)/ResponseScan.o

$(SOURCE_DIR)/jsoncpp.o: $(SOURCE_DIR)/jsoncpp/jsoncpp.cpp $(SOURCE_DIR)/jsoncpp/json/json.h  $(SOURCE_DIR)/jsoncpp/json/json-forwards.h
	$(CC) $(LFLAGS) -c $(SOURCE_DIR)/jsoncpp/jsoncpp.cpp -o $(SOURCE_DIR)/jsoncpp.o


clean:
	rm $(OBJS) $(SOURCE_DIR)/program.o