all: strdif
EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc loadExchange.cc

%: %.cc
	mpic++ -o $@.exe $< $(EXTRAFILES)
	
bg: strdif.cc
	mpicxx -o $@.exe $< $(EXTRAFILES)

smt:
	mpisubmit.bg -n 128 bg.exe


report: all
	rm -rf reportdata
	echo "" > ./reportdata
	mpirun -np 4 ./strdif.exe >> ./reportdata;
	mpirun -np 8 ./strdif.exe >> ./reportdata;
	mpirun -np 16 ./strdif.exe >> ./reportdata;
	mpirun -np 32 ./strdif.exe >> ./reportdata;

clean:
	rm -rf *.exe *.o
	rm -rf reportdata
	rm -rf core* *.out *.err
