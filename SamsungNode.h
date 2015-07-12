#pragma once

#include "Optional.h"
#include "NonNullPtr.h"
#include <type_traits>
#include <utility>
#include <deque>
#include <string>

namespace Samsung {

    template <typename T>
    class List;

    template <typename T>
    class ListNode
    {
    public:
        bool is_terminal() const { return !body_; }
        ListNode<T>& get_next() // an non terminal node can not have a nullptr as next
        {
            assert(body_);
            return *body_->next_;
        }
        const ListNode<T>& get_next() const 
        {
            assert(body_);
            return *body_->next_;
        }
        void set_next(ListNode<T>& rhs)
        {
            assert(body_);
            body_->next_ = rhs;
        }
        T& get_val()
        {
            assert(body_);
            return body_->val_;
        }
        const T& get_val() const
        {
            assert(body_);
            return body_->val_;
        }

        std::string dbg2string() const
        {
            using namespace std;
            if (!body_)
                return "Terminator";
            return to_string(body_->val_);
        }

        // Once again this type of emplace constructor might be questionable
        // but it's seems reasonalbe in this situation
        template <typename ...Args>
        ListNode(ListNode<T>& next,
            Args&&... args) :
            body_
        {
            next,
            std::forward<Args>(args)...
        } {}



        ListNode() = default; // creating a terminator node
        ListNode(const ListNode<T>&) = default;
        ListNode<T>& operator= (const ListNode<T>&) = default;
        ListNode(ListNode<T>&&) noexcept(std::is_nothrow_move_constructible<T>::value) = default;
        ListNode<T>& operator= (ListNode<T>&&) noexcept(std::is_nothrow_move_assignable<T>::value &&
            std::is_nothrow_move_constructible<T>::value) = default;
    private:
        template <typename>
        friend class List;

        // TODO: think, put ListNode inside List class??
        // TODO: think: make a stadard iterator alike?
        
        struct Body
        {
            template <typename ...Args>
            Body (ListNode<T>& next, Args&&... args):
                val_ {std::forward<Args>(args)...},
                next_ {next}
            {}
            T val_;
            Tools::NonNullPtr<ListNode<T>> next_;
        };
        Tools::Optional<Body> body_; // none indicates node terminal
    };
}

