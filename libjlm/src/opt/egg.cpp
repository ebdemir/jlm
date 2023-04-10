#include "libjlm/include/jlm/opt/egg.hpp"
#include "jlm/ir/RvsdgModule.hpp"

#include <jive/view.hpp>

namespace jlm {

	auto egg::run(RvsdgModule& module, StatisticsCollector& stats) -> void
	{
		jive::view(module.Rvsdg().root(), stdout);
		jive::view_xml(module.Rvsdg().root(), stdout);
	}
}
