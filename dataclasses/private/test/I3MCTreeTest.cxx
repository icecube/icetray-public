/**
 * @file I3MCTreeTest.cxx
 * @brief
 * 
 * copyright (C) 2013 the icecube collaboration
 * 
 * $Id$
 * @version $Revision$
 * @date $Date$
 */
#include <vector>
#include <algorithm>
#include <I3Test.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <icetray/I3Frame.h>
#include <sstream>
#include <archive/xml_iarchive.hpp>
#include <archive/xml_oarchive.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

using namespace std;

// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3MCTreeTest);

I3Particle makeParticle()
{
  I3Particle p;
  p.SetTime(0.0);
  p.SetLength(0.0);
  p.SetEnergy(0.0);
  p.SetPos(0.0,0.0,0.0);
  p.SetDir(0.0,0.0);
  return p;
}


// Run all the TreeBase::Tree tests (the I3MCTree methods)

TEST(constructors)
{
  I3MCTree t;
  I3MCTree tt;
  ENSURE( t == tt , "empty trees are not equal");
  
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  t1.insert(p1);
  
  ENSURE( I3ParticleID(*t1.begin()) == I3ParticleID(p1) , "*begin() != p1");
  ENSURE( t1.begin() != t1.end() , "Tree t1 is empty");
  ENSURE( (bool)t1.get_head() , "Tree is empty");
  
  ENSURE( t1.get_head() == p1 , "Particle does not match");
  
  I3MCTree t2(p1);
  ENSURE( t2.get_head() == p1 , "Particle2 does not match");
  ENSURE( t2.begin() != t2.end() , "Tree t2 is empty");
  ENSURE( t1 == t2 , "Particle constructor failed");
  
  I3MCTree t3 = t1;
  ENSURE( t3.begin() != t3.end() , "Tree t3 is empty");
  ENSURE( t1 == t3 , "t3 assignment operator failed");
  
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  I3MCTree t4(t1);
  ENSURE( t4.begin() != t4.end() , "Tree t4 is empty");
  ENSURE( t1 == t4 , "t4 assignment operator failed");
  
  I3MCTree t5(makeParticle());
  ENSURE( t5.begin() != t5.end() , "Tree t5 is empty");
  ENSURE( t1 != t5 , "!= operator failed");
}

TEST(pre_order_iterator)
{
  I3MCTree t1;
  ENSURE( t1.begin() == t1.end() );
  I3MCTree t2(makeParticle());
  I3MCTree::iterator iter(t2);
  ENSURE( iter != t2.end() , "begin() == end()");
  ENSURE( t2.get_head() == *iter , "begin() != head");
  iter++;
  ENSURE( iter == t2.end() , "iter does not reach end()");
  I3Particle head = makeParticle();
  I3MCTree t3(head);
  I3Particle p1 = makeParticle();
  t3.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t3.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t3.append_child(p1,p3);
  I3Particle p4 = makeParticle();
  t3.append_child(p2,p4);
  iter = t3.begin();
  ENSURE( iter != t3.end() , "hit end instead of head" );
  ENSURE( *iter == head );
  iter++;
  ENSURE( iter != t3.end() , "hit end instead of p1" );
  ENSURE( *iter == p1);
  iter++;
  ENSURE( iter != t3.end() , "hit end instead of p3" );
  ENSURE( *iter == p3);
  iter++;
  ENSURE( iter != t3.end() , "hit end instead of p2" );
  ENSURE( *iter == p2);
  iter++;
  ENSURE( iter != t3.end() , "hit end instead of p4" );
  ENSURE( *iter == p4);
  iter++;
  ENSURE( iter == t3.end() );
  
  I3MCTree t4;
  ENSURE( t4.cbegin() == t4.cend() , "const_iterator end broken" );
  I3MCTree t5(makeParticle());
  I3MCTree::const_iterator iter2(t5);
  ENSURE( iter2 != t5.cend() , "const_iterator begin() == end()");
  ENSURE( t5.get_head() == *iter2 , "const_iterator begin() != head");
  iter2++;
  ENSURE( iter2 == t5.cend() , "const_iterator iter does not reach end()");
  I3Particle head2 = makeParticle();
  I3MCTree t6(head2);
  I3Particle p5 = makeParticle();
  t6.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t6.append_child(head2,p6);
  I3Particle p7 = makeParticle();
  t6.append_child(p5,p7);
  I3Particle p8 = makeParticle();
  t6.append_child(p6,p8);
  iter2 = t6.cbegin();
  ENSURE( iter2 != t6.cend() , "const_iterator hit end instead of head" );
  ENSURE( *iter2 == head2 );
  iter2++;
  ENSURE( iter2 != t6.cend() , "const_iterator hit end instead of p1" );
  ENSURE( *iter2 == p5);
  iter2++;
  ENSURE( iter2 != t6.cend() , "const_iterator hit end instead of p3" );
  ENSURE( *iter2 == p7);
  iter2++;
  ENSURE( iter2 != t6.cend() , "const_iterator hit end instead of p2" );
  ENSURE( *iter2 == p6);
  iter2++;
  ENSURE( iter2 != t6.cend() , "const_iterator hit end instead of p4" );
  ENSURE( *iter2 == p8);
  iter2++;
  ENSURE( iter2 == t6.cend() );
}

TEST(post_order_iterator)
{
  I3MCTree t1;
  ENSURE( t1.begin_post() == t1.end_post() );
  I3MCTree t2(makeParticle());
  I3MCTree::post_order_iterator iter(t2);
  ENSURE( iter != t2.end_post() , "begin() == end()");
  ENSURE( t2.get_head() == *iter , "begin() != head");
  iter++;
  ENSURE( iter == t2.end_post() , "iter does not reach end()");
  I3Particle head = makeParticle();
  I3MCTree t3(head);
  I3Particle p1 = makeParticle();
  t3.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t3.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t3.append_child(p1,p3);
  I3Particle p4 = makeParticle();
  t3.append_child(p2,p4);
  iter = t3.begin_post();
  ENSURE( iter != t3.end_post() , "hit end instead of p3" );
  ENSURE( *iter == p3);
  iter++;
  ENSURE( iter != t3.end_post() , "hit end instead of p1" );
  ENSURE( *iter == p1);
  iter++;
  ENSURE( iter != t3.end_post() , "hit end instead of p4" );
  ENSURE( *iter == p4);
  iter++;
  ENSURE( iter != t3.end_post() , "hit end instead of p2" );
  ENSURE( *iter == p2);
  iter++;
  ENSURE( iter != t3.end_post() , "hit end instead of head" );
  ENSURE( *iter == head );
  iter++;
  ENSURE( iter == t3.end_post() );
  
  I3MCTree t4;
  ENSURE( t4.cbegin_post() == t4.cend_post() , "const_iterator end broken" );
  I3MCTree t5(makeParticle());
  I3MCTree::post_order_const_iterator iter2(t5);
  ENSURE( iter2 != t5.cend_post() , "const_iterator begin() == end()");
  ENSURE( t5.get_head() == *iter2 , "const_iterator begin() != head");
  iter2++;
  ENSURE( iter2 == t5.cend_post() , "const_iterator iter does not reach end()");
  I3Particle head2 = makeParticle();
  I3MCTree t6(head2);
  I3Particle p5 = makeParticle();
  t6.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t6.append_child(head2,p6);
  I3Particle p7 = makeParticle();
  t6.append_child(p5,p7);
  I3Particle p8 = makeParticle();
  t6.append_child(p6,p8);
  iter2 = t6.cbegin_post();
  ENSURE( iter2 != t6.cend_post() , "const_iterator hit end instead of p3" );
  ENSURE( *iter2 == p7);
  iter2++;
  ENSURE( iter2 != t6.cend_post() , "const_iterator hit end instead of p1" );
  ENSURE( *iter2 == p5);
  iter2++;
  ENSURE( iter2 != t6.cend_post() , "const_iterator hit end instead of p4" );
  ENSURE( *iter2 == p8);
  iter2++;
  ENSURE( iter2 != t6.cend_post() , "const_iterator hit end instead of p2" );
  ENSURE( *iter2 == p6);
  iter2++;
  ENSURE( iter2 != t6.cend_post() , "const_iterator hit end instead of head" );
  ENSURE( *iter2 == head2 );
  iter2++;
  ENSURE( iter2 == t6.cend_post() );
}

