#include "geysa.h"
#include "pattern.h"
#include "pdb.h"
#include "if1.h"
#include "builtin.h"
#include "fun.h"
#include "pnode.h"
#include "fa.h"
#include "var.h"
#include "ast.h"

// Key to names of position variables
//  ABCD 
//   A = a | f, actual or formal (MANDITORY)
//   B = c, cannonical (OPTIONAL) 
//   C = p, positional (OPTIONAL) 
//   D = p, position (MANDITORY) 
// Examples:
//  acpp - actual, cannonical, positional position
//  acnp - actual, cannonical, named position
//  ap - actual non-cannonical, positional or named position

typedef Map<Fun *, Match *> MatchMap;

class Matcher {
 public:
  AVar *send;
  AVar *arg0;
  Partial_kind partial;
  Vec<Match *> *matches;
  MatchMap match_map;
  Vec<Fun *> *all_matches, *partial_matches;
  Vec<MPosition *> *all_positions;
  Map<MPosition *, int> mapped_positions;

  // C++'s manditory redundant declarations
  int pattern_match_sym(Sym *, MPosition *, Vec<Fun *> *, Vec<Fun *> &, int, Vec<Sym *> &);
  void build_positional_map(MPosition &, int, Vec<Fun *> **);
  void update_match_map(AVar *, CreationSet *, MPosition *, MPosition *, Vec<Fun *> &);
  void find_arg_matches(AVar *, MPosition &, MPosition *, MPosition *, Vec<Fun *> **, int, int);
  void find_all_matches(CreationSet *, Vec<AVar *> &, Vec<Fun *> **, MPosition &, int);
  void record_all_positions();
  void find_best_cs_match(Vec<CreationSet *> &, MPosition &p, Vec<Fun *> &, Vec<Fun *> &, int);
  void find_best_matches(Vec<AVar *> &, Vec<CreationSet *> &, Vec<Fun *> &, MPosition &, 
			 Vec<Fun *> &, int, int iarg = 0);
  int covers_formals(Fun *, Vec<CreationSet *> &, MPosition &, int);
  void build_Match(Fun *, Match *);
  void instantiation_wrappers_and_partial_application(Vec<Fun *> &);
  Fun *build(Match *m, Vec<Fun *> &matches);
  void generic_arguments(Vec<CreationSet *> &, MPosition &, Vec<Fun *> &, int);
  void set_filters(Vec<CreationSet *> &, MPosition &, Vec<Fun *> &);
  void cannonicalize_matches();

  Matcher(AVar *send, AVar *arg0, Partial_kind partial, Vec<Match *> *matches);
};

static ChainHash<MPosition *, MPositionHashFuns> cannonical_mposition;

static Sym *
dispatch_type(Sym *a) {
  if (is_Sym_OUT(a)) 
    return sym_unknown;
  if (a->is_symbol)
    return a;
  if (a->type && a->type->is_symbol)
    return a->type;
  if (a->must_specialize && a->must_specialize->type_kind != Type_VARIABLE)
    return a->must_specialize;
  return sym_any;
}

void
MPosition::copy(MPosition &p) {
  pos.copy(p.pos);
  parent = p.parent;
}

MPosition::MPosition(MPosition &p) {
  copy(p);
}

MPosition *
cannonicalize_mposition(MPosition &p) {
  MPosition *cp = cannonical_mposition.get(&p);
  if (cp)
    return cp;
  cp = new MPosition;
  cp->pos.copy(p.pos);
  cannonical_mposition.put(cp);
  if (cp->pos.n > 1) {
    MPosition pp(*cp);
    pp.pos.n--;
    cp->parent = cannonicalize_mposition(pp);
  }
  return cp;
}

static MPosition *
to_formal(MPosition *acp, Match *m) {
  MPosition *fcp = m->actual_to_formal_position.get(acp);
  MPosition *result = fcp ? fcp : acp;
  return result;
}

static MPosition *
to_actual(MPosition *fcp, Match *m) {
  MPosition *acp = m->formal_to_actual_position.get(fcp);
  MPosition *result = acp ? acp : fcp;
  return result;
}

static MPosition *
cannonicalize_to_formal(MPosition &ap, Match *m) {
  return to_formal(cannonical_mposition.get(&ap), m);
}

