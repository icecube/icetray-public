#!/usr/bin/env python

import sys

import unittest

from icecube import icetray, dataio, dataclasses
from icecube.dataclasses import I3MCTree,I3Particle,I3ParticleID

from I3Tray import I3Units

def makeParticle():
  p = I3Particle();
  p.time = 0.0
  p.length = 0.0
  p.energy = 0.0
  p.pos = dataclasses.I3Position(0.0,0.0,0.0)
  p.dir = dataclasses.I3Direction(0.0,0.0)
  return p

class TreeTest(unittest.TestCase):
  def test_constructors(self):
    t = I3MCTree()
    self.assertTrue( t.empty(), "tree is not empty" )
    
    p1 = makeParticle()
    t2 = I3MCTree(p1)
    self.assertTrue( not t2.empty(), "tree is empty" )
    self.assertTrue( t2.get_head() == p1, "head != p1" )
    self.assertTrue( t2.has(p1), "p1 is not in the tree")
        
    t3 = I3MCTree(t2)
    self.assertTrue( not t3.empty(), "t3 is empty" )
    self.assertTrue( t3.get_head() == p1, "copy constructor missing p1" )
  
  def test_insertion(self):
    head = makeParticle()
    t = I3MCTree()
    self.assertTrue( t.empty(), "tree is not empty" )
    
    t.insert(head)
    self.assertTrue( not t.empty(), "tree is empty after insertion" )
    self.assertTrue( t.get_head() == head, "head is not head" )
    
    p1 = makeParticle()
    t.insert_after(head,p1)
    self.assertTrue( t.next_sibling(head) == p1, "p1 is not the sibling of head" )
    
    p2 = makeParticle()
    t.insert(p1,p2)
    self.assertTrue( t.next_sibling(head) == p2, "p2 is not the next sibling of head" )
    self.assertTrue( t.next_sibling(p2) == p1, "p1 is not the next sibling of p2" )
    
    p3 = makeParticle()
    t.insert_after(p3)
    self.assertTrue( t.next_sibling(p1) == p3, "p3 is not inserted as the last primary" )
    
    p4 = makeParticle()
    t.insert_after(p2,p4)
    self.assertTrue( t.next_sibling(p2) == p4, "p4 is not the next sibling of p2" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    t2.append_child(head2,makeParticle())
    t.insert_subtree(p3,t2,head2)
    self.assertTrue( t.next_sibling(p1) == head2, "head2 missing" )
    self.assertTrue( t.children(head2), "children of subtree2 not copied" )
    
    head3 = makeParticle()
    t3 = I3MCTree(head3)
    t3.append_child(head3,makeParticle())
    t.insert_subtree_after(head2,t3,head3)
    self.assertTrue( t.next_sibling(head2) == head3, "head3 missing" )
    self.assertTrue( t.children(head3), "children of subtree3 not copied" )
  
  def test_appending(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    self.assertTrue( t.first_child(head) == p1, "p1 is not first child" )
    
    p2 = makeParticle()
    t.append_child(head,p2)
    self.assertTrue( t.first_child(head) == p1, "p1 is not still first child" )
    self.assertTrue( t.next_sibling(p1) == p2, "p2 is not sibling of p1" )
    
    new_children = [makeParticle() for x in range(10)]
    t.append_children(p1,new_children)
    self.assertTrue( t.first_child(p1) == new_children[0], "p1 first child is wrong" )
    self.assertTrue( t.children(p1) == new_children, "children are not equal" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    t2.append_child(head2,makeParticle())
    t.append_child(p2,t2,head2)
    self.assertTrue( t.first_child(p2) == head2, "head2 missing" )
    self.assertTrue( t.children(head2), "children of subtree2 not copied" )
  
  def test_erase(self):
    t = I3MCTree(makeParticle())
    self.assertTrue( not t.empty(), "t is empty" )
    t.clear()
    self.assertTrue( t.empty(), "t is not empty" )
    
    head = makeParticle()
    t2 = I3MCTree(head)
    p1 = makeParticle()
    t2.append_child(head,p1)
    t2.append_child(p1,makeParticle())
    t2.erase(p1)
    self.assertTrue( not t2.children(head), "children remain" )
    
    p2 = makeParticle()
    t2.append_child(head,p2)
    p3 = makeParticle()
    t2.append_child(p2,p3)
    t2.append_child(p3,makeParticle())
    t2.append_child(p3,makeParticle())
    t2.append_child(p3,makeParticle())
    t2.erase_children(p2)
    self.assertTrue( t2.at(p2), "p2 disappeared" )
    self.assertTrue( not t2.children(p2), "p2 still has children" )
  
  def test_replace(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p2 = makeParticle()
    t.replace(p1,p2)
    self.assertTrue( t.at(p2), "p2 not in tree" )
    self.assertTrue( t.first_child(head) == p2, "p2 not in right place" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p3 = makeParticle()
    t2.append_child(head2,p3)
    t2.replace(p3,t,head)
    self.assertTrue( t2.first_child(head2) == head, "p3 still present" )
    self.assertTrue( t2.children(head) == [p2], "p2 not in t2" )
  
  def test_flatten(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    new_children = [makeParticle() for x in range(10)]
    t.append_children(p1,new_children)
    t.flatten(p1)
    self.assertTrue( not t.children(p1), "p1 still has children" )
    self.assertTrue( t.children(head) == [p1]+new_children,
           "p1 has siblings in the right order" )
  
  def test_reparent(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p2 = makeParticle()
    t.append_child(head,p2)
    p1_children = [makeParticle() for x in range(10)]
    t.append_children(p1,p1_children)
    t.reparent(p2,p1)
    self.assertTrue( not t.children(p1), "p1 still has children" )
    self.assertTrue( t.children(p2) == p1_children, "p2 does not have the children" )
  
  def test_merge(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p2 = makeParticle()
    t2.append_child(head2,p2)
    
    t.merge(t2)
    self.assertTrue( t.get_head() == head, "head missing" )
    self.assertTrue( t.next_sibling(head) == head2, "head2 missing" )
    self.assertTrue( t.children(head) == [p1], "p1 missing" )
    self.assertTrue( t.children(head2) == [p2], "p2 missing" )
  
  def test_swap(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p2 = makeParticle()
    t2.append_child(head2,p2)
    
    t.swap(t2)
    self.assertTrue( t.get_head() == head2, "head2 missing" )
    self.assertTrue( t2.get_head() == head, "head missing" )
    self.assertTrue( t.children(head2) == [p2], "p2 missing" )
    self.assertTrue( t2.children(head) == [p1], "p1 missing" )
  
  def test_iterator(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p1_children = [makeParticle() for x in range(3)]
    t.append_children(p1,p1_children)
    p2_children = [makeParticle() for x in range(3)]
    t.append_children(p1_children[1],p2_children)
    
    # tree should look like:
    #       head
    #         |
    #         V
    #         p1
    #       / | \
    #      /  V  \
    #  p1c1 p1c2  p1c3
    #         |
    #         V
    #       / | \
    #      /  V  \
    #  p2c1 p2c2  p2c3
    
    pre_order = [head,p1,p1_children[0],p1_children[1],p2_children[0],
                 p2_children[1],p2_children[2],p1_children[2]]
    post_order = [p1_children[0],p2_children[0],p2_children[1],
                  p2_children[2],p1_children[1],p1_children[2],p1,head]
    leaf_iter = [p1_children[0],p2_children[0],p2_children[1],
                 p2_children[2],p1_children[2]]
    
    self.assertTrue( list(t) == pre_order, "default iter failed")
    self.assertTrue( list(t.pre_order_iter()) == pre_order, "pre_order failed" )
    self.assertTrue( list(t.post_order_iter()) == post_order, "post_order failed" )
    self.assertTrue( list(t.sibling_iter(p1_children[0])) == p1_children,
           "sibling_iter failed" )
    fast_iter_ret = set([x.minor_id for x in t.fast_iter()])
    fast_iter_ret ^= set([x.minor_id for x in pre_order])
    self.assertTrue( not fast_iter_ret , "fast_iter failed" )
    leaf_iter_ret = set([x.minor_id for x in t.leaf_iter()])
    leaf_iter_ret ^= set([x.minor_id for x in leaf_iter])
    self.assertTrue( not leaf_iter_ret , "leaf_iter failed" )

  def test_error_cases(self):
    t = I3MCTree()
    p1 = makeParticle()
    t2 = I3MCTree(p1)
    self.assertTrue( t2.has(p1), "p1 is not in the tree")
    self.assertTrue( p1 == t2[p1], ".at() does not return p1")
    try:
        t[p1]
    except IndexError:
        pass
    else:
        raise Exception('missing particle lookup did not raise')
    try:
        t[1]
    except IndexError:
        pass
    else:
        raise Exception('missing particle lookup did not raise')

    self.assertTrue( p1 == t2[-1], "[-1] does not return p1")
    try:
        t[-1]
    except IndexError:
        pass
    else:
        raise Exception('missing particle lookup did not raise')
    try:
        t2[-5]
    except IndexError:
        pass
    else:
        raise Exception('missing particle lookup did not raise')

    p2 = makeParticle()
    t2.append_child(p1,p2)
    self.assertEqual( p2, t2[1])
    self.assertEqual( [p1,p2], t2[:])
    self.assertEqual( [p2], t2[1:])
    self.assertEqual( [p1], t2[:-1])
    self.assertEqual( [p1], t2[::2])

    self.assertTrue( p1 in t2 )
    self.assertFalse( p1 in t )
    self.assertTrue( p1 not in t )


  def test_bool_operator(self):
    t = I3MCTree()
    self.assertFalse(bool(t))
    t = I3MCTree( makeParticle())
    self.assertTrue(bool(t))        

  def test_tree_traversal(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    p2 = makeParticle()
    t.append_child(head,p1)
    t.append_child(head,p2)

    self.assertEqual( head, t.parent(p1) )
    self.assertEqual( head, t.parent(p2) )

    p3 = makeParticle()
    try:
        t.parent(p3)
    except IndexError:
        pass
    else:
        raise Exception('did not raise exception')

    self.assertEqual( p1, t.previous_sibling(p2) )
    try:
        t.previous_sibling(head)
    except IndexError:
        pass
    else:
        raise Exception('did not raise exception')

  def test_replace_head_and_copy(self):
    head = makeParticle()
    t = I3MCTree()
    t.insert(head)
    p1 = makeParticle()
    c1 = makeParticle()
    c2 = makeParticle()
    c3 = makeParticle()
    t.insert_after(p1)
    t.append_child(head,c1)
    t.append_child(head,c2)
    t.append_child(head,c3)

    p3 = makeParticle()
    t.replace(head, p3)

    self.assertEqual(t.get_heads(), [p3, p1])
    self.assertEqual(t.children(p3), [c1,c2,c3])
    self.assertEqual(t.parent(c1), p3)
    self.assertEqual(t.parent(c2), p3)
    self.assertEqual(t.parent(c3), p3)

    t2 = I3MCTree(t)

    self.assertEqual(t2.get_heads(), [p3, p1])
    self.assertEqual(t2.children(p3), [c1,c2,c3])
    self.assertEqual(t2.parent(c1), p3)
    self.assertEqual(t2.parent(c2), p3)
    self.assertEqual(t2.parent(c3), p3)

if __name__ == '__main__':
  unittest.main()
