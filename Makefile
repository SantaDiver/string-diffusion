all: strdif
EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc loadExchange.cc
STEPS_TO_COUNT = 10000
COMM_WIDTH = 4
BALANCE_ONCE_AT = 100

%: %.cc
	mpic++ -o $@.exe $< $(EXTRAFILES)
	
bg: strdif.cc
	mpicxx -o $@.exe $< $(EXTRAFILES)

bgreport:
	rm -rf reportdata
	touch ./reportdata
	mpisubmit.bg -n 128 bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 1 >> ./reportdata
	mpisubmit.bg -n 128 bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 10 >> ./reportdata
	mpisubmit.bg -n 128 bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 100 >> ./reportdata
	mpisubmit.bg -n 128 bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 1000 >> ./reportdata
	mpisubmit.bg -n 128 bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 10000 >> ./reportdata

report: all
	rm -rf reportdata
	touch ./reportdata
	mpirun -np 4 ./strdif.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) $(BALANCE_ONCE_AT)  >> ./reportdata;
	mpirun -np 8 ./strdif.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) $(BALANCE_ONCE_AT)  >> ./reportdata;
	mpirun -np 16 ./strdif.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) $(BALANCE_ONCE_AT) >> ./reportdata;
	
clean:
	rm -rf *.exe *.o
	rm -rf reportdata
	rm -rf core* *.out *.err
	
send:
	scp -r "/home/nikolay/Рабочий стол/string-diffusion" edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru:~/strdif

connect:
	ssh edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru
