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

void
db_hello_cmd(db_expr_t addr, boolean_t have_addr, db_expr_t	count, char* modif)
{
	if (have_addr) {
		db_printf("Address: %x\n", addr);	
	} else {
		db_printf("Address not available.\n");
	}

	db_printf("Count: %d\n", count);
	db_printf("Modif: '%s'\n", modif);
}

