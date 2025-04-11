#ifndef BLOCK_H
#define BLOCK_H

#include "statement.h"

#include <vector>

#include "../visitor.h"

namespace cantrip::ast {

struct block : public statement
{
    std::vector<statement*> stmts;

    ~block() {
        for (std::size_t i = 0; i < stmts.size(); i++)
            delete stmts[i];
    }

    void accept(visitor* v) override {
        v->visit_block(this);
    }
}; /* class block */

} /* namespace cantrip::ast */


#endif /* end of include guard: BLOCK_H */
