#pragma once

#include "standard.hpp"

namespace csrc
{
    enum ContainerType
    {
        /** @brief Represents a 20-ft container */
        CONTAINER_20 = 20,

        /** @brief Represents a 40-ft container */
        CONTAINER_40 = 40,
    };

    class Request
    {
    public:
        /** @brief Request ID */
        const std::size_t id;

        /** @brief Size of container, either 20 or 40 */
        const ContainerType size;

        /** @brief Pickup and drop locations */
        const std::size_t pickup, drop;

        /** @brief Whether the trailer is also picked up or dropped */
        const bool pickup_trailer, drop_trailer;

        /** @brief Duration to pickup and drop containers */
        const int pickup_duration, drop_duration;

        /** @brief Read data from stdin */
        static std::shared_ptr<Request> read();

        Request(
            const std::size_t &id,
            const ContainerType &size,
            const std::size_t &pickup,
            const std::size_t &drop,
            const bool &pickup_trailer,
            const bool &drop_trailer,
            const int &pickup_duration,
            const int &drop_duration)
            : id(id),
              size(size),
              pickup(pickup),
              drop(drop),
              pickup_trailer(pickup_trailer),
              drop_trailer(drop_trailer),
              pickup_duration(pickup_duration),
              drop_duration(drop_duration) {}
    };

    std::shared_ptr<Request> Request::read()
    {
        std::string expect_req;
        std::cin >> expect_req;

        if (expect_req == "#")
        {
            return nullptr;
        }

        std::size_t id;
        std::cin >> id;

        int size;
        std::cin >> size;

        std::size_t pickup;
        std::string pickup_action;
        int pickup_duration;
        std::cin >> pickup >> pickup_action >> pickup_duration;

        std::size_t drop;
        std::string drop_action;
        int drop_duration;
        std::cin >> drop >> drop_action >> drop_duration;

        return std::make_shared<Request>(
            id,
            size == 20 ? CONTAINER_20 : CONTAINER_40,
            pickup,
            drop,
            pickup_action == "PICKUP_CONTAINER_TRAILER",
            drop_action == "DROP_CONTAINER_TRAILER",
            pickup_duration,
            drop_duration);
    }

    class Config
    {
    private:
        static std::vector<std::vector<int>> _calculate_vrp_matrix(
            const std::size_t &size,
            const std::vector<std::vector<int>> &distances,
            const std::size_t &trailer_depot,
            const int &trailer_pickup_time,
            const std::vector<std::size_t> &depots,
            const std::vector<std::shared_ptr<Request>> &requests);

    public:
        static std::shared_ptr<Config> instance;

        /** @brief Total number of locations */
        const std::size_t size;

        /** @brief Travel time between locations */
        const std::vector<std::vector<int>> distances;

        /** @brief Depot with an infinite amount of trailers */
        const std::size_t trailer_depot;

        /** @brief Trailer pickup time */
        const int trailer_pickup_time;

        /** @brief Truck depot locations */
        const std::vector<std::size_t> depots;

        /** @brief List of requests */
        const std::vector<std::shared_ptr<Request>> requests;

        /**
         * @brief The travel time matrix of the MDVRP problem that the original problem reduced to.
         *
         * The matrix contains `requests.size() + depots.size()` points, where the first `requests.size()`
         * points are the requests and the last `depots.size()` points are the truck depots.
         */
        const std::vector<std::vector<int>> vrp_matrix;

        Config(
            const std::size_t &size,
            const std::vector<std::vector<int>> &distances,
            const std::size_t &trailer_depot,
            const int &trailer_pickup_time,
            const std::vector<std::size_t> &depots,
            const std::vector<std::shared_ptr<Request>> &requests)
            : size(size),
              distances(distances),
              trailer_depot(trailer_depot),
              trailer_pickup_time(trailer_pickup_time),
              depots(depots),
              requests(requests),
              vrp_matrix(
                  _calculate_vrp_matrix(
                      size,
                      distances,
                      trailer_depot,
                      trailer_pickup_time,
                      depots,
                      requests)) {}

        /** @brief Read data from stdin */
        static void read();
    };

    std::shared_ptr<Config> Config::instance = nullptr;

    std::vector<std::vector<int>> Config::_calculate_vrp_matrix(
        const std::size_t &size,
        const std::vector<std::vector<int>> &distances,
        const std::size_t &trailer_depot,
        const int &trailer_pickup_time,
        const std::vector<std::size_t> &depots,
        const std::vector<std::shared_ptr<Request>> &requests)
    {
        std::size_t vrp_size = requests.size() + depots.size();
        std::vector<std::vector<int>> vrp_matrix(vrp_size, std::vector<int>(vrp_size));

        for (std::size_t i = 0; i < requests.size(); i++)
        {
            for (std::size_t j = 0; j < requests.size(); j++)
            {
                if (requests[i]->drop_trailer != requests[j]->pickup_trailer)
                {
                    vrp_matrix[i][j] = distances[requests[i]->drop][trailer_depot] + distances[trailer_depot][requests[j]->pickup];
                    if (requests[i]->drop_trailer)
                    {
                        vrp_matrix[i][j] += trailer_pickup_time;
                    }
                }
                else
                {
                    vrp_matrix[i][j] = distances[requests[i]->drop][requests[j]->pickup];
                }
            }
        }

        for (std::size_t truck = 0; truck < depots.size(); truck++)
        {
            for (std::size_t i = 0; i < requests.size(); i++)
            {
                std::size_t j = truck + requests.size();
                if (requests[i]->pickup_trailer)
                {
                    vrp_matrix[j][i] = distances[depots[truck]][trailer_depot] + trailer_pickup_time + distances[trailer_depot][requests[i]->pickup];
                }
                else
                {
                    vrp_matrix[j][i] = distances[depots[truck]][requests[i]->pickup];
                }

                if (requests[i]->drop_trailer)
                {
                    vrp_matrix[i][j] = distances[requests[i]->drop][trailer_depot] + trailer_pickup_time + distances[trailer_depot][depots[truck]];
                }
                else
                {
                    vrp_matrix[i][j] = distances[requests[i]->drop][depots[truck]];
                }
            }
        }

        for (std::size_t i = 0; i < depots.size(); i++)
        {
            for (std::size_t j = 0; j < depots.size(); j++)
            {
                vrp_matrix[i + requests.size()][j + requests.size()] = distances[depots[i]][depots[j]];
            }
        }

        return vrp_matrix;
    }

    void Config::read()
    {
        std::string ignored; // Scan and ignore certain tokens
        std::cin >> ignored;

        std::size_t size;
        std::cin >> size;

        std::cin >> ignored >> ignored;

        std::vector<std::vector<int>> distances(size, std::vector<int>(size));
        for (std::size_t i = 0; i < size * size; i++)
        {
            std::size_t u, v;
            int d;
            std::cin >> u >> v >> d;

            distances[u - 1][v - 1] = d;
        }

        std::size_t trailer_depot;
        int trailer_pickup_time;
        std::cin >> ignored >> trailer_depot >> trailer_pickup_time;

        std::size_t trucks_count;
        std::cin >> ignored >> trucks_count;

        std::vector<std::size_t> depots(trucks_count);
        for (std::size_t i = 0; i < trucks_count; i++)
        {
            std::cin >> ignored >> depots[i];
        }

        std::vector<std::shared_ptr<Request>> requests;
        while (true)
        {
            auto request = Request::read();
            if (request == nullptr)
            {
                break;
            }

            requests.push_back(request);
        }

        instance = std::make_shared<Config>(size, distances, trailer_depot, trailer_pickup_time, depots, requests);
    }
}
