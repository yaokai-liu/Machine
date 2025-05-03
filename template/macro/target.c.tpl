${license}

#include "generated/macro/reduce.gen.h"

typedef void *fn_reduce(Token argv[], MacroContext *, const Allocator *);

fn_reduce * const MACRO_PRODUCTS[] = {
  ${assign_reduces}
};
