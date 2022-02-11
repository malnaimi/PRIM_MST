CC	=	/opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
CCLINK	=	/opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
SHELL	=	/bin/sh

EXEC	= prim_mst_mpi

main: prim_mst_mpi.c
	$(CC) -o $(EXEC) $(EXEC).c -lm -ldl

clean:
	/bin/rm -f $(EXEC) $(EXEC)*.o $(EXEC)*.s

