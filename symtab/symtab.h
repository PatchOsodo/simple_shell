#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "../node.h"

#define MAX_SYMTAB	256

/* the type of a symbol table entry's value */
/**
 * enum symbol_type_e - the type of a symbol table entry's value
 * @SYM_STR: Symbol string
 * @SYM_FUNC: Symbol function
 *
 * Description: The type description of a symbol table entry's value
 */
enum symbol_type_e
{
	SYM_STR,
	SYM_FUNC,
};

/* the symbol table entry structure */
/**
 * struct symtab_entry_s - sym tab entry structure
 * @name: name of the shell variable
 * @val_type: SYM_STR for shell variables, SYM_FUNC for shell functions
 * @val: string value
 * @flags: indicates the different properties we'll assign to variables
 *         and functions
 * @next: pointer to the next symbol table entry
 * @func_body: the Abstract Syntax Tree of the function body
 *
 * Description:  represents our symbol table entries.
*/
struct symtab_entry_s
{
	char     *name;
	enum      symbol_type_e val_type;
	char     *val;
	unsigned  int flags;
	struct    symtab_entry_s *next;
	struct    node_s *func_body;
};

/* the symbol table structure */
/**
 * struct symtab_s - represents a single symbol table
 * @level: 0 for the global symbol table, 1 and above for local symbol tables
 * @first: pointer to the first entry in the table's linked list
 * @last: pointer to the last entry in the table's linked list
 *
 * Description: structure represents a single symbol table. For starters,
 *              we'll use one symbol table, in which we'll define all our shell
 *               variables
 */
struct symtab_s
{
	int    level;
	struct symtab_entry_s *first, *last;
};

/* values for the flags field of struct symtab_entry_s */
#define FLAG_EXPORT (1 << 0) /* export entry to forked commands */

/* the symbol table stack structure */
/**
 * struct symtab_stack_s - structure represents our symbol table stack
 * @symtab_count: the number of symbol tables currently on the stack
 * @symtab_list: an array of pointers to the stack's symbol tables.
 * @global_symtab: pointers to the global symbol tables
 * @local_symtab: pointers to the local symbol tables
 *
 * Description: A stack is a Last-In-First-Out, or LIFO, data structure,
 *           in which the last item added (or pushed) is the first item removed
 *           (or popped). The struct symtab_stack_s structure represents our
 *           symbol table stack
 */
struct symtab_stack_s
{
	int    symtab_count;
	struct symtab_s *symtab_list[MAX_SYMTAB];
	struct symtab_s *global_symtab, *local_symtab;
};


struct symtab_s       *new_symtab(int level);
struct symtab_s       *symtab_stack_push(void);
struct symtab_s       *symtab_stack_pop(void);
int rem_from_symtab(struct symtab_entry_s *entry, struct symtab_s *symtab);
struct symtab_entry_s *add_to_symtab(char *symbol);
struct symtab_entry_s *do_lookup(char *str, struct symtab_s *symtable);
struct symtab_entry_s *get_symtab_entry(char *str);
struct symtab_s       *get_local_symtab(void);
struct symtab_s       *get_global_symtab(void);
struct symtab_stack_s *get_symtab_stack(void);
void init_symtab(void);
void dump_local_symtab(void);
void free_symtab(struct symtab_s *symtab);
void symtab_entry_setval(struct symtab_entry_s *entry, char *val);

#endif
