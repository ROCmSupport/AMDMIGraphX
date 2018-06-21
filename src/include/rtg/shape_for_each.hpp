#ifndef RTG_GUARD_RTGLIB_SHAPE_FOR_EACH_HPP
#define RTG_GUARD_RTGLIB_SHAPE_FOR_EACH_HPP

#include <rtg/shape.hpp>
#include <algorithm>

namespace rtg {

template <class F>
void shape_for_each(const rtg::shape& s, F f)
{
    // Ensure calls to f use const ref to vector
    auto call = [&f](const std::vector<std::size_t>& i) { f(i); };
    std::vector<std::size_t> indices(s.lens().size());
    for(std::size_t i = 0; i < s.elements(); i++)
    {
        std::transform(s.strides().begin(),
                       s.strides().end(),
                       s.lens().begin(),
                       indices.begin(),
                       [&](std::size_t stride, std::size_t len) {
                           assert(len > 0 and stride > 0);
                           return (i / stride) % len;
                       });
        call(indices);
    }
}

} // namespace rtg

#endif
