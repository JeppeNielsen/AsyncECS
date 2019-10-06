//
//  IdHelper.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 25/09/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once

template<typename T>
class IdHelper {
public:
    template<typename...Type>
    static const size_t GetId() {
        static std::size_t id = idCounter++;
        return id;
    }
private:
    static std::size_t idCounter;
};

template<typename T>
std::size_t IdHelper<T>::idCounter {0};
