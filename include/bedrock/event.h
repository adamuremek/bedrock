#ifndef BEDROCK_EVENT_H
#define BEDROCK_EVENT_H

#include <functional>
#include <unordered_map>
#include <stack>
#include <memory>

namespace Bedrock {
    template<typename ReturnType, typename... Args>
    class EventCallback {
    private:
        inline static std::stack<uint32_t> freeUIDs{};
        std::function<ReturnType(Args...)> callback;
        uint32_t UID{0};

        // Generate UID for every EventCallback instance.
        static uint32_t generateUID() {
            static uint32_t nextUID{1};
            if (!freeUIDs.empty()) {
                uint32_t top = freeUIDs.top();
                freeUIDs.pop();
                return top;
            }
            return nextUID++;
        }

    public:
        // Default constructor
        EventCallback() : UID{generateUID()} {}

        // Explicitly define copy constructor
        EventCallback(const EventCallback &other) : callback(other.callback), UID{other.getUID()} {};

        // Explicitly copy assignment operator
        EventCallback& operator=(const EventCallback& other) {
            if (this != &other) {
                callback = other.callback;
                UID = other.UID;
            }
            return *this;
        }

        // Existing move constructor and move assignment
        EventCallback(EventCallback&& other) noexcept = default;
        EventCallback& operator=(EventCallback&& other) noexcept = default;

        ~EventCallback() { freeUIDs.push(UID); }

        // Constructor for callable objects, function pointers, and lambdas
        template<typename Callable,
                typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, EventCallback>>>
        explicit EventCallback(Callable &&func) : callback(std::forward<Callable>(func)), UID{generateUID()} {}

        // Constructor for member function pointers
        template<typename T>
        EventCallback(T *obj, ReturnType (T::*memFunc)(Args...)) : callback([=](Args... args) -> ReturnType {
            return (obj->*memFunc)(std::forward<Args>(args)...);
        }), UID{generateUID()} {}

        // Invoke the stored callback
        ReturnType operator()(Args... args) const {
            return callback(std::forward<Args>(args)...);
        }

        [[nodiscard]] inline uint32_t getUID() const { return UID; }
    };


    template<typename ReturnType, typename... Args>
    class Event {
    private:
        std::unordered_map<uint32_t, EventCallback<ReturnType, Args...>> callbacks;
    public:
        // Overload for lvalues: copies the callback
        void subscribe(const EventCallback<ReturnType, Args...>& callback) {
            callbacks[callback.getUID()] = callback;
        }

        // Overload for rvalues: moves the callback
        void subscribe(EventCallback<ReturnType, Args...>&& callback) {
            callbacks.emplace(callback.getUID(), std::move(callback));
        }

        void unsubscribe(const EventCallback<ReturnType, Args...>& callback) {
            callbacks.erase(callback.getUID());
        }

        void invoke(Args... args){
            for(auto& pair : callbacks){
                pair.second(std::forward<Args>(args)...);
            }
        }

        size_t count() { return callbacks.size(); }

        void clear() { callbacks.clear(); }
    };
}

#endif //BEDROCK_EVENT_H
