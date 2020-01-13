#ifndef I3MCTREE_IMPL_H
#define I3MCTREE_IMPL_H

/**
 * @file I3MCTree_impl.h
 * @brief Implementation for TreeBase
 *
 * copyright (C) 2013 the icecube collaboration
 *
 * $Id: I3MCTree.hh 112306 2013-10-28 22:43:59Z david.schultz $
 * @version $Revision: 112306 $
 * @date $Date: 2013-10-28 17:43:59 -0500 (Mon, 28 Oct 2013) $
 */
#include <cassert>
#include <stack>
#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>

#include <icetray/serialization.h>

namespace TreeBase {
  /*
   * A note about the tree_hash_map::iterator
   * The iterator is a *pair<Key,treeNode>
   * So to get the hash_map key, use iter->first
   * and to get the treeNode, use iter->second
   */

  template<typename T, typename Key, typename Hash>
  Tree<T,Key,Hash>::Tree()
  { }

  template<typename T, typename Key, typename Hash>
  Tree<T,Key,Hash>::Tree(const T& value)
  {
    insert(value);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  Tree<T,Key,Hash>::Tree(const iterator_base<Derived,const T,Storage>& other)
  {
    Tree(*(other.node_.treePtr_));
  }

  template<typename T, typename Key, typename Hash>
  Tree<T,Key,Hash>::Tree(const Tree<T,Key,Hash>& copy)
    : internalMap(copy.internalMap), head_(copy.head_)
  {
    // Repoint all relationships from copy.internalMap to entries in internalMap
    for (typename tree_hash_map::iterator node = internalMap.begin(); node != internalMap.end(); node++) {
      typename tree_hash_map::iterator source_node;
      if (node->second.parent != NULL) {
        source_node = internalMap.find(node->second.parent->data);
        assert(source_node != internalMap.end());
        node->second.parent = &(source_node->second);
      }
      if (node->second.firstChild != NULL) {
        source_node = internalMap.find(node->second.firstChild->data);
        assert(source_node != internalMap.end());
        node->second.firstChild = &(source_node->second);
      }
      if (node->second.nextSibling != NULL) {
        source_node = internalMap.find(node->second.nextSibling->data);
        assert(source_node != internalMap.end());
        node->second.nextSibling = &(source_node->second);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  Tree<T,Key,Hash>::~Tree()
  {
    clear();
  }
  
  template<typename T, typename Key, typename Hash>
  std::ostream& Tree<T,Key,Hash>::Print(std::ostream& s) const{
    s << "[I3MCTree:\n";
    for(Tree::const_iterator iter=cbegin(),end=cend(); iter!=end; iter++){
      for(unsigned int d=0; d<depth(*iter)+1; d++)
        s << "  ";
      s << *iter << '\n';
    }
    s << ']';
    return(s);
  }

  template<typename T, typename Key, typename Hash>
  const Tree<T,Key,Hash>
  Tree<T,Key,Hash>::operator=(const Tree<T,Key,Hash>& other)
  {
    Tree<T,Key,Hash> tmp(other);
    swap(tmp);
    return *this;
  }

  template<typename T, typename Key, typename Hash>
  bool
  Tree<T,Key,Hash>::operator==(const Tree<T,Key,Hash>& other) const
  {
    if (head_)
      return (head_ == other.head_ && *head_ == *other.head_
              && internalMap == other.internalMap);
    else
      return (head_ == other.head_);
  }

  template<typename T, typename Key, typename Hash>
  bool
  Tree<T,Key,Hash>::operator!=(const Tree<T,Key,Hash>& other) const
  {
    if (head_)
      return (head_ != other.head_ || *head_ != *other.head_
              || internalMap != other.internalMap);
    else
      return (head_ != other.head_);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::at(const Key& key) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter == internalMap.end())
      return optional_value();
    else
      return optional_value(iter->second.data);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::operator[](const Key& key) const
  {
    return at(key);
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::iterator
  Tree<T,Key,Hash>::find(const Key& key)
  {
    return iterator(*this,key);
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::const_iterator
  Tree<T,Key,Hash>::find(const Key& key) const
  {
    return const_iterator(*this,key);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::parent(const Key& key) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter == internalMap.end() || iter->second.parent == NULL)
      return optional_value();
    else {
      return optional_value(iter->second.parent->data);
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,T,Storage>
  Tree<T,Key,Hash>::parent(const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return iterator_base<Derived,T,Storage>(*this,end_);
    const optional_value ret = parent(*iter);
    if (ret)
      return iterator_base<Derived,T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,const T,Storage>
  Tree<T,Key,Hash>::parent(const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return iterator_base<Derived,const T,Storage>(*this,end_);
    const optional_value ret = parent(*iter);
    if (ret)
      return iterator_base<Derived,const T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,const T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::previous_sibling(const Key& key) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter == internalMap.end())
      return optional_value();
    const treeNode* n = &(iter->second);
    const treeNode* keynode = &(iter->second);
    if (n->parent == NULL) {
      if (head_) {
        // try iterating over head_ siblings
        iter = internalMap.find(*head_);
        if (iter != internalMap.end()) {
          n = &(iter->second);
          while (n->nextSibling != NULL) {
            if (n->nextSibling == keynode)
              return optional_value(n->data);
            n = n->nextSibling;
          }
        }
      }
      return optional_value();
    }
    n = n->parent;
    assert( n->firstChild != NULL );
    n = n->firstChild;
    while (n->nextSibling != NULL) {
      if (n->nextSibling == keynode)
        return optional_value(n->data);
      n = n->nextSibling;
    }
    return optional_value();
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,T,Storage>
  Tree<T,Key,Hash>::previous_sibling(
      const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter ==iterator_base<Derived,T,Storage>(*this,end_))
      return iterator_base<Derived,T,Storage>(*this,end_);
    const optional_value ret = previous_sibling(*iter);
    if (ret)
      return iterator_base<Derived,T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,const T,Storage>
  Tree<T,Key,Hash>::previous_sibling(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter ==iterator_base<Derived,const T,Storage>(*this,end_))
      return iterator_base<Derived,const T,Storage>(*this,end_);
    const optional_value ret = previous_sibling(*iter);
    if (ret)
      return iterator_base<Derived,const T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,const T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::next_sibling(const Key& key) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter == internalMap.end() || iter->second.nextSibling == NULL)
      return optional_value();
    else
      return optional_value(iter->second.nextSibling->data);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,T,Storage>
  Tree<T,Key,Hash>::next_sibling(const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return iterator_base<Derived,T,Storage>(*this,end_);
    const optional_value ret = next_sibling(*iter);
    if (ret)
      return iterator_base<Derived,T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,const T,Storage>
  Tree<T,Key,Hash>::next_sibling(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return iterator_base<Derived,const T,Storage>(*this,end_);
    const optional_value ret = next_sibling(*iter);
    if (ret)
      return iterator_base<Derived,const T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,const T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  const std::vector<T>
  Tree<T,Key,Hash>::children(const Key& key) const
  {
    std::vector<T> result;
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter != internalMap.end() && iter->second.firstChild != NULL) {
      const treeNode* n = &(iter->second);
      n = n->firstChild;
      result.push_back(n->data);
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        result.push_back(n->data);
      }
    }
    return result;
  }

  template<typename T, typename Key, typename Hash>
  template<class Derived,class Storage>
  typename Tree<T,Key,Hash>::template sibling_iter<T>
  Tree<T,Key,Hash>::children(const iterator_base<Derived,T,Storage>& iter)
  {
    sibling_iter<T> ret(iter);
    return first_child(ret);
  }

  template<typename T, typename Key, typename Hash>
  template<class Derived,class Storage>
  typename Tree<T,Key,Hash>::template sibling_iter<const T>
  Tree<T,Key,Hash>::children(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    sibling_iter<const T> ret(iter);
    return first_child(ret);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::first_child(const Key& key) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(key);
    if (iter == internalMap.end() || iter->second.firstChild == NULL)
      return optional_value();
    else
      return optional_value(iter->second.firstChild->data);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,T,Storage>
  Tree<T,Key,Hash>::first_child(const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return iterator_base<Derived,T,Storage>(*this,end_);
    const optional_value ret = first_child(*iter);
    if (ret)
      return iterator_base<Derived,T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,const T,Storage>
  Tree<T,Key,Hash>::first_child(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return iterator_base<Derived,const T,Storage>(*this,end_);
    const optional_value ret = first_child(*iter);
    if (ret)
      return iterator_base<Derived,const T,Storage>(*this,*ret);
    else
      return iterator_base<Derived,const T,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::clear()
  {
    if (head_) {
      internalMap.clear();
      head_ = boost::none;
    }
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::eraseRightSiblings_(const Key& key)
  {
    // erase this node and all right siblings and children
    typename tree_hash_map::iterator iter = internalMap.find(key);
    if (iter == internalMap.end())
      return;
    if (iter->second.nextSibling != NULL)
      eraseRightSiblings_(iter->second.nextSibling->data);
    if (iter->second.firstChild != NULL)
      erase_children(key);
    internalMap.erase(iter);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::erase(const Key& key)
  {
    typename tree_hash_map::iterator iter = internalMap.find(key);
    if (iter == internalMap.end())
      return;
    if (iter->second.firstChild != NULL)
      erase_children(key);
    const optional_value prev = previous_sibling(key);
    if (prev) {
      // attach next sibling to previous sibling
      typename tree_hash_map::iterator iter2 = internalMap.find(*prev);
      assert( iter2 != internalMap.end() );
      iter2->second.nextSibling = iter->second.nextSibling;
    } else if (iter->second.parent != NULL) {
      // attach next sibling as first child of parent
      iter->second.parent->firstChild = iter->second.nextSibling;
    }
    if (key == head_) {
      if (iter->second.nextSibling != NULL)
        head_ = iter->second.nextSibling->data;
      else
        head_ = end_;
    }
    // erase node from map
    internalMap.erase(iter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::erase(const iterator_base<Derived,Value,Storage>& iter)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      iterator_base<Derived,Value,Storage> ret_iter(*this,*iter);
      do {
        ret_iter++;
      } while(ret_iter != iterator_base<Derived,Value,Storage>(*this,end_)
              && is_in_subtree(*iter,*ret_iter));
      erase(*iter);
      return ret_iter;
    }
    return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::erase_children(const Key& key) {
    typename tree_hash_map::iterator iter = internalMap.find(key);
    if (iter == internalMap.end())
      return;
    if (iter->second.firstChild != NULL) {
      eraseRightSiblings_(iter->second.firstChild->data);
      iter->second.firstChild = NULL;
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::erase_children(const iterator_base<Derived,Value,Storage>& iter)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      erase_children(*iter);
      return iter;
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  const typename Tree<T,Key,Hash>::optional_value
  Tree<T,Key,Hash>::get_head() const
  {
    optional_value ret;
    if (head_) {
      typename tree_hash_map::const_iterator iter = internalMap.find(*head_);
      if (iter != internalMap.end())
        ret = iter->second.data;
    }
    return ret;
  }

  template<typename T, typename Key, typename Hash>
  const std::vector<T>
  Tree<T,Key,Hash>::get_heads() const
  {
    std::vector<T> ret;
    if (head_) {
      typename tree_hash_map::const_iterator iter = internalMap.find(*head_);
      const treeNode* n;
      if (iter != internalMap.end()) {
        n = &(iter->second);
        do {
          ret.push_back(n->data);
          n = n->nextSibling;
        } while (n != NULL);
      }
    }
    return ret;
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::append_child(const Key& node, const T& child)
  {
    typename tree_hash_map::iterator iter = internalMap.find(child);
    assert( iter == internalMap.end() );
    iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    treeNode newNode = treeNode(child);
    newNode.parent = &(iter->second);
    assert( newNode.parent != NULL );
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(child,newNode));
    i3_assert( insertResult.second );
    treeNode* n = &(iter->second);
    assert( n != NULL );
    if (n->firstChild != NULL) {
      // add as last child
      n = n->firstChild;
      while ( n->nextSibling != NULL ) {
        n = n->nextSibling;
      }
      n->nextSibling = &(insertResult.first->second);
    } else {
      // add as only child
      n->firstChild = &(insertResult.first->second);
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::append_child(const iterator_base<Derived,Value,Storage>& iter,
      const T& child)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      append_child(*iter,child);
      return iterator_base<Derived,Value,Storage>(*this,child);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::append_child(const Key& node, const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    assert( !subtree_in_tree(otherTree,node2) );
    typename tree_hash_map::const_iterator iter = otherTree.internalMap.find(node2);
    assert( iter != otherTree.internalMap.end() );
    append_child(node,iter->second.data);
    if (iter->second.firstChild != NULL) {
      // append all children of node2
      const treeNode* n = &(iter->second);
      n = n->firstChild;
      append_child(node2,otherTree,n->data);
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        append_child(node2,otherTree,n->data);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::append_child(const iterator_base<Derived,Value,Storage>& iter,
      const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    i3_assert( iter.node_.treePtr_ != &otherTree );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      append_child(*iter,otherTree,node2);
      return iterator_base<Derived,Value,Storage>(*this,node2);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::append_child(const iterator_base<Derived,Value,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    i3_assert( iter.node_.treePtr_ != otherIter.node_.treePtr_ );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_) &&
        otherIter != iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_)) {
      append_child(*iter,*(otherIter.node_.treePtr_),*otherIter);
      return iterator_base<Derived,Value,Storage>(*this,*otherIter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::append_children(const Key& node, const std::vector<T>& children)
  {
    if (!children.empty()) {
      typename std::vector<T>::const_iterator iter = children.begin();
      append_child(node,*iter);
      const T* lastChild = &(*iter);
      for(iter++;iter!=children.end();iter++) {
        insert_after(*lastChild,*iter);
        lastChild = &(*iter);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::append_children(
      const iterator_base<Derived,Value,Storage>& iter,
      const std::vector<T>& children)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      append_children(*iter,children);
      return first_child(iter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Iterator>
  void
  Tree<T,Key,Hash>::append_children(const Key& node, Iterator iter,
      const Iterator& iter_end)
  {
    if (iter != iter_end) {
      append_child(node,*iter);
      const T* lastChild = &(*iter);
      for(iter++;iter!=iter_end;iter++) {
        insert_after(*lastChild,*iter);
        lastChild = &(*iter);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Iterator>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::append_children(const iterator_base<Derived,Value,Storage>& iter,
      Iterator iter_begin, const Iterator& iter_end)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      append_children(*iter,iter_begin,iter_end);
      return first_child(iter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert(const T& node2)
  {
    treeNode newNode = treeNode(node2);
    if (head_) {
      typename tree_hash_map::iterator iter = internalMap.find(*head_);
      assert( iter != internalMap.end() );
      newNode.nextSibling = &(iter->second);
    }
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(node2,newNode));
    i3_assert( insertResult.second );
    head_ = node2;
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert_after(const T& node2)
  {
    treeNode newNode = treeNode(node2);
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(node2,newNode));
    i3_assert( insertResult.second );
    if (head_) {
      typename tree_hash_map::iterator iter = internalMap.find(*head_);
      assert( iter != internalMap.end() );
      treeNode* n = &(iter->second);
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
      }
      n->nextSibling = &(insertResult.first->second);
    } else
      head_ = node2;
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert(const Key& node, const T& node2)
  {
    typename tree_hash_map::iterator iter = internalMap.find(node2);
    assert( iter == internalMap.end() );
    iter = internalMap.find(node);
    if (iter == internalMap.end())
      return insert(node2);
    treeNode newNode = treeNode(node2);
    newNode.parent = iter->second.parent;
    newNode.nextSibling = &(iter->second);
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(node2,newNode));
    i3_assert( insertResult.second );
    const optional_value prev = previous_sibling(node);
    if (prev) {
      iter = internalMap.find(*prev);
      assert( iter != internalMap.end() );
      iter->second.nextSibling = &(insertResult.first->second);
    } else if (iter->second.parent != NULL) {
      // there is no left sibling, so node2 is the new left-most sibling
      treeNode* n = &(iter->second);
      n = n->parent;
      n->firstChild = &(insertResult.first->second);
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert(const iterator_base<Derived,Value,Storage>& iter,
      const T& node2)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_))
      insert(*iter,node2);
    else
      insert(node2);
    return iterator_base<Derived,Value,Storage>(*this,node2);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert_after(const Key& node, const T& node2)
  {
    typename tree_hash_map::iterator iter = internalMap.find(node2);
    assert( iter == internalMap.end() );
    iter = internalMap.find(node);
    if (iter == internalMap.end())
      return insert_after(node2);
    treeNode newNode = treeNode(node2);
    newNode.parent = iter->second.parent;
    newNode.nextSibling = iter->second.nextSibling;
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(node2,newNode));
    i3_assert( insertResult.second );
    iter->second.nextSibling = &(insertResult.first->second);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert_after(const iterator_base<Derived,Value,Storage>& iter,
      const T& node2)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_))
      insert_after(*iter,node2);
    else
      insert_after(node2);
    return iterator_base<Derived,Value,Storage>(*this,node2);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert_subtree(const Key& node, const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    assert( !subtree_in_tree(otherTree,node2) );
    typename tree_hash_map::const_iterator iter = otherTree.internalMap.find(node2);
    assert( iter != otherTree.internalMap.end() );
    insert(node,iter->second.data);
    if (iter->second.firstChild != NULL) {
      // append all children of node2
      const treeNode* n = &(iter->second);
      n = n->firstChild;
      append_child(node2,otherTree,n->data);
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        append_child(node2,otherTree,n->data);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert_subtree(
      const iterator_base<Derived,Value,Storage>& iter,
      const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    i3_assert( iter.node_.treePtr_ != &otherTree );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      insert_subtree(*iter,otherTree,node2);
      return iterator_base<Derived,Value,Storage>(*this,node2);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert_subtree(
      const iterator_base<Derived,Value,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    i3_assert( iter.node_.treePtr_ != otherIter.node_.treePtr_ );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_) &&
        otherIter != iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_)) {
      insert_subtree(*iter,*(otherIter.node_.treePtr_),*otherIter);
      return iterator_base<Derived,Value,Storage>(*this,*otherIter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::insert_subtree_after(const Key& node,
      const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    assert( !subtree_in_tree(otherTree,node2) );
    typename tree_hash_map::const_iterator iter = otherTree.internalMap.find(node2);
    assert( iter != otherTree.internalMap.end() );
    insert_after(node,iter->second.data);
    if (iter->second.firstChild != NULL) {
      // append all children of node2
      const treeNode* n = &(iter->second);
      n = n->firstChild;
      append_child(node2,otherTree,n->data);
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        append_child(node2,otherTree,n->data);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert_subtree_after(
      const iterator_base<Derived,Value,Storage>& iter,
      const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    i3_assert( iter.node_.treePtr_ != &otherTree );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      insert_subtree_after(*iter,otherTree,node2);
      return iterator_base<Derived,Value,Storage>(*this,node2);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::insert_subtree_after(
      const iterator_base<Derived,Value,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    i3_assert( iter.node_.treePtr_ != otherIter.node_.treePtr_ );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_) &&
        otherIter != iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_)) {
      insert_subtree_after(*iter,*(otherIter.node_.treePtr_),*otherIter);
      return iterator_base<Derived,Value,Storage>(*this,*otherIter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::replace(const Key& node, const T& node2)
  {
    typename tree_hash_map::iterator iter = internalMap.find(node2);
    assert( iter == internalMap.end() );
    iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    treeNode newNode = treeNode(node2);
    newNode.parent = iter->second.parent;
    newNode.nextSibling = iter->second.nextSibling;
    newNode.firstChild = iter->second.firstChild;
    std::pair<typename tree_hash_map::iterator,bool> insertResult;
    insertResult = internalMap.insert(std::make_pair(node2,newNode));
    i3_assert( insertResult.second );
    const optional_value prev = previous_sibling(node);
    if (prev) {
      typename tree_hash_map::iterator iter2 = internalMap.find(*prev);
      assert( iter2 != internalMap.end() );
      iter2->second.nextSibling = &(insertResult.first->second);
    } else if (iter->second.parent != NULL) {
      // there is no left sibling, so node2 is the new left-most sibling
      iter->second.parent->firstChild = &(insertResult.first->second);
    }
    if (newNode.firstChild) {
        treeNode* n = newNode.firstChild;
        while (n) {
            n->parent = &(insertResult.first->second);
            n = n->nextSibling;
        }
    }
    internalMap.erase(iter);
    if (node == head_) {
        head_ = node2;
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::replace(const iterator_base<Derived,Value,Storage>& iter,
      const T& node2)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      replace(*iter,node2);
      return iterator_base<Derived,Value,Storage>(*this,node2);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::replace(const Key& node, const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    assert( !subtree_in_tree(otherTree,node2) );
    typename tree_hash_map::const_iterator iter = otherTree.internalMap.find(node2);
    assert( iter != otherTree.internalMap.end() );
    erase_children(node);
    replace(node,iter->second.data);
    if (iter->second.firstChild != NULL) {
      // append all children of node2
      const treeNode* n = &(iter->second);
      n = n->firstChild;
      append_child(node2,otherTree,n->data);
      while (n->nextSibling) {
        n = n->nextSibling;
        append_child(node2,otherTree,n->data);
      }
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::replace(const iterator_base<Derived,Value,Storage>& iter,
      const Tree<T,Key,Hash>& otherTree, const Key& node2)
  {
    i3_assert( iter.node_.treePtr_ != &otherTree );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      replace(*iter,otherTree,node2);
      return iterator_base<Derived,Value,Storage>(*this,node2);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::replace(const iterator_base<Derived,Value,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    i3_assert( iter.node_.treePtr_ != otherIter.node_.treePtr_ );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_) &&
        otherIter != iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_)) {
      replace(*iter,*(otherIter.node_.treePtr_),*otherIter);
      return iterator_base<Derived,Value,Storage>(*this,*otherIter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::flatten(const Key& node)
  {
    typename tree_hash_map::iterator iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    if (iter->second.firstChild != NULL) {
      // move children to siblings
      treeNode* n = &(iter->second);
      n = n->firstChild;
      n->parent = iter->second.parent;
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        n->parent = iter->second.parent;
      }
      n->nextSibling = iter->second.nextSibling;
      iter->second.nextSibling = iter->second.firstChild;
      iter->second.firstChild = NULL;
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::flatten(const iterator_base<Derived,Value,Storage>& iter)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_))
      flatten(*iter);
    return iter;
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::reparent(const Key& node, const Key& from)
  {
    typename tree_hash_map::iterator iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    typename tree_hash_map::iterator iter2 = internalMap.find(from);
    assert( iter2 != internalMap.end() );
    if (iter2->second.firstChild != NULL) {
      // move firstChild
      treeNode* n = &(iter->second);
      if (n->firstChild != NULL) {
        n = n->firstChild;
        while (n->nextSibling != NULL) {
          n = n->nextSibling;
        }
        n->nextSibling = iter2->second.firstChild;
      } else {
        n->firstChild = iter2->second.firstChild;
      }
      // set parents of all children
      treeNode* n2 = &(iter2->second);
      n2 = n2->firstChild;
      n2->parent = &(iter->second);
      while (n2->nextSibling != NULL) {
        n2 = n2->nextSibling;
        n2->parent = &(iter->second);
      }
      iter2->second.firstChild = NULL;
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::reparent(const iterator_base<Derived,Value,Storage>& iter,
      const Key& from)
  {
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_)) {
      reparent(*iter,from);
      return iterator_base<Derived,Value,Storage>(*this,from);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Value,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  typename Tree<T,Key,Hash>::template iterator_base<Derived,Value,Storage>
  Tree<T,Key,Hash>::reparent(const iterator_base<Derived,Value,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    i3_assert( iter.node_.treePtr_ == otherIter.node_.treePtr_ );
    if (iter != iterator_base<Derived,Value,Storage>(*this,end_) &&
        otherIter != iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_)) {
      reparent(*iter,*otherIter);
      return iterator_base<Derived,Value,Storage>(*this,*otherIter);
    } else
      return iterator_base<Derived,Value,Storage>(*this,end_);
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::merge(const Tree<T,Key,Hash>& otherTree)
  {
    // step through primaries, copying subtrees as necessary
    i3_assert(*this != otherTree);
    if (!otherTree.head_)
      return;
    typename tree_hash_map::const_iterator iter = otherTree.internalMap.find(*(otherTree.head_));
    if (iter == internalMap.end())
      return;
    const treeNode* n = &(iter->second);
    const treeNode* n2 = NULL;
    while (n != NULL) {
      if (!subtree_in_tree(otherTree,n->data)) {
        insert_after(n->data);
        n2 = n->firstChild;
        while (n2 != NULL) {
          append_child(n->data,otherTree,n2->data);
          n2 = n2->nextSibling;
        }
      }
      n = n->nextSibling;
    }
  }

  template<typename T, typename Key, typename Hash>
  void
  Tree<T,Key,Hash>::swap(Tree<T,Key,Hash>& other)
  {
    std::swap(head_,other.head_);
    internalMap.swap(other.internalMap);
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::size() const
  {
    return internalMap.size();
  }

  template<typename T, typename Key, typename Hash>
  bool
  Tree<T,Key,Hash>::empty() const
  {
    return internalMap.empty();
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::depth(const Key& node) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(node);
    if (iter == internalMap.end())
      return 0;
    const treeNode* n = &(iter->second);
    size_type d(0);
    while (n->parent != NULL) {
      d++;
      n = n->parent;
    }
    return d;
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::depth(
      const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return 0;
    else
      return depth(*iter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::depth(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return 0;
    else
      return depth(*iter);
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_children(const Key& node) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    const treeNode* n = &(iter->second);
    size_type d(0);
    if (n->firstChild == NULL)
      return d;
    d++;
    n = n->firstChild;
    while (n->nextSibling != NULL) {
      d++;
      n = n->nextSibling;
    }
    return d;
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_children(
      const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return 0;
    else
      return number_of_children(*iter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_children(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return 0;
    else
      return number_of_children(*iter);
  }

  template<typename T, typename Key, typename Hash>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_siblings(const Key& node) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(node);
    assert( iter != internalMap.end() );
    if (iter->second.parent == NULL) {
      if (!head_)
        return 0;
      iter = internalMap.find(*head_);
      assert( iter != internalMap.end() );
      const treeNode* n = &(iter->second);
      size_type d(0);
      do {
        if (node != n->data)
          d++;
        n = n->nextSibling;
      } while (n != NULL);
      return d;
    } else {
      return number_of_children(iter->second.parent->data)-1;
    }
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_siblings(
      const iterator_base<Derived,T,Storage>& iter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return 0;
    else
      return number_of_siblings(*iter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  typename Tree<T,Key,Hash>::size_type
  Tree<T,Key,Hash>::number_of_siblings(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return 0;
    else
      return number_of_siblings(*iter);
  }

  template<typename T, typename Key, typename Hash>
  bool
  Tree<T,Key,Hash>::is_in_subtree(const Key& haystack, const Key& needle) const
  {
    if (needle == haystack) {
      return true;
    }
    typename tree_hash_map::const_iterator iter = internalMap.find(haystack);
    assert( iter != internalMap.end() );
    const treeNode* n = &(iter->second);
    if (n->firstChild != NULL) {
      n = n->firstChild;
      if (is_in_subtree(n->data,needle))
        return true;
      while (n->nextSibling != NULL) {
        n = n->nextSibling;
        if (is_in_subtree(n->data,needle))
          return true;
      }
    }
    return false;
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  bool
  Tree<T,Key,Hash>::is_in_subtree(
      const iterator_base<Derived,T,Storage>& iter,
      const Key& node2)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return false;
    else
      return is_in_subtree(*iter,node2);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  bool
  Tree<T,Key,Hash>::is_in_subtree(
      const iterator_base<Derived,const T,Storage>& iter,
      const Key& node2) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return false;
    else
      return is_in_subtree(*iter,node2);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  bool
  Tree<T,Key,Hash>::is_in_subtree(
      const iterator_base<Derived,T,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter)
  {
    if (iter == iterator_base<Derived,T,Storage>(*this,end_) ||
        otherIter == iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_))
      return false;
    else
      return is_in_subtree(*iter,*otherIter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage,
           typename Derived2,typename Value2,typename Storage2>
  bool
  Tree<T,Key,Hash>::is_in_subtree(
      const iterator_base<Derived,const T,Storage>& iter,
      const iterator_base<Derived2,Value2,Storage2>& otherIter) const
  {
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_) ||
        otherIter == iterator_base<Derived2,Value2,Storage2>(*(otherIter.node_.treePtr_),end_))
      return false;
    else
      return is_in_subtree(*iter,*otherIter);
  }

  template<typename T, typename Key, typename Hash>
  bool
  Tree<T,Key,Hash>::subtree_in_tree(const Tree<T,Key,Hash>& other, const Key& node) const
  {
    typename tree_hash_map::const_iterator iter = internalMap.find(node);
    if (iter != internalMap.end())
      return true;
    iter = other.internalMap.find(node);
    if (iter == other.internalMap.end())
      return false;
    const treeNode* n = &(iter->second);
    bool ret = false;
    if (n->firstChild != NULL) {
      n = n->firstChild;
      ret |= subtree_in_tree(other,n->data);
      while (!ret && n->nextSibling != NULL) {
        n = n->nextSibling;
        ret |= subtree_in_tree(other,n->data);
      }
    }
    return ret;
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  bool
  Tree<T,Key,Hash>::subtree_in_tree(
      const iterator_base<Derived,T,Storage>& iter)
  {
    i3_assert( iter.node_.treePtr_ != this );
    if (iter == iterator_base<Derived,T,Storage>(*this,end_))
      return false;
    else
      return subtree_in_tree(*(iter.node_.treePtr_),*iter);
  }

  template<typename T, typename Key, typename Hash>
  template<typename Derived,typename Storage>
  bool
  Tree<T,Key,Hash>::subtree_in_tree(
      const iterator_base<Derived,const T,Storage>& iter) const
  {
    i3_assert( iter.node_.treePtr_ != this );
    if (iter == iterator_base<Derived,const T,Storage>(*this,end_))
      return false;
    else
      return subtree_in_tree(*(iter.node_.treePtr_),*iter);
  }


  static const uint32_t CHUNK_SIZE_ = 65535;

  // make a filter to apply to old trees to fix up problems
  template<typename T, typename Key, typename Hash>
  struct load_old_filter
  {
    static T call(Tree<T,Key,Hash>* tree_ptr,const T& item)
    {
      return item;
    }
  };
  template<typename Key, typename Hash>
  struct load_old_filter<I3Particle,Key,Hash>
  {
    static I3Particle call(Tree<I3Particle,Key,Hash>* tree_ptr,const I3Particle& item)
    {
      // In old simulation 'Dark' particles had a copy as a child and therefore
      // the same ParticleID.  This was always forbidden, but not easy to enforce
      // in the old tree.  It's enforced in the new tree.  We want a clone here
      // for the sole purpose of generating a new unique ID.
      if (item.GetShape() == I3Particle::Dark)
        return item.Clone();
      // Any duplicate particles get a new ID by default
      // TODO: track these changes and keep the ID for the particle with daughters
      if (tree_ptr->at(item))
        return item.Clone();
      return item;
    }
  };

  template<typename T, typename Key, typename Hash>
  template<class Archive>
  void
  Tree<T,Key,Hash>::load(Archive & ar, unsigned version)
  {
    clear();
    if (version == 0) {
      // load old-style Tree
      pre_order_iterator iter(*this),iter2;
      pre_order_iterator end = this->end();
      std::stack<pre_order_iterator> iters;
      unsigned int count=0;
      std::stack<unsigned int> counts;
      T item;

      ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
      ar & icecube::serialization::make_nvp("count", count);
      if(count)
        counts.push(count);

      while(!counts.empty()) {
        ar & icecube::serialization::make_nvp("item", item);
        ar & icecube::serialization::make_nvp("count", count);
        item = load_old_filter<T,Key,Hash>::call(this,item);
        if(iters.empty()) {
          iter2 = this->begin();
          do {
            iter = iter2;
            iter2 = this->next_sibling(iter);
          } while (iter2 != this->end());
          iter = this->insert_after(iter,item);
        } else {
          iter = this->append_child(iters.top(), item);
        }
        counts.top()--;

        if(!counts.top()) {
          counts.pop();
          if(!iters.empty())
            iters.pop();
        }

        if(count) {
          counts.push(count);
          iters.push(iter);
        }
      }
    } else {
      // load new Tree
      ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
      boost::dynamic_bitset<uint64_t> nullMask(CHUNK_SIZE_);
      std::vector<I3Particle> dataChunk;
      // TODO: think about implementing our own hash_map<allocator>
      // and swapping hash_maps to make L2 caching better
      std::pair<Key,treeNode > * newNodes;
      std::pair<typename tree_hash_map::iterator,bool> insertResult;
      uint32_t chunkSize(0),numElements(0), i(0), elements(0);
      bool firstChild(true);
      treeNode* n = NULL;
      I3Particle p;
      do {
        nullMask.reset();
        i=0;
        elements=0;
        ar & make_nvp("numBits",chunkSize);
        if (chunkSize <= 0)
          break;
        nullMask.resize(chunkSize);
        std::vector<unsigned long> vec;
        ar & make_nvp("chunkMask",vec);
        from_block_range(vec.begin(), vec.end(), nullMask);
        numElements = nullMask.count();
        newNodes = new std::pair<Key,treeNode >[numElements];
        if (!head_ && numElements > 0) {
          // take first element as root
          ar & make_nvp("particle",p);
          newNodes[0].first = p;
          newNodes[0].second.data = p;
          insertResult = internalMap.insert(newNodes[0]);
          i3_assert( insertResult.second );
          head_ = p;
          n = &(insertResult.first->second);
          i++;
          elements++;
        }
        for(;i<chunkSize;i++) {
          if (nullMask[i]) {
            ar & make_nvp("particle",p);
            newNodes[elements].second.data = p;
            newNodes[elements].first = p;
            insertResult = internalMap.insert(newNodes[elements]);
            i3_assert( insertResult.second );
            if (firstChild) {
              n->firstChild = &(insertResult.first->second);
              insertResult.first->second.parent = n;
            } else {
              n->nextSibling = &(insertResult.first->second);
              insertResult.first->second.parent = n->parent;
            }
            n = &(insertResult.first->second);
            firstChild = true;
            elements++;
          } else {
            if (firstChild)
              firstChild = false;
            else if (n->parent != NULL)
              n = n->parent;
            else {
              chunkSize = 0; // hit root, so break both loops
              break;
            }
          }
        }
        delete[] newNodes;
      } while (chunkSize == CHUNK_SIZE_);
    }
  }


  template<typename T, typename Key, typename Hash>
  template<class Archive>
  void
  Tree<T,Key,Hash>::save(Archive & ar, unsigned version) const
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

    // do pre_order_iteration and find child and sibling pointers that are null
    uint32_t nelements(0);
    if (!head_) {
      ar & make_nvp("numBits",nelements);
      return;
    }
    boost::dynamic_bitset<uint64_t> nullMask(CHUNK_SIZE_);
    std::vector<const I3Particle*> dataChunk;
    typename tree_hash_map::const_iterator iter = internalMap.find(*head_);
    const treeNode* n = &(iter->second);
    const treeNode* nprev = NULL;
    bool firstChild(true);
    do {
      for(nelements=0;nelements<CHUNK_SIZE_;nelements++) {
        if (n != NULL) {
          nullMask.set(nelements,1);
          dataChunk.push_back(&(n->data));
          nprev = n;
          n = n->firstChild;
          firstChild = true;
        } else {
          if (firstChild) {
            firstChild = false;
            n = nprev->nextSibling;
          } else {
            if (nprev == NULL)
              break; // found root
            n = nprev->parent;
            if (n != NULL)
              n = n->nextSibling;
            nprev = nprev->parent;
          }
          nullMask.set(nelements,0);
        }
      }
      ar & make_nvp("numBits",nelements);
      nullMask.resize(nelements);
      // Convert bitset into a vector
      std::vector<unsigned long> vec(nullMask.num_blocks());
      to_block_range(nullMask, vec.begin());
      ar & make_nvp("chunkMask",vec);
      BOOST_FOREACH(const I3Particle* part, dataChunk)
        ar & make_nvp("particle",*part);
      nullMask.reset();
      dataChunk.clear();
    } while (nelements >= CHUNK_SIZE_);
  }

};


#endif
