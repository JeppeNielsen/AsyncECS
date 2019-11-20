#pragma once
#include <string>

namespace AsyncECS {
class ClassNameHelper {

    template<typename Class>
    static std::string GetNameInternal() {
#ifdef WIN32
        const std::string functionName = __FUNCTION__;
#else
        const std::string functionName = __PRETTY_FUNCTION__;
#endif
        const std::string token = "Class = ";
        size_t equal = functionName.find(token) + token.size();
        return functionName.substr(equal, functionName.size() - equal - 1);
    }
    
public:
    template<typename T>
    static std::string& GetName() {
        static std::string name = GetNameInternal<T>();
        return name;
    }
};
}
