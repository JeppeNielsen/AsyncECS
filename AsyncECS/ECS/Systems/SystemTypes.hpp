//
//  SystemTypes.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "TupleHelper.hpp"
#include "SystemDependencies.hpp"

namespace AsyncECS {

template<typename ...T>
struct SystemTypes {
    
    template<typename System>
    constexpr static auto GetDependency() {
        if constexpr (AsyncECS::Internal::has_IsSystemDependencies<System, void()>::value) {
            return std::tuple_cat(System::GetDependenciesRecursive(),std::tuple<System>{});
        } else {
            return std::tuple<System>{};
        }
    }

    constexpr static auto GetDependencies() {
        return std::tuple_cat(GetDependency<T>()...);
    }

    using AllSystems = decltype(GetDependencies());
    using UniqueSystems = TupleHelper::UniqueTypes<AllSystems>;
    
    
    template<typename System, typename Component, typename ...S>
    constexpr static auto GetSystemsWithComponentInternal(std::tuple<S...>& tuple) {
        if constexpr (System::template HasComponentType<Component>() ||
                      System::template HasComponentType<const Component>()) {
            
            return std::make_tuple(&std::get<System>(tuple));
        } else {
            return std::tuple<>{};
        }
    }
    
    template<typename Component, typename ...S>
    constexpr static auto GetSystemsWithComponent(std::tuple<S...>& tuple) {
        return std::tuple_cat(GetSystemsWithComponentInternal<S, Component>(tuple)...);
    }
    
};

}
