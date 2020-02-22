//
//  SystemBase.h
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "TupleHelper.hpp"

namespace AsyncECS {

template<typename...T>
struct SystemBase {

    template<typename O>
    std::tuple<O*> GetComponentType(const O* ptr) const {
        return std::make_tuple((O*)nullptr);
    }
    
    template<typename O>
    std::tuple<O*> GetComponentType(O* ptr) const {
        return std::make_tuple((O*)1);
    }

    std::tuple<T*...> GetComponentTypes() {
        return std::tuple_cat(GetComponentType((T*)nullptr)...);
    }
    
    template<typename ComponentType>
    bool HasComponentType() {
        return TupleHelper::HasType<ComponentType, std::tuple<T...> >::value;
    }

    template<typename O, typename Components>
    const O& Get(const GameObject gameObject, Components& components, const O* ptr) const {
        return (const O&)std::get<ComponentContainer<std::remove_const_t<O>>>(components).GetConst(gameObject);
    }
    
    template<typename O, typename Components>
    O& Get(const GameObject gameObject, Components& components, O* ptr) const {
        return (O&)((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).GetNoChange(gameObject);
    }
   
    template<typename Components>
    std::tuple<T&...> GetComponentValuesFromGameObject(const GameObject gameObject, Components& components) const {
        return std::tie(Get(gameObject, components, (T*)nullptr)...);
    }
    
    template<typename O, typename Components>
    void ChangeComponent(const GameObject gameObject, Components& components, const O* ptr) const {
        /*empty*/ // no change for const components
    }
    
    template<typename O, typename Components>
    void ChangeComponent(const GameObject gameObject, Components& components, O* ptr) const {
        ((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).SetChanged(gameObject);
    }
    
    template<typename Components>
    void ChangeComponents(const GameObject gameObject, Components& components) const {
        using expander = int[];
        (void) expander { 0, ((void)ChangeComponent(gameObject, components, (T*)nullptr),0)... };
    }
    
    template<typename Components, typename ComponentObjects>
    GameObjectCollection& GetObjects(ComponentObjects& componentObjects) const {
        const int numElements = sizeof...(T);
        
        int indicies[] { TupleHelper::Index<ComponentContainer<std::remove_const_t<std::remove_pointer_t<T>>>, Components>::value... };
        
        int foundIndex = indicies[0];
        int min = (int)componentObjects[foundIndex].objects.size();
        for(int i = 1; i < numElements; ++i) {
            int index = indicies[i];
            int size =  (int)componentObjects[index].objects.size();
            if (size < min) {
                min =  size;
                foundIndex = index;
            }
        }
        return componentObjects[foundIndex];
    }
    
    template<typename Components>
    const GameObjectCollection::Objects& GetChangedObjects(const Components& components) const {
        const int numElements = sizeof...(T);
        
        const GameObjectCollection::Objects* objects[] { &std::get<ComponentContainer<std::remove_const_t<T>>>(components).changedThisFrame.objects... };
        
        int max = (int)objects[0]->size();
        int foundIndex = 0;
        for(int i = 1; i < numElements; ++i) {
            int size = (int)objects[i]->size();
            if (size>max) {
                foundIndex = i;
                max = size;
            }
        }
        return *objects[foundIndex];
    }
};

}