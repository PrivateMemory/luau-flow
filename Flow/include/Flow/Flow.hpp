#ifndef FLOW_MGR_H
#define FLOW_MGR_H
#include <Flow/FlowSng.hpp>
#include <functional>

struct lua_TValue;
struct VmProtection {
    lua_TValue** base;
};

struct lua_State;


using pre_op_t = bool(*)(lua_State*,std::uint32_t*);
using post_op_t = void(*)(lua_State*, std::uint32_t*);

class Flow : public Singleton<Flow> {
    pre_op_t pre_op;
    post_op_t post_op;

    VmProtection vm_protection;
public:
    void NewProtection(lua_TValue** base) {
        vm_protection.base = base;
    }

    void ProtectedCall(lua_State* L, std::uint32_t* pc, std::function<void()> f) {
        L->ci->savedpc = pc;
        f();
        *vm_protection.base = L->base;
    }

    void set_pre_op(pre_op_t op) {
        pre_op = op;
    }

        bool has_pre_op() {
        return pre_op != nullptr;
    }

    bool has_post_op() {
        return post_op != nullptr;
    }

    void set_post_op(post_op_t op) {
        post_op = op;
    }

    bool do_pre_op(lua_State* L, std::uint32_t* pc) {
        if (has_pre_op())
            return pre_op(L, pc);
        return true; // pass by default
    }

    void do_post_op(lua_State* L, std::uint32_t* pc) {
        if (has_post_op())
            post_op(L, pc);
    }


};

#endif // FLOW_MGR_H