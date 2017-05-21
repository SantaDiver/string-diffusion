all: strdif
EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc

%: %.cc
	mpic++ -o $@ $< $(EXTRAFILES) -fopenmp -m64

report: all
	rm -rf reportdata
	echo "" > ./reportdata
	mpirun -np 1 ./strdif >> ./reportdata;
	mpirun -np 2 ./strdif >> ./reportdata;
	mpirun -np 4 ./strdif >> ./reportdata;

animate:
	python anim.py

clean:
	rm -rf strdif *.o
	rm -rf reportdata
