SHELL := /bin/bash
SDIR := src
IDIR := include
ODIR := build
LDIR := lib

CC:=gcc
CFLAGS:=-I$(IDIR) -Wall -Werror -std=gnu11 -march=native -g -fno-omit-frame-pointer
LIBS:=-pthread -Wl,-rpath=/usr/local/lib -L/usr/local/lib -lpfm

src_common := $(shell echo $(SDIR)/{test/transp.c,util/{measure,util,vector}.c})
src_maxsq := $(shell echo $(SDIR)/{exchange/three-mirrors.c,shuffle/exch-geom.c,square-transp/lower-tri.c,transpose/max-square.c})
src_cycles := $(shell echo $(SDIR)/transpose/cycles.c)
src_oop := $(shell echo $(SDIR)/transpose/out-of-place.c)

.PHONY: all clean list

all: transp_maxsq transp_cycles transp_ooplace

clean:
	rm -f $(patsubst %.c,%.o,$(src_common) $(src_maxsq) $(src_cycles) $(src_oop))
	rm -rf $(ODIR)

%.o: %.c
	@$(CC) -c -o $@ $? $(CFLAGS)

transp_maxsq: $(patsubst %.c,%.o,$(src_common) $(src_maxsq))
	@mkdir -p $(ODIR)
	@$(CC) -o $(ODIR)/$@ $(CFLAGS) $? $(LIBS)
	@printf "OK: %-18.18s: MaxSquare rectangular transposer\n" "$@"

transp_cycles: $(patsubst %.c,%.o,$(src_common) $(src_cycles))
	@$(CC) -o $(ODIR)/$@ $(CFLAGS) $? $(LIBS)
	@printf "OK: %-18.18s: Cycles rectangular transposer\n" "$@"

transp_ooplace: $(patsubst %.c,%.o,$(src_common) $(src_oop))
	@$(CC) -o $(ODIR)/$@ $(CFLAGS) $? $(LIBS)	
	@printf "OK: %-18.18s: Simple out-of-place rectangular transposer\n" "$@"

depends:
	@echo "Common Code:"
	@echo $(src_common) | sed -e 's| |\n|g'
	@echo -e "\nMax-Square:"
	@echo $(src_maxsq) | sed -e 's| |\n|g'
	@echo -e "\nCycles:"
	@echo $(src_cycles) | sed -e 's| |\n|g'
	@echo -e "\nOut of place:"
	@echo $(src_oop) | sed -e 's| |\n|g'

list:
	@make -pRrq : 2>/dev/null |\
	awk -v RS= -F: '{if ($$1 !~ "^[#.]") {print $$1}}' |\
	egrep -v -e '^[^[:alnum:]]' -e '^$@$$' -e '^list$$'