TEST(sibling_iterator)
{
  I3MCTree t1(makeParticle());
  I3MCTree::sibling_iterator iter(t1);
  ENSURE( iter == t1.end_sibling() , "empty != end()");
  iter = t1.begin_sibling(*t1.get_head());
  ENSURE( t1.get_head() == *iter , "begin() != head");
  ENSURE( *t1.begin() == *t1.begin_sibling(t1.begin()) ,
         "init with another iterator failed");
  iter++;
  ENSURE( iter == t1.end_sibling() , "iter does not reach end()");
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  I3Particle p1 = makeParticle();
  t2.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t2.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t2.append_child(p1,p3);
  I3Particle p4 = makeParticle();
  t2.append_child(p2,p4);
  iter = t2.begin_sibling(head);
  ENSURE( iter != t2.end_sibling() , "hit end instead of head" );
  ENSURE( *iter == head );
  iter++;
  ENSURE( iter == t2.end_sibling() );
  iter = t2.begin_sibling(p1);
  ENSURE( iter != t2.end_sibling() , "hit end instead of p1" );
  ENSURE( *iter == p1);
  iter++;
  ENSURE( iter != t2.end_sibling() , "hit end instead of p2" );
  ENSURE( *iter == p2);
  iter++;
  ENSURE( iter == t2.end_sibling() );
  iter = t2.begin_sibling(p2);
  ENSURE( iter != t2.end_sibling() , "hit end instead of p2" );
  ENSURE( *iter == p2);
  iter++;
  ENSURE( iter == t2.end_sibling() );
  iter = t2.begin_sibling(p3);
  ENSURE( iter != t2.end_sibling() , "hit end instead of p3" );
  ENSURE( *iter == p3);
  iter++;
  ENSURE( iter == t2.end_sibling() );
  iter = t2.begin_sibling(p4);
  ENSURE( iter != t2.end_sibling() , "hit end instead of p4" );
  ENSURE( *iter == p4);
  iter++;
  ENSURE( iter == t2.end_sibling() );
  
  I3MCTree t3(makeParticle());
  I3MCTree::sibling_const_iterator iter2(t3);
  ENSURE( iter2 == t3.cend_sibling() , "const_iterator empty != end()");
  iter2 = t3.cbegin_sibling(*t3.get_head());
  ENSURE( t3.get_head() == *iter2 , "const_iterator begin() != head");
  ENSURE( *t3.begin() == *t3.cbegin_sibling(t3.cbegin()) ,
         "const_iterator init with another iterator failed");
  iter2++;
  ENSURE( iter2 == t3.cend_sibling() , "const_iterator iter does not reach end()");
  I3Particle head2 = makeParticle();
  I3MCTree t4(head2);
  I3Particle p5 = makeParticle();
  t4.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t4.append_child(head2,p6);
  I3Particle p7 = makeParticle();
  t4.append_child(p5,p7);
  I3Particle p8 = makeParticle();
  t4.append_child(p6,p8);
  iter2 = t4.cbegin_sibling(head2);
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of head" );
  ENSURE( *iter2 == head2 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 == t4.cend_sibling() );
  iter2 = t4.cbegin_sibling(p5);
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of p5" );
  ENSURE( *iter2 == p5 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of p6" );
  ENSURE( *iter2 == p6 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 == t4.cend_sibling() );
  iter2 = t4.cbegin_sibling(p6);
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of p6" );
  ENSURE( *iter2 == p6 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 == t4.cend_sibling() );
  iter2 = t4.cbegin_sibling(p7);
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of p7" );
  ENSURE( *iter2 == p7 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 == t4.cend_sibling() );
  iter2 = t4.cbegin_sibling(p8);
  ENSURE( iter2 != t4.cend_sibling() , "const_iterator hit end instead of p8" );
  ENSURE( *iter2 == p8 , "const_iterator failure");
  iter2++;
  ENSURE( iter2 == t4.cend_sibling() , "const_iterator failed to end");
}

TEST(fast_iterator)
{
  I3MCTree t1;
  ENSURE( t1.begin_fast() == t1.end_fast() );
  I3MCTree t2(makeParticle());
  I3MCTree::fast_iterator iter(t2);
  ENSURE( iter != t2.end_fast() , "begin() == end()");
  ENSURE( t2.get_head() == *iter , "begin() != head");
  iter++;
  ENSURE( iter == t2.end_fast() , "iter does not reach end()");
  I3Particle head = makeParticle();
  I3MCTree t3(head);
  I3Particle p1 = makeParticle();
  t3.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t3.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t3.append_child(p1,p3);
  I3Particle p4 = makeParticle();
  t3.append_child(p2,p4);
  iter = t3.begin_fast();
  std::vector<I3Particle> nodes;
  for (;iter != t3.end_fast();iter++)
    nodes.push_back(*iter);
  ENSURE( !nodes.empty() , "nodes is empty" );
  ENSURE( std::find(nodes.begin(),nodes.end(),head) != nodes.end() ,
         "head not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p1) != nodes.end() ,
         "p1 not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p2) != nodes.end() ,
         "p2 not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p3) != nodes.end() ,
         "p3 not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p4) != nodes.end() ,
         "p4 not found");
  ENSURE( nodes.size() == t3.size() , "nodes.size() incorrect" );
  
  I3MCTree t4;
  ENSURE( t4.cbegin_fast() == t4.cend_fast() , "const_iterator end failure" );
  I3MCTree t5(makeParticle());
  I3MCTree::fast_const_iterator iter2(t5);
  ENSURE( iter2 != t5.cend_fast() , "const_iterator begin() == end()");
  ENSURE( t5.get_head() == *iter2 , "const_iterator begin() != head");
  iter2++;
  ENSURE( iter2 == t5.cend_fast() , "const_iterator iter does not reach end()");
  I3Particle head2 = makeParticle();
  I3MCTree t6(head2);
  I3Particle p5 = makeParticle();
  t6.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t6.append_child(head2,p6);
  I3Particle p7 = makeParticle();
  t6.append_child(p5,p7);
  I3Particle p8 = makeParticle();
  t6.append_child(p6,p8);
  iter2 = t6.cbegin_fast();
  std::vector<I3Particle> nodes2;
  for (;iter2 != t6.cend_fast();iter2++)
    nodes2.push_back(*iter2);
  ENSURE( !nodes2.empty() , "const_iterator nodes is empty" );
  ENSURE( std::find(nodes2.begin(),nodes2.end(),head2) != nodes2.end() ,
         "const_iterator head2 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p5) != nodes2.end() ,
         "const_iterator p5 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p6) != nodes2.end() ,
         "const_iterator p6 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p7) != nodes2.end() ,
         "const_iterator p7 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p8) != nodes2.end() ,
         "const_iterator p8 not found");
  ENSURE( nodes2.size() == t6.size() ,
         "const_iterator nodes2.size() incorrect" );
}

TEST(leaf_iterator)
{
  I3MCTree t1;
  ENSURE( t1.begin_leaf() == t1.end_leaf() );
  I3MCTree t2(makeParticle());
  I3MCTree::leaf_iterator iter(t2);
  ENSURE( iter != t2.end_leaf() , "begin() == end()");
  ENSURE( t2.get_head() == *iter , "begin() != head");
  iter++;
  ENSURE( iter == t2.end_leaf() , "iter does not reach end()");
  I3Particle head = makeParticle();
  I3MCTree t3(head);
  I3Particle p1 = makeParticle();
  t3.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t3.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t3.append_child(p1,p3);
  I3Particle p4 = makeParticle();
  t3.append_child(p1,p4);
  iter = t3.begin_leaf();
  std::vector<I3Particle> nodes;
  for (;iter != t3.end_leaf();iter++)
    nodes.push_back(*iter);
  ENSURE( !nodes.empty() , "nodes is empty" );
  ENSURE( std::find(nodes.begin(),nodes.end(),head) == nodes.end() ,
         "head found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p1) == nodes.end() ,
         "p1 found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p2) != nodes.end() ,
         "p2 not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p3) != nodes.end() ,
         "p3 not found");
  ENSURE( std::find(nodes.begin(),nodes.end(),p4) != nodes.end() ,
         "p4 not found");
  ENSURE( nodes.size() == 3 , "nodes.size() incorrect" );
  
  I3MCTree t4;
  ENSURE( t4.cbegin_leaf() == t4.cend_leaf() , "const_iterator end failure" );
  I3MCTree t5(makeParticle());
  I3MCTree::leaf_const_iterator iter2(t5);
  ENSURE( iter2 != t5.cend_leaf() , "const_iterator begin() == end()");
  ENSURE( t5.get_head() == *iter2 , "const_iterator begin() != head");
  iter2++;
  ENSURE( iter2 == t5.cend_leaf() , "const_iterator iter does not reach end()");
  I3Particle head2 = makeParticle();
  I3MCTree t6(head2);
  I3Particle p5 = makeParticle();
  t6.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t6.append_child(head2,p6);
  I3Particle p7 = makeParticle();
  t6.append_child(p5,p7);
  I3Particle p8 = makeParticle();
  t6.append_child(p5,p8);
  iter2 = t6.cbegin_leaf();
  std::vector<I3Particle> nodes2;
  for (;iter2 != t6.cend_leaf();iter2++)
    nodes2.push_back(*iter2);
  ENSURE( !nodes2.empty() , "const_iterator nodes is empty" );
  ENSURE( std::find(nodes2.begin(),nodes2.end(),head2) == nodes2.end() ,
         "const_iterator head2 found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p5) == nodes2.end() ,
         "const_iterator p5 found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p6) != nodes2.end() ,
         "const_iterator p6 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p7) != nodes2.end() ,
         "const_iterator p7 not found");
  ENSURE( std::find(nodes2.begin(),nodes2.end(),p8) != nodes2.end() ,
         "const_iterator p8 not found");
  ENSURE( nodes2.size() == 3 ,
         "const_iterator nodes2.size() incorrect" );
}

TEST(iterator_conversion)
{
  // note that this is more a compile test than a runtime test
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  
  // default iterator
  I3MCTree::iterator iter(t1,head);
  ENSURE(iter == t1.begin());
  ENSURE(iter != t1.end());
  iter = t1.begin();
  I3MCTree::iterator iter_end = t1.end();
  ENSURE(iter_end == t1.end());
  ENSURE(I3MCTree::iterator(t1) == t1.begin());
  
  I3MCTree::const_iterator iter_const(t1,head);
  ENSURE(iter_const == t1.cbegin());
  ENSURE(iter_const != t1.cend());
  iter_const = t1.cbegin();
  I3MCTree::const_iterator iter_cend = t1.cend();
  ENSURE(iter_cend == t1.cend());
  ENSURE(I3MCTree::const_iterator(t1) == t1.cbegin());
  
  // pre_order
  I3MCTree::pre_order_iterator pre_order_iter(t1,head);
  ENSURE(pre_order_iter == t1.begin());
  ENSURE(pre_order_iter != t1.end());
  pre_order_iter = t1.begin();
  I3MCTree::pre_order_iterator pre_order_iter_end = t1.end();
  ENSURE(pre_order_iter_end == t1.end());
  ENSURE(I3MCTree::pre_order_iterator(t1) == t1.begin());
  
  I3MCTree::pre_order_const_iterator pre_order_iter_const(t1,head);
  ENSURE(pre_order_iter_const == t1.cbegin());
  ENSURE(pre_order_iter_const != t1.cend());
  pre_order_iter_const = t1.cbegin();
  I3MCTree::pre_order_const_iterator pre_order_iter_cend = t1.cend();
  ENSURE(pre_order_iter_cend == t1.cend());
  ENSURE(I3MCTree::pre_order_const_iterator(t1) == t1.cbegin());
  
  // post_order
  I3MCTree::post_order_iterator post_order_iter(t1,head);
  ENSURE(post_order_iter == t1.begin_post());
  ENSURE(post_order_iter != t1.end_post());
  post_order_iter = t1.begin_post();
  I3MCTree::post_order_iterator post_order_iter_end = t1.end_post();
  ENSURE(post_order_iter_end == t1.end_post());
  ENSURE(I3MCTree::post_order_iterator(t1) == t1.begin_post());
  
  I3MCTree::post_order_const_iterator post_order_iter_const(t1,head);
  ENSURE(post_order_iter_const == t1.cbegin_post());
  ENSURE(post_order_iter_const != t1.cend_post());
  post_order_iter_const = t1.cbegin_post();
  I3MCTree::post_order_const_iterator post_order_iter_cend = t1.cend_post();
  ENSURE(post_order_iter_cend == t1.cend_post());
  ENSURE(I3MCTree::post_order_const_iterator(t1) == t1.cbegin_post());
  
  // sibling
  I3MCTree::sibling_iterator sibling_iter(t1,head);
  ENSURE(sibling_iter == t1.begin_sibling(head));
  ENSURE(sibling_iter != t1.end_sibling());
  sibling_iter = t1.begin_sibling(head);
  I3MCTree::sibling_iterator sibling_iter_end = t1.end_sibling();
  ENSURE(sibling_iter_end == t1.end_sibling());
  ENSURE(I3MCTree::sibling_iterator(t1) == t1.end_sibling());
  
  I3MCTree::sibling_const_iterator sibling_iter_const(t1,head);
  ENSURE(sibling_iter_const == t1.cbegin_sibling(head));
  ENSURE(sibling_iter_const != t1.cend_sibling());
  sibling_iter_const = t1.cbegin_sibling(head);
  I3MCTree::sibling_const_iterator sibling_iter_cend = t1.cend_sibling();
  ENSURE(sibling_iter_cend == t1.cend_sibling());
  ENSURE(I3MCTree::sibling_const_iterator(t1) == t1.cend_sibling());
  
  // fast
  I3MCTree::fast_iterator fast_iter(t1,head);
  ENSURE(fast_iter == t1.begin_fast());
  ENSURE(fast_iter != t1.end_fast());
  fast_iter = t1.begin_fast();
  I3MCTree::fast_iterator fast_iter_end = t1.end_fast();
  ENSURE(fast_iter_end == t1.end_fast());
  ENSURE(I3MCTree::fast_iterator(t1) == t1.begin_fast());
  
  I3MCTree::fast_const_iterator fast_iter_const(t1,head);
  ENSURE(fast_iter_const == t1.cbegin_fast());
  ENSURE(fast_iter_const != t1.cend_fast());
  fast_iter_const = t1.cbegin_fast();
  I3MCTree::fast_const_iterator fast_iter_cend = t1.cend_fast();
  ENSURE(fast_iter_cend == t1.cend_fast());
  ENSURE(I3MCTree::fast_const_iterator(t1) == t1.cbegin_fast());
  
  // leaf
  I3MCTree::leaf_iterator leaf_iter(t1,head);
  ENSURE(leaf_iter == t1.begin_leaf());
  ENSURE(leaf_iter != t1.end_leaf());
  leaf_iter = t1.begin_leaf();
  I3MCTree::leaf_iterator leaf_iter_end = t1.end_leaf();
  ENSURE(leaf_iter_end == t1.end_leaf());
  ENSURE(I3MCTree::leaf_iterator(t1) == t1.begin_leaf());
  
  I3MCTree::leaf_const_iterator leaf_iter_const(t1,head);
  ENSURE(leaf_iter_const == t1.cbegin_leaf());
  ENSURE(leaf_iter_const != t1.cend_leaf());
  leaf_iter_const = t1.cbegin_leaf();
  I3MCTree::leaf_const_iterator leaf_iter_cend = t1.cend_leaf();
  ENSURE(leaf_iter_cend == t1.cend_leaf());
  ENSURE(I3MCTree::leaf_const_iterator(t1) == t1.cbegin_leaf());
  
  // mixing iterators
  iter = pre_order_iter;
  iter = post_order_iter;
  iter = sibling_iter;
  iter = fast_iter;
  iter = leaf_iter;
  
  pre_order_iter = iter;
  pre_order_iter = post_order_iter;
  pre_order_iter = sibling_iter;
  pre_order_iter = fast_iter;
  pre_order_iter = leaf_iter;
  
  post_order_iter = iter;
  post_order_iter = pre_order_iter;
  post_order_iter = sibling_iter;
  post_order_iter = fast_iter;
  post_order_iter = leaf_iter;
  
  sibling_iter = iter;
  sibling_iter = pre_order_iter;
  sibling_iter = post_order_iter;
  sibling_iter = fast_iter;
  sibling_iter = leaf_iter;
  
  fast_iter = iter;
  fast_iter = pre_order_iter;
  fast_iter = post_order_iter;
  fast_iter = sibling_iter;
  fast_iter = leaf_iter;
  
  leaf_iter = iter;
  leaf_iter = pre_order_iter;
  leaf_iter = post_order_iter;
  leaf_iter = sibling_iter;
  leaf_iter = fast_iter;
  
  iter_const = pre_order_iter_const;
  iter_const = post_order_iter_const;
  iter_const = sibling_iter_const;
  iter_const = fast_iter_const;
  iter_const = leaf_iter_const;
  
  pre_order_iter_const = iter_const;
  pre_order_iter_const = post_order_iter_const;
  pre_order_iter_const = sibling_iter_const;
  pre_order_iter_const = fast_iter_const;
  pre_order_iter_const = leaf_iter_const;
  
  post_order_iter_const = iter_const;
  post_order_iter_const = pre_order_iter_const;
  post_order_iter_const = sibling_iter_const;
  post_order_iter_const = fast_iter_const;
  post_order_iter_const = leaf_iter_const;
  
  sibling_iter_const = iter_const;
  sibling_iter_const = pre_order_iter_const;
  sibling_iter_const = post_order_iter_const;
  sibling_iter_const = fast_iter_const;
  sibling_iter_const = leaf_iter_const;
  
  fast_iter_const = iter_const;
  fast_iter_const = pre_order_iter_const;
  fast_iter_const = post_order_iter_const;
  fast_iter_const = sibling_iter_const;
  fast_iter_const = leaf_iter_const;
  
  leaf_iter_const = iter_const;
  leaf_iter_const = pre_order_iter_const;
  leaf_iter_const = post_order_iter_const;
  leaf_iter_const = sibling_iter_const;
  leaf_iter_const = fast_iter_const;
  
  // mixing constructors
  I3MCTree::iterator iter1(iter);
  I3MCTree::iterator iter2(pre_order_iter);
  I3MCTree::iterator iter3(post_order_iter);
  I3MCTree::iterator iter4(sibling_iter);
  I3MCTree::iterator iter5(fast_iter);
  I3MCTree::iterator iter6(leaf_iter);

  I3MCTree::pre_order_iterator pre_order_iter1(iter);
  I3MCTree::pre_order_iterator pre_order_iter2(pre_order_iter);
  I3MCTree::pre_order_iterator pre_order_iter3(post_order_iter);
  I3MCTree::pre_order_iterator pre_order_iter4(sibling_iter);
  I3MCTree::pre_order_iterator pre_order_iter5(fast_iter);
  I3MCTree::pre_order_iterator pre_order_iter6(leaf_iter);
  
  I3MCTree::post_order_iterator post_order_iter1(iter);
  I3MCTree::post_order_iterator post_order_iter2(pre_order_iter);
  I3MCTree::post_order_iterator post_order_iter3(post_order_iter);
  I3MCTree::post_order_iterator post_order_iter4(sibling_iter);
  I3MCTree::post_order_iterator post_order_iter5(fast_iter);
  I3MCTree::post_order_iterator post_order_iter6(leaf_iter);
  
  I3MCTree::sibling_iterator sibling_iter1(iter);
  I3MCTree::sibling_iterator sibling_iter2(pre_order_iter);
  I3MCTree::sibling_iterator sibling_iter3(post_order_iter);
  I3MCTree::sibling_iterator sibling_iter4(sibling_iter);
  I3MCTree::sibling_iterator sibling_iter5(fast_iter);
  I3MCTree::sibling_iterator sibling_iter6(leaf_iter);
  
  I3MCTree::fast_iterator fast_iter1(iter);
  I3MCTree::fast_iterator fast_iter2(pre_order_iter);
  I3MCTree::fast_iterator fast_iter3(post_order_iter);
  I3MCTree::fast_iterator fast_iter4(sibling_iter);
  I3MCTree::fast_iterator fast_iter5(fast_iter);
  I3MCTree::fast_iterator fast_iter6(leaf_iter);
  
  I3MCTree::leaf_iterator leaf_iter1(iter);
  I3MCTree::leaf_iterator leaf_iter2(pre_order_iter);
  I3MCTree::leaf_iterator leaf_iter3(post_order_iter);
  I3MCTree::leaf_iterator leaf_iter4(sibling_iter);
  I3MCTree::leaf_iterator leaf_iter5(fast_iter);
  I3MCTree::leaf_iterator leaf_iter6(leaf_iter);
  
  I3MCTree::const_iterator iter_const1(iter_const);
  I3MCTree::const_iterator iter_const2(pre_order_iter_const);
  I3MCTree::const_iterator iter_const3(post_order_iter_const);
  I3MCTree::const_iterator iter_const4(sibling_iter_const);
  I3MCTree::const_iterator iter_const5(fast_iter_const);
  I3MCTree::const_iterator iter_const6(leaf_iter_const);

  I3MCTree::pre_order_const_iterator pre_order_iter_const1(iter_const);
  I3MCTree::pre_order_const_iterator pre_order_iter_const2(pre_order_iter_const);
  I3MCTree::pre_order_const_iterator pre_order_iter_const3(post_order_iter_const);
  I3MCTree::pre_order_const_iterator pre_order_iter_const4(sibling_iter_const);
  I3MCTree::pre_order_const_iterator pre_order_iter_const5(fast_iter_const);
  I3MCTree::pre_order_const_iterator pre_order_iter_const6(leaf_iter_const);
  
  I3MCTree::post_order_const_iterator post_order_iter_const1(iter_const);
  I3MCTree::post_order_const_iterator post_order_iter_const2(pre_order_iter_const);
  I3MCTree::post_order_const_iterator post_order_iter_const3(post_order_iter_const);
  I3MCTree::post_order_const_iterator post_order_iter_const4(sibling_iter_const);
  I3MCTree::post_order_const_iterator post_order_iter_const5(fast_iter_const);
  I3MCTree::post_order_const_iterator post_order_iter_const6(leaf_iter_const);
  
  I3MCTree::sibling_const_iterator sibling_iter_const1(iter_const);
  I3MCTree::sibling_const_iterator sibling_iter_const2(pre_order_iter_const);
  I3MCTree::sibling_const_iterator sibling_iter_const3(post_order_iter_const);
  I3MCTree::sibling_const_iterator sibling_iter_const4(sibling_iter_const);
  I3MCTree::sibling_const_iterator sibling_iter_const5(fast_iter_const);
  I3MCTree::sibling_const_iterator sibling_iter_const6(leaf_iter_const);
  
  I3MCTree::fast_const_iterator fast_iter_const1(iter_const);
  I3MCTree::fast_const_iterator fast_iter_const2(pre_order_iter_const);
  I3MCTree::fast_const_iterator fast_iter_const3(post_order_iter_const);
  I3MCTree::fast_const_iterator fast_iter_const4(sibling_iter_const);
  I3MCTree::fast_const_iterator fast_iter_const5(fast_iter_const);
  I3MCTree::fast_const_iterator fast_iter_const6(leaf_iter_const);
  
  I3MCTree::leaf_const_iterator leaf_iter_const1(iter_const);
  I3MCTree::leaf_const_iterator leaf_iter_const2(pre_order_iter_const);
  I3MCTree::leaf_const_iterator leaf_iter_const3(post_order_iter_const);
  I3MCTree::leaf_const_iterator leaf_iter_const4(sibling_iter_const);
  I3MCTree::leaf_const_iterator leaf_iter_const5(fast_iter_const);
  I3MCTree::leaf_const_iterator leaf_iter_const6(leaf_iter_const);
  
  // now switch between const and non-const
  I3MCTree::const_iterator iter_const11(iter);
  I3MCTree::const_iterator iter_const21(pre_order_iter);
  I3MCTree::const_iterator iter_const31(post_order_iter);
  I3MCTree::const_iterator iter_const41(sibling_iter);
  I3MCTree::const_iterator iter_const51(fast_iter);
  I3MCTree::const_iterator iter_const61(leaf_iter);
  
  I3MCTree::pre_order_const_iterator pre_order_iter_const11(iter);
  I3MCTree::pre_order_const_iterator pre_order_iter_const21(pre_order_iter);
  I3MCTree::pre_order_const_iterator pre_order_iter_const31(post_order_iter);
  I3MCTree::pre_order_const_iterator pre_order_iter_const41(sibling_iter);
  I3MCTree::pre_order_const_iterator pre_order_iter_const51(fast_iter);
  I3MCTree::pre_order_const_iterator pre_order_iter_const61(leaf_iter);
  
  I3MCTree::post_order_const_iterator post_order_iter_const11(iter);
  I3MCTree::post_order_const_iterator post_order_iter_const21(pre_order_iter);
  I3MCTree::post_order_const_iterator post_order_iter_const31(post_order_iter);
  I3MCTree::post_order_const_iterator post_order_iter_const41(sibling_iter);
  I3MCTree::post_order_const_iterator post_order_iter_const51(fast_iter);
  I3MCTree::post_order_const_iterator post_order_iter_const61(leaf_iter);
  
  I3MCTree::sibling_const_iterator sibling_iter_const11(iter);
  I3MCTree::sibling_const_iterator sibling_iter_const21(pre_order_iter);
  I3MCTree::sibling_const_iterator sibling_iter_const31(post_order_iter);
  I3MCTree::sibling_const_iterator sibling_iter_const41(sibling_iter);
  I3MCTree::sibling_const_iterator sibling_iter_const51(fast_iter);
  I3MCTree::sibling_const_iterator sibling_iter_const61(leaf_iter);
  
  I3MCTree::fast_const_iterator fast_iter_const11(iter);
  I3MCTree::fast_const_iterator fast_iter_const21(pre_order_iter);
  I3MCTree::fast_const_iterator fast_iter_const31(post_order_iter);
  I3MCTree::fast_const_iterator fast_iter_const41(sibling_iter);
  I3MCTree::fast_const_iterator fast_iter_const51(fast_iter);
  I3MCTree::fast_const_iterator fast_iter_const61(leaf_iter);
  
  I3MCTree::leaf_const_iterator leaf_iter_const11(iter);
  I3MCTree::leaf_const_iterator leaf_iter_const21(pre_order_iter);
  I3MCTree::leaf_const_iterator leaf_iter_const31(post_order_iter);
  I3MCTree::leaf_const_iterator leaf_iter_const41(sibling_iter);
  I3MCTree::leaf_const_iterator leaf_iter_const51(fast_iter);
  I3MCTree::leaf_const_iterator leaf_iter_const61(leaf_iter);
  
  // now do initializing assignment
  I3MCTree::iterator iter12 = iter;
  I3MCTree::iterator iter22 = pre_order_iter;
  I3MCTree::iterator iter32 = post_order_iter;
  I3MCTree::iterator iter42 = sibling_iter;
  I3MCTree::iterator iter52 = fast_iter;
  I3MCTree::iterator iter62 = leaf_iter;

  I3MCTree::pre_order_iterator pre_order_iter12 = iter;
  I3MCTree::pre_order_iterator pre_order_iter22 = pre_order_iter;
  I3MCTree::pre_order_iterator pre_order_iter32 = post_order_iter;
  I3MCTree::pre_order_iterator pre_order_iter42 = sibling_iter;
  I3MCTree::pre_order_iterator pre_order_iter52 = fast_iter;
  I3MCTree::pre_order_iterator pre_order_iter62 = leaf_iter;
  
  I3MCTree::post_order_iterator post_order_iter12 = iter;
  I3MCTree::post_order_iterator post_order_iter22 = pre_order_iter;
  I3MCTree::post_order_iterator post_order_iter32 = post_order_iter;
  I3MCTree::post_order_iterator post_order_iter42 = sibling_iter;
  I3MCTree::post_order_iterator post_order_iter52 = fast_iter;
  I3MCTree::post_order_iterator post_order_iter62 = leaf_iter;
  
  I3MCTree::sibling_iterator sibling_iter12 = iter;
  I3MCTree::sibling_iterator sibling_iter22 = pre_order_iter;
  I3MCTree::sibling_iterator sibling_iter32 = post_order_iter;
  I3MCTree::sibling_iterator sibling_iter42 = sibling_iter;
  I3MCTree::sibling_iterator sibling_iter52 = fast_iter;
  I3MCTree::sibling_iterator sibling_iter62 = leaf_iter;
  
  I3MCTree::fast_iterator fast_iter12 = iter;
  I3MCTree::fast_iterator fast_iter22 = pre_order_iter;
  I3MCTree::fast_iterator fast_iter32 = post_order_iter;
  I3MCTree::fast_iterator fast_iter42 = sibling_iter;
  I3MCTree::fast_iterator fast_iter52 = fast_iter;
  I3MCTree::fast_iterator fast_iter62 = leaf_iter;
  
  I3MCTree::leaf_iterator leaf_iter12 = iter;
  I3MCTree::leaf_iterator leaf_iter22 = pre_order_iter;
  I3MCTree::leaf_iterator leaf_iter32 = post_order_iter;
  I3MCTree::leaf_iterator leaf_iter42 = sibling_iter;
  I3MCTree::leaf_iterator leaf_iter52 = fast_iter;
  I3MCTree::leaf_iterator leaf_iter62 = leaf_iter;
  
  I3MCTree::const_iterator iter_const12 = iter_const;
  I3MCTree::const_iterator iter_const22 = pre_order_iter_const;
  I3MCTree::const_iterator iter_const32 = post_order_iter_const;
  I3MCTree::const_iterator iter_const42 = sibling_iter_const;
  I3MCTree::const_iterator iter_const52 = fast_iter_const;
  I3MCTree::const_iterator iter_const62 = leaf_iter_const;
  
  I3MCTree::pre_order_const_iterator pre_order_iter_const12 = iter_const;
  I3MCTree::pre_order_const_iterator pre_order_iter_const22 = pre_order_iter_const;
  I3MCTree::pre_order_const_iterator pre_order_iter_const32 = post_order_iter_const;
  I3MCTree::pre_order_const_iterator pre_order_iter_const42 = sibling_iter_const;
  I3MCTree::pre_order_const_iterator pre_order_iter_const52 = fast_iter_const;
  I3MCTree::pre_order_const_iterator pre_order_iter_const62 = leaf_iter_const;
  
  I3MCTree::post_order_const_iterator post_order_iter_const12 = iter_const;
  I3MCTree::post_order_const_iterator post_order_iter_const22 = pre_order_iter_const;
  I3MCTree::post_order_const_iterator post_order_iter_const32 = post_order_iter_const;
  I3MCTree::post_order_const_iterator post_order_iter_const42 = sibling_iter_const;
  I3MCTree::post_order_const_iterator post_order_iter_const52 = fast_iter_const;
  I3MCTree::post_order_const_iterator post_order_iter_const62 = leaf_iter_const;
  
  I3MCTree::sibling_const_iterator sibling_iter_const12 = iter_const;
  I3MCTree::sibling_const_iterator sibling_iter_const22 = pre_order_iter_const;
  I3MCTree::sibling_const_iterator sibling_iter_const32 = post_order_iter_const;
  I3MCTree::sibling_const_iterator sibling_iter_const42 = sibling_iter_const;
  I3MCTree::sibling_const_iterator sibling_iter_const52 = fast_iter_const;
  I3MCTree::sibling_const_iterator sibling_iter_const62 = leaf_iter_const;
  
  I3MCTree::fast_const_iterator fast_iter_const12 = iter_const;
  I3MCTree::fast_const_iterator fast_iter_const22 = pre_order_iter_const;
  I3MCTree::fast_const_iterator fast_iter_const32 = post_order_iter_const;
  I3MCTree::fast_const_iterator fast_iter_const42 = sibling_iter_const;
  I3MCTree::fast_const_iterator fast_iter_const52 = fast_iter_const;
  I3MCTree::fast_const_iterator fast_iter_const62 = leaf_iter_const;
  
  I3MCTree::leaf_const_iterator leaf_iter_const12 = iter_const;
  I3MCTree::leaf_const_iterator leaf_iter_const22 = pre_order_iter_const;
  I3MCTree::leaf_const_iterator leaf_iter_const32 = post_order_iter_const;
  I3MCTree::leaf_const_iterator leaf_iter_const42 = sibling_iter_const;
  I3MCTree::leaf_const_iterator leaf_iter_const52 = fast_iter_const;
  I3MCTree::leaf_const_iterator leaf_iter_const62 = leaf_iter_const;
  
  //Tell the compiler that variables are 'used', suppressing unhelpful warnings
  #define MARK_USED(r, data, elem) while(0){ elem++; }
  #define TEST_ITERATORS \
  (fast_iter12)(fast_iter22)(fast_iter32)(fast_iter42)(fast_iter52)(fast_iter62)\
  (fast_iter_const1)(fast_iter_const2)(fast_iter_const3)\
  (fast_iter_const4)(fast_iter_const5)(fast_iter_const6)\
  (fast_iter_const12)(fast_iter_const22)(fast_iter_const32)\
  (fast_iter_const42)(fast_iter_const52)(fast_iter_const62)\
  (leaf_iter12)(leaf_iter22)(leaf_iter32)(leaf_iter42)(leaf_iter52)(leaf_iter62)\
  (leaf_iter_const6)(leaf_iter_const12)(leaf_iter_const22)(leaf_iter_const32)\
  (leaf_iter_const42)(leaf_iter_const52)(leaf_iter_const62)\
  (sibling_iter_const12)(sibling_iter_const22)(sibling_iter_const32)\
  (sibling_iter_const42)(sibling_iter_const52)(sibling_iter_const62)
  BOOST_PP_SEQ_FOR_EACH(MARK_USED, _, TEST_ITERATORS)
  #undef MARK_USED
  #undef TEST_ITERATORS
}

TEST(empty)
{
  I3MCTree t1;
  ENSURE( t1.empty() , "tree is not empty");
  
  I3MCTree t2(makeParticle());
  ENSURE( !t2.empty() , "tree is empty");
}

TEST(clear)
{
  I3MCTree t1(makeParticle());
  ENSURE( !t1.empty() , "tree is empty");
  
  t1.clear();
  ENSURE( t1.empty() , "tree did not clear");
}

TEST(get_heads)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  t1.insert_after(p1);
  t1.insert_after(p2);
  t1.insert_after(p3);
  t1.insert_after(p4);
  
  std::vector<I3Particle> heads = t1.get_heads();
  ENSURE( heads.size() == 4 );
  ENSURE( heads.at(0) == p1 );
  ENSURE( heads.at(1) == p2 );
  ENSURE( heads.at(2) == p3 );
  ENSURE( heads.at(3) == p4 );
}

