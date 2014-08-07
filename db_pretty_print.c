/*-
 * Copyright (c) 2014 Daniel Lovasko
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

static void
db_pretty_print()
{
	linker_ctf_t kernel_ctf_data;

	if (!linker_ctf_get(linker_kernel_file, &kernel_ctf_data)) {
		db_printf("ERROR: Unable to load the kernel CTF data.\n");
		return;
	} else {
		db_printf("%d %d %d\n", kernel_ctf_data.ctfcnt, kernel_ctf_data.nsym, 
			kernel_ctf_data.strcnt);
	}
}

void
db_pretty_print_cmd(db_expr_t addr, boolean_t have_addr, db_expr_t count, 
	char* modif)
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
	

	/* copy the second argument if it exists */
	if (t[1] != tEOL) {
		/* append a space character */
		type_name[idx] = ' ';
		idx++;

		for (; idx < token_string_size[0] + 1 + token_string_size[1]; idx++)
			type_name[idx] = token_string[1][idx - token_string_size[0] - 1];
	}

	db_printf("Type name = '%s'", type_name);
	db_pretty_print();
}

