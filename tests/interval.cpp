#include "../src/core/interval.hpp"

#include <limits>
#include <vector>
#include <cassert>
#include <random>

int main(int argc, char * const argv[])
{
    using namespace sfx::interval;
    using inter = interval<float>;
    using checker = contains<float>;

    std::size_t N = 1 << 16;
    std::random_device rd;
    std::mt19937 gen{rd()};

    auto random_test = [&gen, N](checker check)
    {
        std::uniform_real_distribution dis{check.i.begin, check.i.end};
        for (std::size_t i=0 ; i<N ; ++i)
            assert(check(dis(gen)));
    };

    inter zero_one{0.f, 1.f};
    
    checker check{zero_one};
    assert(check(0.f));
    assert(check(0.5f));
    assert(!check(1.f));
    random_test(check);

    inter reals_plus{0.f, std::numeric_limits<float>::max()};

    check = checker{reals_plus};
    assert(check(0.f));
    assert(check(1.f));
    assert(check(965485.f));
    random_test(check);

    return 0;
}