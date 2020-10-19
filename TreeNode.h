#ifndef INTERFACE4__TREENODE_H_
#define INTERFACE4__TREENODE_H_

template<class T>
class TreeNode {
  explicit TreeNode (T value);
  TreeNode (const TreeNode<T> &t);
  TreeNode &operator= (const TreeNode<T> &t);
  ~TreeNode ();
  const TreeNode<T> *first_child () const;
  const TreeNode<T> *next_brother () const;
  const TreeNode<T> *last_child () const;
  const TreeNode<T> *add_child (TreeNode<T> *t);
  T value;
 private:
  TreeNode<T> *last_child_ ();
  TreeNode<T> *first_child_;
  TreeNode<T> *next_brother_;
};

#endif //INTERFACE4__TREENODE_H_
