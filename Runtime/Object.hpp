#include <vector>
#include <string>
#include <iostream>

using ConstOwnedString = const std::string;

struct Type;

struct Object {
    Object* klass;
    ConstOwnedString name;
    bool owner;
    void* data;
    
    Object(
        Object* _type,
        std::string_view _name,
        bool _owner = false,
        void* _data = nullptr
    ) : klass(_type),
        name(std::move(_name)),
        owner(_owner),
        data(_data) { }
    
    Type* type() {
        return (Type*)klass->data;
    }
    
    ~Object();
};

using Destroy_t = void(*)(void*);

struct Type {
    Destroy_t Destroy;
    
    Type(Destroy_t _Destroy): Destroy(_Destroy) { }
};

Object::~Object() {
    if(data != nullptr) {
        if(owner) {
            type()->Destroy(data);
        }
        data = nullptr;
    }
}

template<class T>
void DefaultDestroy(void* pobj) {
    T* obj = (T*)pobj;
    delete obj;
}

template<class T>
Type ConstructCType() {
    return Type(&DefaultDestroy<T>);
}

struct Scope {
    using Children = std::vector<std::unique_ptr<Object>>;
    Children children;
    
    Object* Find(std::string_view name) {
        auto i = std::find_if(
            children.begin(), children.end(),
            [&name] (const std::unique_ptr<Object>& d) {
                return d->name == name;
            }
        );
        
        if(i == children.end())
            return nullptr;
        
        return i->get();
    }
};

#include "Types.hpp"

template<class... Args>
void pass(Args&&...) { }

template<class... Args>
struct Core {
    using types = Types<Args...>;
    using names = const char*(&)[types::Size];
    
    Type _ctypes[types::Size];
    std::unique_ptr<Object> _global;
    std::vector<Object*> scopes;
    
    Scope* global() {
        return (Scope*)_global->data;
    }
    Scope* local() {
        return (Scope*)scopes.back()->data;
    }
    
    Core(names n)
      : _ctypes { ConstructCType<Args>()... },
        _global(make_global(n)),
        scopes { _global.get() } { }
    
    template<class T>
    constexpr static size_t index() {
        return types::template Index<T>;
    }
    
    std::unique_ptr<Object> make_global(names n) {
        auto p = std::make_unique<Object>(
            nullptr, "_global", true, new Scope()
        );
        Object* t = nullptr;
        pass((global()->children.push_back(
            std::make_unique<Object>(
                 t,
                 n[index<Args>()],
                 false,
                 &_ctypes[index<Args>()]
             )
        ), t == nullptr ? t = global()->children.front().get(): nullptr)...);
        global()->children.front()->klass = t;
        p->klass = ctype<Scope>();
        return p;
    }
    
    template<class T>
    Object* ctype() {
        return global()->children[index<T>()].get();
    }
    
    Object* Resolve(std::string_view name) {
        Object* res = nullptr;
        for(auto i = scopes.rbegin(); i < scopes.rend() && res == nullptr; ++i)
            res = ((Scope*)(*i)->data)->Find(name);
        return res;
    }
    
    Object* Declare(Object* type, std::string_view name, bool owner = false, void* data = nullptr) {
        local()->children.push_back(std::make_unique<Object>(type, name, owner, data));
        return local()->children.back().get();
    }
    
    template<class T>
    Object* Declare(std::string_view name, bool owner = false, void* data = nullptr) {
        return Declare(ctype<T>(), name, owner, data);
    }
    
    template<class T>
    Object* Alias(T& t, std::string_view name) {
        return Declare<T>(name, false, &t);
    }
    
#define Observe(X) Alias(X, #X)
};

void TestObject() {
    const char* n[] { "Type", "Scope", "Int" };
    Core<Type, Scope, int> c(n);
    
    Object* c_int = c.Resolve("Int");
    if(c_int == nullptr) {
        return;
    }
    Object* i = c.Declare(c_int, "i", true, new int(5));
    
    int p = -5;
    std::cout << "before: " << p << std::endl;
    Object* _p = c.Observe(p);
    *(int*)_p->data = *(int*)i->data;
    std::cout << "after: " << p << std::endl;
    
    std::cout << i->klass->klass->klass->name << std::endl;
}
