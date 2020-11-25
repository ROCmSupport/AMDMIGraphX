
#include "verify_program.hpp"
#include <migraphx/program.hpp>
#include <migraphx/generate.hpp>
#include <migraphx/operators.hpp>

migraphx::instruction_ref
<<<<<<< HEAD
add_layernorm(migraphx::program& p, migraphx::instruction_ref x, std::vector<size_t> dims)
=======
add_layernorm(migraphx::module& m, migraphx::instruction_ref x, std::vector<size_t> dims)
>>>>>>> ac627b187620129964160c91c2654205f5a6092a
{
    auto scale =
        m.add_parameter("scale", migraphx::shape{migraphx::shape::float_type, {dims.back()}});
    auto bias =
        m.add_parameter("bias", migraphx::shape{migraphx::shape::float_type, {dims.back()}});
    auto epsilon  = m.add_literal(1e-12f);
    auto exponent = m.add_literal(2.0f);

    auto mean            = m.add_instruction(migraphx::op::reduce_mean({2}), x);
    auto mean_mbcast     = m.add_instruction(migraphx::op::multibroadcast{{dims}}, mean);
    auto sub             = m.add_instruction(migraphx::op::sub{}, x, mean_mbcast);
    auto exponent_mbcast = m.add_instruction(migraphx::op::multibroadcast{{dims}}, exponent);
    auto pow             = m.add_instruction(migraphx::op::pow{}, sub, exponent_mbcast);
    auto var             = m.add_instruction(migraphx::op::reduce_mean({2}), pow);
    auto epsilon_mbcast =
        m.add_instruction(migraphx::op::multibroadcast{{1, dims.at(1), 1}}, epsilon);
    auto add_epsilon  = m.add_instruction(migraphx::op::add{}, var, epsilon_mbcast);
    auto sqrt         = m.add_instruction(migraphx::op::sqrt{}, add_epsilon);
    auto sqrt_mbcast  = m.add_instruction(migraphx::op::multibroadcast{dims}, sqrt);
    auto div          = m.add_instruction(migraphx::op::div{}, sub, sqrt_mbcast);
    auto scale_mbcast = m.add_instruction(migraphx::op::multibroadcast{dims}, scale);
    auto mul          = m.add_instruction(migraphx::op::mul{}, scale_mbcast, div);
    auto bias_mbcast  = m.add_instruction(migraphx::op::multibroadcast{dims}, bias);
    return m.add_instruction(migraphx::op::add{}, mul, bias_mbcast);
}

struct test_layernorm : verify_program<test_layernorm>
{
    migraphx::program create_program() const
    {
        migraphx::program p;
        auto* mm                 = p.get_main_module();
        std::vector<size_t> dims = {1, 1, 5};
        auto x = mm->add_parameter("x", migraphx::shape{migraphx::shape::float_type, dims});
        add_layernorm(*mm, x, dims);
        return p;
    }
};

struct test_layernorm2 : verify_program<test_layernorm2>
{
    migraphx::program create_program() const
    {
        migraphx::program p;
        auto* mm                 = p.get_main_module();
        std::vector<size_t> dims = {1, 4, 24};
        auto x = mm->add_parameter("x", migraphx::shape{migraphx::shape::float_type, dims});
        add_layernorm(*mm, x, dims);
        return p;
    }
};

struct test_layernorm_triadd : verify_program<test_layernorm_triadd>
{
    migraphx::program create_program() const
    {
        migraphx::program p;
        auto* mm                 = p.get_main_module();
        std::vector<size_t> dims = {1, 4, 24};
        auto x    = mm->add_parameter("x", migraphx::shape{migraphx::shape::float_type, dims});
        auto y    = mm->add_parameter("y", migraphx::shape{migraphx::shape::float_type, dims});
        auto z    = mm->add_parameter("z", migraphx::shape{migraphx::shape::float_type, dims});
        auto add1 = mm->add_instruction(migraphx::op::add{}, x, y);
        auto add2 = mm->add_instruction(migraphx::op::add{}, add1, z);
        add_layernorm(*mm, add2, dims);
        return p;
    }
};