TEST(at)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  t1.insert_after(p1);
  t1.insert_after(p2);
  
  ENSURE( t1.at(p1) == p1 );
  ENSURE( t1.at(p2) == p2 );
  ENSURE( t1[p1] == p1 );
  ENSURE( t1[p2] == p2 );
}

TEST(find)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  t1.insert_after(p1);
  t1.insert_after(p2);
  
  ENSURE( *(t1.find(p1)) == p1 );
  
  const I3MCTree t2(t1);
  ENSURE( *(t2.find(p2)) == p2 );
}

TEST(append_child)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  
  std::vector<I3Particle> children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p2 , "p2 is not first child");
  ENSURE( children2.at(1) == p3 , "p3 is not second child");
  
  I3MCTree t2(makeParticle());
  t2.append_child(*t2.get_head(),t1,p1);
  
  // test assertions
  bool fails_as_expected(false);
  try{
    // attempting at add a subtree to itself should throw
    t2.append_child(*t2.get_head(), t2, p1);
  }catch(const std::exception& e){
    fails_as_expected = true;
  }
  ENSURE(fails_as_expected);

  std::vector<I3Particle> children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  
  std::vector<I3Particle> children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  
  I3MCTree t3(makeParticle());
  I3Particle p4 = *t3.get_head();
  std::vector<I3Particle> newchildren;
  for (int i=0;i<50;i++) {
    newchildren.push_back(makeParticle());
  }
  t3.append_children(p4,newchildren);
  std::vector<I3Particle> children5 = t3.children(p4);
  ENSURE( !children5.empty() , "no children5");
  ENSURE( children5 == newchildren , "added children incorrect");
}

