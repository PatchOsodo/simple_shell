#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../shell.h"
#include "../node.h"
#include "../parser.h"
#include "symtab.h"

struct symtab_stack_s symtab_stack;
int    symtab_level;
/**
 * init_symtab - initializes our symbol table stack, then allocates memory for,
 *          and initializes, our global symbol table.
 * Return: Null
 */
void init_symtab(void)
{
	symtab_stack.symtab_count = 1;
	symtab_level = 0;

	struct symtab_s *global_symtab = malloc(sizeof(struct symtab_s));

	if (!global_symtab)
	{
		fprintf(stderr, "fatal error: no memory for global symbol table\n");
		exit(EXIT_FAILURE);
	}

	memset(global_symtab, 0, sizeof(struct symtab_s));
	symtab_stack.global_symtab  = global_symtab;
	symtab_stack.local_symtab   = global_symtab;
	symtab_stack.symtab_list[0] = global_symtab;
	global_symtab->level        = 0;
}

/**
 * new_symtab - creates new symbol table
 * @level: 0 for the global symbol table, 1 and above for local symbol tables
 * Return: symtab
 */
struct symtab_s *new_symtab(int level)
{
	struct symtab_stack_s symtab_stack;
	int    symtab_level;

	struct symtab_s *symtab = malloc(sizeof(struct symtab_s));

	if (!symtab)
	{
		fprintf(stderr, "fatal error: no memory for new symbol table\n");
		exit(EXIT_FAILURE);
	}

	memset(symtab, 0, sizeof(struct symtab_s));
	symtab->level = level;
	return (symtab);
}

/**
 * free_symtab - frees the symbol table
 * @symtab: the symtab to be freed
 * Return: NULL if symtab is free
 */
void free_symtab(struct symtab_s *symtab)
{
	if (symtab == NULL)
		return;

	struct symtab_entry_s *entry = symtab->first;

	while (entry)
	{
		if (entry->name)
			free(entry->name);

		if (entry->val)
			free(entry->val);

		if (entry->func_body)
			free_node_tree(entry->func_body);

		struct symtab_entry_s *next = entry->next;

		free(entry);
		entry = next;
	}

	free(symtab);
}

/**
 * dump_local_symtab - prints the contents of the local symbol table
 * Return: NULL
 */
void dump_local_symtab(void)
{
	struct symtab_s *symtab = symtab_stack.local_symtab;
	int i = 0;
	int indent = symtab->level * 4;

	fprintf(stderr, "%*sSymbolTable [Level %d]:\r\n", indent, " ", symtab->level);
	fprintf(stderr, "%*s===========================\r\n", indent, " ");
	fprintf(stderr, "%*s  No          Symbol             Val\r\n", indent, " ");
	fprintf(stderr, "%*s----- ------------------------ -------\r\n", indent, " ");

	struct symtab_entry_s *entry = symtab->first;

	while (entry)
	{
		fprintf(stderr, "%*s[%04d] %-32s '%s'\r\n", indent, " ",
			i++, entry->name, entry->val);
		entry = entry->next;
	}

	fprintf(stderr, "%*s----- ------------------------ -------\r\n", indent, " ");
}

/**
 * add_to_symtab - adds a new entry to the local symbol table
 * @symbol: symbol to be added
 * Return: Entry
 */
struct symtab_entry_s *add_to_symtab(char *symbol)
{
	if (!symbol || symbol[0] == '\0')
		return (NULL);

	struct symtab_s *st = symtab_stack.local_symtab;
	struct symtab_entry_s *entry = NULL;

	if ((entry = do_lookup(symbol, st)))
		return (entry);

	entry = malloc(sizeof(struct symtab_entry_s));

	if (!entry)
	{
		fprintf(stderr, "fatal error: no memory for new symbol table entry\n");
		exit(EXIT_FAILURE);
	}

	memset(entry, 0, sizeof(struct symtab_entry_s));
	entry->name = malloc(strlen(symbol) + 1);

	if (!entry->name)
	{
		fprintf(stderr, "fatal error: no memory for new symbol table entry\n");
		exit(EXIT_FAILURE);
	}

	strcpy(entry->name, symbol);

	if (!st->first)
	{
		st->first      = entry;
		st->last       = entry;
	}
	else
	{
		st->last->next = entry;
		st->last       = entry;
	}

	return (entry);
}

/**
 * rem_from_symtab - removes the symbol table entry whose key
 *             matches the given name
 * @entry: entry
 * @symtab: the symbol table
 * Return: res
 */
