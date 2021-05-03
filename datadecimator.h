#pragma once
#include "point.h"
#include <stack>
#include <cmath>

template <class Container>
struct DataDecimator
{
    using value_type        = typename Container::value_type;
    using storage_t         = Container;
    using iterator          = typename storage_t::iterator;
    using const_iterator    = typename storage_t::const_iterator;


    static storage_t douglas_peucker(const storage_t& storage, double d)
    {
        if(d == 0) {
            return storage;
        }
        if(storage.empty()) {
            return {};
        }
        storage_t dec_storage;
        dec_storage.push_back(storage.front());
        std::stack<std::pair<const_iterator, const_iterator>> q;
        q.push({ static_cast<const_iterator>(storage.begin()), static_cast<const_iterator>(storage.end()) });
        while(!q.empty()) {
            auto range = q.top();
            auto last = std::prev(range.second);
            double k[] = {
                last->y - range.first->y,
                last->x - range.first->x,
                last->x * range.first->y - range.first->x * last->y
            };
            double maxDist = 0;
            const_iterator maxIt;
            for(auto it = std::next(range.first); it < last; ++it) {
                auto dist = std::abs(k[0]*it->x - k[1]*it->y + k[2]);
                if(dist > maxDist) {
                    maxDist = dist;
                    maxIt = it;
                }
            }
            q.pop();
            if(maxDist / std::sqrt(k[0]*k[0] + k[1]*k[1]) >= d) {
                q.push({ maxIt, range.second });
                dec_storage.push_back(*maxIt);
                q.push({ range.first, std::next(maxIt) });
            }
        }
        dec_storage.push_back(storage.back());
    return dec_storage;
    }

    static storage_t largest_line_bucket(const storage_t& storage, int bucket_count)
    {
        if(storage.empty()) {
            return {};
        }

        long int values_size = storage.size();
        int size = (values_size - 2) / bucket_count;
        unsigned i = 0, j = 0, t = 0;
        double side_length = 0, side_length_current = 0;
        double a_line = 0, b_line = 0;
        storage_t dec_storage;
        const_iterator maxIt;
        const_iterator It{static_cast<const_iterator>(storage.begin())};
        ++It;

        if(((values_size - 2) % bucket_count) > 0)
            ++bucket_count;

        dec_storage.push_back(storage.front());

        for(i = 0; i < bucket_count; ++i)
        {
            side_length = 0;
            t = i * size + 1;

            for(j = t; j < t + size && It < storage.end(); ++j, ++It)
            {
                a_line = It->x - dec_storage.end()->x;
                b_line = It->y - dec_storage.end()->y;
                side_length_current = std::sqrt(a_line * a_line + b_line * b_line);

                if(side_length_current > side_length)
                {
                    side_length = side_length_current;
                    maxIt = It;
                }
            }

            dec_storage.push_back({maxIt->x, maxIt->y});
        }

        dec_storage.push_back(storage.back());

        return dec_storage;
    }

    static storage_t reuman_vitkam(const storage_t& storage, double d)
    {
        if(d == 0) {
            return storage;
        }
        if(storage.empty()) {
            return {};
        }
        storage_t dec_storage;
        dec_storage.push_back(storage.front());

        const_iterator st1{static_cast<const_iterator>(storage.begin())};
        double x1 = st1->x;
        double y1 = st1->y;
        auto st=st1;

        const_iterator st2 = ++st;
        double x2 = st2->x;
        double y2 = st2->y;
        dec_storage.push_back(*st2);

        for(const_iterator It=++st; It!=storage.end(); It++){

            double l = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y2-y2));
            double x=It->x;
            double y=It->y;
            bool f = false;

            double h = fabs(((x2-x1) * (y-y1) - (y2-y1) * (x-x1))/l);

            if (h < d) {
                st = It;
                f = true;
            }
            else {
                if (f) dec_storage.push_back(*st);
                f = false;
                x1 = x2;
                y1 = y2;
                x2 = x;
                y2 = y;
                dec_storage.push_back(*It);
            }
        }

        dec_storage.push_back(storage.back());
    return dec_storage;
    }
};