TEST(append_child_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  I3Particle p2 = makeParticle();
  ENSURE( t1.append_child(t1.begin(),p2) != t1.end() ,
         "failed to append child");
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  I3Particle p3 = makeParticle();
  I3MCTree::fast_const_iterator iter(t1.cbegin());
  ENSURE( t1.append_child(iter,p3) != t1.cend_fast() ,
         "failed to append child");
  
  std::vector<I3Particle> children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p2 , "p2 is not first child");
  ENSURE( children2.at(1) == p3 , "p3 is not second child");
  
  I3MCTree t2(makeParticle());
  ENSURE( t2.append_child(t2.begin(),t1,p1) != t2.end() ,
         "failed to append subtree");
  std::vector<I3Particle> children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  std::vector<I3Particle> children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  t2.clear();
  t2.insert(makeParticle());
  ENSURE( t2.append_child(t2.cbegin_fast(),t1,p1) != t2.cend_fast() ,
         "failed to append subtree");
  children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  t2.clear();
  t2.insert(makeParticle());
  ENSURE( t2.append_child(t2.begin(),t1.begin()) != t2.end() ,
         "failed to append subtree");
  children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  t2.clear();
  t2.insert(makeParticle());
  ENSURE( t2.append_child(t2.cbegin_fast(),t1.begin()) != t2.cend_fast() ,
         "failed to append subtree");
  children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  t2.clear();
  t2.insert(makeParticle());
  ENSURE( t2.append_child(t2.begin(),iter) != t2.end() ,
         "failed to append subtree");
  children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  t2.clear();
  t2.insert(makeParticle());
  ENSURE( t2.append_child(t2.cbegin_fast(),iter) != t2.cend_fast() ,
         "failed to append subtree");
  children3 = t2.children(*t2.get_head());
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p1 , "p1 is not first child of t2");
  ENSURE( children3.size() == 1 , "children3.size != 1");
  children4 = t2.children(p1);
  ENSURE( !children4.empty() , "no children4");
  ENSURE( children4.front() == p2 , "p2 is not first child of t2.p1");
  ENSURE( children4.at(1) == p3 , "p3 is not second child of t2.p1");
  
  I3MCTree t3(makeParticle());
  I3Particle p4 = *t3.get_head();
  std::vector<I3Particle> newchildren;
  for (int i=0;i<50;i++) {
    newchildren.push_back(makeParticle());
  }
  ENSURE( t3.append_children(t3.begin(),newchildren) != t3.end(),
         "failed to append multiple children");
  std::vector<I3Particle> children5 = t3.children(p4);
  ENSURE( !children5.empty() , "no children5");
  ENSURE( children5 == newchildren , "added children incorrect");
  t3.clear();
  t3.insert(makeParticle());
  p4 = *t3.get_head();
  ENSURE( t3.append_children(t3.cbegin_fast(),newchildren) != t3.cend_fast(),
         "failed to append multiple children");
  children5 = t3.children(p4);
  ENSURE( !children5.empty() , "no children5");
  ENSURE( children5 == newchildren , "added children incorrect");
  
  t3.clear();
  t3.insert(makeParticle());
  p4 = *t3.get_head();
  ENSURE( t3.append_children(t3.begin(),newchildren.begin(),newchildren.end()) != t3.end(),
         "failed to append multiple children");
  std::vector<I3Particle> children6 = t3.children(p4);
  ENSURE( !children6.empty() , "no children5");
  ENSURE( children6 == newchildren , "added children incorrect");
}

TEST(erase)
{
  I3MCTree t1(makeParticle());
  
  I3MCTreeUtils::AddPrimary(t1, makeParticle());
  // Erase *first* primary, 
  t1.erase(*t1.get_head());
  ENSURE( !t1.empty() );
  // Ensure that head_ is still valid
  for (I3MCTree::iterator it=t1.begin(); it!=t1.end(); it++) {
      ENSURE(it->GetID() != I3ParticleID());
  }
  t1.erase(*t1.get_head());
  ENSURE( t1.empty() );
  // Ensure that head_ is still valid
  for (I3MCTree::iterator it=t1.begin(); it!=t1.end(); it++) {
      ENSURE(it->GetID() != I3ParticleID());
  }
  
  t1 = I3MCTree(makeParticle());
  I3Particle head = t1.get_head();
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  I3Particle p2 = makeParticle();
  t1.append_child(head,p2);
  I3Particle p3 = makeParticle();
  t1.append_child(head,p3);
  ENSURE( !t1.empty() , "tree is empty");
  
  t1.erase(p3);
  ENSURE( !t1.at(p3) , "child was not erased");
  ENSURE( !t1.empty() , "tree is empty after erasing child");
  
  t1.erase(head);
  ENSURE( !t1.at(head) , "head was not erased");
  ENSURE( !t1.at(p1) , "p1 was not erased");
  ENSURE( !t1.at(p2) , "p2 was not erased");
  ENSURE( t1.empty() , "tree did not clear");
  
  I3MCTree t2(makeParticle());
  I3Particle head2 = t2.get_head();
  I3Particle p4 = makeParticle();
  t2.append_child(head2,p4);
  I3Particle p5 = makeParticle();
  t2.append_child(head2,p5);
  I3Particle p6 = makeParticle();
  t2.append_child(head2,p6);
  ENSURE( !t2.empty() , "tree is empty");
  
  I3MCTree::iterator iter(t2,p6);
  I3MCTree::iterator iter_ret(t2,p6);
  iter_ret++;
  ENSURE( t2.erase(iter) == iter_ret , "erase() did not return next iter");
  ENSURE( !t2.at(p6) , "child was not erased");
  ENSURE( !t2.empty() , "tree is empty after erasing child");
  
  iter_ret = t2.end();
  ENSURE( t2.erase(t2.begin()) == iter_ret , "erase() did not return next iter");
  ENSURE( !t2.at(head2) , "head was not erased");
  ENSURE( !t2.at(p4) , "p4 was not erased");
  ENSURE( !t2.at(p5) , "p5 was not erased");
  ENSURE( t2.empty() , "tree did not clear");
  
  I3MCTree t3(makeParticle());
  I3Particle head3 = t3.get_head();
  I3Particle p7 = makeParticle();
  t3.append_child(head3,p7);
  I3Particle p8 = makeParticle();
  t3.append_child(head3,p8);
  I3Particle p9 = makeParticle();
  t3.append_child(head3,p9);
  ENSURE( !t3.empty() , "tree is empty");
  
  I3MCTree::fast_const_iterator iter2(t3,p9);
  I3MCTree::fast_const_iterator iter2_ret(t3,p9);
  iter2_ret++;
  ENSURE( t3.erase(iter2) == iter2_ret , "erase() did not return next iter");
  ENSURE( !t3.at(p9) , "child was not erased");
  ENSURE( !t3.empty() , "tree is empty after erasing child");
  
  I3MCTree::fast_const_iterator iter3(t3,head3);
  iter2_ret = t3.cend_fast();
  ENSURE( t3.erase(iter3) == iter2_ret , "erase() did not return next iter");
  ENSURE( !t3.at(head3) , "head was not erased");
  ENSURE( !t3.at(p7) , "p7 was not erased");
  ENSURE( !t3.at(p8) , "p8 was not erased");
  ENSURE( t3.empty() , "tree did not clear");
  
  I3MCTree t4(makeParticle());
  I3Particle head4 = t4.get_head();
  I3Particle p10 = makeParticle();
  t4.append_child(head4,p10);
  I3Particle p11 = makeParticle();
  t4.append_child(head4,p11);
  I3Particle p12 = makeParticle();
  t4.append_child(head4,p12);
  ENSURE( !t4.empty() , "tree is empty");
  
  I3MCTree::post_order_iterator iter4(t4,p11);
  I3MCTree::post_order_iterator iter4_ret(t4,p11);
  iter4_ret++;
  ENSURE( t4.erase(iter4) == iter4_ret , "erase() did not return next iter");
  ENSURE( !t4.at(p11) , "child was not erased");
  ENSURE( !t4.empty() , "tree is empty after erasing child");
  
  iter4 = I3MCTree::post_order_iterator(t4,head4);
  iter4_ret = t4.end_post();
  ENSURE( t4.erase(iter4) == iter4_ret , "erase() did not return next iter");
  ENSURE( !t4.at(head4) , "head was not erased");
  ENSURE( !t4.at(p10) , "p4 was not erased");
  ENSURE( !t4.at(p12) , "p5 was not erased");
  ENSURE( t4.empty() , "tree did not clear");
}

