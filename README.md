# PopFilter Project

## Goals

There are many noise reduction tools, but barely one that is capable detecting and
fixing the "pop" sound which is often heard on scratched or dirty vinyl records.

1. Find a common frequency curve of the "pop" sound
2. Check the sound file for similar curves
3. Cut out the affected part
4. Interpolate the sound using samples before/after
5. Make it faster (DFT is somewhat slow)

Currently at stage 1.


## Actual goals

1. Learn how to C++11
2. Learn how to read and analyse sound files
3. Fiddle a bit with Discrete Fourier Analysis
4. Write simple utilities for later use in other projects
5. Maybe get this working somewhen (would be amazing)


## Requirements

- C++11 compiler
- [gnuplot](http://www.gnuplot.info/) to draw nice graphs
- [libogg and libvorbis](https://xiph.org/)


## License

BSD 3-clause (or "BSD License 2.0"), see LICENSE.txt

Based upon choices of the ogg and vorbis libraries.
