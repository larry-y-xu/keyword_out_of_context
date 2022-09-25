# Description of files

Uses dynamically allocated memory to solve the keyword out of context problem.
Taking as input a file containing the keywords, a file containing all words 
to be excluded from the keywords, the program will generate an output file
consisting of all keywords and their frequency

* ```makefile```: Needed to build the assignment. In order to
construct the ```kwoc3``` executable, enter either ```make``` or
```make kwoc3```.

* ```kwoc3.c```: The C file containing the ```main()``` function.

* ```emalloc.[ch]```: Source code and header file for the 
implementation of an error checked malloc function

* ```listy.[ch]```: Linked-list routines
