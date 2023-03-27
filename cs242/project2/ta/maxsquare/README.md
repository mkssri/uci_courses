# Max-Square for cs242

Three algorithms are presented. All of them rectangular matrix transposers:
- MaxSquare: In-place. Partially parallel. Recursive split-transpose-merge of maximum square matrix and remainder.
- Cycles: In-place. Sequential. Based on cyclic shift of interleaved cycles.
- Out of place: Parallel. Read from matrix A, and write into new matrix T(A). This is just a baseline for comparison.

## Dependencies
### Performance monitoring library: `perform/pfmlib.h`
Development library and header files to create performance monitoring applications for the perf_events interface (such as cache events).

```
# On Ubuntu
sudo apt-get install libpfm4-dev

# On Fedora
sudo dnf install libpfm-devel
```


## Building/Using

The makefile has 4 targets:
- `transp_maxsq` : build the MaxSquare binary.
- `transp_cycles` : build the Cycles binary.
- `transp_ooplace` : build the Out-Of-Place transposer binary.
- `all` : build all of the above.
- `depends` : list the source files composing each of the implementations.
- `clean` : remove all compile-time produced files.

To know how to use each produced binary (in `/build`), run them with the option `--help`. They should document themselves.

For questions, write to parraca@uci.edu
