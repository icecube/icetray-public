/**
 * @file I3MCTree.h
 * @brief Tree of I3Particles
 * 
 * copyright (C) 2013 the icecube collaboration
 * 
 * $Id$
 * @version $Revision$
 * @date $Date$
 */

#ifndef DATACLASSES_I3MCTREE_H_INCLUDED
#define DATACLASSES_I3MCTREE_H_INCLUDED

#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <stdint.h>

#include <boost/none.hpp>
#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>
#include <icetray/I3Logging.h>

#include <I3/hash_map.h>
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3ParticleID.h"
#include "dataclasses/physics/detail/I3MCTree_fwd.h"


/* Define basic tree structures here instead of in I3Tree so 
   we don't break other tree operations with the I3MCTree upgrade
*/
namespace TreeBase {
  static const unsigned tree_version_ = 1;
  
  
  /**
   * A generic tree node
   */
  template<typename T>
  class TreeNode {
    public:
      TreeNode<T>* parent;
      TreeNode<T>* firstChild;
      TreeNode<T>* nextSibling;
      T data;
      
      TreeNode() : parent(NULL),firstChild(NULL),nextSibling(NULL) { }
      TreeNode(const T& d) : parent(NULL),firstChild(NULL),nextSibling(NULL),data(d) { }
      T operator*() { return data; }
      T operator=(const TreeNode<T>& other) { return data = other.data; }
      T operator=(const T& otherData) { return data = otherData; }
      bool operator==(const TreeNode<T>& other) const
      {
        if (!(data == other.data))
          return false;
        if (parent == NULL) {
          if (other.parent != NULL)
            return false;
        } else {
          if (other.parent == NULL)
            return false;
          else if (!(parent->data == other.parent->data))
            return false;
        }
        if (firstChild == NULL) {
          if (other.firstChild != NULL)
            return false;
        } else {
          if (other.firstChild == NULL)
            return false;
          else if (!(firstChild->data == other.firstChild->data))
            return false;
        }
        if (nextSibling == NULL) {
          if (other.nextSibling != NULL)
            return false;
        } else {
          if (other.nextSibling == NULL)
            return false;
          else if (!(nextSibling->data == other.nextSibling->data))
            return false;
        }
        return true;
      }
      bool operator==(const T& otherData) const { return data == otherData; }
      bool operator!=(const TreeNode<T>& other) const { return !(*this == other); }
      bool operator!=(const T& otherData) const { return !(*this == otherData); }
  };
  
  /** \class Tree
   * \brief A generic tree class for hashable, unique data types.
   *
   * \tparam T is the data type
   * \tparam Key is the lookup type (T must be able to be implicitly converted to Key)
   * \tparam Hash is the hash function to convert from Key to size_t
   *
   * Note that Key must be unique and equality comparable.
   */
  template<typename T, typename Key, typename Hash>
  class Tree : public I3FrameObject {
    public:
      typedef T                         value_type;
      typedef T*                        pointer;
      typedef T&                        reference;
      typedef size_t                    size_type;
      typedef ptrdiff_t                 difference_type;
      typedef std::forward_iterator_tag iterator_category;
      
      /** \typedef optional_value
       * The optional_value is used when returning by value
       * when there might not be anything to return.
       */
      typedef boost::optional<T>  optional_value;
    protected:
      /** \typedef TreeHashKey
       * A boost::optional version of Key
       */
      typedef boost::optional<Key> TreeHashKey;
      /** \typedef treeNode
       * A TreeNode capable of holding the value_type (T)
       */
      typedef TreeNode<value_type> treeNode;
      /** \typedef tree_hash_map
       * a hash_map of Key : treeNode
       */
      typedef hash_map<Key,treeNode > tree_hash_map;
      
      tree_hash_map internalMap; /**< the actual hash_map storage **/
      TreeHashKey head_; /**< the first top-level node **/
      TreeHashKey end_; /**< a special end node that doesn't really exist **/
      
      /**
       * Erase any siblings (and their children) to the right of the node
       */
      void eraseRightSiblings_(const Key&);
    
    private:
      /**
       * Iterator Const Traits
       * 
       * Define TreeValue and TreeHashMapIter types.
       */
      template <typename ValueType, int dummy=0>
      struct iterator_const_traits;
      template <int dummy>
      struct iterator_const_traits <T,dummy>
      {
        typedef Tree<T,Key,Hash> TreeValue;
        typedef typename tree_hash_map::iterator TreeHashMapIter;
      };
      template <int dummy>
      struct iterator_const_traits <const T,dummy>
      {
        typedef const Tree<T,Key,Hash> TreeValue;
        typedef typename tree_hash_map::const_iterator TreeHashMapIter;
      };
      
