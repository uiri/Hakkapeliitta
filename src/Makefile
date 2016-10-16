ESDK=$(EPIPHANY_HOME)
ELIBS=-L $(ESDK)/tools/host/lib
EINCS=-I $(ESDK)/tools/host/include
ELDF=$(ESDK)/bsps/current/fast.ldf

FILES = main.cpp benchmark.cpp bitboards.cpp counter.cpp evaluation.cpp history.cpp killer.cpp movegen.cpp movesort.cpp pht.cpp position.cpp search.cpp tt.cpp uci.cpp zobrist.cpp syzygy/tbprobe.cpp utils/threadpool.cpp utils/epiphany.c task.c
FLAGS = -pthread -std=c++11 -Ofast -Wall -flto -march=native -s -DNDEBUG -Wl,--no-as-needed
LIBS= -le-hal -le-loader

make: $(FILES)
	g++ $(FLAGS) $(EINCS) $(ELIBS) $(LIBS) $(FILES) -o Hakkapeliitta

e_task.elf: e_task.c e_task.h
	e-gcc -T $(ELDF) $^ -o $@ -le-lib
