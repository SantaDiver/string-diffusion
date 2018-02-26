all: strdif
EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc loadExchange.cc

%: %.cc
	mpic++ -o $@.exe $< $(EXTRAFILES)
	
bg:
	mpicxx -o $@.exe $< $(EXTRAFILES)

smt:
	mpisubmit.bg -n 128 strdif.exe


report: all
	rm -rf reportdata
	echo "" > ./reportdata
	mpirun -np 4 ./strdif.exe >> ./reportdata;
	mpirun -np 8 ./strdif.exe >> ./reportdata;
	mpirun -np 16 ./strdif.exe >> ./reportdata;
	mpirun -np 32 ./strdif.exe >> ./reportdata;

animate:
	python anim.py

clean:
	rm -rf strdif.exe *.o
	rm -rf reportdata
