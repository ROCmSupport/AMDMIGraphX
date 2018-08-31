#ifndef MIGRAPH_GUARD_MIGRAPHLIB_HIP_HPP
#define MIGRAPH_GUARD_MIGRAPHLIB_HIP_HPP

#include <migraph/operators.hpp>
#include <utility>

namespace migraph {
namespace gpu {

migraph::argument allocate_gpu(const migraph::shape& s, bool host = false);

migraph::argument to_gpu(migraph::argument arg, bool host = false);

migraph::argument from_gpu(migraph::argument arg);

void gpu_sync();
void copy_to_gpu(char* dst, const char* src, std::size_t size);

struct hip_allocate
{
    std::string tag{};
    std::string name() const { return "hip::allocate"; }
    shape compute_shape(const std::vector<shape>& inputs) const
    {
        check_shapes{inputs}.has(1);
        return inputs.front();
    }
    argument compute(context&, const shape& output_shape, const std::vector<argument>&) const
    {
        return allocate_gpu(output_shape);
    }
};

struct hip_load
{
    shape s;
    std::size_t offset = 0;
    std::string name() const { return "hip::load"; }
    shape compute_shape(const std::vector<shape>& inputs) const
    {
        check_shapes{inputs}.has(1);
        return s;
    }
    argument compute(context&, const shape&, const std::vector<argument>& args) const
    {
        return {s, args[0].data() + offset};
    }
};

struct hip_sync
{
    std::string tag{};
    std::string name() const { return "hip::sync"; }
    shape compute_shape(const std::vector<shape>& inputs) const
    {
        if(inputs.empty())
            return {};
        else
            return inputs.front();
    }
    argument compute(context&, const shape&, const std::vector<argument>& args) const
    {
        gpu_sync();
        if(args.empty())
            return {};
        else
            return args.front();
    }
};

struct hip_write
{
    std::string name() const { return "hip::write"; }
    shape compute_shape(const std::vector<shape>& inputs) const
    {
        check_shapes{inputs}.has(1);
        return inputs.front();
    }
    argument compute(context&, const shape&, const std::vector<argument>& args) const
    {
        return to_gpu(args.front());
    }
};

struct hip_memcpy
{
    std::string name() const { return "hip_memcpy"; }
    shape compute_shape(std::vector<shape> inputs) const { return inputs.at(2); }
    argument compute(context&, shape output_shape, std::vector<argument> args) const
    {
        std::size_t* p_data = reinterpret_cast<std::size_t*>(args.at(1).data());
        char* dst           = args.at(0).data() + p_data[0];
        const char* src     = args.at(2).data();
        std::size_t size    = args.at(2).get_shape().bytes();
        copy_to_gpu(dst, src, size);
        return {output_shape, dst};
    }
};
} // namespace gpu
} // namespace migraph

#endif