// Given type, cp, partial_matches, return funs, using done
int 
Matcher::pattern_match_sym(Sym *type, MPosition *acp, Vec<Fun *> *local_matches,
			   Vec<Fun *> &funs, int out_of_position, Vec<Sym *> &done) 
{
  if (!out_of_position) {	// actual == formal
    MPosition *fcp = acp;
    int found = 0;
    if (type->match_type) {
      Vec<Fun *> *fs = type->match_type->funs.get(fcp);
      if (fs) {
	Vec<Fun *> ffs;
	if (local_matches) {
	  local_matches->set_intersection(*fs, ffs);
	  fs = &ffs;
	}
	found = 1;
	funs.set_union(*fs);
      }
    }
    forv_Sym(tt, type->dispatch_order)
      if (done.set_add(tt))
	found = pattern_match_sym(tt, acp, local_matches, funs, 0, done) || found;
    return found;
  } else {			// actual != formal
    int found = 0;
    // for all the local possible matches, if the type is permitted at the argument
    Vec<Match *> matches;
    Vec<MPosition *> positions;
    forv_Fun(f, *local_matches) if (f) {
      Match *m = match_map.get(f);
      MPosition *fcp = m->actual_to_formal_position.get(acp);
      Sym *m_type = dispatch_type(m->fun->arg_syms.get(fcp));
      if (m_type->specializers.set_in(type)) {
	funs.set_add(m->fun);
	found = 1;
      }
    }
    return found;
  }
}

Matcher::Matcher(AVar *asend, AVar *aarg0, Partial_kind apartial, Vec<Match *> *amatches) {
  send = asend;
  arg0 = aarg0;
  partial = apartial;
  matches = amatches;
  matches->clear();
  all_matches = 0;
  
  // use summary information from previous analysis iterations to restrict the search
  if (send->var->def->callees) {
    all_matches = new Vec<Fun *>(send->var->def->callees->funs);
    all_positions = &send->var->def->callees->arg_positions;
  } else {
    if (aarg0->out->n == 1)
      all_matches = send->var->def->code->ast->visible_functions(aarg0->out->v[0]->sym);
    else {
      forv_CreationSet(cs, *aarg0->out) {
	Vec<Fun *> *v = send->var->def->code->ast->visible_functions(cs->sym);
	if (v) {
	  if (!all_matches)
	    all_matches = new Vec<Fun *>;
	  all_matches->set_union(*v);
	}
      }
    }
    all_positions = 0;
  }
  if (all_matches)
    partial_matches = new Vec<Fun *>(*all_matches);
  else
    partial_matches = 0;
}

void
Matcher::update_match_map(AVar *a, CreationSet *cs, MPosition *acp, MPosition *acpp,
			  Vec<Fun *> &new_matches) 
{
  forv_Fun(f, new_matches) if (f) {
    Match *m = match_map.get(f);
    if (!m) {
      m = new Match(f);
      match_map.put(f, m); 
    }
    AType *t = m->actual_filters.get(acp);
    if (!t) {
      t = new AType; 
      m->actual_filters.put(acp, t);
      if (acp != acpp)
	m->actual_named_to_positional.put(acp, acpp);
    }
    t->set_add(cs);
    m->actuals.put(acpp, a);
  }
}

int
positional_to_named(CreationSet *cs, AVar *av, MPosition &pp, MPosition *np) {
  if ((!cs || cs->sym == sym_tuple) && av->var->sym->arg_name) {
    np->copy(pp);
    np->set_top(av->var->sym->arg_name);
    return 1;
  }
  return 0;
}

