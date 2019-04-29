# CMPS104A
# ASSIGNMENT: ASG1
# NAME 1: Shlok Gharia
# EMAIL 1: sgharia@ucsc.edu
# NAME 2: Arsh Malhotra
# EMAIL 2: amalhot3@ucsc.edu

COMPILECPP = g++ -std=gnu++17 -g -O0 -Wall -Wextra -Wold-style-cast
MAKEDEPCPP = g++ -std=gnu++17 -MM

MKFILE   = Makefile
DEPFILE  = Makefile.dep
SOURCES  = string_set.cpp auxdebug.cpp oc.cpp
HEADERS  = string_set.h auxdebug.h
OBJECTS  = ${SOURCES:.cpp=.o}
EXECBIN  = oc
SRCFILES = ${HEADERS} ${SOURCES} ${MKFILE}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} ${OBJECTS} -o ${EXECBIN}

%.o : %.cpp
	${COMPILECPP} -c $<

ci :
	cid + ${SRCFILES}

clean :
	- rm ${OBJECTS} ${DEPFILE}

spotless : clean
	- rm ${EXECBIN} *.str

${DEPFILE} :
	${MAKEDEPCPP} ${SOURCES} >${DEPFILE}

deps :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

include ${DEPFILE}