      /**
       * Iterator Storage Class
       *
       * Keep track of the actual data type storage of the tree,
       * as well as a pointer to the tree itself.
       */
      template <typename StorageType,typename ValueType,
                typename TreeType,typename TreeHashMapIter>
      struct iterator_storage_impl;
      template <typename ValueType,typename TreeType,typename TreeHashMapIter>
      struct iterator_storage_impl <TreeHashKey,ValueType,TreeType,TreeHashMapIter>
      {
        // types
        TreeType* treePtr_;
        TreeHashKey storage_;
        // constructors
        explicit iterator_storage_impl() { }
        explicit iterator_storage_impl(TreeType& t)
          : treePtr_(&t), storage_(t.end_) { }
        explicit iterator_storage_impl(TreeType& t, ValueType v)
          : treePtr_(&t), storage_(t.end_)
        {
          TreeHashMapIter iter = treePtr_->internalMap.find(v);
          if (iter != treePtr_->internalMap.end())
            storage_ = *iter;
        }
        explicit iterator_storage_impl(TreeType& t, const Key& k)
          : treePtr_(&t), storage_(t.end_)
        {
          TreeHashMapIter iter = treePtr_->internalMap.find(k);
          if (iter != treePtr_->internalMap.end())
            storage_ = iter->second.data;
        }
        explicit iterator_storage_impl(TreeType& t, TreeHashKey hk)
          : treePtr_(&t), storage_(hk) { }
        explicit iterator_storage_impl(TreeType& t, TreeHashMapIter iter)
          : treePtr_(&t), storage_(t.end_)
        {
          if (iter != treePtr_->internalMap.end())
            storage_ = iter->second.data;
        }
        template<typename V,typename TT,typename THMI>
        iterator_storage_impl(const iterator_storage_impl<TreeHashKey,
            V,TT,THMI>& rhs)
          : treePtr_(rhs.treePtr_), storage_(rhs.storage_) { }
        template<typename V,typename TT,typename THMI>
        iterator_storage_impl(const iterator_storage_impl<THMI,
            V,TT,THMI>& rhs)
          : treePtr_(rhs.treePtr_)
        {
          if (rhs.storage_ != rhs.treePtr_->internalMap.end())
            storage_ = rhs.storage_->second.data;
        }
        // operators
        ValueType& dereference() const
        {
          i3_assert( storage_ != treePtr_->end_ );
          assert( storage_ );
          TreeHashMapIter iter = treePtr_->internalMap.find(*storage_);
          assert( iter != treePtr_->internalMap.end() );
          return iter->second.data;
        }
        template<typename Storage,typename V,typename TT,typename THMI>
        iterator_storage_impl<TreeHashKey,ValueType,TreeType,TreeHashMapIter>&
        operator=(const iterator_storage_impl<Storage,V,TT,THMI>& rhs)
        {
          iterator_storage_impl<TreeHashKey,ValueType,TreeType,TreeHashMapIter> other =
              iterator_storage_impl<TreeHashKey,ValueType,TreeType,TreeHashMapIter>(rhs);
          treePtr_ = other.treePtr_;
          storage_ = other.storage_;
          return *this;
        }
        TreeHashKey& operator=(const TreeHashKey& rhs)
        {
          storage_ = rhs;
          return storage_;
        }
        const T& operator=(const T& rhs)
        {
          storage_ = rhs;
          return rhs;
        }
        bool operator==(const iterator_storage_impl<TreeHashKey,ValueType,
            TreeType,TreeHashMapIter>& rhs) const
        {
          return (treePtr_ == rhs.treePtr_ && storage_ == rhs.storage_);
        }
        bool operator!=(const iterator_storage_impl<TreeHashKey,ValueType,
            TreeType,TreeHashMapIter>& rhs) const
        {
          return (treePtr_ != rhs.treePtr_ || storage_ != rhs.storage_);
        }
      };
      template <typename ValueType,typename TreeType,typename TreeHashMapIter>
      struct iterator_storage_impl <TreeHashMapIter,ValueType,TreeType,TreeHashMapIter>
      {
        // types
        TreeType* treePtr_;
        TreeHashMapIter storage_;
        // constructors
        explicit iterator_storage_impl() { }
        explicit iterator_storage_impl(TreeType& t) : treePtr_(&t),
          storage_(t.internalMap.end()) { }
        explicit iterator_storage_impl(TreeType& t, ValueType v)
          : treePtr_(&t), storage_(t.internalMap.find(v)) { }
        explicit iterator_storage_impl(TreeType& t, const Key& k)
          : treePtr_(&t), storage_(t.internalMap.find(k)) { }
        explicit iterator_storage_impl(TreeType& t, TreeHashKey hk)
          : treePtr_(&t), storage_(t.internalMap.end())
        {
          if (hk)
            storage_ = treePtr_->internalMap.find(*hk);
        }
        explicit iterator_storage_impl(TreeType& t, TreeHashMapIter iter)
          : treePtr_(&t), storage_(iter) { }
        template<typename V,typename TT,typename THMI>
        iterator_storage_impl(const iterator_storage_impl<TreeHashKey,
            V,TT,THMI>& rhs)
          : treePtr_(rhs.treePtr_), storage_(rhs.treePtr_->internalMap.end())
        {
          if (rhs.storage_)
            storage_ = treePtr_->internalMap.find(*(rhs.storage_));
        }
        template<typename V,typename TT,typename THMI>
        iterator_storage_impl(const iterator_storage_impl<THMI,
            V,TT,THMI>& rhs)
          : treePtr_(rhs.treePtr_), storage_(rhs.storage_) { }
        // operators
        ValueType& dereference() const
        {
          assert( storage_ != treePtr_->internalMap.end() );
          return storage_->second.data;
        }
        template<typename Storage,typename V,typename TT,typename THMI>
        iterator_storage_impl<TreeHashMapIter,ValueType,TreeType,TreeHashMapIter>&
        operator=(const iterator_storage_impl<Storage,V,TT,THMI>& rhs)
        {
          iterator_storage_impl<TreeHashMapIter,ValueType,TreeType,TreeHashMapIter> other =
              iterator_storage_impl<TreeHashMapIter,ValueType,TreeType,TreeHashMapIter>(rhs);
          treePtr_ = other.treePtr_;
          storage_ = other.storage_;
          return *this;
        }
        TreeHashMapIter& operator=(const TreeHashMapIter& rhs)
        {
          storage_ = rhs;
          return storage_;
        }
        bool operator==(const iterator_storage_impl<TreeHashMapIter,
            ValueType,TreeType,TreeHashMapIter>& rhs) const
        {
          return (treePtr_ == rhs.treePtr_ && storage_ == rhs.storage_);
        }
        bool operator!=(const iterator_storage_impl<TreeHashMapIter,
            ValueType,TreeType,TreeHashMapIter>& rhs) const
        {
          return (treePtr_ != rhs.treePtr_ || storage_ != rhs.storage_);
        }
      };
    public:
      // note: required to use underscores on std methods
      //       to match tree_indexing_suite
      