void
Matcher::find_arg_matches(AVar *a, MPosition &ap, MPosition *acp, MPosition *acpp, 
			  Vec<Fun *> **local_matches, int recurse, int out_of_position)
{
  a->arg_of_send.set_add(send);
  if (!all_positions || all_positions->set_in(acp)) {
    Vec<Fun *> funs;
    if (!a->out->n) {
      Vec<Fun *> new_funs;
      Vec<Sym *> done;
      pattern_match_sym(sym_unknown, acp, *local_matches, new_funs, out_of_position, done);
      update_match_map(a, sym_unknown->abstract_type->v[0], acp, acpp, new_funs);
      funs.set_union(new_funs);
    } else {
      forv_CreationSet(cs, *a->out) if (cs) {
	Sym *sym = cs->sym;
	if (a->var->sym->aspect) {
	  if (!a->var->sym->aspect->implementors.in(cs->sym))
	    continue;
	  sym = a->var->sym->aspect;
	}
	Vec<Sym *> done;
	Vec<Fun *> new_funs, *new_funs_p = &new_funs;
	if (!pattern_match_sym(sym, acp, *local_matches, new_funs, out_of_position, done))
	  continue;
	update_match_map(a, cs, acp, acpp, new_funs);
	if (recurse && cs->vars.n) // for recursive ap == app !!
	  find_all_matches(cs, cs->vars, &new_funs_p, ap, out_of_position);
	funs.set_union(new_funs);
      }
    }
    if (!*local_matches) {
      *local_matches = new Vec<Fun *>(funs);
    } else {
      Vec<Fun *> saved_matches;
      saved_matches.move(**local_matches);
      forv_Fun(f, saved_matches) if (f) {
	if (funs.set_in(f))
	  (*local_matches)->set_add(f);
	else if (acpp->parent && !f->arg_syms.get(to_formal(acpp->parent, match_map.get(f)))->is_pattern)
	  (*local_matches)->set_add(f); 
      }
      (*local_matches)->set_union(funs);
    }
  }
}

int
compar_last_position(const void *ai, const void *aj) {
  MPosition *i = *(MPosition**)ai;
  MPosition *j = *(MPosition**)aj;
  if (i->pos.n > j->pos.n)
    return 1;
  else if (i->pos.n < j->pos.n)
    return -1;
  int ii = Position2int(i->pos.v[i->pos.n-1]);
  int jj = Position2int(j->pos.v[j->pos.n-1]);
  return (ii > jj) ? 1 : ((ii < jj) ? -1 : 0);
}

void
Matcher::build_positional_map(MPosition &app, int nactuals, Vec<Fun *> **funs) {
  if (*funs) {
    forv_Fun(f, **funs) if (f) {
      Match *m = match_map.get(f);
      Vec<MPosition *> fcpps_for_fcnps, acpps_for_acnps, unused_acpps, unused_fcpps;
      // map and collect named argument positions (at this level)
      for (int i = 0; i < m->actual_filters.n; i++) {
	MPosition *acp = m->actual_filters.v[i].key;
	if (acp && !acp->last_is_positional() && acp->prefix_to_last(app)) {
	  MPosition *acnp = acp;
	  MPosition fnp;
	  if (acp->parent)
	    fnp.copy(*to_formal(acp->parent, m));
	  fnp.push(acp->last());
	  MPosition *fcnp = cannonicalize_mposition(fnp);
	  MPosition *acpp = m->actual_named_to_positional.get(acnp);
	  MPosition *fcpp = f->named_to_positional.get(fcnp);
	  acpps_for_acnps.set_add(acpp);
	  fcpps_for_fcnps.set_add(fcpp);
	  m->actual_to_formal_position.put(acpp, fcpp);
	  m->formal_to_actual_position.put(fcpp, acpp);
	  if (acpp != fcpp)
	    mapped_positions.put(acpp, 1);
	}
      }
      app.push(1);
      // collect actual positions not used by named arguments (at this level)
      for (int i = 0; i < nactuals; i++) {
	MPosition *acpp = cannonicalize_mposition(app);
	if (!acpps_for_acnps.set_in(acpp))
	  unused_acpps.add(acpp);
	app.inc();
      }
      app.pop();
      // sort unused positions
      qsort(unused_acpps.v, unused_acpps.n, sizeof(void*), compar_last_position);
      // build a maps for actual positionals <-> formal positionals
      MPosition fpp(*cannonicalize_to_formal(app, m));
      fpp.push(1);
      for (int i = 0; i < unused_acpps.n; i++) {
	MPosition *fcpp = cannonicalize_mposition(fpp);
	if (fcpps_for_fcnps.set_in(fcpp)) {
	  fpp.inc();
	  continue;
	}
	m->actual_to_formal_position.put(unused_acpps.v[i], fcpp);
	m->formal_to_actual_position.put(fcpp, unused_acpps.v[i]);
	if (unused_acpps.v[i] != fcpp)
	  mapped_positions.put(unused_acpps.v[i], 1);
	fpp.inc();
      }
    }
  }
}