TEST(erase_children)
{
  I3MCTree t1(makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  ENSURE( !t1.empty() , "tree is empty");
  
  t1.erase_children(*t1.get_head());
  ENSURE( !t1.empty() , "tree is empty after erase_children");
  
  std::vector<I3Particle> children = t1.children(*t1.get_head());
  ENSURE( children.empty() , "did not delete all children");
  
  I3MCTree t2(makeParticle());
  t2.append_child(*t2.get_head(),makeParticle());
  t2.append_child(*t2.get_head(),makeParticle());
  t2.append_child(*t2.get_head(),makeParticle());
  ENSURE( !t2.empty() , "tree is empty");
  
  ENSURE( t2.erase_children(t2.begin()) == t2.begin() ,
         "erase_children() did not return same iterator");
  ENSURE( !t2.empty() , "tree is empty after erase_children");
  
  I3MCTree::sibling_iterator children2 = t2.children(t2.begin());
  ENSURE( children2 == t2.end_sibling() , "did not delete all children");
  
  I3MCTree t3(makeParticle());
  t3.append_child(*t3.get_head(),makeParticle());
  t3.append_child(*t3.get_head(),makeParticle());
  t3.append_child(*t3.get_head(),makeParticle());
  ENSURE( !t3.empty() , "tree is empty");
  
  I3MCTree::fast_const_iterator iter(t3.begin());
  ENSURE( iter != t3.cend_fast() , "iter is empty");
  ENSURE( t3.erase_children(iter) == iter ,
         "erase_children() did not return same iterator");
  ENSURE( !t3.empty() , "tree is empty after erase_children");
  ENSURE( t3.size() == 1 , "tree is not just a head");
  
  I3MCTree::sibling_const_iterator children3 = t3.children(iter);
  ENSURE( children3 == t3.cend_sibling() , "did not delete all children");
}

TEST(manual_traversal)
{
  I3MCTree t1(makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  ENSURE( !t1.empty() , "tree is empty");
  
  ENSURE( (bool)t1.get_head() , "tree does not have a head");
  I3Particle head = t1.get_head();
  ENSURE( (bool)t1.at(head) , "at(head) failed");
  ENSURE( *t1.at(head) == head , "at(head) != head");
  
  std::vector<I3Particle> children = t1.children(head);
  ENSURE( !children.empty() , "no children of head");
  ENSURE( children.size() == 3 , "there are not three children");
  ENSURE( (bool)t1.first_child(head) , "can't find first child");
  ENSURE( *t1.first_child(head) == children.at(0) , "children.at(0) != first_child");
  ENSURE( head == *t1.parent(children.at(0)) , "children.at(0) parent != head");
  ENSURE( head == *t1.parent(children.at(1)) , "children.at(1) parent != head");
  ENSURE( head == *t1.parent(children.at(2)) , "children.at(2) parent != head");
  I3Particle child1 = children.front();
  
  ENSURE( (bool)t1.at(child1) , "at(child1) failed");
  ENSURE( (bool)t1.parent(child1) , "child1 does not have a parent");
  ENSURE( head == *t1.parent(child1) , "child1 parent != head");
  
  ENSURE( (bool)t1.next_sibling(child1) , "child1 does not have a next_sibling");
  I3Particle child2 = t1.next_sibling(child1);
  ENSURE( child2 == children.at(1) , "child2 is not the second child");
  
  ENSURE( (bool)t1.previous_sibling(child2) , "child2 does not have a previous_sibling");
  I3Particle prev_sib = t1.previous_sibling(child2);
  ENSURE( (bool)t1.at(prev_sib) , "previous_sibling not in tree");
  ENSURE( prev_sib == children.at(0) , "prev_sibling is not the first child");
  ENSURE( !( prev_sib == children.at(1) ), "prev_sibling = children.at(1)");
  ENSURE( !( prev_sib == children.at(2) ), "prev_sibling = children.at(2)");
  ENSURE( child1 == prev_sib , "siblings do not match");
  
  ENSURE( (bool)t1.parent(child2) , "child2 does not have a parent");
  ENSURE( head == *t1.parent(child2) , "child2 parent != head");
}

TEST(manual_traversal_iter)
{
  I3MCTree t1(makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  t1.append_child(*t1.get_head(),makeParticle());
  ENSURE( !t1.empty() , "tree is empty");
  
  ENSURE( (bool)t1.get_head() , "tree does not have a head");
  I3Particle head = t1.get_head();
  ENSURE( (bool)t1.at(head) , "at(head) failed");
  ENSURE( *t1.at(head) == head , "at(head) != head");
  
  I3MCTree::sibling_iterator children = t1.begin_sibling(++t1.begin());
  ENSURE( children != t1.end_sibling() , "no children of head");
  ENSURE( *t1.first_child(t1.begin()) == *children ,
         "children(0) != first_child");
  ENSURE( head == *t1.parent(children) , "children(0) parent != head");
  I3MCTree::iterator child1 = (I3MCTree::sibling_iterator)children++;
  I3MCTree::iterator child2 = t1.next_sibling(child1);
  ENSURE( t1.next_sibling(child1) == (I3MCTree::iterator)children ,
         "child1 does not have a next_sibling");
  ENSURE( head == *t1.parent(children++) , "children(1) parent != head");
  ENSURE( head == *t1.parent(children++) , "children(2) parent != head");
  ENSURE( children == t1.end_sibling() , "children.size != 3");
  
  I3MCTree::iterator prev_sib = t1.previous_sibling(child2);
  ENSURE( prev_sib == child1 , "child2 does not have a previous_sibling");
  ENSURE( child1 == prev_sib , "siblings do not match");
  
  ENSURE( t1.parent(child2) == t1.begin() , "child2 parent != head");
}

TEST(first_child)
{
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,p2);
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  ENSURE( !t1.empty() , "tree is empty");
  ENSURE( t1.first_child(p1) == p2 , "first_child incorrect");
  ENSURE( t1.first_child(t1.begin()) == ++t1.begin() ,
         "iter first_child incorrect");
  I3MCTree::sibling_const_iterator iter(t1,p1), iter2(t1,p2);
  ENSURE( t1.first_child(iter) == iter2 ,
         "sibling_const_iter first_child incorrect");
  I3MCTree::fast_iterator iter3(t1,p1), iter4(t1,p2);
  ENSURE( t1.first_child(iter3) == iter4 ,
         "fast_iter first_child incorrect");
  ENSURE( iter4 == t1.first_child(iter3) ,
         "fast_iter2 first_child incorrect");
}

TEST(insert)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  I3Particle p3 = makeParticle();
  t1.insert(p2,p3);
  
  std::vector<I3Particle> children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == p2 , "p2 is not second child");
  
  I3Particle p4 = makeParticle();
  t1.insert(p2,p4);
  
  std::vector<I3Particle> children3 = t1.children(p1);
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p3 , "p3 is not first child");
  ENSURE( children3.at(1) == p4 , "p4 is not second child");
  ENSURE( children3.at(2) == p2 , "p2 is not third child");
  
  p1 = makeParticle();
  t1 = I3MCTree(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  p2 = makeParticle();
  t1.append_child(p1,p2);
  
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  p3 = makeParticle();
  I3MCTree::iterator iter(t1,p2);
  ENSURE( t1.insert(iter,p3) != t1.end() ,
         "insert() returned end()");
  
  children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == p2 , "p2 is not second child");
  
  p4 = makeParticle();
  I3MCTree::fast_const_iterator iter2(t1,p2);
  ENSURE( t1.insert(iter2,p4) != t1.cend_fast() ,
         "insert() returned end()");
  
  children3 = t1.children(p1);
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p3 , "p3 is not first child");
  ENSURE( children3.at(1) == p4 , "p4 is not second child");
  ENSURE( children3.at(2) == p2 , "p2 is not third child");
}

TEST(insert_after)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  I3Particle p3 = makeParticle();
  t1.insert_after(p2,p3);
  
  std::vector<I3Particle> children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p2 , "p2 is not first child");
  ENSURE( children2.at(1) == p3 , "p3 is not second child");
  
  I3Particle p4 = makeParticle();
  t1.insert_after(p2,p4);
  
  std::vector<I3Particle> children3 = t1.children(p1);
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p2 , "p2 is not first child");
  ENSURE( children3.at(1) == p4 , "p4 is not second child");
  ENSURE( children3.at(2) == p3 , "p3 is not third child");
  
  p1 = makeParticle();
  t1 = I3MCTree(p1);
  
  ENSURE( (bool)t1.get_head() , "can't get head node");
  
  p2 = makeParticle();
  t1.append_child(p1,p2);
  
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p2 , "child not there");
  
  p3 = makeParticle();
  I3MCTree::iterator iter(t1,p2);
  ENSURE( t1.insert_after(iter,p3) != t1.end() ,
         "insert() returned end()");
  
  children2 = t1.children(p1);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p2 , "p2 is not first child");
  ENSURE( children2.at(1) == p3 , "p3 is not second child");
  
  p4 = makeParticle();
  I3MCTree::fast_const_iterator iter2(t1,p2);
  ENSURE( t1.insert_after(iter2,p4) != t1.cend_fast() ,
         "insert_after() returned end()");
  
  children3 = t1.children(p1);
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.front() == p2 , "p2 is not first child");
  ENSURE( children3.at(1) == p4 , "p4 is not second child");
  ENSURE( children3.at(2) == p3 , "p3 is not third child");
}

TEST(insert_subtree)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  I3Particle firstChild = makeParticle();
  t2.append_child(head,firstChild);
  
  t2.insert_subtree(firstChild,t1,p3);
  
  std::vector<I3Particle> children = t2.children(head);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p3 , "p3 not there");
  ENSURE( children.back() == firstChild , "firstChild not there");
  
  I3Particle head2 = makeParticle();
  I3MCTree t3(head2);
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  
  t2.insert_subtree(firstChild,t3,head2);
  
  std::vector<I3Particle> children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == firstChild , "firstChild is not third child");
  
  std::vector<I3Particle> children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
}

TEST(insert_subtree_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  I3Particle firstChild = makeParticle();
  t2.append_child(head,firstChild);
  
  I3MCTree::iterator iter(t2,firstChild);
  t2.insert_subtree(iter,t1,p3);
  
  std::vector<I3Particle> children = t2.children(head);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == p3 , "p3 not there");
  ENSURE( children.back() == firstChild , "firstChild not there");
  
  I3Particle head2 = makeParticle();
  I3MCTree t3(head2);
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  
  I3MCTree::fast_const_iterator iter2(t2,firstChild);
  t2.insert_subtree(iter2,t3,head2);
  
  std::vector<I3Particle> children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == firstChild , "firstChild is not third child");
  
  std::vector<I3Particle> children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head3 = makeParticle();
  I3MCTree t4(head3);
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  
  t2.insert_subtree(iter2,t4.begin());
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == head3 , "head3 is not third child");
  ENSURE( children2.at(3) == firstChild , "firstChild is not fourth child");
  
  children3 = t2.children(children2.at(2));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head4 = makeParticle();
  I3MCTree t5(head4);
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  
  t2.insert_subtree(iter,t5.cbegin());
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == head3 , "head3 is not third child");
  ENSURE( children2.at(3) == head4 , "head4 is not fourth child");
  ENSURE( children2.at(4) == firstChild , "firstChild is not fifth child");
  
  children3 = t2.children(children2.at(3));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head5 = makeParticle();
  I3MCTree t6(head5);
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  
  I3MCTree::fast_iterator iter3(t6.begin());
  t2.insert_subtree(iter,iter3);
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == head3 , "head3 is not third child");
  ENSURE( children2.at(3) == head4 , "head4 is not fourth child");
  ENSURE( children2.at(4) == head5 , "head5 is not fifth child");
  ENSURE( children2.at(5) == firstChild , "firstChild is not sixth child");
  
  children3 = t2.children(children2.at(4));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head6 = makeParticle();
  I3MCTree t7(head6);
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  
  I3MCTree::fast_iterator iter4(t7.begin());
  t2.insert_subtree(iter2,iter4);
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == p3 , "p3 is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == head3 , "head3 is not third child");
  ENSURE( children2.at(3) == head4 , "head4 is not fourth child");
  ENSURE( children2.at(4) == head5 , "head5 is not fifth child");
  ENSURE( children2.at(5) == head6 , "head6 is not sixth child");
  ENSURE( children2.at(6) == firstChild , "firstChild is not seventh child");
  
  children3 = t2.children(children2.at(5));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
}

