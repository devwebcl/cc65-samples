#  cc65 samples

This is a repository with several programs ported to cc65. 
Each program will have a reference pointing to the original source.

### Examples

 * [Hello World](https://github.com/devwebcl/cc65-samples/blob/master/src/hello-world/hello.c): it shows size of different data types/size,free memory left, [prototype](https://github.com/devwebcl/cc65-samples/blob/master/src/hello-world/prototype.c).
 * [Ghostbusters Account](https://github.com/devwebcl/cc65-samples/blob/master/src/ghostbusters/ghostbcc.c): it generates accounts for Ghostbusters video game
 * [Cloak of Darkness](https://github.com/devwebcl/cc65-samples/blob/master/src/cloak/cloak.c): the hello-world text adventure game ported to a8 through Scott Adams engine.
 * [Roman Numbers](https://github.com/devwebcl/cc65-samples/blob/master/src/romans/romans.c): clever solution to convert from base 10 to roman numbers. This solution comes from [Knuth's TeX](http://mirrors.ctan.org/systems/knuth/dist/tex/tex.web).
 * [C Pointers](https://github.com/devwebcl/cc65-samples/blob/master/src/pointers/pointers.c): Basic examples of pointers for int, arrays, strings, etc.
 * [Knuth–Morris–Pratt algorithm](https://github.com/devwebcl/cc65-samples/blob/master/src/kmp/kmp.c): Knuth–Morris–Pratt algorithm implementation.
 * [Dijkstra's shortest path algorithm](https://github.com/devwebcl/cc65-samples/blob/master/src/dijkstra/dijkstra.c): Dijkstra's shortest path algorithm implementation.
 * [Pi -spigot-](https://github.com/devwebcl/cc65-samples/blob/master/src/pi/pi-spigot-algorithm.c):  * [Dijkstra's shortest path algorithm](https://github.com/devwebcl/cc65-samples/blob/master/src/dijkstra/dijkstra.c): Dijkstra's shortest path algorithm implementation.

 

### Compilation
You need to set system:

```sh
 cl65 -t atari hello.c -o hello.xex
```


### TODO

 - add more examples :)
 - add README to each sample



### License

MIT