void
Matcher::find_all_matches(CreationSet *cs, Vec<AVar *> &args, Vec<Fun *> **funs, 
			  MPosition &app, int out_of_position) 
{
  // determine named arguments
  int some_named = 0;
  app.push(1);
  forv_AVar(av, args) {
    MPosition anp;
    if (positional_to_named(cs, av, app, &anp)) {
      MPosition *acpp = cannonicalize_mposition(app);
      if (acpp) {
	MPosition *acnp = cannonicalize_mposition(anp);
	find_arg_matches(av, anp, acnp, acpp, funs, 0, out_of_position);
	some_named = 1;
      }
    }
    app.inc();
  }
  // build positional map
  app.pop();
  if (some_named)
    build_positional_map(app, args.n, funs);
  // match all arguments
  app.push(1);
  forv_AVar(av, args) {
    MPosition *acpp = cannonicalize_mposition(app);
    if (acpp) {
      int local_out_of_position = out_of_position;
      if (!local_out_of_position) {
	if (mapped_positions.get(acpp))
	  local_out_of_position = 1;
      }
      find_arg_matches(av, app, acpp, acpp, funs, 1, local_out_of_position);
    }
    app.inc();
  }
  app.pop();
}

int
subsumes(Match *x, Match *y, MPosition &app, int nargs) {
  int result = 0;
  app.push(1);
  for (int i = 0; i < nargs; i++) {
    MPosition *acpp = cannonicalize_mposition(app);
    Sym *xtype = x->formal_dispatch_types.get(to_formal(acpp, x));
    Sym *ytype = y->formal_dispatch_types.get(to_formal(acpp, y));
    if (xtype == ytype)
      goto Lnext;
    if (xtype->specializers.set_in(ytype)) {
      if (result >= 0)
	result = 1;
      else {
	result = 0;
	goto Lreturn;
      }
    } else if (ytype->specializers.set_in(xtype)) {
      if (result <= 0)
	result = -1;
      else {
	result = 0;
	goto Lreturn;
      }
    }
  Lnext:
    app.inc();
  }
 Lreturn:
  app.pop();
  return result;
}

void
Matcher::set_filters(Vec<CreationSet *> &csargs, MPosition &app, Vec<Fun *> &matches) {
  forv_Fun(f, matches) {
    Match *m = match_map.get(f);
    app.push(1);
    for (int i = 0; i < csargs.n; i++) {
      MPosition *fcpp = cannonicalize_to_formal(app, m);
      AType *t = m->formal_filters.get(fcpp);
      if (!t) {
	t = new AType; 
	m->formal_filters.put(fcpp, t);
      }
      t->set_add(csargs.v[i]);
      app.inc();
    }
    app.pop();
  }
}

void
Matcher::build_Match(Fun *f, Match *m) {
  if (match_map.get(f))
    return;
  Match *mm = new Match(f);
  match_map.put(f, mm);
  // actuals
  form_MPositionAVar(x, m->actuals)
    mm->actuals.put(to_actual(x->key, m), x->value);
  mm->partial = m->partial;
}

template <class C>
class MapCacheHashFns  {
 public:
  static unsigned int hash(C a) {
    unsigned int h = 0, x = 0;
    for (int i = 0; i < a->n; i++) if (a->v[i].key) {
      h += open_hash_multipliers[x++ % 256] * (uintptr_t)a->v[i].key;
      h += open_hash_multipliers[x++ % 256] * (uintptr_t)a->v[i].value;
    }
    return h;
  }
  static int equal(C a, C b) {
    for (int i = 0; i < a->n; i++)
      if (a->v[i].key && b->get(a->v[i].key) != a->v[i].value)
	return 0;
    for (int i = 0; i < b->n; i++)
      if (b->v[i].key && a->get(b->v[i].key) != b->v[i].value)
	return 0;
    return 1;
  }
};
typedef Map<MPosition *, Sym*> CoercionMap;
typedef Map<Sym *, Sym*> GenericMap;
typedef Map<MPosition *, MPosition*> OrderMap;
class CoercionCache { public: HashMap<CoercionMap *, MapCacheHashFns<CoercionMap *>, Fun *> cache; };
class GenericCache { public: HashMap<GenericMap *, MapCacheHashFns<GenericMap *>, Fun *> cache; };
class OrderCache { public: HashMap<OrderMap *, MapCacheHashFns<OrderMap *>, Fun *> cache; };

