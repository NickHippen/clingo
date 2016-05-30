// {{{ GPL License

// This file is part of gringo - a grounder for logic programs.
// Copyright Roland Kaminski

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// }}}

#ifndef CLINGO_H
#define CLINGO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// {{{1 errors and warnings

// Note: Errors can be recovered from. If a control function fails, the
// corresponding control object must be destroyed.  An exception is the
// clingo_control_parse function, which does not invalidate the corresponding
// control object (probably this function will become a free function in the
// future).
enum clingo_error {
    clingo_error_success   = 0,
    clingo_error_fatal     = 1,
    clingo_error_runtime   = 2,
    clingo_error_logic     = 3,
    clingo_error_bad_alloc = 4,
    clingo_error_unknown   = 5
};
typedef int clingo_error_t;

enum clingo_warning {
    clingo_warning_operation_undefined = -1, //< Undefined arithmetic operation or weight of aggregate.
    clingo_warning_atom_undefined      = -2, //< Undefined atom in program.
    clingo_warning_file_included       = -3, //< Same file included multiple times.
    clingo_warning_variable_unbounded  = -4, //< CSP Domain undefined.
    clingo_warning_global_variable     = -5, //< Global variable in tuple of aggregate element.
    clingo_warning_total               =  5, //< Total number of warnings.
};
typedef int clingo_warning_t;

// the union of error and warning codes
typedef int clingo_message_code_t;
char const *clingo_message_code_str(clingo_message_code_t code);

typedef void clingo_logger_t(clingo_message_code_t, char const *, void *);

// {{{1 symbol
// {{{2 types

enum clingo_symbol_type {
    clingo_symbol_type_inf = 0,
    clingo_symbol_type_num = 1,
    clingo_symbol_type_str = 4,
    clingo_symbol_type_fun = 5,
    clingo_symbol_type_sup = 7
};
typedef int clingo_symbol_type_t;
typedef struct clingo_symbol {
    uint64_t rep;
} clingo_symbol_t;
typedef struct clingo_symbol_span {
    clingo_symbol_t const *first;
    size_t size;
} clingo_symbol_span_t;
typedef clingo_error_t clingo_string_callback_t (char const *, void *);

// {{{2 construction

void clingo_symbol_new_num(int num, clingo_symbol_t *sym);
void clingo_symbol_new_sup(clingo_symbol_t *sym);
void clingo_symbol_new_inf(clingo_symbol_t *sym);
clingo_error_t clingo_symbol_new_str(char const *str, clingo_symbol_t *sym);
clingo_error_t clingo_symbol_new_id(char const *id, bool sign, clingo_symbol_t *sym);
clingo_error_t clingo_symbol_new_fun(char const *name, clingo_symbol_span_t args, bool sign, clingo_symbol_t *sym);

// {{{2 inspection

clingo_error_t clingo_symbol_num(clingo_symbol_t sym, int *num);
clingo_error_t clingo_symbol_name(clingo_symbol_t sym, char const **name);
clingo_error_t clingo_symbol_string(clingo_symbol_t sym, char const **str);
clingo_error_t clingo_symbol_sign(clingo_symbol_t sym, bool *sign);
clingo_error_t clingo_symbol_args(clingo_symbol_t sym, clingo_symbol_span_t *args);
clingo_symbol_type_t clingo_symbol_type(clingo_symbol_t sym);
clingo_error_t clingo_symbol_to_string(clingo_symbol_t sym, clingo_string_callback_t *cb, void *data);

// {{{2 comparison

size_t clingo_symbol_hash(clingo_symbol_t sym);
bool clingo_symbol_eq(clingo_symbol_t a, clingo_symbol_t b);
bool clingo_symbol_lt(clingo_symbol_t a, clingo_symbol_t b);

// {{{1 symbolic literals

typedef struct clingo_symbolic_literal {
    clingo_symbol_t atom;
    bool sign;
} clingo_symbolic_literal_t;

