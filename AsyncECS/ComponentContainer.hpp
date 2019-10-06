//
//  Container.hpp
//  ECS
//
//  Created by Jeppe Nielsen on 25/07/2018.
//  Copyright © 2018 Jeppe Nielsen. All rights reserved.
//

#pragma once

#include <vector>
#include <assert.h>
#include <limits>

template<typename T>
struct ComponentContainer {

    const std::uint32_t NUllIndex = std::numeric_limits<std::uint32_t>::max();

    ComponentContainer() = default;
    virtual ~ComponentContainer() = default;

    ComponentContainer(const ComponentContainer &) = delete;
    ComponentContainer(ComponentContainer &&) = default;

    ComponentContainer & operator=(const ComponentContainer &) = delete;
    ComponentContainer & operator=(ComponentContainer &&) = default;
    
    bool Contains(const int index) const {
        return index<indicies.size() && indicies[index] != NUllIndex;
    }
    
    void CreateIndex(const int index) {
        assert(!Contains(index));
        if (index>=indicies.size()) {
            indicies.resize(index + 1, NUllIndex);
        }
        indicies[index] = (std::uint32_t)elements.size();
        references.emplace_back(1);
    }
    
    void Create(const int index) {
        CreateIndex(index);
        elements.resize(elements.size() + 1);
    }
    
    template<typename... Args>
    std::enable_if_t<!std::is_constructible<T, Args...>::value, void>
    Create(const int index, Args&& ... args) {
        CreateIndex(index);
        elements.emplace_back(T{std::forward<Args>(args)...});
    }
    
    template<typename... Args>
    std::enable_if_t<std::is_constructible<T, Args...>::value, void>
    Create(const int index, Args&& ... args) {
        CreateIndex(index);
        elements.emplace_back(std::forward<Args>(args)...);
    }
    
    void Reference(const int index, const int referenceIndex) {
        assert(!Contains(index));
        assert(Contains(referenceIndex));
        if (index>=indicies.size()) {
            indicies.resize(index + 1, NUllIndex);
        }
        
        indicies[index] = referenceIndex;
        ++references[referenceIndex];
    }
    
    void Destroy(const int index) {
        assert(Contains(index));
        
        if ((references[index]--)==0) {
            auto tmp = std::move(elements.back());
            elements[index] = std::move(tmp);
            elements.pop_back();
            references[index] = references.back();
            references.pop_back();
            indicies[indicies.size() - 1] = index;
        }
        
        indicies[index] = NUllIndex;
    }
    
    T& Get(const int index) const {
        return (T&)elements[indicies[index]];
    }
    
    std::vector<T> elements;
    std::vector<std::uint32_t> indicies;
    std::vector<std::uint16_t> references;
};
