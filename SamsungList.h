#pragma once

/* List from samsung interview task.
 *
 * This class has to have 3 properties:
 * 1) we should be able to remove a node, without
 *    link to the head
 * 2) we should be able to revert it
 * 3) it might have loops, and we should be able to detect those
 *
 */

/*
  push_back and push_front will not invalidate any references to a deque's elements.
  http://stackoverflow.com/questions/913070/why-does-push-back-or-push-front-invalidate-a-deques-iterators
  One of my mistakes on the interview, was using vector. I knew about pointer invalidation
  just didn't have much time to think
*/
#include <deque> // for memory managment
#include <utility>
#include <iostream>
/*
 * Cause we might have loops, we can not put memory managment into
 * the node class itself, like in traditional list
 * so, instead, we'll have some external memory manegment facility
 */

#include "SamsungNode.h"
#include "NonNullPtr.h"
#include <iterator>

namespace Samsung {
    template <typename T>
    class List
    {
    public:
        using Node = ListNode<T>;
        template <typename ...Args>
        void emplace_front(Args&&... args)
        {
            memory_.emplace_back(*head_, std::forward<Args>(args)...);
            head_ = memory_.back();
        }
        List() : memory_ {1}, // creating one terminal node
                 head_ { memory_.back() }
        {}
        template <typename It>
        List(It first, It last);

        List(std::initializer_list<T> in_lst) :
            List(in_lst.begin(), in_lst.end())
        {}
        void remove(Node& rhs)
        {
            assert(!rhs.is_terminal());
            rhs = std::move(rhs.get_next());
        }
        Node& get_head() {
            return *head_;
        }
        Node* has_loops() const // returns some node in a loop
                                // if there is one
        {
            if (head_->is_terminal())
                return nullptr;
            auto one_step = head_;
            auto two_step = head_;
            two_step = two_step->get_next();
            for (;;) {
                for (int i = 0; i < 2; ++i) {
                    if (two_step->is_terminal())
                        return nullptr;
                    if (two_step == one_step)
                        return two_step.get();
                    two_step = two_step->get_next();
                }
                one_step = one_step->get_next();
            }
            
        }
        void loop(Node& loop_end)
        {
            assert(!has_loops());
            assert(!head_->is_terminal());
            auto last = head_;
            while (!last->get_next().is_terminal())
                last = last->get_next();
            last->set_next(loop_end);
        }

        void reverse()
        {
            assert(!has_loops());
            if (head_->is_terminal()) return;
            auto new_head = Tools::NonNullPtr<Node>(memory_.front()); // the first element in memory ought to be an empty node
            while (!head_->is_terminal()) {
                auto tmp = head_;
                head_ = head_->get_next();
                tmp->set_next(*new_head);
                new_head = std::move(tmp);
            }
            head_ = std::move(new_head);
        }
        
        template <typename Functor>
        void for_each(Functor func) const // doesn't guarantie, that if a loop occured, each node will be processed once
                                          // TODO: provide non const version
        {
            auto in_loop = has_loops();
            auto iter = head_;
            if (in_loop) {
                int loop_iterations = 0;
                for (;;) // TODO: add conversions
                {
                    if (iter == Tools::NonNullPtr<Node>(*in_loop)) {
                        ++loop_iterations;
                        if (loop_iterations == 2)
                            return;
                    }

                    func(*iter);
                    iter = iter->get_next();
                }
            }
            else
                while (!iter->is_terminal())
                {
                    func(*iter);
                    iter = iter->get_next();
                }
        }
        
        void dbg_print () const
        {
            for_each(
                [](const Node& rhs) {
                std::cout << rhs.dbg2string() << ' ';
            });           
            std::cout << std::endl;
        }
    private:
        std::deque<Node> memory_; // we are using memory to create head, so members ought to be listed
                                  // in this order
        Tools::NonNullPtr<Node> head_;
        template <typename It>
        List(It first, It last, std::bidirectional_iterator_tag) : List()
        {
            if (last == first)
                return;
            --last;
            while (last != first) {
                emplace_front(*last);
                --last;
            }
            emplace_front(*last);
        }
        template <typename It>
        List(It first, It last, std::input_iterator_tag) : List()
        {
            while (first != last) {
                emplace_front(*first);
                ++first;
            }
            reverse();
        }
    };

    template<typename T>
    template <typename It>
    inline List<T>::List(It first, It last) : List(first, last, typename std::iterator_traits<It>::iterator_category())
    {}

}
