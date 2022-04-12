CC = g++ -Wall

all: logger_so logger

logger_so:
	$(CC) -o logger.so -shared -fPIC logger.cpp -ldl

logger:
	$(CC) hw2.cpp -o logger

.PHONY:clean
clean:
	rm logger.so logger *.txt