class DefaultCacheHashFns  {
 public:
  static unsigned int hash(Vec<MPosition*> *a) {
    unsigned int h = 0, x = 0;
    forv_MPosition(p, *a)
      h += open_hash_multipliers[x++ % 256] * (uintptr_t)a;
    return h;
  }
  static int equal(Vec<MPosition *> *a,  Vec<MPosition *> *b) {
    if (a->n != b->n)
      return 0;
    for (int i = 0; i < a->n; i++)
      if (a->v[i] != b->v[i])
	return 0;
    return 1;
  }
};
class DefaultCache { public: HashMap<Vec<MPosition*> *, DefaultCacheHashFns, Fun *> cache; };

Fun *
Matcher::build(Match *m, Vec<Fun *> &matches) {
  Fun *f = m->fun;
  if (m->generic_substitutions.n) {
    GenericCache *c = f->generic_cache;
    if (!c) c = f->generic_cache = new GenericCache;
    if (!(f = c->cache.get(&m->generic_substitutions))) {
      f = if1->callback->instantiate_generic(m);
      c->cache.put(&m->generic_substitutions, f);
    }
  }
  if (m->default_args.n) {
    qsort(m->default_args.v, m->default_args.n, sizeof(void*), compar_last_position);
    DefaultCache *c = f->default_cache;
    if (!c) c = f->default_cache = new DefaultCache;
    if (!(f = c->cache.get(&m->default_args))) {
      f = if1->callback->default_wrapper(m);
      c->cache.put(&m->default_args, f);
    }
  }
  if (m->coercion_substitutions.n) {
    CoercionCache *c = f->coercion_cache;
    if (!c) c = f->coercion_cache = new CoercionCache;
    if (!(f = c->cache.get(&m->coercion_substitutions))) {
      f = if1->callback->coercion_wrapper(m);
      c->cache.put(&m->coercion_substitutions, f);
    }
  }
  int order_change = 0;
  form_MPositionMPosition(p, m->formal_to_actual_position) {
    if (p->key->is_positional() && p->value->is_positional()) {
      if (p->key != p->value)
	order_change = 1;
      m->order_substitutions.put(p->key, p->value);
    }
  }
  if (order_change) {
    OrderCache *c = f->order_cache;
    if (!c) c = f->order_cache = new OrderCache;
    if (!(f = c->cache.get(&m->order_substitutions))) {
      f = if1->callback->order_wrapper(m);
      c->cache.put(&m->order_substitutions, f);
    }
  } else
    m->order_substitutions.clear();
  if (f != m->fun)
    build_Match(f, m);
  return f;
}

void 
Matcher::instantiation_wrappers_and_partial_application(Vec<Fun *> &matches) {
  Vec<Fun *> new_matches, complete;
  forv_Fun(f, matches) {
    Match *m = match_map.get(f);
    f = build(m, matches);
    if (!m->partial)
      complete.add(f);
    new_matches.add(f);
  }
  if (complete.n > 1)
    type_violation(ATypeViolation_DISPATCH_AMBIGUITY, arg0, arg0->out, send, 
		   new Vec<Fun *>(complete));
  matches.move(new_matches);
}

int
Matcher::covers_formals(Fun *f, Vec<CreationSet *> &csargs, MPosition &p, int top_level) {
  int result = 1;
  Match *m = match_map.get(f);
  Vec<MPosition *> formals;
  p.push(1);
  for (int i = 0; i < csargs.n; i++) {
    MPosition *cp = cannonicalize_to_formal(p, m);
    formals.set_add(cp);
    p.inc();
  }
  p.pop();
  for (int i = 0; i < f->arg_positions.n; i++) {
    MPosition *pp = f->arg_positions.v[i];
    if (pp->prefix_to_last(p) && pp->last_is_positional()) {
      if (formals.set_in(pp))
	continue;
      if (f->default_args.get(pp)) {
	m->default_args.add(pp);
	continue;
      }
      if (top_level && partial != Partial_NEVER)
	m->partial = 1;
      else
	result = 0;
    }
  }
  return result;
}

