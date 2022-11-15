#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../shell.h"
#include "../node.h"
#include "../parser.h"
#include "symtab.h"

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
