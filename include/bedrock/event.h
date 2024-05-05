#ifndef BEDROCK_EVENT_H
#define BEDROCK_EVENT_H

#include <functional>
#include <vector>

namespace Bedrock{

    template<typename... Args>
    class Event{
    private:
        std::unordered_map<uint64_t, std::function<void(Args...)>> callbacks;

        static inline uint64_t hash(uint64_t a, uint64_t b){
            return a ^ (b << 1 | b >> 63);
        }
    public:
        template<typename T>
        void subscribe(T* obj,void(T::*func)(Args...)){
            uintptr_t funcPtrVal = 0;
            auto objPtrVal = reinterpret_cast<uintptr_t >(obj);
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, objPtrVal);

            callbacks[hashcode] = [=](Args... args){ (obj->*func)(args...); };
        }

        void subscribe(void(*func)(Args...)){
            uintptr_t funcPtrVal = 0;
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, funcPtrVal);

            callbacks[hashcode] = [=](Args... args){ func(args...); };
        }

        template<typename T>
        void unsubscribe(T* obj,void(T::*func)(Args...)){
            uintptr_t funcPtrVal = 0;
            auto objPtrVal = reinterpret_cast<uintptr_t >(obj);
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, objPtrVal);

            auto it = callbacks.find(hashcode);
            if(it != callbacks.end()){
                callbacks.erase(hashcode);
            }
        }

        void unsubscribe(void(*func)(Args...)){
            uintptr_t funcPtrVal = 0;
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, funcPtrVal);

            auto it = callbacks.find(hashcode);
            if(it != callbacks.end()){
                callbacks.erase(hashcode);
            }
        }

        void invoke(Args... args){
            for(auto& pair : callbacks){
                pair.second(args...);
            }
        }

        int count(){
            return callbacks.size();
        }

        void clear(){
            callbacks.clear();
        }
    };

    template<>
    class Event<>{
    private:
        std::unordered_map<uint64_t, std::function<void()>> callbacks;

        static inline uint64_t hash(uint64_t a, uint64_t b){
            return a ^ (b << 1 | b >> 63);
        }
    public:


        template<typename T>
        void subscribe(T* obj,void(T::*func)()){
            uintptr_t funcPtrVal = 0;
            auto objPtrVal = reinterpret_cast<uintptr_t >(obj);
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, objPtrVal);

            callbacks[hashcode] = [=]() -> void { (obj->*func)(); };
        }

        void subscribe(void(*func)()){
            uintptr_t funcPtrVal = 0;
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, funcPtrVal);

            callbacks[hashcode] = [=]() -> void{ func(); };
        }

        template<typename T>
        void unsubscribe(T* obj,void(T::*func)()){
            uintptr_t funcPtrVal = 0;
            uintptr_t objPtrVal = reinterpret_cast<uintptr_t >(obj);
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, objPtrVal);

            auto it = callbacks.find(hashcode);
            if(it != callbacks.end()){
                callbacks.erase(hashcode);
            }
        }

        void unsubscribe(void(*func)()){
            uintptr_t funcPtrVal = 0;
            std::memcpy(&funcPtrVal, &func, sizeof(func));
            uint64_t hashcode = hash(funcPtrVal, funcPtrVal);

            auto it = callbacks.find(hashcode);
            if(it != callbacks.end()){
                callbacks.erase(hashcode);
            }
        }

        void invoke(){
            for(auto& pair : callbacks){
                pair.second();
            }
        }

        void clear(){
            callbacks.clear();
        }
    };
}

#endif //BEDROCK_EVENT_H
