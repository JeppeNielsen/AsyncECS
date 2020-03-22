//
//  SystemDependencies.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "TupleHelper.hpp"
#include "HasMethodHelper.hpp"

namespace AsyncECS {

namespace Internal {
    HAS_METHOD(IsSystemDependencies)
}

template<typename ...T>
struct SystemDependencies {

    void IsSystemDependencies() {
       
    }

    template<typename B>
    static constexpr B GetDependencies() {
        return B();
    }

    static constexpr std::tuple<T...> GetDependencies() {
        return std::make_tuple(GetDependencies<T>()...);
    }
    
    template<typename B>
    static constexpr auto GetDependenciesRecursiveInternal() {
        if constexpr (AsyncECS::Internal::has_IsSystemDependencies<B, void()>::value) {
            return std::tuple_cat(B::GetDependenciesRecursive(), std::tuple<B>{});
        } else {
            return std::tuple<B>{};
        }
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

}
