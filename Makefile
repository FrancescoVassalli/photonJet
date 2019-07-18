all: berephotonjet

berephotonjet: berephotonjet.cc
	g++ berephotonjet.cc $(PYTHIA8)/lib/libpythia8.a -o berephotonjet  -I$(PYTHIA8)/include -O2 -ansi -pedantic -W -Wall -Wshadow -fPIC -Wl,-rpath,$(PYTHIA8)/lib -ldl `root-config --libs --cflags`
