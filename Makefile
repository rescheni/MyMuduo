test:Timestamp.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -rf test
