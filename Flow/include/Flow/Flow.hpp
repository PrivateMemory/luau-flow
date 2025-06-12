#ifndef FLOW_MGR_H
#define FLOW_MGR_H
#include <Flow/FlowSng.hpp>
#include <functional>

/**
 * @brief Lua pre declerations
 *
 */
struct lua_TValue;
struct Closure;
struct lua_State;

/**
 * Execution context for the current luau_execute loop
 * gives access to the locals of luau_execute which can be helpful to reimplement ops
 * double pointer because we only hot patch the local variables of the function (cleaner, if you want to patch just use lua state)
 */
class ExecutionContext
{
public:
    std::uint32_t** pc; // local pc
    Closure** cl;       // local cl
    lua_TValue** base;  // local base aka an StkId
    lua_TValue** k;     // local constant table

    ExecutionContext(std::uint32_t** pc, Closure** cl, lua_TValue** base, lua_TValue** k)
        : pc(pc), cl(cl), base(base), k(k)
    {
    }
};

using pre_op_t = bool (*)(lua_State*, ExecutionContext);
using post_op_t = void (*)(lua_State*, ExecutionContext);

class Flow : public Singleton<Flow>
{
    pre_op_t pre_op;
    post_op_t post_op;

public:

    void set_pre_op(pre_op_t op)
    {
        pre_op = op;
    }

    bool has_pre_op()
    {
        return pre_op != nullptr;
    }

    bool has_post_op()
    {
        return post_op != nullptr;
    }

    void set_post_op(post_op_t op)
    {
        post_op = op;
    }

    bool do_pre_op(lua_State* L, ExecutionContext ctx)
    {
        if (has_pre_op())
            return pre_op(L, ctx);
        return true; // pass by default
    }

    void do_post_op(lua_State* L, ExecutionContext ctx)
    {
        if (has_post_op())
            post_op(L, ctx);
    }
};

#endif // FLOW_MGR_H