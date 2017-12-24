.SUFFIXES:
.PHONY:    pc,pc2,pcm,fp,f,fi,fc,clean

compilador := g++ -std=c++11
flagsc     := -Wall

r: relojes
	./$<

t: tiempos
	./$<

e1: ejecutivo1
	./$<

e1c: ejecutivo1-compr
	./$<

e2: ejecutivo2
	./$<

%_exe: %.cpp
	$(compilador) $(flagsc) -o $@ $<

clean:
	rm -rf *_exe *.dSYM