int rem_from_symtab(struct symtab_entry_s *entry, struct symtab_s *symtab)
{
	int res = 0;

	if (entry->val)
		free(entry->val);

	if (entry->func_body)
		free_node_tree(entry->func_body);

	free(entry->name);

	if (symtab->first == entry)
	{
		symtab->first = symtab->first->next;

		if (symtab->last == entry)
			symtab->last = NULL;
		res = 1;
	}
	else
	{
		struct symtab_entry_s *e = symtab->first;
		struct symtab_entry_s *p = NULL;

		while (e && e != entry)
		{
			p = e;
			e = e->next;
		}

		if (e == entry)
		{
			p->next = entry->next;
			res = 1;
		}
	}

	free(entry);
	return (res);
}

/**
 * do_lookup - searches the given symbol table, starting with the first entry
 * @str: string to be looked up
 * @symtable: Symbol table
 * Return: NULL or Entry
 */
struct symtab_entry_s *do_lookup(char *str, struct symtab_s *symtable)
{
	if (!str || !symtable)
		return (NULL);

	struct symtab_entry_s *entry = symtable->first;

	while (entry)
	{
		if (strcmp(entry->name, str) == 0)
			return (entry);
		entry = entry->next;
	}

	return (NULL);
}

/**
 * get_symtab_entry - searches for a symbol table entry whose key
 *                     matches the given name
 * @str: String to be searched for
 * Return: NULL or entry
 */
struct symtab_entry_s *get_symtab_entry(char *str)
{
	int i = symtab_stack.symtab_count - 1;

	do {
		struct symtab_s *symtab = symtab_stack.symtab_list[i];
		struct symtab_entry_s *entry = do_lookup(str, symtab);

		if (entry)
			return (entry);
	} while (--i >= 0);

	return (NULL);
}

/**
 * symtab_entry_setval - frees the memory used to store the old entry's value
 * @entry: the entry
 * @val: value of the entry
 * Return: NULL
 */
void symtab_entry_setval(struct symtab_entry_s *entry, char *val)
{
	if (entry->val)
		free(entry->val);

	if (!val)
		entry->val = NULL;
	else
	{
		char *val2 = malloc(strlen(val) + 1);

		if (val2)
			strcpy(val2, val);
		else
			fprintf(stderr, "error: no memory for symbol table entry's value\n");

		entry->val = val2;
	}
}

/**
 * symtab_stack_add - adds the given symbol table to the stack,
 *                and assigns the newly added table as the local symbol table.
 * @symtab: symbol table
 */
void symtab_stack_add(struct symtab_s *symtab)
{
	symtab_stack.symtab_list[symtab_stack.symtab_count++] = symtab;
	symtab_stack.local_symtab = symtab;
}

/**
 * symtab_stack_push - creates a new symbol table and pushes it to top of stack
 * Return: st
 */
struct symtab_s *symtab_stack_push(void)
{
	struct symtab_s *st = new_symtab(++symtab_level);

	symtab_stack_add(st);
	return (st);
}

/**
 * symtab_stack_pop - removes the symbol table on top of the stack
 * Return: NULL or st
 */
struct symtab_s *symtab_stack_pop(void)
{
	if (symtab_stack.symtab_count == 0)
		return (NULL);

	struct symtab_s *st = symtab_stack.symtab_list[symtab_stack.symtab_count - 1];

	symtab_stack.symtab_list[--symtab_stack.symtab_count] = NULL;
	symtab_level--;

	if (symtab_stack.symtab_count == 0)
	{
		symtab_stack.local_symtab  = NULL;
		symtab_stack.global_symtab = NULL;
	}
	else
		symtab_stack.local_symtab = symtab_stack.symtab_list[symtab_stack.symtab_count - 1];

	return (st);
}

/**
 * get_local_symtab - returns pointer to the local symbol table
 * Return: pointer to the local symbol table
 */
struct symtab_s *get_local_symtab(void)
{
	return (symtab_stack.local_symtab);
}

/**
 * get_global_symtab - returns pointer to the global symbol table
 * Return: pointer to the global symbol table
 */
struct symtab_s *get_global_symtab(void)
{
	return (symtab_stack.global_symtab);
}

/**
 * get_symtab_stack - returns a pointer to the symbol table stack
 * Return: symtab_stack
 */
struct symtab_stack_s *get_symtab_stack(void)
{
	return (&symtab_stack);
}