typedef struct clingo_symbolic_literal_span {
    clingo_symbolic_literal_t const *first;
    size_t size;
} clingo_symbolic_literal_span_t;

// {{{1 part

typedef struct clingo_part {
    char const *name;
    clingo_symbol_span_t params;
} clingo_part_t;
typedef struct clingo_part_span {
    clingo_part_t const *first;
    size_t size;
} clingo_part_span_t;

// {{{1 module (there should only ever be one module)

typedef struct clingo_module clingo_module_t;

clingo_error_t clingo_module_new(clingo_module_t **mod);
void clingo_module_free(clingo_module_t *mod);

// {{{1 model

enum clingo_show_type {
    clingo_show_type_csp   = 1,
    clingo_show_type_shown = 2,
    clingo_show_type_atoms = 4,
    clingo_show_type_terms = 8,
    clingo_show_type_comp  = 16,
    clingo_show_type_all   = 15
};
typedef unsigned clingo_show_type_t;
typedef struct clingo_model clingo_model_t;
bool clingo_model_contains(clingo_model_t *m, clingo_symbol_t atom);
// Note the result is valid until the next call to clingo_model_atoms on the same clingo_model_t object
clingo_error_t clingo_model_atoms(clingo_model_t *m, clingo_show_type_t show, clingo_symbol_span_t *ret);

// {{{1 solve result

enum clingo_solve_result {
    clingo_solve_result_sat         = 1,
    clingo_solve_result_unsat       = 2,
    clingo_solve_result_exhausted   = 4,
    clingo_solve_result_interrupted = 8
};
typedef unsigned clingo_solve_result_t;

// {{{1 solve iter

typedef struct clingo_solve_iter clingo_solve_iter_t;
clingo_error_t clingo_solve_iter_next(clingo_solve_iter_t *it, clingo_model_t **m);
clingo_error_t clingo_solve_iter_get(clingo_solve_iter_t *it, clingo_solve_result_t *ret);
clingo_error_t clingo_solve_iter_close(clingo_solve_iter_t *it);

// {{{1 truth value

enum clingo_truth_value {
    clingo_truth_value_free = 0,
    clingo_truth_value_true = 1,
    clingo_truth_value_false = 2
};
typedef int clingo_truth_value_t;

// {{{1 ast

typedef struct clingo_location {
    char const *begin_file;
    char const *end_file;
    size_t begin_line;
    size_t end_line;
    size_t begin_column;
    size_t end_column;
} clingo_location_t;
typedef struct clingo_ast clingo_ast_t;
typedef struct clingo_ast_span {
    clingo_ast_t const *first;
    size_t size;
} clingo_ast_span_t;
struct clingo_ast {
    clingo_location_t location;
    clingo_symbol_t value;
    clingo_ast_span_t children;
};
typedef clingo_error_t clingo_ast_callback_t (clingo_ast_t const *, void *);
typedef clingo_error_t clingo_add_ast_callback_t (void *, clingo_ast_callback_t *, void *);

// {{{1 propagator

// {{{2 basic

typedef int32_t lit_t;
typedef uint32_t id_t;

// {{{2 signature

typedef struct clingo_signature {
    uint64_t rep;
} clingo_signature_t;

typedef struct clingo_signature_span {
    clingo_signature_t const *first;
    size_t size;
} clingo_signature_span_t;

// {{{2 domain

