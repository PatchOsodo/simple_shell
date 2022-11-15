#include <stdio.h>
#include "shell.h"

void print_prompt1(void)
{
	struct symtab_entry_s *entry = get_symtab_entry("PS1");

	if (entry && entry->val)
	{
		fprintf(stderr, "%s", entry->val);
	}
	else
	{
		fprintf(stderr, "$ ");
	}
}

void print_prompt2(void)
{
	fprintf(stderr, "> ");
}
