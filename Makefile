build:
	g++ -std=c++11 -c *.cpp
	g++ *.o -o tracer.exe

clean:
	rm *.o
	rm tracer.exe
