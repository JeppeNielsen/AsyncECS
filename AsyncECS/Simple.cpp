#include <iostream>
#include "AllTests.hpp"
#include "Scene.hpp"
#include <variant>
#include "HasMethodHelper.hpp"

using namespace AsyncECS;

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Limit {
    float x;
    float y;
};

struct Renderable {
    int imageNo;
};

struct MovementSystem : System<Position, const Velocity> {
    void Update(Position& position, const Velocity& velocity) {
        position.x += velocity.x;
        position.y += velocity.y;
        std::cout << "Movement: "<< position.x << ", " << position.y <<std::endl;
    }
};

struct RenderSystem : System<const Position, const Renderable> {
    void Update(const Position& position, const Renderable& renderable) {
        DrawRenderable(position, renderable);
    }
    
    void DrawRenderable(const Position& position, const Renderable& renderable) {
        std::cout << "Render: "<< position.x << ", " << position.y <<std::endl;
    }
};

struct LimitSystem : SystemChangedGameObject<const Position, const Limit>, SceneModifier<Limit, Position> {
    void Update(GameObject gameObject, const Position& position, const Limit& limit) {
        if (position.x>=limit.x) {
            Modify([gameObject] (Modifier& modifier) {
                modifier.RemoveGameObject(gameObject);
                
                auto createdGameObject = modifier.CreateGameObject();
                
                std::cout << createdGameObject << std::endl;
                modifier.AddComponent<Limit>(createdGameObject, 0.0f, 0.0f);
                modifier.AddComponent<Position>(createdGameObject);
                
            });
            
        }
        std::cout << "LimitSystem : "<<gameObject << std::endl;
    }
};






/*
HAS_METHOD(serialize)



struct X {
    template<typename Arg>
    int serialize(const Arg& arg) const { return arg; }
};

struct Y {
    int bla;
};

template<typename T, typename Arg, typename I = void>
struct Serializer {
    void Serialize(T& val, const Arg& arg) {
        std::cout << "default" << std::endl;
    }
};

template<typename T, typename Arg>
struct Serializer<T, Arg, typename std::enable_if<has_serialize<T, int(const Arg&)>::value>::type> {
    void Serialize(T& val, const Arg& arg) {
        int ret = val. template serialize<Arg>(arg);
        std::cout << "specialized " << ret << std::endl;
    }
};

struct Bla {

    template<typename T>
    int serialize(const T& val) {
        return val;
    }
};

struct Bla2 : public Bla {

};



HAS_METHOD(SetScene)

template<typename T, typename I = void>
struct SetModifiableComponents {
    void Set(T& system) {
       
    }
};

template<typename T>
struct SetModifiableComponents<T, typename std::enable_if<has_SetScene<T, void(IScene&)>::value>::type> {
    void Set(T& system) {
        std::cout << typeid(system).name() << std::endl;
    }
};

struct Base {

private:
    void SetScene(IScene& scene) {
        std::cout << "Yes";
    }

};

template<typename T>
struct ShouldHaveSetScene : Base{
    
};

*/



template <typename T>
struct HasInitializeMethod {
    struct dummy { };

    template <typename C, typename P>
    static auto test(P * p) -> decltype(std::declval<C>().Initialize(), std::true_type());

    template <typename, typename>
    static std::false_type test(...);

    typedef decltype(test<T, dummy>(nullptr)) type;
    static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value;
};

struct NonInitializer {


};

struct Initializer {
    void Initialize() {
    
    }
};

HAS_METHOD(Initialize)







template <typename T>
void print_type() {
    std::cout<<(__PRETTY_FUNCTION__)<< std::endl;
}

template<typename T, typename I = void>
struct is_not_double {
    using value = std::false_type;
};
template<typename T>
struct is_not_double<T, typename std::enable_if<has_Initialize<T, void()>::value>::type> {
    using value = std::true_type;
};


template <typename T>
void Invoke(T& obj) {

    if constexpr (has_Initialize<T, void()>::value) {
        obj.Initialize();
    }

//    AsyncECS::TupleHelper::static_if<has_Initialize<T, void()>>(obj, [](T& o) {
  //      o.Initialize();
    //});
}

int main() {

    Initializer i;
    Invoke(i);
    
    NonInitializer ni;
    Invoke(ni);

    /*
    TupleHelper::static_if<has_Initialize<Initializer, void()>>(initializer, [](Initializer& initializer) {
        initializer.Initialize();
    });
    
    TupleHelper::static_if<has_Initialize<Initializer, void()>>(initializer, [](Initializer& initializer) {
       initializer.Initialize();
    });
    */


    /*
    SetModifiableComponents<LimitSystem> limitSystemSet;
    LimitSystem limitSystem;
    limitSystemSet.Set(limitSystem);
    
    ShouldHaveSetScene<int> shouldHaveSetScene;
    
    SetModifiableComponents<ShouldHaveSetScene<int>> test;
    test.Set(shouldHaveSetScene);
    
    X x;
    Y y;
    Bla2 bla;
    
    Serializer<X, float> s;
    s.Serialize(x, 32.0f);
    
    Serializer<Y, int> ss;
    ss.Serialize(y, 12);
    
    Serializer<Bla2, int> sss;
    sss.Serialize(bla, 54);
    
    std::cout << has_serialize<X, int(const std::string&)>::value << std::endl;// will print 1
    
    if (has_serialize<X, int(const int&)>::value) {
    
    }
    */
    
/*
    AllTests alltests;
    alltests.Run();
*/

    using Components = ComponentTypes<Position, Velocity, Renderable, Limit>;
    using Systems = SystemTypes<MovementSystem, RenderSystem, LimitSystem>;
    using RegistryType = Registry<Components>;

    RegistryType registry;
    Scene<RegistryType, Systems> scene(registry);
    
    auto gameObject = scene.CreateGameObject();
    scene.AddComponent<Position>(gameObject, 0.0f, 0.0f);
    scene.AddComponent<Velocity>(gameObject, 1.0f, 2.0f);
    scene.AddComponent<Renderable>(gameObject);
    scene.AddComponent<Limit>(gameObject, 3.0f, 0.0f);
    
    for (int i=0; i<10; i++) {
    
        scene.Update();
    
        if (registry.IsGameObjectValid(gameObject)) {
            auto& p = scene.GetComponent<Position>(gameObject);
            std::cout << "Position " << p.x << " " << p.y << std::endl;
        }
    }
    std::cout << " graph: "<<std::endl;
    scene.WriteGraph(std::cout);
    
    return 0;
}
