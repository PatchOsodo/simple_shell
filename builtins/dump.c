#include "../shell.h"
#include "../symtab/symtab.h"
/**
 * dump - prints the contents of the local symbol table.
 * @argc: argument counter
 * @argv: argument vector
 * Return: 0
 */
int dump(int argc, char **argv)
{
	dump_local_symtab();
	return (0);
}
