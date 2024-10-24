#pragma once

#include "standard.hpp"

namespace csrc
{
    enum ContainerType
    {
        CONTAINER_20 = 20,
        CONTAINER_40 = 40,
    };

    class Request
    {
    private:
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

    public:
        // Request ID
        const std::size_t id;

        // Size of container, either 20 or 40
        const ContainerType size;

        // Pickup and drop locations
        const std::size_t pickup, drop;

        // Whether the trailer is also picked up or dropped
        const bool pickup_trailer, drop_trailer;

        // Duration to pickup and drop containers
        const int pickup_duration, drop_duration;

        // Read data from stdin
        static Request *read();
    };

    Request *Request::read()
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

        return new Request(
            id,
            size == 20 ? CONTAINER_20 : CONTAINER_40,
            pickup,
            drop,
            pickup_action == "PICKUP_CONTAINER_TRAILER",
            drop_action == "DROP_CONTAINER_TRAILER",
            pickup_duration,
            drop_duration);
    }

    class Problem
    {
    private:
        Problem(
            const std::size_t &size,
            const std::vector<std::vector<int>> &distances,
            const std::size_t &trailer,
            const int &trailer_pickup_time,
            const std::vector<std::size_t> &trucks,
            const std::vector<Request *> &requests)
            : size(size),
              distances(distances),
              trailer(trailer),
              trailer_pickup_time(trailer_pickup_time),
              trucks(trucks),
              requests(requests) {}

    public:
        static Problem *instance;

        // Total number of locations
        const std::size_t size;

        // Travel time between locations
        const std::vector<std::vector<int>> distances;

        // Trailer location
        const std::size_t trailer;

        // Trailer pickup time
        const int trailer_pickup_time;

        // Initial truck locations
        const std::vector<std::size_t> trucks;

        // List of requests
        const std::vector<Request *> requests;

        // Read data from stdin
        static void read();
    };

    Problem *Problem::instance = nullptr;
    void Problem::read()
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

        std::size_t trailer;
        int trailer_pickup_time;
        std::cin >> ignored >> trailer >> trailer_pickup_time;

        std::size_t trucks_count;
        std::cin >> ignored >> trucks_count;

        std::vector<std::size_t> trucks(trucks_count);
        for (std::size_t i = 0; i < trucks_count; i++)
        {
            std::cin >> ignored >> trucks[i];
        }

        std::vector<Request *> requests;
        while (true)
        {
            Request *request = Request::read();
            if (request == nullptr)
            {
                break;
            }

            requests.push_back(request);
        }

        instance = new Problem(size, distances, trailer, trailer_pickup_time, trucks, requests);
    }
}
