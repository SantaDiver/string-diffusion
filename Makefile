EXTRAFILES = pieceOfWebFunc.cc generatingFuncs.cc loadExchange.cc
STEPS_TO_COUNT = 1000
COMM_WIDTH = 2
BALANCE_ONCE_AT = 1
N_PROCESSORS = 4
W_CLOCK_TIME = 600

all: strdif
%: %.cc
	$$HOME/opt/usr/local/bin/mpic++ -o $@.exe $< $(EXTRAFILES)

run: all
	$$HOME/opt/usr/local/bin/mpirun -np $(N_PROCESSORS) ./strdif.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) $(BALANCE_ONCE_AT)

report: all
	rm -rf reportdata
	touch reportdata
	for number in 1 5 $(STEPS_TO_COUNT); do \
		$$HOME/opt/usr/local/bin/mpirun -np $(N_PROCESSORS) ./strdif.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) $$number >> reportdata ; \
	done

bg: strdif.cc
	mpicxx -o $@.exe $< $(EXTRAFILES)

bgreport:
	rm -rf reportdata
	touch ./reportdata
	mpisubmit.bg -n $(N_PROCESSORS) -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 1
	mpisubmit.bg -n $(N_PROCESSORS) -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 10
	mpisubmit.bg -n $(N_PROCESSORS) -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 100
	mpisubmit.bg -n $(N_PROCESSORS) -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 1000
	mpisubmit.bg -n $(N_PROCESSORS) -w $(W_CLOCK_TIME) bg.exe $(STEPS_TO_COUNT) $(COMM_WIDTH) 10000

clean:
	rm -rf *.exe *.o
	rm -rf reportdata
	rm -rf core* *.out *.err

send:
	scp -r "." edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru:~/

connect:
	ssh edu-cmc-sqi18-423-11@bluegene.hpc.cs.msu.ru

print:
	cat *.out
