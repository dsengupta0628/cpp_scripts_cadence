CFLAGS = -D_UNIX 

OBJS = ./obj/main.o ./obj/read_ckt.o ./obj/myFilter.o

OPT = -c -g -w $(CFLAGS) 

do_filter: $(OBJS) 	./include/define.h
	g++ -pg -o do_filter $(OBJS) $(INCLUDE) $(LIBS)

	rm -f core*
	
./obj/read_ckt.o:	./src/read_ckt.cpp ./include/read_ckt.h
	g++ -o ./obj/read_ckt.o $(OPT) ./src/read_ckt.cpp
	
./obj/myFilter.o:	./src/myFilter.cpp ./include/myFilter.h
	g++ -o ./obj/myFilter.o $(OPT) ./src/myFilter.cpp	

./obj/main.o:	./src/main.cpp  
	g++ -o ./obj/main.o $(OPT) ./src/main.cpp

clean:
	rm -f do_filter ./obj/*.o ./auxfiles/* ./gmon.out
