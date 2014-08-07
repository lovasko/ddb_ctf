#include <sys/param.h>
#include <sys/queue.h>
#include <sys/linker.h>
#include <sys/systm.h>

#include <ddb/ddb.h>

#include <ddb/db_lex.h>
#include <ddb/db_output.h>
#include <ddb/db_command.h>
#include <ddb/db_sym.h>
#include <ddb/db_access.h>

/* 
 * Two additional arguments representing the type name. It is not mandatory to 
 * use both arguments, the two-argument situation applies only in case of 
 * 'struct foo', 'union bar', or 'enum baz'.
 */
char token_string[2][TOK_STRING_SIZE];

/* Size of the token strings. */
unsigned int token_string_size[2];

/* 
 * Type name to search for. This string contains the joining of the token 
 * string with a space in-between.
 */
char type_name[TOK_STRING_SIZE*2 + 1];

void
db_hello_cmd(db_expr_t addr, boolean_t have_addr, db_expr_t	count, char* modif)
{
	int t[2];
	unsigned int idx;
	unsigned int i;

	/* set the tokens to default to EOL */
	t[0] = tEOL;
	t[1] = tEOL;

	/* check the presence of an address */
	if (!have_addr) {
		db_printf("Address not supplied.\n");
		return;
	}

	db_printf("Count: %d\n", count);
	db_printf("Modif: '%s'\n", modif);

	/* read one or two arguments */
	for (i = 0; i < 2; i++) {
		t[i] = db_read_token();

		/* in case the token read is an end of line, stop the parsing */
		/* TODO maybe we want to check for iIDENT only */
		if (t[i] == tEOL)
			break;

		/* copy the token string to my memory */
		for (idx = 0; idx < TOK_STRING_SIZE && db_tok_string[idx]; idx++)
			token_string[i][idx] = db_tok_string[idx];

		token_string_size[i] = idx;
	}

	db_skip_to_eol();

	/* check for the presence of the first addition argument */
	if (t[0] == tEOL) {
		db_printf("No type name supplied.");
		return;
	}

	/* clear the type name string */
	for (idx = 0; idx < TOK_STRING_SIZE*2; idx++)
		type_name[idx] = '\0';
	
	/* copy the first additional argument to the final type name */
	for (idx = 0;	idx < token_string_size[0]; idx++)
		type_name[idx] = token_string[0][idx];
	
	/* append a space character */
	type_name[idx] = ' ';
	idx++;

	/* copy the second argument if it exists */
	if (t[1] != tEOL) {
		for (; idx < token_string_size[0] + 1 + token_string_size[1]; idx++)
			type_name[idx] = token_string[1][idx - token_string_size[0] - 1];
	}

	db_printf("Type name = '%s'", type_name);
}