typedef struct clingo_symbolic_atom_iter {
    uint32_t domain_offset;
    uint32_t atom_offset;
} clingo_symbolic_atom_iter_t;
typedef struct clingo_symbolic_atoms clingo_symbolic_atoms_t;
clingo_symbolic_atom_iter_t clingo_symbolic_atoms_iter(clingo_symbolic_atoms_t *dom, clingo_signature_t *sig);
clingo_symbolic_atom_iter_t clingo_symbolic_atoms_lookup(clingo_symbolic_atoms_t *dom, clingo_symbol_t atom);
// Note the result is valid until the next call to clingo_symbolic_atoms_signatures on the same clingo_symbolic_atoms_t object
clingo_error_t clingo_symbolic_atoms_signatures(clingo_symbolic_atoms_t *dom, clingo_signature_span_t *ret);
size_t clingo_symbolic_atoms_length(clingo_symbolic_atoms_t *dom);
clingo_error_t clingo_symbolic_atoms_atom(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, clingo_symbol_t *sym);
clingo_error_t clingo_symbolic_atoms_literal(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, lit_t *lit);
clingo_error_t clingo_symbolic_atoms_fact(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, bool *fact);
clingo_error_t clingo_symbolic_atoms_external(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, bool *external);
clingo_error_t clingo_symbolic_atoms_next(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, clingo_symbolic_atom_iter *next);
clingo_error_t clingo_symbolic_atoms_valid(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_iter_t atm, bool *valid);

// {{{2 theory

typedef struct clingo_theory_data clingo_theory_data_t;
/*
struct TheoryData {
    enum class TermType { Tuple, List, Set, Function, Number, Symbol };
    enum class AtomType { Head, Body, Directive };

    virtual TermType termType(Id_t) const = 0;
    virtual int termNum(Id_t value) const = 0;
    virtual char const *termName(Id_t value) const = 0;
    virtual Potassco::IdSpan termArgs(Id_t value) const = 0;
    virtual Potassco::IdSpan elemTuple(Id_t value) const = 0;
    // This shall map to ids of literals in aspif format.
    virtual Potassco::LitSpan elemCond(Id_t value) const = 0;
    virtual Lit_t elemCondLit(Id_t value) const = 0;
    virtual Potassco::IdSpan atomElems(Id_t value) const = 0;
    virtual Potassco::Id_t atomTerm(Id_t value) const = 0;
    virtual bool atomHasGuard(Id_t value) const = 0;
    virtual Potassco::Lit_t atomLit(Id_t value) const = 0;
    virtual std::pair<char const *, Id_t> atomGuard(Id_t value) const = 0;
    virtual Potassco::Id_t numAtoms() const = 0;
    virtual std::string termStr(Id_t value) const = 0;
    virtual std::string elemStr(Id_t value) const = 0;
    virtual std::string atomStr(Id_t value) const = 0;
    virtual ~TheoryData() noexcept = default;
};
*/

// {{{2 clauses

enum clingo_clause_type {
    clingo_clause_type_learnt          = 0,
    clingo_clause_type_static          = 1,
    clingo_clause_type_volatile        = 2,
    clingo_clause_type_volatile_static = 3
};
typedef int clingo_clause_type_t;

typedef struct clingo_lit_span {
    lit_t const *first;
    size_t size;
} clingo_lit_span_t;

// {{{2 init

typedef struct clingo_propagate_init clingo_propagate_init_t;
clingo_error_t clingo_propagate_init_map_lit(clingo_propagate_init_t *init, lit_t lit, lit_t *ret);
clingo_error_t clingo_propagate_init_add_watch(clingo_propagate_init_t *init, lit_t lit);
int clingo_propagator_init_threads(clingo_propagate_init_t *init);
clingo_error_t clingo_propagate_init_symbolic_atoms(clingo_propagate_init_t *init, clingo_symbolic_atoms_t **ret);
clingo_error_t clingo_propagate_init_theory_data(clingo_propagate_init_t *init, clingo_theory_data_t **ret);

// {{{2 assignment

