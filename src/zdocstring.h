// zdocstring.h - Defines a docstring list struct that merges duplicate docstrings.
// zdoclist { zdocstring* }
// zdocstring { string type, (string args)* }
//
// [[[WHEN INCLUDING THIS FILE, MAKE SURE TO HAVE *ONE* #include PRECEDED BY
//  * THE FOLLOWING:
//  *     #define ZDOCSTRING_IMPL
//  * This way the actual code can be included into your final binary once.
//  *     #define ZDOCSTRING_IMPL
//  *     #include "zdocstring.h"
//  * Enjoy!
// ]]]
#ifndef ZDOCSTRING_H
	#define ZDOCSTRING_H
	#ifdef __cplusplus
extern "C" {
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>


typedef struct {
	size_t length;
	size_t capacity;
	char const *type;
	char const **argsList;
} DocString;

typedef struct {
	size_t length;
	size_t capacity;
	DocString *docStringTypes;
} DocStringList;

DocString docstring_new(char const *const type, size_t initialCapacity);
DocStringList docstring_list_new(size_t initialCapacity);

int docstring_add(DocString *self, char const *const args);
int docstring_list_add(DocStringList *self, DocString toAdd);
int docstring_list_add_fit(DocStringList *self, char const *const type, char const *const args);

void docstring_clean(DocString *self);
void docstring_list_clean(DocStringList *self);


	#ifdef ZDOCSTRING_IMPL


DocString docstring_new(char const *type, size_t initialCapacity)
{
	return (DocString) {
		.length = 0,
		.capacity = initialCapacity,
		.type = type,
		.argsList = malloc(sizeof(char *) * initialCapacity),
	};
}

DocStringList docstring_list_new(size_t initialCapacity)
{
	return (DocStringList) {
		.length = 0,
		.capacity = initialCapacity,
		.docStringTypes = malloc(sizeof(DocString) * initialCapacity),
	};
}

int docstring_add(DocString *self, char const *const args)
{
	if (++self->length > self->capacity) {
		self->capacity *= 2;

		void *newPtr = realloc((void *) self->argsList, sizeof(char *) * self->capacity);
		if (newPtr == NULL) {
			perror("Failed to realloc docstring arg list array");
			return 1;
		} else {
			self->argsList = newPtr;
		}
	}

	self->argsList[self->length - 1] = args;
	return 0;
}

int docstring_list_add(DocStringList *self, DocString toAdd)
{
	if (++self->length > self->capacity) {
		self->capacity *= 2;

		void *newPtr = realloc((void *) self->docStringTypes, sizeof(DocString) * self->capacity);
		if (newPtr == NULL) {
			perror("Failed to realloc docstring list");
			return 1;
		} else {
			self->docStringTypes = newPtr;
		}
	}

	self->docStringTypes[self->length - 1] = toAdd;
	return 0;
}

int docstring_list_add_fit(DocStringList *self, char const *const type, char const *const args)
{
	ssize_t foundIdx = -1;
	for (size_t i = 0; i < self->length; i++) {
		if (strcmp(self->docStringTypes[i].type, type) == 0) {
			foundIdx = i;
			break;
		}
	}

	if (foundIdx != -1) {
		// throw away the duplicate since it won't be used
		free((void *) type);
		return docstring_add(&self->docStringTypes[foundIdx], args);
	} else {
		DocString toAdd = docstring_new(type, 1);
		if (docstring_add(&toAdd, args) != 0)
			return 1;

		return docstring_list_add(self, toAdd);
	}
}

void docstring_clean(DocString *self)
{
	for (int i = 0; i < self->length; i++)
		free((void *) self->argsList[i]);
	
	free((void *) self->argsList);
	free((void *) self->type);
}

void docstring_list_clean(DocStringList *self)
{
	for (int i = 0; i < self->length; i++)
		docstring_clean(&self->docStringTypes[i]);

	free(self->docStringTypes);
}


	#undef ZDOCSTRING_IMPL
	#endif

	#ifdef __cplusplus
} /* extern "C" */
	#endif
#endif
