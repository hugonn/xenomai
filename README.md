# xenomai
Repositorio referente ao trabalho t2 da cadeira de Sistemas de Tempo Real PUCRS

# Como rodar

- Primeiro export nas flags necessárias dentro do ambiente RTI 
export LDFLAGS=`xeno-config --skin=native --ldflags`
export CFLAGS=`xeno-config --skin=native --cflags`

- Depois compilar e executar 
   gcc $CFLAGS $LDFLAGS prog.c -o ex06a
   
   ./ex06a