typedef struct clingo_assignment clingo_assignment_t;
bool clingo_assignment_has_conflict(clingo_assignment_t *ass);
uint32_t clingo_assignment_decision_level(clingo_assignment_t *ass);
bool clingo_assignment_has_lit(clingo_assignment_t *ass, lit_t lit);
clingo_error_t clingo_assignment_value(clingo_assignment_t *ass, lit_t lit, clingo_truth_value_t *ret);
clingo_error_t clingo_assignment_level(clingo_assignment_t *ass, lit_t lit, uint32_t *ret);
clingo_error_t clingo_assignment_decision(clingo_assignment_t *ass, uint32_t level, lit_t *ret);
clingo_error_t clingo_assignment_is_fixed(clingo_assignment_t *ass, lit_t lit, bool *ret);
clingo_error_t clingo_assignment_is_true(clingo_assignment_t *ass, lit_t lit, bool *ret);
clingo_error_t clingo_assignment_is_false(clingo_assignment_t *ass, lit_t lit, bool *ret);

// {{{2 control

typedef struct clingo_propagate_control clingo_propagate_control_t;
id_t clingo_propagate_control_id(clingo_propagate_control_t *ctl);
clingo_assignment_t *clingo_propagate_control_assignment(clingo_propagate_control_t *ctl);
clingo_error_t add_clause(clingo_propagate_control_t *ctl, clingo_lit_span_t clause, clingo_clause_type_t prop, bool *ret);
clingo_error_t propagate(clingo_propagate_control_t *ctl, bool *ret);

// {{{2 propagator

typedef struct clingo_propagator {
    clingo_error_t (*clingo_propagate_callback_t) (clingo_propagate_control_t *ctl, clingo_lit_span_t changes, void *data);
    clingo_error_t (*clingo_undo_callback_t) (clingo_propagate_control_t *ctl, clingo_lit_span_t changes, void *data);
    clingo_error_t (*clingo_check_callback_t) (clingo_propagate_control_t *ctl, void *data);
    void *data;
} clingo_propagator_t;

// }}}2

// {{{1 control

typedef struct clingo_string_span {
    char const * const *first;
    size_t size;
} clingo_string_span_t;
typedef clingo_error_t clingo_model_handler_t (clingo_model_t*, void *, bool *);
typedef clingo_error_t clingo_symbol_span_callback_t (clingo_symbol_span_t, void *);
typedef clingo_error_t clingo_ground_callback_t (clingo_location_t, char const *, clingo_symbol_span_t, void *, clingo_symbol_span_callback_t *, void *);
typedef struct clingo_control clingo_control_t;
clingo_error_t clingo_control_new(clingo_module_t *mod, clingo_string_span_t args, clingo_logger_t *logger, void *data, unsigned message_limit, clingo_control_t **ctl);
void clingo_control_free(clingo_control_t *ctl);
clingo_error_t clingo_control_add(clingo_control_t *ctl, char const *name, clingo_string_span_t params, char const *part);
clingo_error_t clingo_control_ground(clingo_control_t *ctl, clingo_part_span_t vec, clingo_ground_callback_t *cb, void *data);
clingo_error_t clingo_control_solve(clingo_control_t *ctl, clingo_model_handler_t *mh, void *data, clingo_symbolic_literal_span_t assumptions, clingo_solve_result_t *ret);
clingo_error_t clingo_control_solve_iter(clingo_control_t *ctl, clingo_symbolic_literal_span_t assumptions, clingo_solve_iter_t **it);
clingo_error_t clingo_control_assign_external(clingo_control_t *ctl, clingo_symbol_t atom, clingo_truth_value_t value);
clingo_error_t clingo_control_release_external(clingo_control_t *ctl, clingo_symbol_t atom);
clingo_error_t clingo_control_parse(clingo_control_t *ctl, char const *program, clingo_ast_callback_t *cb, void *data);
clingo_error_t clingo_control_add_ast(clingo_control_t *ctl, clingo_add_ast_callback_t *cb, void *data);
clingo_error_t clingo_control_register_propagator(clingo_control_t *ctl, clingo_propagator_t propagator);

// }}}1

#ifdef __cplusplus
}
#endif

#endif
