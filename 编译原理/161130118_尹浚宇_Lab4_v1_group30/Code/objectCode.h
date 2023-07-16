#ifndef OBJECTCODE_H
#define OBJECTCODE_H

#include "interCode.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NAMESIZE 64
#define HASHSIZE 0x3fff
#define BEGIN 7
#define END 25

typedef struct Data_* Data;
typedef struct Reg_* Reg;

struct Data_ {
	char* name;
	int offset;
	struct Data_* next;
};

struct Reg_ {
	char* name;
};

void make_objectCodes(char* fileName);

#endif