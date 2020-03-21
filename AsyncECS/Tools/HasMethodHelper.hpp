//
//  HasMethodHelper.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 16/03/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once

#define HAS_METHOD(methodName) \
  template<typename, typename T> \
  struct has_##methodName { \
      static_assert( \
          std::integral_constant<T, false>::value, \
          "Second template parameter needs to be of function type."); \
  };\
    \
  template<typename C, typename Ret, typename... Args> \
  struct has_##methodName<C, Ret(Args...)> { \
  private: \
      template<typename T> \
      static constexpr auto check(T*) \
      -> typename\
          std::is_same< \
              decltype( std::declval<T>().methodName ( std::declval<Args>()... ) ), \
              Ret   \
          >::type;  \
                        \
      template<typename>\
      static constexpr std::false_type check(...);\
                \
      typedef decltype(check<C>(0)) type;\
        \
  public:\
      static constexpr bool value = type::value;\
  };
  
