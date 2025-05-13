${license}

#ifndef MACHINE_RULES_H
#define MACHINE_RULES_H

#include "err.h"
#include "parse/target.h"
#include "parse/context.h"

typedef void *fn_parse_reduce(Token argv[], ParseContext *, ErrInfo *, const Allocator *);

enum MACHINE_PRODUCT_RULE_ENUM {
  ${enum_reduces}
};

${reduces}

#endif  // MACHINE_RULES_H
