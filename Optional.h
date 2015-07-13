#pragma once
#include <utility>
#include <type_traits>
#include <cassert>

namespace Tools {
    /*
     * Boost is not particulary friendy with move semantics
     *
     * TODO: think, this optional has missdesigned forwarding
     * constructor, if one is to write generic code
     * and do smth like Optional<T>{std::forward<Args>(args)...}
     * in case of zero args, there would be no value!
     */

    template <typename T>
    class Optional
    {
    public:
        Optional() {}

        // templated constructors is always a questionable thing
        // but in this case, it seems reasonable
        template <typename First, typename ...Args>
        Optional(First&& first, Args&&... args)
        {
            new (&memory_.val_)T(std::forward<First>(first), std::forward<Args>(args)...);
            is_valid_ = true;
        }
        template <typename ...Args>
        void emplace(Args&&... args)
        {
            destroy();
            new (&memory_.val_)T(std::forward<Args>(args)...);
            is_valid_ = true;
        }
        Optional(const Optional<T>& rhs)
        {
            if (rhs.is_valid_)
                new (&memory_.val)T(rhs.val_);
        }
        Optional<T>& operator= (const Optional<T>& rhs)
        {
            auto tmp = rhs;
            *this = std::move(tmp);
            return *this;
        }
        Optional(Optional<T>&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
        {
            if (rhs.is_valid_) {
                new (&memory_.val)T(std::move(rhs.memory_.val_));
                rhs.destroy(); // if an optional has been moved out, it's a null state
            }
        }

        Optional<T>& operator= (Optional<T>&& rhs) noexcept(
            std::is_nothrow_move_constructible<T>::value &&
            std::is_nothrow_move_assignable<T>::value)
        {
            if (is_valid_ && rhs.is_valid_) // move assigment might be more efficient
                                            // then move construction
            {
                memory_.val_ = std::move(rhs.memory_.val_);
                rhs.destroy(); // a questionable thing, but this gives a common behavior, 
                               // if an optional has been moved out
                               // it's in a null state
                return *this;
            }
            if (is_valid_) {
                destroy();
                return *this;
            }
            if (rhs.is_valid_) {
                new (&memory_.val_)T(std::move(rhs.memory_.val_));
                rhs.destroy();
                return *this;
            }
            return *this;
        }
        
        T& operator*()
        {
            assert(is_valid_);
            return memory_.val_;
        }
        const T& operator*() const {
            assert(is_valid_);
            return memory_.val_;
        }

        T* operator->() {
            assert(is_valid_);
            return &memory_.val_;
        }
        const T* operator->() const {
            assert(is_valid_);
            return &memory_.val_;
        }

        // Horray to c++11 and new way of writing safe bool idiom
        explicit operator bool() const {
            return is_valid_;
        }

        
        ~Optional()
        {
            destroy();
        }

    private:
        void destroy() noexcept
        {
            if (is_valid_)
                memory_.val_.~T();
            is_valid_ = false;
        }
        union U // This should provide properly aligned storrage
        {
            T val_;
            U() {} // it's deleted otherwise
        } memory_;
        bool is_valid_ = false;
    };
}

