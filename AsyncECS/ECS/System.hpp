//
//  System.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "ComponentContainer.hpp"
#include "TupleHelper.hpp"

namespace AsyncECS {

template<typename ...T>
struct Dependencies {

    template<typename B>
    static constexpr B GetDependencies() {
        return B();
    }

    static constexpr std::tuple<T...> GetDependencies() {
        return std::make_tuple(GetDependencies<T>()...);
    }
    
    template<typename B>
    static constexpr auto GetDependenciesRecursiveInternal() {
        return std::tuple_cat(B::GetDependenciesRecursive(), std::tuple<B>{});
    }
    
    static constexpr auto GetDependenciesRecursive() {
        return std::tuple_cat(GetDependenciesRecursiveInternal<T>()...);
    }
    
    template<typename Systems>
    static std::tuple<T&...> GetDependenciesReferences(Systems& systems) {
        return std::tie((T&)std::get<T>(systems)...);
    }
};

using NoDependencies = Dependencies<>;

template<typename ...T>
struct SystemTypes {
    
    template<typename System>
    constexpr static auto GetDependency() {
        return std::tuple_cat(System::GetDependenciesRecursive(),std::tuple<System>{});
    }

    constexpr static auto GetDependencies() {
        return std::tuple_cat(GetDependency<T>()...);
    }

    using AllSystems = decltype(GetDependencies());
    using UniqueSystems = TupleHelper::UniqueTypes<AllSystems>;
};


template<typename...T>
struct System {

    using Iterator = std::tuple<T*...>;

    template<typename O, typename Components>
    const O& Get(const int index, const int changedFrame, Components& components, const O* ptr) const {
        return (const O&)std::get<ComponentContainer<std::remove_const_t<O>>>(components).GetConst(index);
    }
    
    template<typename O, typename Components>
    O& Get(const int index, const int changedFrame, Components& components, O* ptr) const {
        return (O&)((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).Get(index, changedFrame);
    }
   
    template<typename Components>
    std::tuple<T&...> GetComponentValuesFromGameObject(const int index, const int changedFrame, Components& components) const {
        return std::tie(Get(index, changedFrame, components, (T*)nullptr)...);
    }
    
    template<typename Components, typename ComponentObjects>
    int GetContainerIndex(ComponentObjects& componentObjects) {
        Iterator it;
        
        int min = std::numeric_limits<int>::max();
        int index = 0;
        TupleHelper::Iterate(it, [&min, &index, &componentObjects] (auto ptr) {
            using Type = std::remove_const_t<std::remove_pointer_t<decltype(ptr)>>;
            using ComponentIndexType = TupleHelper::Index<ComponentContainer<Type>, Components>;
            const auto typeIndex = ComponentIndexType::value;
            auto size = (int)componentObjects[typeIndex].objects.size();
            if (size < min) {
                index = typeIndex;
                min = size;
            }
        });
    
        return index;
    }
};

}