TEST(insert_subtree_after)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  I3Particle firstChild = makeParticle();
  t2.append_child(head,firstChild);
  
  t2.insert_subtree_after(firstChild,t1,p3);
  
  std::vector<I3Particle> children = t2.children(head);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.back() == p3 , "firstChild not there");
  ENSURE( children.front() == firstChild , "p3 not there");
  
  I3Particle head2 = makeParticle();
  I3MCTree t3(head2);
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  
  t2.insert_subtree_after(firstChild,t3,head2);
  
  std::vector<I3Particle> children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == p3 , "p3 is not third child");
  
  std::vector<I3Particle> children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
}

TEST(insert_subtree_after_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  I3Particle firstChild = makeParticle();
  t2.append_child(head,firstChild);
  
  I3MCTree::iterator iter(t2,firstChild);
  t2.insert_subtree_after(iter,t1,p3);
  
  std::vector<I3Particle> children = t2.children(head);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.front() == firstChild , "firstChild not there");
  ENSURE( children.back() == p3 , "p3 not there");
  
  I3Particle head2 = makeParticle();
  I3MCTree t3(head2);
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  
  I3MCTree::fast_const_iterator iter2(t2,firstChild);
  t2.insert_subtree_after(iter2,t3,head2);
  
  std::vector<I3Particle> children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head2 , "head2 is not second child");
  ENSURE( children2.at(2) == p3 , "p3 is not third child");
  
  std::vector<I3Particle> children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head3 = makeParticle();
  I3MCTree t4(head3);
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  
  t2.insert_subtree_after(iter2,t4.begin());
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head3 , "head3 is not second child");
  ENSURE( children2.at(2) == head2 , "head2 is not third child");
  ENSURE( children2.at(3) == p3 , "p3 is not fourth child");
  
  children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head4 = makeParticle();
  I3MCTree t5(head4);
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  
  t2.insert_subtree_after(iter,t5.cbegin());
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head4 , "head4 is not second child");
  ENSURE( children2.at(2) == head3 , "head3 is not third child");
  ENSURE( children2.at(3) == head2 , "head2 is not fourth child");
  ENSURE( children2.at(4) == p3 , "p3 is not fifth child");
  
  children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head5 = makeParticle();
  I3MCTree t6(head5);
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  
  I3MCTree::fast_iterator iter3(t6.begin());
  t2.insert_subtree_after(iter,iter3);
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head5 , "head5 is not second child");
  ENSURE( children2.at(2) == head4 , "head4 is not third child");
  ENSURE( children2.at(3) == head3 , "head3 is not fourth child");
  ENSURE( children2.at(4) == head2 , "head2 is not fifth child");
  ENSURE( children2.at(5) == p3 , "p3 is not sixth child");
  
  children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
  
  I3Particle head6 = makeParticle();
  I3MCTree t7(head6);
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  
  I3MCTree::fast_iterator iter4(t7.begin());
  t2.insert_subtree_after(iter2,iter4);
  
  children2 = t2.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.front() == firstChild , "firstChild is not first child");
  ENSURE( children2.at(1) == head6 , "head6 is not second child");
  ENSURE( children2.at(2) == head5 , "head5 is not third child");
  ENSURE( children2.at(3) == head4 , "head4 is not fourth child");
  ENSURE( children2.at(4) == head3 , "head3 is not fifth child");
  ENSURE( children2.at(5) == head2 , "head2 is not sixth child");
  ENSURE( children2.at(6) == p3 , "p3 is not seventh child");
  
  children3 = t2.children(children2.at(1));
  ENSURE( !children3.empty() , "no children3");
  ENSURE( children3.size() == 3 , "children3.size != 3");
}

TEST(replace)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());

  I3Particle p2 = makeParticle();
  t1.replace(p3,p2);
  
  ENSURE( !t1.at(p3) , "p3 is still in tree");
  ENSURE( (bool)t1.at(p2) , "p2 is not in tree");
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == p2 , "p2 not second child");
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  
  t1.replace(p2,t2,head);
  
  ENSURE( !t1.at(p2) , "p2 is still in tree");
  ENSURE( (bool)t1.at(head) , "head is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head , "head not second child");
  std::vector<I3Particle> children2 = t1.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t2 not in t1");
}

TEST(replace_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p1,p3);
  t1.append_child(p1,makeParticle());

  I3Particle p2 = makeParticle();
  I3MCTree::iterator iter(t1,p3);
  t1.replace(iter,p2);
  
  ENSURE( !t1.at(p3) , "p3 is still in tree");
  ENSURE( (bool)t1.at(p2) , "p2 is not in tree");
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == p2 , "p2 not second child");

  I3Particle p4 = makeParticle();
  I3MCTree::fast_const_iterator iter2(t1,p2);
  t1.replace(iter2,p4);
  
  ENSURE( !t1.at(p2) , "p2 is still in tree");
  ENSURE( (bool)t1.at(p4) , "p4 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == p4 , "p4 not second child");
  
  I3Particle head = makeParticle();
  I3MCTree t2(head);
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  
  I3MCTree::iterator iter3(t1,p4);
  t1.replace(iter3,t2,head);
  
  ENSURE( !t1.at(p4) , "p4 is still in tree");
  ENSURE( (bool)t1.at(head) , "head is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head , "head not second child");
  std::vector<I3Particle> children2 = t1.children(head);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t2 not in t1");
  
  I3Particle head2 = makeParticle();
  I3MCTree t3(head2);
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  t3.append_child(head2,makeParticle());
  
  I3MCTree::fast_const_iterator iter4(t1,head);
  t1.replace(iter4,t3,head2);
  
  ENSURE( !t1.at(head) , "head is still in tree");
  ENSURE( (bool)t1.at(head2) , "head2 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head2 , "head2 not second child");
  children2 = t1.children(head2);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t3 not in t1");
  
  I3Particle head3 = makeParticle();
  I3MCTree t4(head3);
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  t4.append_child(head3,makeParticle());
  
  I3MCTree::iterator iter5(t1,head2);
  t1.replace(iter5,t4.begin());
  
  ENSURE( !t1.at(head2) , "head2 is still in tree");
  ENSURE( (bool)t1.at(head3) , "head3 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head3 , "head3 not second child");
  children2 = t1.children(head3);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t4 not in t1");
  
  I3Particle head4 = makeParticle();
  I3MCTree t5(head4);
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  t5.append_child(head4,makeParticle());
  
  I3MCTree::iterator iter6(t1,head3);
  I3MCTree::fast_const_iterator iter7(t5,head4);
  t1.replace(iter6,iter7);
  
  ENSURE( !t1.at(head3) , "head3 is still in tree");
  ENSURE( (bool)t1.at(head4) , "head4 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head4 , "head4 not second child");
  children2 = t1.children(head4);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t5 not in t1");
  
  I3Particle head5 = makeParticle();
  I3MCTree t6(head5);
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  t6.append_child(head5,makeParticle());
  
  I3MCTree::fast_const_iterator iter8(t1,head4);
  t1.replace(iter8,t6.begin());
  
  ENSURE( !t1.at(head4) , "head4 is still in tree");
  ENSURE( (bool)t1.at(head5) , "head5 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head5 , "head5 not second child");
  children2 = t1.children(head5);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t6 not in t1");
  
  I3Particle head6 = makeParticle();
  I3MCTree t7(head6);
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  t7.append_child(head6,makeParticle());
  
  I3MCTree::fast_const_iterator iter9(t1,head5);
  I3MCTree::fast_const_iterator iter10(t7,head6);
  t1.replace(iter9,iter10);
  
  ENSURE( !t1.at(head5) , "head5 is still in tree");
  ENSURE( (bool)t1.at(head6) , "head6 is not in tree");
  children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(1) == head6 , "head6 not second child");
  children2 = t1.children(head6);
  ENSURE( !children2.empty() , "no children2");
  ENSURE( children2.size() == 3 , "3 children of t7 not in t1");
}

TEST(flatten)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  I3Particle p4 = makeParticle();
  t1.append_child(p2,p4);
  I3Particle p5 = makeParticle();
  t1.append_child(p3,p5);
  
  t1.flatten(p2);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.size() == 3 , "children.size != 3");
  ENSURE( children.at(0) == p2 , "p2 not first child");
  ENSURE( children.at(1) == p3 , "p3 not second child");
  ENSURE( children.at(2) == p4 , "p4 not third child");
  
  children = t1.children(p3);
  ENSURE( !children.empty() , "no children2");
  ENSURE( children.size() == 1 , "children2.size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children2");
}

TEST(flatten_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  I3Particle p4 = makeParticle();
  t1.append_child(p2,p4);
  I3Particle p5 = makeParticle();
  t1.append_child(p3,p5);
  
  I3MCTree::iterator iter(t1,p2);
  t1.flatten(iter);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.size() == 3 , "children.size != 3");
  ENSURE( children.at(0) == p2 , "p2 not first child");
  ENSURE( children.at(1) == p3 , "p3 not second child");
  ENSURE( children.at(2) == p4 , "p4 not third child");
  
  children = t1.children(p3);
  ENSURE( !children.empty() , "no children2");
  ENSURE( children.size() == 1 , "children2.size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children2");
  
  I3MCTree t2(p1);
  t2.append_child(p1,p2);
  t2.append_child(p2,p3);
  t2.append_child(p2,p4);
  t2.append_child(p3,p5);
  
  I3MCTree::fast_const_iterator iter2(t2,p2);
  t2.flatten(iter);
  
  children = t2.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.size() == 3 , "children.size != 3");
  ENSURE( children.at(0) == p2 , "p2 not first child");
  ENSURE( children.at(1) == p3 , "p3 not second child");
  ENSURE( children.at(2) == p4 , "p4 not third child");
  
  children = t2.children(p3);
  ENSURE( !children.empty() , "no children2");
  ENSURE( children.size() == 1 , "children2.size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children2");
}

TEST(reparent)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  I3Particle p4 = makeParticle();
  t1.append_child(p2,p4);
  I3Particle p5 = makeParticle();
  t1.append_child(p3,p5);
  
  t1.reparent(p4,p3);
  
  std::vector<I3Particle> children = t1.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t1.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
}

TEST(reparent_iter)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  I3Particle p4 = makeParticle();
  t1.append_child(p2,p4);
  I3Particle p5 = makeParticle();
  t1.append_child(p3,p5);
  
  I3MCTree::iterator iter(t1,p4);
  t1.reparent(iter,p3);
  
  std::vector<I3Particle> children = t1.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t1.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
  
  I3MCTree t2(p1);
  t2.append_child(p1,p2);
  t2.append_child(p2,p3);
  t2.append_child(p2,p4);
  t2.append_child(p3,p5);
  
  I3MCTree::fast_const_iterator iter2(t2,p4);
  t2.reparent(iter2,p3);
  
  children = t2.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t2.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
  
  I3MCTree t3(p1);
  t3.append_child(p1,p2);
  t3.append_child(p2,p3);
  t3.append_child(p2,p4);
  t3.append_child(p3,p5);
  
  I3MCTree::iterator iter3(t3,p4);
  I3MCTree::iterator iter4(t3,p3);
  t3.reparent(iter3,iter4);
  
  children = t3.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t3.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
  
  I3MCTree t4(p1);
  t4.append_child(p1,p2);
  t4.append_child(p2,p3);
  t4.append_child(p2,p4);
  t4.append_child(p3,p5);
  
  I3MCTree::fast_const_iterator iter5(t4,p4);
  I3MCTree::iterator iter6(t4,p3);
  t4.reparent(iter5,iter6);
  
  children = t4.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t4.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
  
  I3MCTree t5(p1);
  t5.append_child(p1,p2);
  t5.append_child(p2,p3);
  t5.append_child(p2,p4);
  t5.append_child(p3,p5);
  
  I3MCTree::iterator iter7(t5,p4);
  I3MCTree::fast_const_iterator iter8(t5,p3);
  t5.reparent(iter7,iter8);
  
  children = t5.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t5.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
  
  I3MCTree t6(p1);
  t6.append_child(p1,p2);
  t6.append_child(p2,p3);
  t6.append_child(p2,p4);
  t6.append_child(p3,p5);
  
  I3MCTree::fast_const_iterator iter9(t6,p4);
  I3MCTree::fast_const_iterator iter10(t6,p3);
  t6.reparent(iter9,iter10);
  
  children = t6.children(p3);
  ENSURE( children.empty() , "p3 still has children");
  
  children = t6.children(p4);
  ENSURE( !children.empty() , "no children of p4");
  ENSURE( children.size() == 1 , "children(p4).size != 1");
  ENSURE( children.at(0) == p5 , "p5 not first child of children(p4)");
}

TEST(merge)
{
  I3Particle p1 = makeParticle();
  I3MCTree t1(p1);
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3MCTree t2(p2);
  t2.append_child(p2,p3);
  t2.append_child(p2,p4);
  t2.append_child(p2,p5);
  
  ENSURE( !t1.at(p2) , "p2 is already in t1");
  t1.merge(t2);
  ENSURE( (bool)t1.at(p2) , "p2 is not in t1");
  ENSURE( (bool)t1.at(p3) , "p3 is not in t1");
  ENSURE( (bool)t1.at(p4) , "p4 is not in t1");
  ENSURE( (bool)t1.at(p5) , "p5 is not in t1");
  ENSURE( t1.number_of_children(p2) == 3 );
}


