
#include "jlm/ir/RvsdgModule.hpp"
#include "jlm/opt/optimization.hpp"
#include "jlm/util/Statistics.hpp"

namespace jlm {

class egg final : public optimization 
{
    auto run(RvsdgModule &module, StatisticsCollector &stats) -> void;
};

} // namespace jlm
