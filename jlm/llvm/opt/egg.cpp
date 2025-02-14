#include "jlm/llvm/opt/egg.hpp"
#include "jlm/llvm/ir/RvsdgModule.hpp"

#include <cstdio>
#include <jlm/rvsdg/view.hpp>

namespace jlm {

	auto egg::run(RvsdgModule& module, StatisticsCollector& stats) -> void
	{
		auto f { std::fopen("tmp.rvsdg", "w") };
		jive::view(module.Rvsdg().root(), f);
		// jive::view_xml(module.Rvsdg().root(), f);
	}
}