      /**
       * Iterator base (for tree-based iterators)
       */
      template<class Derived, class Value, class StorageType>
      class iterator_base
        : public boost::iterator_facade<
            iterator_base<Derived,Value,StorageType>,
            Value,
            boost::forward_traversal_tag
          >
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<StorageType, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit iterator_base() { }
          explicit iterator_base(TreeValue& ext) : node_(ext)
          { node_ = derived_().first_(); }
          explicit iterator_base(TreeValue& ext, const T* v)
            : node_(ext,*v) { }
          explicit iterator_base(TreeValue& ext, const Key& k)
            : node_(ext,k) { }
          template <typename D,typename V,typename S> explicit iterator_base(const iterator_base<D,V,S>& iter)
            : node_(*(iter.node_.treePtr_))
          { node_ = iter.node_; }
          template <typename D,typename S>
          iterator_base<Derived,Value,StorageType>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<Derived,Value,StorageType> ret (iter);
            node_ = ret.node_;
            return *this;
          }
        protected:
          StorageType first_() { return this->node_.treePtr_->head_; };
          void next_() { };
          storage_traits node_;
        private:
          friend class Tree<T,Key,Hash>;
          explicit iterator_base(TreeValue& ext, const TreeHashKey& h) :
            node_(ext,h) { }
          Derived& derived_()
          {
            return *static_cast<Derived*>(this);
          }
          friend class boost::iterator_core_access;
          Value& dereference() const
          {
            return node_.dereference();
          }
          bool equal(iterator_base<Derived,Value,StorageType> const & other) const
          {
            return (node_ == other.node_);
          }
          void increment()
          {
            derived_().next_();
          }
      };
      
      /**
       * Pre order iterator: O(n)
       */
      template <class Value>
      class pre_order : public iterator_base<pre_order<Value>,Value,TreeHashKey>
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<TreeHashKey, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit pre_order() { }
          pre_order(TreeValue& ext)
            : iterator_base<pre_order<Value>,Value,TreeHashKey>(ext) { }
          explicit pre_order(const pre_order<T>& iter)
            : iterator_base<pre_order<Value>,Value,TreeHashKey>(iter) { }
          pre_order(const iterator_base<pre_order<Value>,Value,TreeHashKey>& iter)
            : iterator_base<pre_order<Value>,Value,TreeHashKey>(iter) { }
          template <typename D,typename S> pre_order(const iterator_base<D,Value,S>& iter)
            : iterator_base<pre_order<Value>,Value,TreeHashKey>(iter) { }
          explicit pre_order(TreeValue& ext, const Value* value) :
            iterator_base<pre_order<Value>,Value,TreeHashKey>(ext,value) { }
          explicit pre_order(TreeValue& ext, const Key& k) :
            iterator_base<pre_order<Value>,Value,TreeHashKey>(ext,k) { }
          template <typename D,typename S>
          pre_order<Value>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<pre_order<Value>,Value,TreeHashKey>::operator=(iter);
            return *this;
          }
          operator pre_order<const Value>() const
          {
            return(pre_order<const Value>(this->dereference()));
          }
        private:
          friend class Tree<T,Key,Hash>;
          friend class iterator_base<pre_order<Value>,Value,TreeHashKey>;
          explicit pre_order(TreeValue& ext, const TreeHashKey& k) :
            iterator_base<pre_order<Value>,Value,TreeHashKey>(ext,k) { }
        protected:
          TreeHashKey first_()
          { return this->node_.treePtr_->head_; };
          void next_()
          {
            TreeHashMapIter iter = this->node_.treePtr_->internalMap.find(*(this->node_.storage_));
            if (iter == this->node_.treePtr_->internalMap.end()) {
              this->node_ = this->node_.treePtr_->end_;
              return;
            }
            const treeNode* n = &(iter->second);
            if (n->firstChild != NULL)
              n = n->firstChild;
            else if (n->nextSibling != NULL)
              n = n->nextSibling;
            else if (n->parent != NULL) {
              // go to a parent's next sibling
              do {
                n = n->parent;
                if (n->nextSibling != NULL)
                  break;
              } while (n->parent != NULL);
              if (n->nextSibling != NULL)
                n = n->nextSibling;
              else {
                this->node_ = this->node_.treePtr_->end_;
                return;
              }
            } else {
              this->node_ = this->node_.treePtr_->end_;
              return;
            }
            assert( n != NULL );
            this->node_ = n->data;
          }
      };
      typedef pre_order<T> pre_order_iterator;
      typedef pre_order<const T> pre_order_const_iterator;
      
      /**
       * Post order iterator: O(n log n)
       */
      template <class Value>
      class post_order : public iterator_base<post_order<Value>,Value,TreeHashKey>
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<TreeHashKey, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit post_order() { }
          post_order(TreeValue& ext) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(ext) { }
          explicit post_order(const post_order<T>& iter)
            : iterator_base<post_order<Value>,Value,TreeHashKey>(iter) { }
          post_order(const iterator_base<post_order<Value>,Value,TreeHashKey>& iter) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(iter) { }
          template <typename D,typename S> post_order(const iterator_base<D,Value,S>& iter) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(iter) { }
          explicit post_order(TreeValue& ext, const Value* value) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(ext,value) { }
          explicit post_order(TreeValue& ext, const Key& k) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(ext,k) { }
          template <typename D, typename S>
          post_order<Value>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<post_order<Value>,Value,TreeHashKey>::operator=(iter);
            return *this;
          }
          operator post_order<const Value>() const
          {
            return(post_order<const Value>(this->dereference()));
          }
        private:
          friend class Tree<T,Key,Hash>;
          friend class iterator_base<post_order<Value>,Value,TreeHashKey>;
          explicit post_order(TreeValue& ext, const TreeHashKey& k) :
            iterator_base<post_order<Value>,Value,TreeHashKey>(ext,k) { }
        protected:
          TreeHashKey first_()
          {
            if (this->node_.treePtr_->head_) {
              // go to leftmost child
              TreeHashMapIter iter = this->node_.treePtr_->internalMap.find(*(this->node_.treePtr_->head_));
              if (iter == this->node_.treePtr_->internalMap.end())
                return this->node_.treePtr_->end_;
              const treeNode* n = &(iter->second);
              while (n->firstChild != NULL) {
                n = n->firstChild;
              }
              return TreeHashKey(n->data);
            } else
              return this->node_.treePtr_->end_;
          }
          void next_()
          {
            TreeHashMapIter iter = this->node_.treePtr_->internalMap.find(*(this->node_.storage_));
            if (iter == this->node_.treePtr_->internalMap.end()) {
              this->node_ = this->node_.treePtr_->end_;
              return;
            }
            const treeNode* n = &(iter->second);
            if (n->nextSibling != NULL) {
              // go to leftmost child of next sibling
              n = n->nextSibling;
              while (n->firstChild != NULL) {
                n = n->firstChild;
              }
            } else if (n->parent != NULL) {
              // go to parent
              n = n->parent;
            } else {
              this->node_ = this->node_.treePtr_->end_;
              return;
            }
            this->node_ = n->data;
          }
      };
      typedef post_order<T> post_order_iterator;
      typedef post_order<const T> post_order_const_iterator;
      
      /**
       * Sibling iterator: O(n)
       */
      template<typename Value>
      class sibling_iter : public iterator_base<sibling_iter<Value>,Value,TreeHashKey>
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<TreeHashKey, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit sibling_iter() { }
          sibling_iter(TreeValue& ext) :
            iterator_base<sibling_iter<Value>,Value,TreeHashKey>(ext) { }
          explicit sibling_iter(const sibling_iter<T>& iter)
            : iterator_base<sibling_iter<Value>,Value,TreeHashKey>(iter) { }
          sibling_iter(const iterator_base<sibling_iter<Value>,Value,TreeHashKey>& iter)
            : iterator_base<sibling_iter<Value>,Value,TreeHashKey>(iter) { }
          template <typename V> sibling_iter(const sibling_iter<V>& iter)
            : iterator_base<sibling_iter<Value>,Value,TreeHashKey>(iter) { }
          template <typename D, typename S> sibling_iter(const iterator_base<D,Value,S>& iter)
            : iterator_base<sibling_iter<Value>,Value,TreeHashKey>(iter) { }
          explicit sibling_iter(TreeValue& ext, const Value* value) :
            iterator_base<sibling_iter<Value>,Value,TreeHashKey>(ext,value) { }
          explicit sibling_iter(TreeValue& ext, const Key& k) :
            iterator_base<sibling_iter<Value>,Value,TreeHashKey>(ext,k) { }
          template <typename D, typename S>
          sibling_iter<Value>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<sibling_iter<Value>,Value,TreeHashKey>::operator=(iter);
            return *this;
          }
          operator sibling_iter<const Value>() const
          {
            return(sibling_iter<const Value>(this->dereference()));
          }
        private:
          friend class Tree<T,Key,Hash>;
          friend class iterator_base<sibling_iter<Value>,Value,TreeHashKey>;
          explicit sibling_iter(TreeValue& ext, const TreeHashKey& k) :
            iterator_base<sibling_iter<Value>,Value,TreeHashKey>(ext,k) { }
        protected:
          TreeHashKey first_()
          { return this->node_.treePtr_->end_; }
          void next_()
          {
            TreeHashMapIter iter = this->node_.treePtr_->internalMap.find(*(this->node_.storage_));
            if (iter == this->node_.treePtr_->internalMap.end() || iter->second.nextSibling == NULL) {
              this->node_ = this->node_.treePtr_->end_;
            } else {
              this->node_ = iter->second.nextSibling->data;
            }
          }
      };
      typedef sibling_iter<T> sibling_iterator;
      typedef sibling_iter<const T> sibling_const_iterator;
      
      /**
       * Fast iterator: O(n)
       * Note that there is no ordering, but this gives the fastest results
       */
      template <typename Value>
      class fast_iter : public iterator_base<fast_iter<Value>,Value,
          typename iterator_const_traits<Value>::TreeHashMapIter>
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<TreeHashMapIter, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit fast_iter() { }
          fast_iter(TreeValue& ext) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(ext) { }
          explicit fast_iter(const fast_iter<T>& iter)
            : iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(iter) { }
          fast_iter(const iterator_base<fast_iter<Value>,Value,TreeHashMapIter>& iter) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(iter) { }
          template <typename D,typename S> fast_iter(const iterator_base<D,Value,S>& iter) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(iter) { }
          explicit fast_iter(TreeValue& ext, Value* v) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(ext,v) { }
          explicit fast_iter(TreeValue& ext, const Key& k) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(ext,k) { }
          template <typename D,typename S>
          fast_iter<Value>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>::operator=(iter);
            return *this;
          }
          operator fast_iter<const Value>() const
          {
            return(fast_iter<const Value>(this->dereference()));
          }
        protected:
          TreeHashMapIter first_()
          { return this->node_.treePtr_->internalMap.begin(); }
          void next_() { this->node_.storage_++; };
        private:
          friend class Tree<T,Key,Hash>;
          friend class iterator_base<fast_iter<Value>,Value,TreeHashMapIter>;
          explicit fast_iter(TreeValue& ext, const TreeHashKey& k) :
            iterator_base<fast_iter<Value>,Value,TreeHashMapIter>(ext,k) { }
      };
      typedef fast_iter<T> fast_iterator;
      typedef fast_iter<const T> fast_const_iterator;
      
      /**
       * Leaf iterator: O(n)
       * Note that there is no ordering of leaves
       */
      template <class Value>
      class leaf_iter : public iterator_base<leaf_iter<Value>,Value,
          typename iterator_const_traits<Value>::TreeHashMapIter>
      {
        protected:
          typedef iterator_const_traits<Value> const_traits;
          typedef typename const_traits::TreeValue TreeValue;
          typedef typename const_traits::TreeHashMapIter TreeHashMapIter;
          typedef iterator_storage_impl<TreeHashMapIter, Value,TreeValue,
              TreeHashMapIter> storage_traits;
        public:
          explicit leaf_iter() { }
          leaf_iter(TreeValue& ext) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(ext) { }
          explicit leaf_iter(const leaf_iter<T>& iter)
            : iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(iter) { }
          leaf_iter(const iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>& iter) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(iter) { }
          template <typename D,typename S>
          leaf_iter(const iterator_base<D,Value,S>& iter) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(iter) { }
          explicit leaf_iter(TreeValue& ext, Value* v) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(ext,v) { }
          explicit leaf_iter(TreeValue& ext, const Key& k) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(ext,k) { }
          template <typename D,typename S>
          leaf_iter<Value>& operator=(const iterator_base<D,Value,S>& iter)
          {
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>::operator=(iter);
            return *this;
          }
          operator leaf_iter<const Value>() const
          {
            return(leaf_iter<const Value>(this->dereference()));
          }
        private:
          friend class Tree<T,Key,Hash>;
          friend class iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>;
          explicit leaf_iter(TreeValue& ext, const TreeHashKey& k) :
            iterator_base<leaf_iter<Value>,Value,TreeHashMapIter>(ext,k) { }
        protected:
          TreeHashMapIter first_()
          {
            TreeHashMapIter iter = this->node_.treePtr_->internalMap.begin(),
                            iterEnd = this->node_.treePtr_->internalMap.end();
            for(;iter != iterEnd; iter++) {
              if (iter->second.firstChild != NULL)
                continue;
              else
                break;
            }
            return iter;
          }
          void next_()
          {
            this->node_.storage_++;
            TreeHashMapIter iterEnd = this->node_.treePtr_->internalMap.end();
            for(;this->node_.storage_ != iterEnd; this->node_.storage_++) {
              if (this->node_.storage_->second.firstChild != NULL)
                continue;
              else
                break;
            }
          }
      };
      typedef leaf_iter<T> leaf_iterator;
      typedef leaf_iter<const T> leaf_const_iterator;
      
      /**
       * Define the default iterator
       */
      typedef pre_order_iterator iterator;
      typedef pre_order_const_iterator const_iterator;
      
      /**
       * Constructors and destructor
       */
      Tree();
      Tree(const T& value);
      template<typename Derived,typename Storage>
      Tree(const iterator_base<Derived,const T,Storage>& other);
      Tree(const Tree<T,Key,Hash>&);
      ~Tree();
    
      std::ostream& Print(std::ostream&) const override;
      
      /**
       * Assignment operator
       */
      const Tree<T,Key,Hash> operator=(const Tree<T,Key,Hash>&);
      
      /**
       * Equality operator to compare two trees
       */
      bool operator==(const Tree<T,Key,Hash>& other) const;
      bool operator!=(const Tree<T,Key,Hash>& other) const;
      
      // pre_order iterator helpers
      inline pre_order_iterator begin()
      { return pre_order_iterator(*this); }
      inline pre_order_const_iterator begin() const
      { return pre_order_const_iterator(*this); }
      inline pre_order_const_iterator cbegin() const
      { return pre_order_const_iterator(*this); }
      inline pre_order_iterator end()
      { return pre_order_iterator(*this,end_); };
      inline pre_order_const_iterator end() const
      { return pre_order_const_iterator(*this,end_); };
      inline pre_order_const_iterator cend() const
      { return pre_order_const_iterator(*this,end_); };
      
      // post_order iterator helpers
      inline post_order_iterator begin_post()
      { return post_order_iterator(*this); }
      inline post_order_const_iterator begin_post() const
      { return post_order_const_iterator(*this); }
      inline post_order_const_iterator cbegin_post() const
      { return post_order_const_iterator(*this); }
      inline post_order_iterator end_post()
      { return post_order_iterator(*this,end_); }
      inline post_order_const_iterator end_post() const
      { return post_order_const_iterator(*this,end_); }
      inline post_order_const_iterator cend_post() const
      { return post_order_const_iterator(*this,end_); }
      
      // sibling iterator helpers
      template<typename D,typename V,typename S>
      sibling_iterator begin_sibling(const iterator_base<D,V,S>& i)
      {
        if (i != iterator_base<D,V,S>(*this,end_))
          return sibling_iterator(*this,*i);
        else
          return sibling_iterator(*this,end_);
      }
      template<typename D,typename V,typename S>
      sibling_const_iterator begin_sibling(const iterator_base<D,V,S>& i) const
      {
        if (i != iterator_base<D,V,S>(*this,end_))
          return sibling_const_iterator(*this,*i);
        else
          return sibling_const_iterator(*this,end_);
      }
      template<typename D,typename V,typename S>
      sibling_const_iterator cbegin_sibling(const iterator_base<D,V,S>& i) const
      {
        if (i != iterator_base<D,V,S>(*this,end_))
          return sibling_const_iterator(*this,*i);
        else
          return sibling_const_iterator(*this,end_);
      }
      inline sibling_iterator begin_sibling(const Key& k)
      { return sibling_iterator(*this,k); }
      inline sibling_const_iterator begin_sibling(const Key& k) const
      { return sibling_const_iterator(*this,k); }
      inline sibling_const_iterator cbegin_sibling(const Key& k) const
      { return sibling_const_iterator(*this,k); }
      template<typename D,typename V,typename S>
      inline sibling_iterator end(const iterator_base<D,V,S>& i)
      { return sibling_iterator(*this,end_); }
      template<typename D,typename V,typename S>
      inline sibling_const_iterator end(const iterator_base<D,V,S>& i) const
      { return sibling_const_iterator(*this,end_); }
      template<typename D,typename V,typename S>
      inline sibling_const_iterator cend(const iterator_base<D,V,S>& i) const
      { return sibling_const_iterator(*this,end_); }
      inline sibling_iterator end_sibling()
      { return sibling_iterator(*this,end_); }
      inline sibling_const_iterator end_sibling() const
      { return sibling_const_iterator(*this,end_); }
      inline sibling_const_iterator cend_sibling() const
      { return sibling_const_iterator(*this,end_); }
      
      // fast iterator helpers
      inline fast_iterator begin_fast()
      { return fast_iterator(*this); }
      inline fast_const_iterator begin_fast() const
      { return fast_const_iterator(*this); }
      inline fast_const_iterator cbegin_fast() const
      { return fast_const_iterator(*this); }
      inline fast_iterator end_fast()
      { return fast_iterator(*this,end_); };
      inline fast_const_iterator end_fast() const
      { return fast_const_iterator(*this,end_); };
      inline fast_const_iterator cend_fast() const
      { return fast_const_iterator(*this,end_); };
      
      // leaf iterator helpers
      inline leaf_iterator begin_leaf()
      { return leaf_iterator(*this); }
      inline leaf_const_iterator begin_leaf() const
      { return leaf_const_iterator(*this); }
      inline leaf_const_iterator cbegin_leaf() const
      { return leaf_const_iterator(*this); }
      inline leaf_iterator end_leaf()
      { return leaf_iterator(*this,end_); };
      inline leaf_const_iterator end_leaf() const
      { return leaf_const_iterator(*this,end_); };
      inline leaf_const_iterator cend_leaf() const
      { return leaf_const_iterator(*this,end_); };
      
      /**
       * \brief Get the (first) head/root value
       */
      const optional_value get_head() const;
      /**
       * \brief Get all of the head/root values.
       */
      const std::vector<T> get_heads() const;
      
      /**
       * Get value at key
       *
       * Note that non-const versions are not provided because some
       * modifications could change the key as well and cannot be allowed.
       */
      const optional_value at(const Key&) const;
      const optional_value operator[](const Key&) const;
      
      /**
       * Get iterator at key
       */
      iterator find(const Key&);
      const_iterator find(const Key&) const;
      
      /**
       * Get parent
       */
      const optional_value parent(const Key&) const;
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,T,Storage>
      parent(const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,const T,Storage>
      parent(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get previous sibling
       */
      const optional_value previous_sibling(const Key&) const;
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,T,Storage>
      previous_sibling(const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,const T,Storage>
      previous_sibling(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get next sibling
       */
      const optional_value next_sibling(const Key&) const;
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,T,Storage>
      next_sibling(const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,const T,Storage>
      next_sibling(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get immediate children
       */
      const std::vector<T> children(const Key&) const;
      
      template<class Derived,class Storage>
      sibling_iter<T>
      children(const iterator_base<Derived,T,Storage>&);
      
      template<class Derived,class Storage>
      sibling_iter<const T>
      children(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get first child
       */
      const optional_value first_child(const Key&) const;
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,T,Storage>
      first_child(const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      iterator_base<Derived,const T,Storage>
      first_child(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Clear all nodes from tree
       */
      void clear();
      
      /**
       * Erase node and children
       */
      void erase(const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      erase(const iterator_base<Derived,Value,Storage>&);
      
      /**
       * Erase children of node
       */
      void erase_children(const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      erase_children(const iterator_base<Derived,Value,Storage>&);
      
      /**
       * Add a child to a node
       */
      void append_child(const Key& node, const T&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      append_child(const iterator_base<Derived,Value,Storage>&, const T&);
      
      /**
       * Add subtree as a child to a node
       */
      void append_child(const Key& node, const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      append_child(const iterator_base<Derived,Value,Storage>&,
                   const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      iterator_base<Derived,Value,Storage>
      append_child(const iterator_base<Derived,Value,Storage>&,
                   const iterator_base<Derived2,Value2,Storage2>&);
      
      /**
       * Add several children to a node
       */
      void append_children(const Key& node, const std::vector<T>&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      append_children(const iterator_base<Derived,Value,Storage>&,
                      const std::vector<T>&);
      
      /**
       * Add multiple children from the iterator range to the node
       *
       * first arg: parent node
       * second arg: begin range
       * third arg: end range
       */
      template<typename Iterator>
      void append_children(const Key& node, Iterator, const Iterator&);
      template<typename Derived,typename Value,typename Storage,
               typename Iterator>
      iterator_base<Derived,Value,Storage>
      append_children(const iterator_base<Derived,Value,Storage>&,
                      Iterator, const Iterator&);
      
      /**
       * Insert at root level before other nodes
       */
      void insert(const T&);
      
      /**
       * Insert at root level after other nodes
       */
      void insert_after(const T&);
      
      /**
       * Insert as sibling before node
       */
      void insert(const Key& node, const T&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      insert(const iterator_base<Derived,Value,Storage>&, const T&);
      
      /**
        * Insert as sibling after node
        */
      void insert_after(const Key& node, const T&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      insert_after(const iterator_base<Derived,Value,Storage>&, const T&);

      /**
       * Insert subtree as sibling before node
       */
      void insert_subtree(const Key& node, const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      insert_subtree(const iterator_base<Derived,Value,Storage>&,
                     const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      iterator_base<Derived,Value,Storage>
      insert_subtree(const iterator_base<Derived,Value,Storage>&,
                     const iterator_base<Derived2,Value2,Storage2>&);
      
      /**
       * Insert subtree as sibling after node
       */
      void insert_subtree_after(const Key& node, const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      insert_subtree_after(const iterator_base<Derived,Value,Storage>&,
                           const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      iterator_base<Derived,Value,Storage>
      insert_subtree_after(const iterator_base<Derived,Value,Storage>&,
                           const iterator_base<Derived2,Value2,Storage2>&);
      
      /**
       * Replace node (keeping children)
       */
      void replace(const Key& node, const T&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      replace(const iterator_base<Derived,Value,Storage>&, const T&);
      
      /**
       * Replace node (and children) with subtree
       */
      void replace(const Key&, const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      replace(const iterator_base<Derived,Value,Storage>&,
              const Tree<T,Key,Hash>&, const Key&);
      
      template<typename Derived,typename Value,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      iterator_base<Derived,Value,Storage>
      replace(const iterator_base<Derived,Value,Storage>&,
              const iterator_base<Derived2,Value2,Storage2>&);
      
      /**
       * Move all children of node to be siblings
       */
      void flatten(const Key&);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      flatten(const iterator_base<Derived,Value,Storage>&);
      
      /**
       * Move all child nodes of 'from' to be children of 'node'
       * Works with two nodes of the same tree.
       */
      void reparent(const Key& node, const Key& from);
      
      template<typename Derived,typename Value,typename Storage>
      iterator_base<Derived,Value,Storage>
      reparent(const iterator_base<Derived,Value,Storage>&,
               const Key&);
      
      template<typename Derived,typename Value,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      iterator_base<Derived,Value,Storage>
      reparent(const iterator_base<Derived,Value,Storage>&,
               const iterator_base<Derived2,Value2,Storage2>&);
      
      /**
       * Merge other tree into this tree
       */
      void merge(const Tree<T,Key,Hash>&);
      
      /**
       * Swap with another tree
       */
      void swap(Tree<T,Key,Hash>&);
      
      /**
       * Size of the tree
       */
      size_type size() const;
      
      /**
       * If the tree is size=0, or empty
       */
      bool empty() const;
      
      /**
       * Compute depth to the root
       */
      size_type depth(const Key&) const;
      
      template<typename Derived,typename Storage>
      size_type depth(const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      size_type depth(const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get the number of children a node has
       */
      size_type number_of_children(const Key&) const;
      
      template<typename Derived,typename Storage>
      size_type number_of_children(
          const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      size_type number_of_children(
          const iterator_base<Derived,const T,Storage>&) const;
      
      /**
       * Get the number of siblings a node has
       */
      size_type number_of_siblings(const Key&) const;
      
      template<typename Derived,typename Storage>
      size_type number_of_siblings(
          const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      size_type number_of_siblings(
          const iterator_base<Derived,const T,Storage>&) const;
       
      /**
       * Search for a key in the subtree
       */
      bool is_in_subtree(const Key& haystack, const Key& needle) const;
      
      template<typename Derived,typename Storage>
      bool is_in_subtree(
          const iterator_base<Derived,T,Storage>&,
          const Key&);
      
      template<typename Derived,typename Storage>
      bool is_in_subtree(
          const iterator_base<Derived,const T,Storage>&,
          const Key&) const;
      
      template<typename Derived,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      bool is_in_subtree(
          const iterator_base<Derived,T,Storage>&,
          const iterator_base<Derived2,Value2,Storage2>&);
      
      template<typename Derived,typename Storage,
               typename Derived2,typename Value2,typename Storage2>
      bool is_in_subtree(
          const iterator_base<Derived,const T,Storage>&,
          const iterator_base<Derived2,Value2,Storage2>&) const;
      
      /**
       * Finds if any key in the subtree is in the tree
       */
      bool subtree_in_tree(const Tree<T,Key,Hash>& other, const Key& node) const;
      
      template<typename Derived,typename Storage>
      bool subtree_in_tree(
          const iterator_base<Derived,T,Storage>&);
      
      template<typename Derived,typename Storage>
      bool subtree_in_tree(
          const iterator_base<Derived,const T,Storage>&) const;
      
    protected:
      friend class icecube::serialization::access;
      
      template <class Archive> void load(Archive & ar, unsigned version);
      
      template <class Archive> void save(Archive & ar, unsigned version) const;
      
      I3_SERIALIZATION_SPLIT_MEMBER();
      
      SET_LOGGER("Tree");
  };
  
  template<>
  std::ostream& Tree<I3Particle,I3ParticleID>::Print(std::ostream& s) const;
};

template<typename T, typename Key=T, typename Hash=hash<Key>>
std::ostream& operator<<(std::ostream& os, const TreeBase::Tree<T,Key,Hash>& tree){
  return(tree.Print(os));
}


I3_CLASS_VERSION(I3MCTree,TreeBase::tree_version_);
I3_POINTER_TYPEDEFS(I3MCTree);
I3_DEFAULT_NAME(I3MCTree);

#include "dataclasses/physics/detail/I3MCTree_impl.h"

#endif // DATACLASSES_I3MCTREE_H_INCLUDED

