#ifndef TESTS_BEHAVIOR_REGISTRY_H
#define TESTS_BEHAVIOR_REGISTRY_H

#include <iostream>
#include <map>
#include <functional>
#include <string>


class BehaviorRegistry{
private:
    std::map<std::string, std::function<void()>> functions;
    BehaviorRegistry(){}
public:


    static BehaviorRegistry& getInstance(){
        static BehaviorRegistry instance;
        return instance;
    }

    void registerBehavior(const std::string& name, std::function<void()> func){
        functions.insert(std::make_pair(name, func));
    }

    void useBehavior(const std::string& name){
        auto it = functions.find(name);
        if(it != functions.end()){
            it->second();
        }else{
            std::cerr << "Behavior not found: " << name << std::endl;
        }
    }
};


inline void registerBehavior(const std::string& name, std::function<void()> func){
    BehaviorRegistry::getInstance().registerBehavior(name, func);
}

inline void useBehavior(const std::string& name){
    BehaviorRegistry::getInstance().useBehavior(name);
}


#endif //TESTS_BEHAVIOR_REGISTRY_H
