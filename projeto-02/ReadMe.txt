================ SEQUENCIAL ================

1.COMPILAR:
	gcc -o palindromo palindromoMP.c -lm

2. EXECUTAR:
	./palindromo <texto de entrada> palavra
	./palindromo <texto de entrada> sentenca

OBS: Não esquecer que para testar no modo sequencial deve-se comentar toda a diretiva pragma do OpenMP.
     O argumento 'sentenca' procura sentencas e palavras  palindromos em um arquivo.

================ OpenMP ================

1.COMPILAR:
	gcc -o palindromo palindromoMP.c -lm -fopenmp
	gcc-4.2 -o palindromo palindromoMP.c -lm -fopenmp (cluster)
	
2. EXECUTAR:
	./palindromo <texto de entrada> palavra
	./palindromo <texto de entrada> sentenca

OBS: Não esquecer de descomentar toda a diretiva pragma da OpenMP.

================ MPI ================

------ No texto MENOR (procura de palindromo por palavra e sentenca) ------

1.COMPILAR:
	mpicc -o palindromo palindromoMpiSentenca.c -lm

2. EXECUTAR LOCALMENTE:
	mpirun -np <quantidade de processo> ./palindromo 

3. EXECUTAR NO CLUSTER:
	mpirun -machinefile host_file -np <quantidade de processo> ./palindromo 

------ No texto MAIOR (procura de palindromo por palavra) ------

1.COMPILAR:
	mpicc -o palindromo palindromoMpiPalavra.c -lm

2. EXECUTAR LOCALMENTE:
	mpirun -np <quantidade de processo> ./palindromo 

3. EXECUTAR NO CLUSTER:
	mpirun -machinefile host_file -np <quantidade de processo> ./palindromo 
	

OBS: host_file é um arquivo que possui os nomes das máquimas a serem utilizadas nos testes.

================ OpenMP/MPI ================

------ No texto MENOR (procura de palindromo por palavra e sentenca) ------

1.COMPILAR:
	mpicc -o palindromo palindromoMpiSentenca.c -lm -fopenmp

2. EXECUTAR:
	mpirun -np <quantidade de processo> ./palindromo 

------ No texto MAIOR (procura de palindromo por palavra) ------

1.COMPILAR:
	mpicc -o palindromo palindromoMpiPalavra.c -lm -fopenmp

2. EXECUTAR:
	mpirun -np <quantidade de processo> ./palindromo 

1.Compilar no Cluster:
	MPI_COMPILE_FLAGS = $(shell mpicc --showme:compile)
	MPI_LINK_FLAGS = $(shell mpicc --showme:link) -fopenmp

	MY_APP=my-source
	CC=gcc-4.2

	my_app: $(MY_APP).c
	$(CC) $(MPI_COMPILE_FLAGS) $(MY_APP).c $(MPI_LINK_FLAGS) -o $(MY_APP)

2.Executar no cluster:
	mpirun -machinefile host_file -np <quantidade de processo> ./palindromo 

OBS: host_file é um arquivo que possui os nomes das máquimas.
     Não esquecer que para testar deve-se descomentar toda a diretiva pragma do OpenMP.
     