static int 
is_generic_type(Sym *t) {
  return t->type_kind == Type_VARIABLE;
}

static int
unify_generic_type(Sym *gtype, Sym *type, Map<Sym *, Sym *> &substitutions) {
  int result = 1;
  switch (gtype->type_kind) {
    case Type_LUB: 
    case Type_GLB: 
    case Type_PRODUCT:
    case Type_RECORD:
    case Type_VECTOR:
    case Type_FUN:
    case Type_REF:
    case Type_TAGGED:
    case Type_PRIMITIVE:
    case Type_APPLICATION:
      return gtype == type;
    case Type_VARIABLE:
      substitutions.put(gtype, type);
      break;
    case Type_NONE: 
    case Type_UNKNOWN: 
    case Type_ALIAS:
      assert(!"bad case");
      return 0;
  }
  return result;
}

static void
instantiate_formal_types(Match *m) {
  form_MPositionSym(x, m->formal_dispatch_types) {
    Sym *type = m->fun->arg_syms.get(x->key)->must_specialize;
    if (is_generic_type(type))
      x->value = if1->callback->instantiate(type, m->generic_substitutions);
  }
}

static int
generic_substitutions(Match **am, MPosition &app, Vec<CreationSet*> args) {
  MPosition local_app(app);
  Match *m = *am;
  local_app.push(1);
  for (int i = 0; i < args.n; i++) {
    MPosition *acpp = cannonicalize_mposition(local_app);
    MPosition *fcpp = to_formal(acpp, m);
    AVar *a = m->actuals.get(acpp);
    CreationSet *cs = args.v[i];
    Sym *concrete_type = a->var->sym->aspect ? a->var->sym->aspect : cs->sym;
    Sym *type = m->fun->arg_syms.get(fcpp)->must_specialize;
    if (type && is_generic_type(type)) {
      if (!unify_generic_type(type, concrete_type, m->generic_substitutions))
	return 0;
      instantiate_formal_types(m);
    }
    local_app.inc();
  }
  return 1;
}

static void
coercion_uses(Match **am, MPosition &app, Vec<CreationSet*> args) {
  Match *m = *am;
  app.push(1);
  for (int i = 0; i < args.n; i++) {
    MPosition *acpp = cannonicalize_mposition(app);
    MPosition *fcpp = to_formal(acpp, m);
    AVar *a = m->actuals.get(acpp);
    CreationSet *cs = args.v[i];
    Sym *concrete_type = a->var->sym->aspect ? a->var->sym->aspect : cs->sym;
    concrete_type = concrete_type->type;
    Sym *formal_type = m->formal_dispatch_types.get(fcpp);
    Sym *coerced_type = concrete_type->coerce_to(formal_type) ;
    if (coerced_type && concrete_type != coerced_type) {
      m->coercion_substitutions.put(fcpp, concrete_type);
      m->formal_dispatch_types.put(fcpp, coerced_type);
    }
    app.inc();
  }
  app.pop();
}

