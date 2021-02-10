WHAT IS dis68?
-------------

dis68 is a disassembler that will disassemble Microware OS9-68K code.  It
will disassemble the code into source code that should rebuild the code into
a module identical to that from which it is disassembled.

The code can be disassembled with the use of files that control the way it
is disassembled.  The main file is a commands file, which declares the modes
which particular instructions are disassembled, etc. Also one or more files
can be used to define the names of labels.  The method of disassembly usually
require disassembly, inspecing the result, adding commands, redisassembly,
and so forth until a satisfactory output is obtained.

This program can be built and run on Microware OS9-68K, Linux, or Windows
systems.
