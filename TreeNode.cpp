#include "TreeNode.h"

template<class T>
TreeNode<T>::TreeNode (T value):value (value) {
    // creates treenode with difined value
    first_child_ = nullptr;
    next_brother_ = nullptr;
}

template<class T>
TreeNode<T>::~TreeNode () {
    // deletes all the nodes connected to itself
    delete first_child_;
    delete next_brother_;
}

template<class T>
const TreeNode<T> *TreeNode<T>::first_child () const {
    // returns pointer to first child as to const
    return first_child_;
}

template<class T>
const TreeNode<T> *TreeNode<T>::next_brother () const {
    // returns pointer to next brother as to const
    return next_brother_;
}

template<class T>
TreeNode<T> *TreeNode<T>::last_child_ () {
    // returns pointer to last child
    TreeNode<T> *curr = first_child_;
    while (curr->next_brother () != nullptr) {
        curr = curr->next_brother ();
    }
    return curr;
}

template<class T>
const TreeNode<T> *TreeNode<T>::last_child () const {
    // returns pointer to last child as to const
    return last_child_ ();
}

template<class T>
const TreeNode<T> *TreeNode<T>::add_child (TreeNode<T> *t) {
    // adds child to node
    last_child_ ()->next_brother_ = t;
    return t;
}
