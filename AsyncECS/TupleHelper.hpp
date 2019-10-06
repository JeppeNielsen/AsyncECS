//
//  TupleHelper.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 25/09/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <tuple>
#include <type_traits>

namespace TupleHelper {

    template<class F, class...Ts, std::size_t...Is>
    constexpr void for_each_in_tuple(const std::tuple<Ts...> & tuple, F func, std::index_sequence<Is...>){
        using expander = int[];
        (void)expander { 0, ((void)func((Ts&)std::get<Is>(tuple)), 0)... };
    }

    template<class F, class...Ts>
    constexpr void Iterate(const std::tuple<Ts...> & tuple, F func){
        for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
    }
    
    template <class Haystack, class Needle>
    struct contains;

    template <class Car, class... Cdr, class Needle>
    struct contains<std::tuple<Car, Cdr...>, Needle> : contains<std::tuple<Cdr...>, Needle>
    {};

    template <class... Cdr, class Needle>
    struct contains<std::tuple<Needle, Cdr...>, Needle> : std::true_type
    {};

    template <class Needle>
    struct contains<std::tuple<>, Needle> : std::false_type
    {};



    template <class Out, class In>
    struct filter;

    template <class... Out, class InCar, class... InCdr>
    struct filter<std::tuple<Out...>, std::tuple<InCar, InCdr...>>
    {
      using type = typename std::conditional<
        contains<std::tuple<Out...>, InCar>::value
        , typename filter<std::tuple<Out...>, std::tuple<InCdr...>>::type
        , typename filter<std::tuple<Out..., InCar>, std::tuple<InCdr...>>::type
      >::type;
    };

    template <class Out>
    struct filter<Out, std::tuple<>>
    {
      using type = Out;
    };


    template <class T>
    using without_duplicates = typename filter<std::tuple<>, T>::type;
    
    
}
