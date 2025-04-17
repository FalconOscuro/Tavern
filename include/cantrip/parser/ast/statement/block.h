#ifndef BLOCK_H
#define BLOCK_H

#include "statement.h"

#include <vector>

#include "../visitor.h"

namespace cantrip::ast {

struct block : public statement
{
    block() = default;
    ~block() = default;

    block(const block&) = delete;
    void operator=(const block&) = delete;

    void accept(visitor* v) override {
        v->visit_block(this);
    }
    
    std::vector<u_statement_ptr> stmts;
}; /* class block */

} /* namespace cantrip::ast */


#endif /* end of include guard: BLOCK_H */
