//
//  SystemDependencies.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "TupleHelper.hpp"

namespace AsyncECS {

template<typename ...T>
struct SystemDependencies {

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
    
    std::tuple<T*...> GetDependencyTypes() {
        return std::tuple<T*...> {};
    }
};

using NoSystemDependencies = SystemDependencies<>;

}
