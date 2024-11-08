#pragma once

#include "config.hpp"

namespace csrc
{
    class Solution
    {
    public:
        /** @brief List of requests that are served by each truck */
        const std::vector<std::vector<std::size_t>> &routes;

        Solution(const std::vector<std::vector<std::size_t>> &routes) : routes(routes) {}

        /**
         * @brief Transform the given problem into a MDVRP and solve with the objective value
         * as minimize working time
         */
        static Solution mdvrp();
    };

    Solution Solution::mdvrp()
    {
        // TODO
    }
}
