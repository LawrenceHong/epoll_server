TARGETS = server client
STATIC_OBJS = main.o client.o
LIBS = -lev
G_FLAGS = -g -O -Wall -I ../
SOURCES = $(wildcard *.cc)
all : $(TARGETS)
.PHONY: all
whoami = $(shell whoami)
vpath %.h ../

server: main.o
	g++ $(G_FLAGS) main.o -o server $(LIBS)

client: client.o
	g++ $(G_FLAGS) client.o -o client $(LIBS)

$(STATIC_OBJS):%.o:%.cc
	g++ -g -c $< -o $@ $(LIBS) -I ../

#chargen.o: chargen.cc
#	g++ -g -c chargen.cc -o chargen.o -lmuduo_net -lmuduo_base -lpthread -lrt

#main.o: main.cc
#	g++ -g -c main.cc -o main.o -lmuduo_net -lmuduo_base -lpthread -lrt

#.PHONY的作用是，如果执行过程中出错，继续
.PHONY: clean clean_target clean_obj
clean: clean_target clean_obj
clean_target:
	-$(RM) $(TARGETS)
clean_obj:
	-$(RM) *.o

.INTERMEDIATE: %.o
FORCE:

print: $(SOURCES)
	@echo $?
	@touch print
