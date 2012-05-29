================ SEQUENCIAL ================

1.COMPILAR:
	gcc -o jacobi jacobi.c

2. EXECUTAR:
	./jacobi <entrada>
	./jacobi <entrada> 

OBS: <entrada> é o nome do arquivo a ser testado.

================ OpenMP ================

1.COMPILAR:
	gcc -o jacobiMP jacobiOpenMP -fopenmp
	gcc-4.2 -o jacobiMP jacobiOpenMP -fopenmp (cluster)
	
2. EXECUTAR:
	./jacobiMP <entrada> 
	./jacobiMP <entrada> 

OBS: <entrada> é o nome do arquivo a ser testado.

================ MPI ================

1.COMPILAR:
	mpicc -o jacobiMPI jacobiMPI.c

2. EXECUTAR LOCALMENTE:
	mpirun -np <quantidade de processo> ./jacobiMPI <entrada> 

3. EXECUTAR NO CLUSTER:
	mpirun -machinefile host_file -np <quantidade de processo> ./jacobiMPI <entrada> 

OBS: host_file é um arquivo que possui os nomes das máquimas a serem utilizadas nos testes
	 e <entrada> é o nome do arquivo a ser testado.


