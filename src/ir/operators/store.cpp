/*
 * Copyright 2017 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/graph.h>
#include <jive/vsdg/statemux.h>

#include <jlm/ir/operators/store.hpp>

namespace jlm {

/* store operator */

store_op::~store_op() noexcept
{}

bool
store_op::operator==(const operation & other) const noexcept
{
	auto op = dynamic_cast<const store_op*>(&other);
	return op
	    && op->nstates_ == nstates_
	    && op->aport_ == aport_
	    && op->vport_ == vport_
	    && op->alignment_ == alignment_;
}

size_t
store_op::narguments() const noexcept
{
	return 2 + nstates();
}

const jive::port &
store_op::argument(size_t index) const noexcept
{
	JLM_DEBUG_ASSERT(index < narguments());
	if (index == 0)
		return aport_;

	if (index == 1)
		return vport_;

	static const jive::port p(jive::mem::type::instance());
	return p;
}

size_t
store_op::nresults() const noexcept
{
	return nstates();
}

const jive::port &
store_op::result(size_t index) const noexcept
{
	JLM_DEBUG_ASSERT(index < nresults());
	static const jive::port p(jive::mem::type::instance());
	return p;
}

std::string
store_op::debug_string() const
{
	return "STORE";
}

std::unique_ptr<jive::operation>
store_op::copy() const
{
	return std::unique_ptr<jive::operation>(new store_op(*this));
}

/* store normal form */

static bool
is_store_mux_reducible(const std::vector<jive::output*> & operands)
{
	JLM_DEBUG_ASSERT(operands.size() > 2);

	auto muxnode = operands[2]->node();
	if (!muxnode || !dynamic_cast<const jive::mux_op*>(&muxnode->operation()))
		return false;

	for (size_t n = 2; n < operands.size(); n++) {
		JLM_DEBUG_ASSERT(dynamic_cast<const jive::mem::type*>(&operands[n]->type()));
		if (operands[n]->node() && operands[n]->node() != muxnode)
			return false;
	}

	return true;
}

static std::vector<jive::output*>
perform_store_mux_reduction(
	const jlm::store_op & op,
	const std::vector<jive::output*> & old_operands)
{
	auto muxnode = old_operands[2]->node();
	auto type = static_cast<const jive::state::type*>(&muxnode->input(0)->type());
	auto address = old_operands[0];
	auto value = old_operands[1];

	auto states = create_store(address, value, muxnode->operands(), op.alignment());
	auto state	= jive::create_state_merge(*type, states);
	return jive::create_state_split(*type, state, op.nstates());
}

store_normal_form::~store_normal_form()
{}

store_normal_form::store_normal_form(
	const std::type_info & opclass,
	jive::node_normal_form * parent,
	jive::graph * graph) noexcept
: simple_normal_form(opclass, parent, graph)
, enable_store_mux_(false)
{
	if (auto p = dynamic_cast<const store_normal_form*>(parent))
		enable_store_mux_ = p->enable_store_mux_;
}

bool
store_normal_form::normalize_node(jive::node * node) const
{
	JLM_DEBUG_ASSERT(is_store_op(node->operation()));
	auto op = static_cast<const jlm::store_op*>(&node->operation());
	auto operands = node->operands();

	if (!get_mutable())
		return true;

	if (get_store_mux_reducible() && is_store_mux_reducible(operands)) {
		auto outputs = perform_store_mux_reduction(*op, operands);
		for (size_t n = 0; n < node->noutputs(); n++)
			node->output(n)->replace(outputs[n]);
		return false;
	}

	return simple_normal_form::normalize_node(node);
}

std::vector<jive::output*>
store_normal_form::normalized_create(
	jive::region * region,
	const jive::simple_op & op,
	const std::vector<jive::output*> & ops) const
{
	JLM_DEBUG_ASSERT(is_store_op(op));
	auto sop = static_cast<const jlm::store_op*>(&op);

	if (!get_mutable())
		return simple_normal_form::normalized_create(region, op, ops);

	auto operands = ops;
	if (get_store_mux_reducible() && is_store_mux_reducible(operands))
		return perform_store_mux_reduction(*sop, operands);

	return simple_normal_form::normalized_create(region, op, operands);
}

void
store_normal_form::set_store_mux_reducible(bool enable)
{
	if (get_store_mux_reducible() == enable)
		return;

	children_set<store_normal_form, &store_normal_form::set_store_mux_reducible>(enable);

	enable_store_mux_ = enable;
	if (get_mutable() && enable)
		graph()->mark_denormalized();
}

}

namespace {

static jive::node_normal_form *
create_store_normal_form(
	const std::type_info & opclass,
	jive::node_normal_form * parent,
	jive::graph * graph)
{
	return new jlm::store_normal_form(opclass, parent, graph);
}

static void __attribute__((constructor))
register_normal_form()
{
	jive::node_normal_form::register_factory(typeid(jlm::store_op), create_store_normal_form);
}

}