TEST(size)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  ENSURE( t1.size() == 1 );
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  ENSURE( t1.size() == 4 );
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  t1.append_child(head,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  ENSURE( t1.size() == 8 );
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  t1.append_child(p2,makeParticle());
  t1.append_child(p2,makeParticle());
  ENSURE( t1.size() == 11 );
}

TEST(depth)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  ENSURE( t1.depth(head) == 0 );
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  ENSURE( t1.depth(p1) == 1 );
  t1.append_child(head,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  ENSURE( t1.depth(p2) == 2 );
  t1.append_child(p2,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  t1.append_child(p3,makeParticle());
  ENSURE( t1.depth(p3) == 3 );
  
  I3MCTree t2(head);
  ENSURE( t2.depth(t2.begin()) == 0 );
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  t2.append_child(head,p1);
  I3MCTree::fast_const_iterator iter(t2,p1);
  ENSURE( t2.depth(iter) == 1 );
  t2.append_child(head,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,p2);
  I3MCTree::leaf_iterator iter2(t2,p2);
  ENSURE( t2.depth(iter2) == 2 );
  t2.append_child(p2,makeParticle());
  t2.append_child(p2,p3);
  t2.append_child(p3,makeParticle());
  I3MCTree::sibling_iterator iter3(t2,p3);
  ENSURE( t2.depth(iter3) == 3 );
}

TEST(number_of_children)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  ENSURE( t1.number_of_children(head) == 0 );
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  ENSURE( t1.number_of_children(head) == 2 );
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  t1.append_child(head,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  ENSURE( t1.number_of_children(p1) == 3 );
  t1.append_child(p2,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  t1.append_child(p3,makeParticle());
  ENSURE( t1.number_of_children(p2) == 2 );
  ENSURE( t1.number_of_children(p3) == 1 );
  
  I3MCTree t2(head);
  ENSURE( t2.number_of_children(t2.begin()) == 0 );
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  ENSURE( t2.number_of_children(head) == 2 );
  t2.append_child(head,p1);
  t2.append_child(head,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,p2);
  I3MCTree::post_order_const_iterator iter(t2,p1);
  ENSURE( t2.number_of_children(iter) == 3 );
  t2.append_child(p2,makeParticle());
  t2.append_child(p2,p3);
  t2.append_child(p3,makeParticle());
  I3MCTree::fast_const_iterator iter2(t2,p2);
  ENSURE( t2.number_of_children(iter2) == 2 );
  I3MCTree::sibling_iterator iter3(t2,p3);
  ENSURE( t2.number_of_children(iter3) == 1 );
}

TEST(number_of_siblings)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  ENSURE( t1.number_of_siblings(head) == 0 );
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  ENSURE( t1.number_of_siblings(p1) == 2 );
  t1.append_child(head,makeParticle());
  ENSURE( t1.number_of_siblings(p1) == 3 );
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  ENSURE( t1.number_of_siblings(p2) == 2 );
  t1.append_child(p2,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  t1.append_child(p3,makeParticle());
  ENSURE( t1.number_of_siblings(p3) == 1 );
  
  I3MCTree t2(head);
  ENSURE( t2.number_of_siblings(t2.begin()) == 0 );
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  t2.append_child(head,p1);
  I3MCTree::post_order_const_iterator iter(t2,p1);
  ENSURE( t2.number_of_siblings(iter) == 2 );
  t2.append_child(head,makeParticle());
  ENSURE( t2.number_of_siblings(iter) == 3 );
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,p2);
  I3MCTree::fast_const_iterator iter2(t2,p2);
  ENSURE( t2.number_of_siblings(iter2) == 2 );
  t2.append_child(p2,makeParticle());
  t2.append_child(p2,p3);
  t2.append_child(p3,makeParticle());
  I3MCTree::sibling_iterator iter3(t2,p3);
  ENSURE( t2.number_of_siblings(iter3) == 1 );
}

TEST(is_in_subtree)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  I3Particle p1 = makeParticle();
  t1.append_child(head,p1);
  ENSURE( t1.is_in_subtree(head,p1) ,"cannot find p1");
  t1.append_child(head,makeParticle());
  t1.append_child(p1,makeParticle());
  t1.append_child(p1,makeParticle());
  I3Particle p2 = makeParticle();
  t1.append_child(p1,p2);
  ENSURE( t1.is_in_subtree(head,p2) ,"cannot find p2 under head");
  ENSURE( t1.is_in_subtree(p1,p2) ,"cannot find p2 under p1");
  t1.append_child(p2,makeParticle());
  I3Particle p3 = makeParticle();
  t1.append_child(p2,p3);
  ENSURE( t1.is_in_subtree(head,p3) ,"cannot find p3 under head");
  ENSURE( t1.is_in_subtree(p1,p3) ,"cannot find p3 under p1");
  ENSURE( t1.is_in_subtree(p2,p3) ,"cannot find p3 under p2");
  I3Particle p4 = makeParticle();
  t1.append_child(p1,p4);
  ENSURE( t1.is_in_subtree(head,p4) ,"cannot find p4 under head");
  ENSURE( t1.is_in_subtree(p1,p4) ,"cannot find p4 under p1");
  ENSURE( !t1.is_in_subtree(p2,p4) ,"found p4 under p2, when not there");
  
  I3MCTree t2(head);
  t2.append_child(head,makeParticle());
  t2.append_child(head,makeParticle());
  t2.append_child(head,p1);
  I3MCTree::iterator iter(t2,p1);
  ENSURE( t2.is_in_subtree(t2.begin(),p1) ,"cannot find p1");
  ENSURE( t2.is_in_subtree(t2.begin(),iter) ,"cannot find p1");
  t2.append_child(head,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,makeParticle());
  t2.append_child(p1,p2);
  I3MCTree::fast_const_iterator iter2(t2,p2);
  ENSURE( t2.is_in_subtree(t2.begin(),iter2) ,"cannot find p2 under head");
  ENSURE( t2.is_in_subtree(iter,iter2) ,"cannot find p2 under p1");
  t2.append_child(p2,makeParticle());
  t2.append_child(p2,p3);
  I3MCTree::sibling_iterator iter3(t2,p3);
  ENSURE( t2.is_in_subtree(t2.begin(),iter3) ,"cannot find p3 under head");
  ENSURE( t2.is_in_subtree(iter,iter3) ,"cannot find p3 under p1");
  ENSURE( t2.is_in_subtree(iter2,iter3) ,"cannot find p3 under p2");
  t2.append_child(p1,p4);
  I3MCTree::post_order_const_iterator iter4(t2,p4);
  ENSURE( t2.is_in_subtree(t2.cbegin(),iter4) ,"cannot find p4 under head");
  ENSURE( t2.is_in_subtree(iter,iter4) ,"cannot find p4 under p1");
  ENSURE( !t2.is_in_subtree(iter2,iter4) ,"found p4 under p2, when not there");
}

TEST(subtree_in_tree)
{
  I3Particle head = makeParticle();
  I3MCTree t1(head);
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  t1.append_child(head,makeParticle());
  
  I3Particle head2 = makeParticle();
  I3MCTree t2(head2);
  t2.append_child(head2,makeParticle());
  I3Particle p1 = makeParticle();
  t2.append_child(head2,p1);
  t2.append_child(head2,makeParticle());
  
  ENSURE( !t1.subtree_in_tree(t2,head2) , "a particle from t2 is in t1");
  
  t1.append_child(head,p1);
  ENSURE( t1.subtree_in_tree(t2,head2) , "a particle from t2 is not in t1");
  ENSURE( (bool)t1.at(p1) , "p1 is not in t1");
  
  I3MCTree t3(head);
  t3.append_child(head,makeParticle());
  t3.append_child(head,makeParticle());
  t3.append_child(head,makeParticle());
  
  I3MCTree t4(head2);
  t4.append_child(head2,makeParticle());
  t4.append_child(head2,p1);
  t4.append_child(head2,makeParticle());
  
  ENSURE( !t3.subtree_in_tree(t4.begin()) , "a particle from t4 is in t3");
  
  t3.append_child(head,p1);
  I3MCTree::fast_const_iterator iter(t4,head2);
  ENSURE( t3.subtree_in_tree(iter) , "a particle from t4 is not in t3");
  ENSURE( (bool)t3.at(p1) , "p1 is not in t3");
}


TEST(xml_serialization)
{
  I3MCTree t1;
  
  ostringstream os;
  {
    icecube::archive::xml_oarchive oa(os);
    oa << icecube::serialization::make_nvp("mytree", t1);
  }
  
  istringstream is;
  I3MCTree t2;
  is.str(os.str());
  {
    icecube::archive::xml_iarchive ia(is);
    ia >> icecube::serialization::make_nvp("mytree", t2);
  }
  
  I3MCTree::iterator iter(t1);
  I3MCTree::iterator iter2(t2);
  while(iter != t1.end() && iter2 != t2.end()) {
    ENSURE(*iter == *iter2);
    iter++;
    iter2++;
  }
  ENSURE(t1 == t2);
  
  t2.insert_after(makeParticle());
  t2.insert_after(makeParticle());
  t2.insert_after(makeParticle());
  ostringstream os2;
  {
    icecube::archive::xml_oarchive oa2(os2);
    oa2 << icecube::serialization::make_nvp("mytree", t2);
  }
  
  istringstream is2;
  I3MCTree t3;
  is2.str(os2.str());
  {
    icecube::archive::xml_iarchive ia2(is2);
    ia2 >> icecube::serialization::make_nvp("mytree", t3);
  }
  iter = t2.begin();
  iter2 = t3.begin();
  while(iter != t2.end() && iter2 != t3.end()) {
    ENSURE(*iter == *iter2);
    iter++;
    iter2++;
  }
  ENSURE(t2 == t3);
  
  ostringstream os3;
  {
    icecube::archive::xml_oarchive oa3(os3);
    oa3 << icecube::serialization::make_nvp("mytree", t3);
  }
  
  istringstream is3;
  I3MCTree t4;
  is3.str(os3.str());
  {
    icecube::archive::xml_iarchive ia3(is3);
    ia3 >> icecube::serialization::make_nvp("mytree", t4);
  }
  iter = t2.begin();
  iter2 = t4.begin();
  while(iter != t2.end() && iter2 != t4.end()) {
    ENSURE(*iter == *iter2);
    iter++;
    iter2++;
  }
  ENSURE(t2 == t4);
}


TEST(xml_serialization_massive)
{
  I3MCTree t1;
  I3Particle p1;
  int nchildren = 0;
  for(int i=0;i<10;i++) {
    vector<I3Particle> children;
    p1 = makeParticle();
    t1.insert_after(p1);
    nchildren = 2000+1000*(i%4);
    for(int j=0;j<nchildren;j++) {
      children.push_back(makeParticle());
    }
    t1.append_children(p1,children);
  }
  
  ostringstream os;
  {
    icecube::archive::xml_oarchive oa(os);
    oa << icecube::serialization::make_nvp("mytree", t1);
  }
  
  istringstream is;
  I3MCTree t2;
  is.str(os.str());
  {
    icecube::archive::xml_iarchive ia(is);
    ia >> icecube::serialization::make_nvp("mytree", t2);
  }
  
  I3MCTree::iterator iter(t1);
  I3MCTree::iterator iter2(t2);
  while(iter != t1.end() && iter2 != t2.end()) {
    ENSURE(*iter == *iter2);
    iter++;
    iter2++;
  }
  ENSURE(t1 == t2);
  
  ostringstream os2;
  {
    icecube::archive::xml_oarchive oa2(os2);
    oa2 << icecube::serialization::make_nvp("mytree", t2);
  }
  
  istringstream is2;
  I3MCTree t3;
  is2.str(os2.str());
  {
    icecube::archive::xml_iarchive ia2(is2);
    ia2 >> icecube::serialization::make_nvp("mytree", t3);
  }
  iter = t1.begin();
  iter2 = t3.begin();
  while(iter != t1.end() && iter2 != t3.end()) {
    ENSURE(*iter == *iter2);
    iter++;
    iter2++;
  }
  ENSURE(t1 == t3);
}

// Now run the I3MCTreeUtils tests

TEST(utils_AddPrimary)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  
  ENSURE( (bool)t1.get_head() , "primary is missing");
  ENSURE( p1.GetShape() != I3Particle::Primary , "AddPrimary had side effect");
  ENSURE( t1.get_head()->GetShape() == I3Particle::Primary , "particle is not primary");
  ENSURE( I3ParticleID(*t1.get_head()) == I3ParticleID(p1) , "primary is incorrect");
}

TEST(utils_AppendChild)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  
  std::vector<I3Particle> children = t1.children(p1);
  ENSURE( !children.empty() , "no children");
  ENSURE( children.at(0) == p2 , "child not found");
}

