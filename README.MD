### WeeChess - chess engine
####  Author: Ewan Crawford
#### Email: ewan.cr@gmail.com
#### License: MIT

Based on 'Programming A Chess Engine in C' youtube series & Stockfish

## TODO
* Add build instructions
* Improve Readme
* Add travis testing
  - Perft
  - Static analysis tools

### Test status
[![Build Status](https://travis-ci.org/EwanC/WeeChess.svg)](https://travis-ci.org/EwanC/WeeChess) Master

## License
Copyright 2017 Ewan Crawford

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### OCL Move Kernel Bug

+ Fix Stack smashing bug


Process 3047 stopped
* thread #1: tid = 3047, 0x00007ffff7328cc9 libc.so.6`gsignal + 57, name = 'WeeChess', stop reason = signal SIGABRT
    frame #0: 0x00007ffff7328cc9 libc.so.6`gsignal + 57
libc.so.6`gsignal:
->  0x7ffff7328cc9 <+57>: cmpq   $-0x1000, %rax
    0x7ffff7328ccf <+63>: ja     0x7ffff7328cea            ; <+90>
    0x7ffff7328cd1 <+65>: rep    
    0x7ffff7328cd2 <+66>: retq   
(lldb) bt
* thread #1: tid = 3047, 0x00007ffff7328cc9 libc.so.6`gsignal + 57, name = 'WeeChess', stop reason = signal SIGABRT
  * frame #0: 0x00007ffff7328cc9 libc.so.6`gsignal + 57
    frame #1: 0x00007ffff732c0d8 libc.so.6`abort + 328
    frame #2: 0x00007ffff7365394 libc.so.6`??? + 708
    frame #3: 0x00007ffff73fcc9c libc.so.6`__fortify_fail + 92
    frame #4: 0x00007ffff73fcc40 libc.so.6`__stack_chk_fail + 16
    frame #5: 0x000000000040f9a3 WeeChess`Search::AlphaBeta(alpha=-135, beta=30000, depth=4, b=0x00007fffffff7020, info=0x00007fffffff6fe0, DoNull=true) + 2502 at search.cpp:327
    frame #6: 0x000000000040fa54 WeeChess`Search::SearchPosition(b=0x00007fffffff7020, info=0x00007fffffff6fe0) + 167 at search.cpp:346
    frame #7: 0x000000000041105c WeeChess`CLI::consoleLoop(b=0x00007fffffff7020, info=0x00007fffffff6fe0) + 466 at protocols.cpp:276
    frame #8: 0x0000000000407b6e WeeChess`main(argc=2, argv=0x00007fffffffd858) + 405 at main.cpp:123
    frame #9: 0x00007ffff7313ec5 libc.so.6`__libc_start_main + 245
    frame #10: 0x00000000004075e9 WeeChess

    *** stack smashing detected ***: ./WeeChess terminated

How to replicate:
	setboard 2kr1b1r/p1p1pppp/2p1q3/3pN3/3PbB2/4P2P/PPP1QPP1/R4RK1 w - - 0 20
	e2a6
	go
