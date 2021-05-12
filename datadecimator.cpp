#include "datadecimator.h"

DataDecimator::storage_t DataDecimator::douglas_peucker(const DataDecimator::storage_t& storage, double d)
{
    if(storage->isEmpty() || d == 0) {
        return storage_t{ new storage_t::value_type(*storage) };
    }
    storage_t dec_storage(new QCPGraphDataContainer);
    dec_storage->add(*storage->begin());
    std::stack<std::pair<const_iterator, const_iterator>> stack;
    stack.push({ storage->constBegin(), storage->constEnd() });
    while(!stack.empty()) {
        auto range = stack.top();
        auto last = std::prev(range.second);
        double k1 = last->value - range.first->value;
        double k2 = last->key - range.first->key;
        double k3 = last->key * range.first->value - range.first->key * last->value;
        double maxDist = 0;
        const_iterator maxIt;
        for(auto it = std::next(range.first); it < last; ++it) {
            auto dist = std::abs(k1*it->key - k2*it->value + k3);
            if(dist > maxDist) {
                maxDist = dist;
                maxIt = it;
            }
        }
        stack.pop();
        if(maxDist / std::sqrt(k1*k1 + k2*k2) >= d) {
            stack.push({ maxIt, range.second });
            dec_storage->add(*maxIt);
            stack.push({ range.first, std::next(maxIt) });
        }
    }
    dec_storage->add(*std::prev(storage->end()));
    return dec_storage;
}

DataDecimator::storage_t DataDecimator::reumann_witkam(const DataDecimator::storage_t &storage, double d)
{
    if(storage->isEmpty()) {
        return storage_t{ new storage_t::value_type(*storage) };
    }
    storage_t dec_storage(new QCPGraphDataContainer);
    dec_storage->add(*storage->begin());
    auto st{storage->begin()};
    double x1 = st->key;
    double y1 = st->value;
    ++st;
    double x2 = st->key;
    double y2 = st->value;
    dec_storage->add(*st);
    bool f = false;
    for(auto It = ++st; It != storage->end(); ++It){
        double x = It->key;
        double y = It->value;
        double dx = x2 - x1;
        double dy = y2 - y1;
        double h = std::abs( dx * (y - y1) - dy * (x - x1)) / std::sqrt(dx * dx + dy * dy);

        if (h < d) {
            st = It;
            f = true;
        }
        else {
            if (f) {
                dec_storage->add(*st);
                f = false;
                x1 = st->key;
                y1 = st->value;
            }
            else {
                x1 = x2;
                y1 = y2;
            }
            x2 = x;
            y2 = y;
            dec_storage->add(*It);
        }
    }
    dec_storage->add(*std::prev(storage->end()));
    return dec_storage;
}

DataDecimator::storage_t DataDecimator::opheim(const DataDecimator::storage_t &storage, double d)
{
    d = (std::prev(storage->end())->key - storage->begin()->key) / 700;
    storage_t dec_storage(new QCPGraphDataContainer);
    if(storage->isEmpty() || d == 0) {
        return storage;
    }
    dec_storage->add(*storage->begin());
    auto st{storage->begin()};
    double x1 = st->key;
    double y1 = st->value;
    bool f = false;
    for(auto It = ++st; It!=storage->end(); ++It){
        double x = It->key;
        double y = It->value;
        double l = std::sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
        if (l < d) {
            st = It;
            f = true;
        }
        else if (f) {
            dec_storage->add(*st);
            x1 = st->key;
            y1 = st->value;
            f = false;
        }
        else{
            x1 = x;
            y1 = y;
            dec_storage->add(*It);
        }
    }
    dec_storage->add(*std::prev(storage->end()));
    return dec_storage;
 }

DataDecimator::storage_t DataDecimator::lang(const DataDecimator::storage_t &storage, double d)
{
    size_t look_ahead = (std::prev(storage->end())->key - storage->begin()->key) / 600;
    storage_t dec_storage(new QCPGraphDataContainer);
    if(storage->isEmpty() || d == 0) {
        return storage;
    }
    dec_storage->add(*storage->begin());
    auto start{storage->begin()};
    auto finish = start;
    for(int i = 0; i < look_ahead && finish != std::prev(storage->end()); ++i){
        finish ++;
    }
    while (1){
        double x1 = start->key;
        double y1 = start->value;
        double x2 = finish ->key;
        double y2 = finish ->value;
        double maxH = 0;
        auto nextIt = finish;
        for (auto it = ++start; it != finish; it++){
            double x = it->key;
            double y = it->value;
            double l = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
            double h = std::abs((x2 - x1) * (y - y1) - (y2 - y1) * (x - x1)) / l;
            if (h > d && h > maxH){
                maxH = h;
                nextIt = it;
            }
        }
        start--;
        for(int i = 0; i < std::distance(start, nextIt) && finish != std::prev(storage->end()); ++i){
            finish ++;
        }
        start = nextIt;
        dec_storage->add(*start);
        if (finish == std::prev(storage->end()))
            break;
    }
    dec_storage->add(*std::prev(storage->end()));
    return dec_storage;
}
