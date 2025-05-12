${license}

#ifndef MACRO_RULES_H
#define MACRO_RULES_H

#include "tokenize/target.h"
#include "tokenize/context.h"

typedef void *fn_macro_reduce(Token argv[], MacroContext *, const Allocator *);

enum MACRO_PRODUCT_RULE_ENUM {
  ${enum_reduces}
};

${reduces}

#endif  // MACRO_RULES_H
