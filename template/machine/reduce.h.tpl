${license}

#ifndef MACHINE_REDUCES_H
#define MACHINE_REDUCES_H

#include "parse/target.h"
#include "parse/context.h"

typedef void *fn_reduce(Token argv[], ParseContext *, const Allocator *);

enum __MACHINE_PRODUCT_RULE_ENUM__ {
  ${enum_reduces}
};

${reduces}

#endif  // MACHINE_REDUCES_H
