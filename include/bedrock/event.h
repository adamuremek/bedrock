#ifndef BEDROCK_EVENT_H
#define BEDROCK_EVENT_H

#include <functional>
#include <vector>

template<typename... Args>
class Event{
private:
    using Callback = std::function<void(Args...)>;
    std::vector<Callback> subscribers;
public:
    void subscribe(const Callback& subscriber){
        subscribers.push_back(subscriber);
    }

    void operator+=(const Callback& subscriber){
        subscribe(subscriber);
    }

    void invoke(Args... args){
        for(auto& subscriber : subscribers){
            subscriber(args...);
        }
    }

    void clear(){
        subscribers.clear();
    }

};

template<>
class Event<>{
private:
    using Callback = std::function<void()>;
    std::vector<Callback> subscribers;
public:
    void subscribe(const Callback& subscriber){
        subscribers.push_back(subscriber);
    }

    void operator+=(const Callback& subscriber){
        subscribe(subscriber);
    }

    void invoke(){
        for(auto& subscriber : subscribers){
            subscriber();
        }
    }

    void clear(){
        subscribers.clear();
    }

};

#endif //BEDROCK_EVENT_H