void
Matcher::find_best_cs_match(Vec<CreationSet *> &csargs, MPosition &app, 
			    Vec<Fun *> &local_matches, 
			    Vec<Fun *> &result, int top_level)
{
  Vec<Match *> covered;
  // collect the matches which cover the argument CreationSets
  forv_Fun(f, local_matches) if (f) {
    Match *m = match_map.get(f);
    app.push(1);
    for (int i = 0; i < csargs.n; i++) {
      MPosition *acpp = cannonicalize_mposition(app);
      MPosition *fcpp = to_formal(acpp, m);
      AType *t = m->actual_filters.get(acpp);
      // is each actual handled
      if (!t || !t->set_in(csargs.v[i])) {
	app.pop();
	goto LnextFun;
      }
      Sym *formal = m->fun->arg_syms.get(fcpp);
      Sym *formal_type = dispatch_type(formal);
      m->formal_dispatch_types.put(fcpp, formal_type);
      app.inc();
    }
    app.pop();
    // do they cover all the formals
    if (covers_formals(f, csargs, app, top_level))
      covered.set_add(m);
  LnextFun:;
  }
  if (!covered.n)
    return;
 Vec<Match *> applicable;
  // record generic substitutions and point-wise applications
  for (int i = 0; i < covered.n; i++) {
    if (!generic_substitutions(&covered.v[i], app, csargs))
      continue;
    coercion_uses(&covered.v[i], app, csargs);
    applicable.add(covered.v[i]);
  }
  Vec<Match *> unsubsumed, subsumed;
  // eliminate those which are subsumed by some other function
  for (int i = 0; i < applicable.n; i++) if (applicable.v[i]) {
    if (subsumed.set_in(applicable.v[i]))
      continue;
    for (int j = i + 1; j < applicable.n; j++) if (applicable.v[j]) {
      switch (subsumes(applicable.v[i], applicable.v[j], app, csargs.n)) {
	case -1: subsumed.set_add(applicable.v[j]); break;
	case 0: break;
	case 1: goto LnextApplicable;
      }
    }
    unsubsumed.add(applicable.v[i]);
  LnextApplicable:;
  }
  // for those remaining, check for ambiguities
  Vec<Fun *> matches, similar, done;
  for (int i = 0; i < unsubsumed.n; i++) {
    Match *x = unsubsumed.v[i];
    if (done.set_in(x->fun))
      continue;
    similar.clear(); similar.add(x->fun);
    for (int j = i + 1; j < unsubsumed.n; j++) {
      Match *y = unsubsumed.v[j];
      if (done.set_in(y->fun))
	continue;
      app.push(1);
      for (int k = 0; k < csargs.n; k++) {
	Sym *xarg = x->fun->arg_syms.get(cannonicalize_to_formal(app, x));
	Sym *yarg = y->fun->arg_syms.get(cannonicalize_to_formal(app, y));
	Sym *xtype = dispatch_type(xarg);
	Sym *ytype = dispatch_type(yarg);
	if (xtype != ytype || xarg->is_pattern != yarg->is_pattern)
	  goto LnextUnsubsumed;
	app.inc();
      }
      similar.add(y->fun); done.set_add(y->fun);
    LnextUnsubsumed:;
      app.pop();
    }
    // for similar functions recurse for patterns to check for subsumption and ambiguities
    app.push(1);
    for (int i = 0; i < csargs.n; i++) {
      Sym *arg = similar.v[0]->arg_syms.get(cannonicalize_to_formal(app, match_map.get(similar.v[0])));
      if (arg->is_pattern) {
	Vec<CreationSet *> local_csargs;
	Vec<Fun *> local_result;
	find_best_matches(csargs.v[i]->vars, local_csargs, similar, app, local_result, 0);
	similar.copy(local_result);
      }
      app.inc();
    }
    app.pop();
    matches.append(similar);
  }
  if (top_level)
    instantiation_wrappers_and_partial_application(matches);
  set_filters(csargs, app, matches);
  result.set_union(matches);
}

void
Matcher::find_best_matches(Vec<AVar *> &args, Vec<CreationSet *> &csargs, 
			   Vec<Fun *> &matches, MPosition &app, 
			   Vec<Fun *> &result, 
			   int top_level,
			   int iarg) 
{
  if (iarg >= args.n)
    find_best_cs_match(csargs, app, matches, result, top_level);
  else {
    csargs.fill(iarg + 1);
    if (!args.v[iarg]->out->n) {
      csargs.v[iarg] = sym_unknown->abstract_type->v[0];
      find_best_matches(args, csargs, matches, app, result, top_level, iarg + 1);
    } else {
      forv_CreationSet(cs, *args.v[iarg]->out) if (cs) {
	csargs.v[iarg] = cs;
	find_best_matches(args, csargs, matches, app, result, top_level, iarg + 1);
      }
    }
  }
}

void
Matcher::record_all_positions() {
  if (!all_positions) {
    all_positions = new Vec<MPosition *>;
    forv_Fun(f, *partial_matches) if (f)
      all_positions->set_union(f->arg_positions);
  }
}

void
Matcher::cannonicalize_matches() {
  forv_Fun(f, *partial_matches) if (f) {
    Match *m = match_map.get(f);
    for (int i = 0; i < m->formal_filters.n; i++)
      if (m->formal_filters.v[i].key)
	m->formal_filters.v[i].value = make_AType(*m->formal_filters.v[i].value);
    matches->add(m);
  }
}

