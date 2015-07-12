#pragma once

#include <cassert>

namespace Tools {

    // It's usefull to have such restrictions be expressed
    // derectly in code, either then in thought
    //
    // I don't fancy std::reference_wrapper, because it has an implisit cast
    template <typename T>
    class NonNullPtr
    {
    public:
        NonNullPtr(T& rhs) : body_{ &rhs } {}
        T* get() { return body_; }
        const T* get() const { return body_; }
        T& operator*() { return *body_; }
        const T& operator*() const { return *body_; }
        T* operator->() { return body_; }
        const T* operator->() const { return body_; }
    private:
        T* body_;
    };
    template <typename T>
    bool operator==(const NonNullPtr<T>& lhs, const NonNullPtr<T>& rhs)
    {
        return lhs.get() == rhs.get();
    }
    template <typename T>
    bool operator!=(const NonNullPtr<T>& lhs, const NonNullPtr<T>& rhs)
    {
        return lhs.get() != rhs.get();
    }
}