TEST(utils_GetPrimaries)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  I3MCTreeUtils::AddPrimary(t1,p2);
  
  std::vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(t1);
  ENSURE( primaries.size() == 2 , "not 2 primaries");
  ENSURE( I3ParticleID(primaries.at(0)) == I3ParticleID(p1) , "p1 not found");
  ENSURE( I3ParticleID(primaries.at(1)) == I3ParticleID(p2) , "p2 not found");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p3);
  I3MCTreeUtils::AddPrimary(*t2,p4);
  I3MCTreeUtils::AddPrimary(*t2,p5);
  
  std::vector<I3Particle*> primaries2 = I3MCTreeUtils::GetPrimariesPtr(t2);
  ENSURE( primaries2.size() == 3 , "I3MCTreePtr: not 3 primaries");
  I3MCTree::iterator iter(*t2,p3);
  ENSURE( primaries2.at(0) == &(*iter) , "I3MCTreePtr: p3 not found");
  iter = I3MCTree::iterator(*t2,p4);
  ENSURE( primaries2.at(1) == &(*iter) , "I3MCTreePtr: p4 not found");
  iter = I3MCTree::iterator(*t2,p5);
  ENSURE( primaries2.at(2) == &(*iter) , "I3MCTreePtr: p5 not found");
  
  I3MCTreeConstPtr t3 = t2;
  std::vector<const I3Particle*> primaries3 = I3MCTreeUtils::GetPrimariesPtr(t3);
  ENSURE( primaries3.size() == 3 , "I3MCTreeConstPtr: not 3 primaries");
  I3MCTree::const_iterator iter2(*t3,p3);
  ENSURE( primaries3.at(0) == &(*iter2) , "I3MCTreeConstPtr: p3 not found");
  iter2 = I3MCTree::const_iterator(*t3,p4);
  ENSURE( primaries3.at(1) == &(*iter2) , "I3MCTreeConstPtr: p4 not found");
  iter2 = I3MCTree::const_iterator(*t3,p5);
  ENSURE( primaries3.at(2) == &(*iter2) , "I3MCTreeConstPtr: p5 not found");
}

TEST(utils_GetDaughters)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  I3MCTreeUtils::AppendChild(t1,p1,p3);
  
  std::vector<I3Particle> children = I3MCTreeUtils::GetDaughters(t1,p1);
  ENSURE( children.size() == 2 , "not 2 children");
  ENSURE( children.at(0) == p2 , "child p2 not found");
  ENSURE( children.at(1) == p3 , "child p3 not found");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3Particle p6 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p4);
  
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p4,p5);
  I3MCTreeUtils::AppendChild(*t2,p4,p6);
  
  std::vector<I3Particle*> children2 = I3MCTreeUtils::GetDaughtersPtr(t2,p4);
  ENSURE( children2.size() == 2 , "I3MCTreePtr: not 2 children");
  I3MCTree::iterator iter(*t2,p5);
  ENSURE( children2.at(0) == &(*iter) , "I3MCTreePtr: child p5 not found");
  iter = I3MCTree::iterator(*t2,p6);
  ENSURE( children2.at(1) == &(*iter) , "I3MCTreePtr: child p6 not found");
  
  I3MCTreeConstPtr t3(t2);
  std::vector<const I3Particle*> children3 = I3MCTreeUtils::GetDaughtersPtr(t3,p4);
  ENSURE( children3.size() == 2 , "I3MCTreeConstPtr: not 2 children");
  I3MCTree::const_iterator iter2(*t3,p5);
  ENSURE( children3.at(0) == &(*iter2) , "I3MCTreeConstPtr: child p5 not found");
  iter2 = I3MCTree::const_iterator(*t3,p6);
  ENSURE( children3.at(1) == &(*iter2) , "I3MCTreeConstPtr: child p6 not found");
  
}

TEST(utils_GetParent)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  
  const I3Particle& ret = I3MCTreeUtils::GetParent(t1,p2);
  ENSURE( I3ParticleID(ret) == I3ParticleID(p1) , "parent(p2) != p1");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p3);
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p3,p4);
  
  I3Particle* ret2 = I3MCTreeUtils::GetParentPtr(t2,p4);
  I3MCTree::iterator iter(*t2,p3);
  ENSURE( ret2 == &(*iter) , "I3MCTreePtr: parent(p4) != p3");
  
  I3MCTreeConstPtr t3(t2);
  const I3Particle* ret3 = I3MCTreeUtils::GetParentPtr(t3,p4);
  I3MCTree::const_iterator iter2(*t3,p3);
  ENSURE( ret3 == &(*iter2) , "I3MCTreeConstPtr: parent(p4) != p3");
}

TEST(utils_HasParent)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  
  ENSURE( I3MCTreeUtils::HasParent(t1,p2) , "hasparent(p2) != true");
  ENSURE( !I3MCTreeUtils::HasParent(t1,p1) , "hasparent(p1) == true");
}

TEST(utils_GetParticle)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  
  const I3Particle& ret = I3MCTreeUtils::GetParticle(t1,p2);
  ENSURE( I3ParticleID(ret) == I3ParticleID(p2) , "get(p2) != p2");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p3);
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p3,p4);
  
  I3Particle* ret2 = I3MCTreeUtils::GetParticlePtr(t2,p4);
  I3MCTree::iterator iter(*t2,p4);
  ENSURE( ret2 == &(*iter) , "I3MCTreePtr: get(p4) != p4");
  
  I3MCTreeConstPtr t3(t2);
  const I3Particle* ret3 = I3MCTreeUtils::GetParticlePtr(t3,p4);
  I3MCTree::const_iterator iter2(*t3,p4);
  ENSURE( ret3 == &(*iter2) , "I3MCTreeConstPtr: get(p4) != p4");
}

TEST(utils_GetPrimary)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  I3MCTreeUtils::AppendChild(t1,p2,p3);
  
  const I3Particle& ret = I3MCTreeUtils::GetPrimary(t1,p3);
  ENSURE( I3ParticleID(ret) == I3ParticleID(p1) , "primary(p3) != p1");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3Particle p6 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p4);
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p4,p5);
  I3MCTreeUtils::AppendChild(*t2,p5,p6);
  
  I3Particle* ret2 = I3MCTreeUtils::GetPrimaryPtr(t2,p6);
  I3MCTree::iterator iter(*t2,p4);
  ENSURE( ret2 == &(*iter) , "I3MCTreePtr: primary(p6) != p4");
  
  I3MCTreeConstPtr t3(t2);
  const I3Particle* ret3 = I3MCTreeUtils::GetPrimaryPtr(t3,p6);
  I3MCTree::const_iterator iter2(*t2,p4);
  ENSURE( ret3 == &(*iter2) , "I3MCTreeConstPtr: primary(p6) != p4");
}

TEST(utils_dump)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3Particle p4 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  I3MCTreeUtils::AppendChild(t1,p1,p3);
  I3MCTreeUtils::AppendChild(t1,p2,p4);
  std::string s = I3MCTreeUtils::Dump(t1);
  //std::cout << std::endl << s << std::endl;
  
  std::vector<std::string> output;
  std::stringstream stream(s);
  std::string line;
  while(std::getline(stream,line)) {
    if (line.length() > 1)
      output.push_back(line);
  }
  ENSURE( output.size() == 4 );
  ENSURE( output.at(0).find_first_not_of(" ") == 0 );
  ENSURE( output.at(1).find_first_not_of(" ") == 2 );
  ENSURE( output.at(2).find_first_not_of(" ") == 4 );
  ENSURE( output.at(3).find_first_not_of(" ") == 2 );
}

TEST(utils_Get)
{
  I3MCTreePtr t1(new I3MCTree);
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t1,p1);
  ENSURE( (bool)t1->get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(*t1,p1,p2);
  I3MCTreeUtils::AppendChild(*t1,p2,p3);
  
  I3Frame f1;
  f1.Put("I3MCTree",t1,I3Frame::Physics);
  ENSURE( f1.Has("I3MCTree") , "error adding tree to frame");
  I3MCTreeConstPtr ret = I3MCTreeUtils::Get(f1,"I3MCTree");
  ENSURE( ret == t1 , "tree not retrieved correctly");
}

unsigned int best_cmp_count;
int best_cmp(const I3Particle& p1, const I3Particle& p2)
{
  best_cmp_count++;
  I3ParticleID pid1(p1), pid2(p2);
  return pid1 < pid2;
}

TEST(utils_GetBest)
{
  using I3MCTreeUtils::GetBest;
  using I3MCTreeUtils::GetBestPtr;
  
  {
    I3MCTree t1;
    I3Particle p1 = makeParticle();
    I3Particle p2 = makeParticle();
    I3Particle p3 = makeParticle();
    I3MCTree::optional_value ret = GetBest(t1,best_cmp);
    ENSURE( !ret , "empty tree should have no best particle");
    I3MCTreeUtils::AddPrimary(t1,p1);
    ENSURE( (bool)t1.get_head() , "primary is missing");
    ret = I3MCTreeUtils::GetBest(t1,best_cmp);
    ENSURE( (bool)ret && ret->GetID() == p1.GetID() , "only particle is not best");
    I3MCTreeUtils::AppendChild(t1,p1,p2);
    I3MCTreeUtils::AppendChild(t1,p2,p3);
    
    best_cmp_count=0;
    ret = I3MCTreeUtils::GetBest(t1,best_cmp);
    ENSURE_EQUAL( best_cmp_count, t1.size()-1 , "wrong number of comparisons performed" );
    ENSURE( (bool)ret && ret->GetID() == p1.GetID() , "p1 is not best");
  }
  
  {
    I3MCTreePtr t2(new I3MCTree);
    I3Particle p4 = makeParticle();
    I3Particle p5 = makeParticle();
    I3Particle p6 = makeParticle();
    I3MCTree::const_iterator ret = GetBestPtr(t2,best_cmp);
    ENSURE( ret==t2->cend() , "empty tree should have no best particle");
    I3MCTreeUtils::AddPrimary(*t2,p4);
    ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
    ret = I3MCTreeUtils::GetBestPtr(t2,best_cmp);
    ENSURE( ret!=t2->cend() && ret->GetID() == p4.GetID() , "only particle is not best");
    I3MCTreeUtils::AppendChild(*t2,p4,p5);
    I3MCTreeUtils::AppendChild(*t2,p5,p6);
    
    best_cmp_count=0;
    ret = I3MCTreeUtils::GetBestPtr(t2,best_cmp);
    I3MCTree::const_iterator iter(*t2,p4);
    ENSURE_EQUAL( best_cmp_count, t2->size()-1 , "wrong number of comparisons performed" );
    ENSURE( ret == iter , "I3MCTreePtr: p4 is not best");
  }
}

bool filter_cmp(const I3Particle& p)
{ return p.GetShape() == I3Particle::Primary; }

TEST(utils_GetFilter)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  I3MCTreeUtils::AppendChild(t1,p2,p3);
  
  std::vector<I3Particle> ret = I3MCTreeUtils::GetFilter(t1,filter_cmp);
  ENSURE( ret.size() == 1 );
  ENSURE( I3ParticleID(ret.at(0)) == I3ParticleID(p1) , "p1 not in filter");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3Particle p6 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p4);
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p4,p5);
  I3MCTreeUtils::AddPrimary(*t2,p6);
  
  const std::vector<I3MCTree::fast_const_iterator> ret2 = I3MCTreeUtils::GetFilterPtr(t2,filter_cmp);
  ENSURE( ret2.size() == 2 );
  I3MCTree::fast_const_iterator iter(*t2,p4);
  ENSURE( std::find(ret2.begin(),ret2.end(),iter) != ret2.end() , "I3MCTreePtr: p4 not in filter");
  iter = I3MCTree::const_iterator(*t2,p6);
  ENSURE( std::find(ret2.begin(),ret2.end(),iter) != ret2.end() , "I3MCTreePtr: p6 not in filter");
}

bool filter_cmp2(const I3Particle& p)
{ return p.GetShape() != I3Particle::Primary; }

TEST(utils_GetBestFilter)
{
  I3MCTree t1;
  I3Particle p1 = makeParticle();
  I3Particle p2 = makeParticle();
  I3Particle p3 = makeParticle();
  I3MCTreeUtils::AddPrimary(t1,p1);
  ENSURE( (bool)t1.get_head() , "primary is missing");
  I3MCTreeUtils::AppendChild(t1,p1,p2);
  I3MCTreeUtils::AppendChild(t1,p2,p3);
  
  const I3MCTree::optional_value ret = I3MCTreeUtils::GetBestFilter(t1,filter_cmp2,best_cmp);
  ENSURE( (bool)ret ,"nothing passed filter" );
  ENSURE( I3ParticleID(*ret) == I3ParticleID(p2) , "p2 not best filtered");
  
  I3MCTreePtr t2(new I3MCTree);
  I3Particle p4 = makeParticle();
  I3Particle p5 = makeParticle();
  I3Particle p6 = makeParticle();
  I3MCTreeUtils::AddPrimary(*t2,p4);
  ENSURE( (bool)t2->get_head() , "I3MCTreePtr: primary is missing");
  I3MCTreeUtils::AppendChild(*t2,p4,p5);
  I3MCTreeUtils::AddPrimary(*t2,p6);
  
  const I3MCTree::const_iterator ret2 = I3MCTreeUtils::GetBestFilterPtr(t2,filter_cmp2,best_cmp);
  ENSURE( ret2 != t2->cend() , "I3MCTreePtr: nothing passed filter");
  I3MCTree::const_iterator iter(*t2,p5);
  ENSURE( ret2 == iter , "I3MCTreePtr: p5 not best filtered");
}




