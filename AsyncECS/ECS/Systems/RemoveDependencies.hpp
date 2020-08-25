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
    HAS_METHOD(IsRemoveDependencies)
}

template<typename ...T>
struct RemoveDependencies {

    void IsRemoveDependencies() {
       
    }

    std::tuple<T*...> GetDependenciesTypesToRemove() {
        return std::tuple<T*...> {};
    }
};

}
