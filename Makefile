####################################################################################################
# All code is split up into src files which stores the main code as well as test programes,        #
# and lib files which contain utility functions. Object files are stored in seperate hidden .obj   #
# subdirectories. External libraries have to be build seperatly and are not handeld by this        #
# makefile                                                                                         #
####################################################################################################

CC = gcc

SRC_DIR = src
OBJ_DIR = src/.obj
LOB_DIR = lib/.obj
LIB_DIR = lib
RNL_DIR = $(LIB_DIR)/ranlux-3.4
# Which libraries should be linked
LNKS = -lm
DBG = -D STATISTICS_DEBUG -D METROPOLIS_DEBUG
# CFLAGS = -g -ftrapv -std=c11 -I $(LIB_DIR) -I $(RNL_DIR) $(LNKS) # $(DBG)
CFLAGS = -g -ftrapv -I $(LIB_DIR) -I $(RNL_DIR) $(LNKS) # $(DBG)
RNL_FLGS = $(RNL_DIR)/ranlux_common.o $(RNL_DIR)/ranlxd.o $(RNL_DIR)/ranlxs.o


all:
	mkdir -p ./app ./tests
	mkdir -p ./lib/.obj
	mkdir -p ./src/.obj
	make main
	make analysation
	make sanalysation
	make uncertainties
	make literature_test
	make prng_test
	make grid_test
	make idx_test
	make metro_test
	make imdf_test

main: $(OBJ_DIR)/main.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o
	$(CC) -o app/main $^ $(RNL_FLGS) $(CFLAGS)

analysation: $(OBJ_DIR)/analysation.o $(LOB_DIR)/statistics.o
	$(CC) -o app/analysation $^ $(RNL_FLGS) $(CFLAGS)

t0s: $(OBJ_DIR)/t0s.o $(LOB_DIR)/metropolis.o $(LOB_DIR)/statistics.o $(LOB_DIR)/grid.o 
	$(CC) -o app/t0s $^ $(RNL_FLGS) $(CFLAGS) $(DBG)

sanalysation: $(OBJ_DIR)/sanalysation.o $(LOB_DIR)/statistics.o
	$(CC) -o app/sanalysation $^ $(RNL_FLGS) $(CFLAGS)

uncertainties: $(OBJ_DIR)/uncertainties.o $(LOB_DIR)/statistics.o
	$(CC) -o app/uncertainties $^ $(RNL_FLGS) $(CFLAGS)

phasetransition: $(OBJ_DIR)/phasetransition.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o $(LOB_DIR)/statistics.o
	$(CC) -o app/phasetransition $^ $(RNL_FLGS) $(CFLAGS)

literature_test: $(OBJ_DIR)/literature_test.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o
	$(CC) -o tests/literature_test $^ $(RNL_FLGS) $(CFLAGS)

prng_test: $(OBJ_DIR)/prng.o
	$(CC) -o tests/prng_test $^ $(RNL_DIR)/ranlux_common.o $(RNL_DIR)/ranlxd.o $(RNL_DIR)/ranlxs.o $(CFLAGS)

grid_test: $(OBJ_DIR)/grid_test.o $(LOB_DIR)/grid.o
	$(CC) -o tests/grid_test $^ $(RNL_FLGS) $(CFLAGS)

idx_test: $(OBJ_DIR)/idx_test.o $(LOB_DIR)/grid.o
	$(CC) -o tests/idx_test $^ $(RNL_FLGS) $(CFLAGS)

delta_h_test: $(OBJ_DIR)/delta_h_test.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o
	$(CC) -o tests/delta_h_test $^ $(RNL_FLGS) $(CFLAGS)

metro_test: $(OBJ_DIR)/metro_test.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o
	$(CC) -o tests/metro_test $^ $(RNL_FLGS) $(CFLAGS)

imdf_test: $(OBJ_DIR)/imdf_test.o $(LOB_DIR)/grid.o $(LOB_DIR)/imdf.o
	$(CC) -o tests/imdf_test $^ $(RNL_FLGS) $(CFLAGS)

observable_test: $(OBJ_DIR)/observable_test.o $(LOB_DIR)/grid.o $(LOB_DIR)/metropolis.o
	$(CC) -o tests/observable_test $^ $(RNL_FLGS) $(CFLAGS)


# Generic rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 

$(LOB_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 

run:
	./app/main

clean:
	rm -f tests/* app/* $(OBJ_DIR)/*.o $(LOB_DIR)/*.o