//
// main dispatch entry point - given a vector of arguments return a vector of matches
//
int
pattern_match(Vec<AVar *> &args, AVar *send, Partial_kind partial, Vec<Match *> *matches) {
  Matcher matcher(send, args.v[0], partial, matches);
  {
    MPosition app;
    matcher.find_all_matches(0, args, &matcher.partial_matches, app, 0);
    if (!matcher.partial_matches || !matcher.partial_matches->n)
      return 0;
  }
  matcher.record_all_positions();
  {
    MPosition app;
    Vec<CreationSet *> csargs;
    Vec<Fun *> result;
    matcher.find_best_matches(args, csargs, *matcher.partial_matches, app, result, 1);
    matcher.partial_matches->copy(result);
    if (!matcher.partial_matches->n)
      return 0;
  }
  matcher.cannonicalize_matches();
  return matches->n;
}

//
// Build dispatch tables
//

static void
insert_fun(FA *fa, Fun *f, Sym *arg, Sym *s, MPosition &p) {
  MPosition *cpnum = cannonicalize_mposition(p), *cpname = 0;
  if (arg->name && !arg->is_constant && !arg->is_symbol) {
    MPosition pp(p);
    pp.set_top(arg->name);
    cpname = cannonicalize_mposition(pp);
  }
  for (MPosition *cp = cpnum; cp; cp = cpname, cpname = 0) { 
    if (fa->patterns->types_set.set_add(s))
      fa->patterns->types.add(s);
    if (!s->match_type) {
      s->match_type = new MType;
      fa->patterns->mtypes.add(s->match_type);
    }
    Vec<Fun *> *funs = s->match_type->funs.get(cp);
    if (!funs) {
      funs = new Vec<Fun *>;
      s->match_type->funs.put(cp, funs);
    }
    funs->set_add(f);
  }
}

static void
build_arg(FA *fa, Fun *f, Sym *a, MPosition &p) {
  insert_fun(fa, f, a, dispatch_type(a), p);
  if (a->is_pattern) {
    p.push(1);
    forv_Sym(aa, a->has)
      build_arg(fa, f, aa, p);
    p.pop();
  }
  p.inc();
}

static void
build(FA *fa) {
  forv_Fun(f, fa->pdb->funs) {
    MPosition p;
    p.push(1);
    insert_fun(fa, f, f->sym, f->sym, p);
    forv_Sym(a, f->sym->has)
      build_arg(fa, f, a, p);
  }
}

void
build_patterns(FA *fa) {
  fa->patterns = new Patterns;
  build(fa);
}

//
// Build argument positions (MPosition)s for functions
// 

static void
build_arg_position(Fun *f, Sym *a, MPosition &fpp) {
  MPosition *fcpp = cannonicalize_mposition(fpp), *fcnp = 0;
  if (a->name && !a->is_constant && !a->is_symbol) {
    MPosition fnp(fpp);
    fnp.set_top(a->name);
    fcnp = cannonicalize_mposition(fnp);
    f->named_to_positional.put(fcnp, fcpp);
  }
  for (MPosition *fcp = fcpp; fcp; fcp = fcnp, fcnp = 0) { 
    f->arg_positions.add(fcp);
    if (!a->var)
      a->var = new Var(a);
    a->var->is_formal = 1;
    f->arg_syms.put(fcp, a);
    if (fcp == fcpp) {
      if (is_Sym_OUT(a))
	f->out_positions.add(fcpp);
      f->positional_arg_positions.add(fcpp);
      if (a->is_pattern) {
	MPosition local_fpp(fpp);
	local_fpp.push(1);
	forv_Sym(aa, a->has) {
	  build_arg_position(f, aa, local_fpp);
	  local_fpp.inc();
	}
	local_fpp.pop();
      }
    }
  }
}

void
build_arg_positions(Fun *f) {
  MPosition fpp;
  cannonicalize_mposition(fpp);
  fpp.push(1);
  forv_Sym(a, f->sym->has) {
    build_arg_position(f, a, fpp);
    fpp.inc();
  }
  forv_MPosition(p, f->positional_arg_positions) {
    Sym *s = f->arg_syms.get(p);
    f->args.put(p, s->var);
  }
  f->rets.add(f->sym->ret->var);
}

void
build_arg_positions(FA *fa) {
  forv_Fun(f, fa->pdb->funs)
    build_arg_positions(f);
}
