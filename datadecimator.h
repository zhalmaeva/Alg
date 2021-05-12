#pragma once
#include "qcustomplot.h"
#include <stack>
#include <cmath>

struct DataDecimator
{
    using storage_t = QSharedPointer<QCPGraphDataContainer>;
    using const_iterator = storage_t::value_type::const_iterator;

    static storage_t douglas_peucker(const storage_t& storage, double d);
    static storage_t reumann_witkam(const storage_t &storage, double d);
    static storage_t opheim(const storage_t& storage, double d = 0);
    static storage_t lang(const storage_t& storage, double d = 0);
    static storage_t llb(const storage_t &storage, double bucket_count);
};
