EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc loadExchange.cc
N_PROCESSORS = 64
W_CLOCK_TIME = 600
STEPS_TO_COUNT = 100
COMM_WIDTH = 4
BALANCE_ONCE_AT = 1
OVERHEAT = 0.3
DIFF_POWER = 1

all: strdif
%: %.cc
	$$HOME/opt/usr/local/bin/mpic++ -o $@.exe $< $(EXTRAFILES)

run: all
	$$HOME/opt/usr/local/bin/mpirun -np 4 ./strdif.exe $(STEPS_TO_COUNT) 2 1 1 1

report: all
	rm -rf reportdata
	touch reportdata
	for number in 1 2 5 10; do \
		$$HOME/opt/usr/local/bin/mpirun -np 4 ./strdif.exe $(STEPS_TO_COUNT) 2 $$number 1 1 >> reportdata ; \
	done

bg: strdif.cc
	mpixlcxx -o $@.exe $< $(EXTRAFILES)

bgreport: bg
	for number in 4 8 16; do \
		mpisubmit.bg -n $$number -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 1 $(OVERHEAT) $(DIFF_POWER); \
		mpisubmit.bg -n $$number -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 10 $(OVERHEAT) $(DIFF_POWER); \
		mpisubmit.bg -n $$number -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 100 $(OVERHEAT) $(DIFF_POWER); \
	done

clean:
	rm -rf *.exe *.o
	rm -rf reportdata
	rm -rf core* *.out *.err

send:
	scp -r "." edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru:~/strdif

connect:
	ssh edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru

print:
	cat *.out
