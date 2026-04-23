/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
  public:
   typedef pair<const Key, T> value_type;

  private:
   struct Node {
       value_type *data;
       Node *left;
       Node *right;
       Node *parent;
       int height;

       Node() : data(nullptr), left(nullptr), right(nullptr), parent(nullptr), height(1) {}

       Node(const Key &k, const T &v, Node *p = nullptr)
           : data(new value_type(k, v)), left(nullptr), right(nullptr), parent(p), height(1) {}

       Node(const value_type &val, Node *p = nullptr)
           : data(new value_type(val)), left(nullptr), right(nullptr), parent(p), height(1) {}

       ~Node() {
           if (data) {
               delete data;
               data = nullptr;
           }
       }
   };

   Node *root;
   size_t num_elements;
   Compare comp;

   int getHeight(Node *node) const {
       return node ? node->height : 0;
   }

   void updateHeight(Node *node) {
       if (node) {
           int lh = getHeight(node->left);
           int rh = getHeight(node->right);
           node->height = (lh > rh ? lh : rh) + 1;
       }
   }

   int getBalance(Node *node) const {
       return node ? getHeight(node->left) - getHeight(node->right) : 0;
   }

   Node *rotateRight(Node *y) {
       Node *x = y->left;
       Node *T2 = x->right;

       x->right = y;
       y->left = T2;

       if (T2) T2->parent = y;
       x->parent = y->parent;
       y->parent = x;

       if (x->parent) {
           if (x->parent->left == y) x->parent->left = x;
           else x->parent->right = x;
       } else {
           root = x;
       }

       updateHeight(y);
       updateHeight(x);
       return x;
   }

   Node *rotateLeft(Node *x) {
       Node *y = x->right;
       Node *T2 = y->left;

       y->left = x;
       x->right = T2;

       if (T2) T2->parent = x;
       y->parent = x->parent;
       x->parent = y;

       if (y->parent) {
           if (y->parent->left == x) y->parent->left = y;
           else y->parent->right = y;
       } else {
           root = y;
       }

       updateHeight(x);
       updateHeight(y);
       return y;
   }

   Node *balance(Node *node) {
       if (!node) return nullptr;

       updateHeight(node);
       int balance = getBalance(node);

       if (balance > 1) {
           if (getBalance(node->left) >= 0) {
               return rotateRight(node);
           } else {
               node->left = rotateLeft(node->left);
               return rotateRight(node);
           }
       }

       if (balance < -1) {
           if (getBalance(node->right) <= 0) {
               return rotateLeft(node);
           } else {
               node->right = rotateRight(node->right);
               return rotateLeft(node);
           }
       }

       return node;
   }

   Node *findNode(Node *node, const Key &key) const {
       while (node) {
           if (comp(key, node->data->first)) {
               node = node->left;
           } else if (comp(node->data->first, key)) {
               node = node->right;
           } else {
               return node;
           }
       }
       return nullptr;
   }

   Node *findMin(Node *node) const {
       while (node && node->left) {
           node = node->left;
       }
       return node;
   }

   Node *findMax(Node *node) const {
       while (node && node->right) {
           node = node->right;
       }
       return node;
   }

   void clearTree(Node *node) {
       if (node) {
           clearTree(node->left);
           clearTree(node->right);
           delete node;
       }
   }

   Node *copyTree(Node *src, Node *parent) {
       if (!src) return nullptr;
       Node *node = new Node(*src->data, parent);
       node->height = src->height;
       node->left = copyTree(src->left, node);
       node->right = copyTree(src->right, node);
       return node;
   }

   Node *insertRec(Node *node, const value_type &value, Node *parent, Node *&newNodePtr, bool &inserted) {
       if (!node) {
           num_elements++;
           Node *newNode = new Node(value, parent);
           newNodePtr = newNode;
           inserted = true;
           return newNode;
       }

       if (comp(value.first, node->data->first)) {
           node->left = insertRec(node->left, value, node, newNodePtr, inserted);
           if (node->left) node->left->parent = node;
       } else if (comp(node->data->first, value.first)) {
           node->right = insertRec(node->right, value, node, newNodePtr, inserted);
           if (node->right) node->right->parent = node;
       } else {
           return node;
       }

       return balance(node);
   }

   Node *eraseRec(Node *node, const Key &key, bool &found) {
       if (!node) {
           found = false;
           return nullptr;
       }

       if (comp(key, node->data->first)) {
           node->left = eraseRec(node->left, key, found);
       } else if (comp(node->data->first, key)) {
           node->right = eraseRec(node->right, key, found);
       } else {
           found = true;
           num_elements--;

           if (!node->left || !node->right) {
               Node *child = node->left ? node->left : node->right;
               Node *parent = node->parent;

               if (!child) {
                   if (parent) {
                       if (parent->left == node) parent->left = nullptr;
                       else parent->right = nullptr;
                   }
                   delete node;
                   node = nullptr;
               } else {
                   child->parent = parent;
                   if (parent) {
                       if (parent->left == node) parent->left = child;
                       else parent->right = child;
                   }
                   delete node;
                   node = child;
               }
           } else {
               Node *successor = findMin(node->right);
               value_type *oldData = node->data;
               node->data = new value_type(*successor->data);
               delete oldData;

			Node *succParent = successor->parent;
			Node *succChild = successor->right;

			if (succChild) succChild->parent = succParent;

			if (succParent == node) {
				succParent->right = succChild;
			} else {
				succParent->left = succChild;
			}

			delete successor;
           }
       }

       if (!node) return nullptr;

       return balance(node);
   }

  public:
   class const_iterator;
   class iterator {
      private:
       Node *ptr;
       map *container;

      public:
       iterator() : ptr(nullptr), container(nullptr) {}

       iterator(Node *p, map *c) : ptr(p), container(c) {}

       iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

       iterator operator++(int) {
           if (!ptr || !container) throw invalid_iterator();
           iterator temp = *this;
           ++(*this);
           return temp;
       }

       iterator &operator++() {
           if (!ptr || !container) throw invalid_iterator();
           if (ptr == container->end().ptr) throw invalid_iterator();

           if (ptr->right) {
               ptr = ptr->right;
               while (ptr->left) ptr = ptr->left;
           } else {
               Node *parent = ptr->parent;
               while (parent && ptr == parent->right) {
                   ptr = parent;
                   parent = parent->parent;
               }
               ptr = parent;
           }
           return *this;
       }

       iterator operator--(int) {
           if (!ptr || !container) throw invalid_iterator();
           iterator temp = *this;
           --(*this);
           return temp;
       }

       iterator &operator--() {
           if (!container) throw invalid_iterator();

           if (!ptr) {
               ptr = container->findMax(container->root);
           } else if (ptr->left) {
               ptr = ptr->left;
               while (ptr->right) ptr = ptr->right;
           } else {
               Node *parent = ptr->parent;
               while (parent && ptr == parent->left) {
                   ptr = parent;
                   parent = parent->parent;
               }
               ptr = parent;
           }

           if (!ptr) throw invalid_iterator();
           return *this;
       }

       value_type &operator*() const {
           if (!ptr) throw invalid_iterator();
           return *ptr->data;
       }

       bool operator==(const iterator &rhs) const {
           return ptr == rhs.ptr && container == rhs.container;
       }

       bool operator==(const const_iterator &rhs) const {
           return ptr == rhs.ptr && container == rhs.container;
       }

       bool operator!=(const iterator &rhs) const {
           return ptr != rhs.ptr || container != rhs.container;
       }

       bool operator!=(const const_iterator &rhs) const {
           return ptr != rhs.ptr || container != rhs.container;
       }

       value_type *operator->() const noexcept {
           return ptr->data;
       }

       friend class map;
       friend class const_iterator;
   };

   class const_iterator {
      private:
       Node *ptr;
       const map *container;

      public:
       const_iterator() : ptr(nullptr), container(nullptr) {}

       const_iterator(Node *p, const map *c) : ptr(p), container(c) {}

       const_iterator(const const_iterator &other) : ptr(other.ptr), container(other.container) {}

       const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

       const_iterator operator++(int) {
           if (!ptr || !container) throw invalid_iterator();
           const_iterator temp = *this;
           ++(*this);
           return temp;
       }

       const_iterator &operator++() {
           if (!ptr || !container) throw invalid_iterator();
           if (ptr == container->cend().ptr) throw invalid_iterator();

           if (ptr->right) {
               ptr = ptr->right;
               while (ptr->left) ptr = ptr->left;
           } else {
               Node *parent = ptr->parent;
               while (parent && ptr == parent->right) {
                   ptr = parent;
                   parent = parent->parent;
               }
               ptr = parent;
           }
           return *this;
       }

       const_iterator operator--(int) {
           if (!ptr || !container) throw invalid_iterator();
           const_iterator temp = *this;
           --(*this);
           return temp;
       }

       const_iterator &operator--() {
           if (!container) throw invalid_iterator();

           if (!ptr) {
               ptr = container->findMax(container->root);
           } else if (ptr->left) {
               ptr = ptr->left;
               while (ptr->right) ptr = ptr->right;
           } else {
               Node *parent = ptr->parent;
               while (parent && ptr == parent->left) {
                   ptr = parent;
                   parent = parent->parent;
               }
               ptr = parent;
           }

           if (!ptr) throw invalid_iterator();
           return *this;
       }

       const value_type &operator*() const {
           if (!ptr) throw invalid_iterator();
           return *ptr->data;
       }

       bool operator==(const iterator &rhs) const {
           return ptr == rhs.ptr;
       }

       bool operator==(const const_iterator &rhs) const {
           return ptr == rhs.ptr;
       }

       bool operator!=(const iterator &rhs) const {
           return ptr != rhs.ptr || container != rhs.container;
       }

       bool operator!=(const const_iterator &rhs) const {
           return ptr != rhs.ptr || container != rhs.container;
       }

       const value_type *operator->() const noexcept {
           return ptr->data;
       }

       friend class map;
   };

   map() : root(nullptr), num_elements(0), comp() {}

   map(const map &other) : root(nullptr), num_elements(0), comp(other.comp) {
       root = copyTree(other.root, nullptr);
       num_elements = other.num_elements;
   }

   map &operator=(const map &other) {
       if (this != &other) {
           clear();
           root = copyTree(other.root, nullptr);
           num_elements = other.num_elements;
           comp = other.comp;
       }
       return *this;
   }

   ~map() {
       clear();
   }

   T &at(const Key &key) {
       Node *node = findNode(root, key);
       if (!node) throw index_out_of_bound();
       return node->data->second;
   }

   const T &at(const Key &key) const {
       Node *node = findNode(root, key);
       if (!node) throw index_out_of_bound();
       return node->data->second;
   }

   T &operator[](const Key &key) {
       Node *node = findNode(root, key);
       if (node) {
           return node->data->second;
       }
       value_type val(key, T());
       pair<iterator, bool> result = insert(val);
       return result.first->second;
   }

   const T &operator[](const Key &key) const {
       return at(key);
   }

   iterator begin() {
       return iterator(findMin(root), this);
   }

   const_iterator cbegin() const {
       return const_iterator(findMin(root), this);
   }

   iterator end() {
       return iterator(nullptr, this);
   }

   const_iterator cend() const {
       return const_iterator(nullptr, this);
   }

   bool empty() const {
       return num_elements == 0;
   }

   size_t size() const {
       return num_elements;
   }

   void clear() {
       clearTree(root);
       root = nullptr;
       num_elements = 0;
   }

   pair<iterator, bool> insert(const value_type &value) {
       Node *newNodePtr = nullptr;
       bool inserted = false;
       root = insertRec(root, value, nullptr, newNodePtr, inserted);
       if (root) root->parent = nullptr;

       if (!inserted) {
           return pair<iterator, bool>(iterator(findNode(root, value.first), this), false);
       }

       return pair<iterator, bool>(iterator(newNodePtr, this), true);
   }

   void erase(iterator pos) {
       if (!pos.ptr || pos.container != this) throw invalid_iterator();
       if (pos == end()) throw invalid_iterator();

       Key key = pos.ptr->data->first;
       bool found;
       root = eraseRec(root, key, found);
       if (!found) throw invalid_iterator();
   }

   size_t count(const Key &key) const {
       return findNode(root, key) != nullptr ? 1 : 0;
   }

   iterator find(const Key &key) {
       Node *node = findNode(root, key);
       return iterator(node, this);
   }

   const_iterator find(const Key &key) const {
       Node *node = findNode(root, key);
       return const_iterator(node, this);
   }
};

}

#endif
