/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author tschmidt
*/
#include <dataclasses/I3Tree.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <archive/xml_iarchive.hpp>
#include <archive/xml_oarchive.hpp>
#include <I3Test.h>

using namespace std;

// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3TreeTest);

int testData_pre_order_iterator[] = {4, 5, 6, 1, 2, 3};
int testData_post_order_iterator[] = {5, 6, 4, 2, 3, 1};
int testData_sibling_iterator[] = {5, 6};
int testData_skip_children[] = {4, 1};
int testData_traversal[] = {4, 1, 5};
int testData_flatten[] = {4, 5, 6, 1};
int testData_swap[] = {1, 4};


void createTree(I3Tree<int>& t)
{
  I3Tree<int>::iterator top;
  top = t.insert(t.begin(), 1);
  t.append_child(top, 2);
  t.append_child(top, 3);
  top = t.insert(t.begin(), 4);
  t.append_child(top, 5);
  t.append_child(top, 6);  
}

TEST(a_insert_and_append)
{
  I3Tree<int> t;
  
  ENSURE(t.empty());
  ENSURE_EQUAL(0, t.size());
  ENSURE(!t.is_valid(t.begin()));
  ENSURE(t.begin() == t.end());

  I3Tree<int>::iterator iter1 = t.insert(t.begin(), 1);
  
  ENSURE(!t.empty());
  ENSURE_EQUAL(1, t.size())
  ENSURE(t.begin() != t.end());
  ENSURE(t.begin() == iter1);
  ENSURE(t.is_valid(iter1));
  ENSURE_EQUAL(0, t.depth(iter1));
  ENSURE_EQUAL(1, *iter1);
  ENSURE_EQUAL(0u, iter1.number_of_children());
  ENSURE_EQUAL(0u, t.number_of_siblings(iter1));
  
  I3Tree<int>::iterator iter2 = t.append_child(iter1, 2);

  ENSURE_EQUAL(2, t.size())
  ENSURE_EQUAL(1u, iter1.number_of_children());
  ENSURE(t.is_valid(iter2));
  ENSURE_EQUAL(1, t.depth(iter2));
  ENSURE_EQUAL(2, *iter2);
  ENSURE_EQUAL(0u, iter2.number_of_children());
  ENSURE_EQUAL(0u, t.number_of_siblings(iter2));
  ENSURE_EQUAL(2, *(t.child(iter1, 0)));
  
  I3Tree<int>::iterator iter3 = t.append_child(iter1, 3);

  ENSURE_EQUAL(3, t.size())
  ENSURE_EQUAL(2u, iter1.number_of_children());
  ENSURE(t.is_valid(iter3));
  ENSURE_EQUAL(1, t.depth(iter3));
  ENSURE_EQUAL(3, *iter3);
  ENSURE_EQUAL(0u, iter3.number_of_children());
  ENSURE_EQUAL(1u, t.number_of_siblings(iter2));
  ENSURE_EQUAL(0u, t.number_of_siblings(iter3));
  ENSURE_EQUAL(3, *(t.child(iter1, 1)));
  
  I3Tree<int>::iterator iter4 = t.append_child(iter1, 4);

  ENSURE_EQUAL(4, t.size())
  ENSURE_EQUAL(3u, iter1.number_of_children());
  ENSURE(t.is_valid(iter4));
  ENSURE_EQUAL(1, t.depth(iter4));
  ENSURE_EQUAL(4, *iter4);
  ENSURE_EQUAL(0u, iter4.number_of_children());
  ENSURE_EQUAL(2u, t.number_of_siblings(iter2));
  ENSURE_EQUAL(1u, t.number_of_siblings(iter3));
  ENSURE_EQUAL(0u, t.number_of_siblings(iter4));
  ENSURE_EQUAL(4, *(t.child(iter1, 2)));
}

TEST(b_pre_order_iterator)
{
  I3Tree<int> t;
  I3Tree<int>::pre_order_iterator iter;
  vector<int> v;
  vector<int> vt(testData_pre_order_iterator, testData_pre_order_iterator + 6);

  createTree(t);  
  
  iter = t.begin();
  while(iter != t.end())
  {
    v.push_back(*iter);
    ++iter;
  }
  ENSURE(v == vt);
}

TEST(c_post_order_iterator)
{
  I3Tree<int> t;
  I3Tree<int>::post_order_iterator iter;
  vector<int> v;
  vector<int> vt(testData_post_order_iterator, testData_post_order_iterator + 6);

  createTree(t);  
  
  iter = t.begin_post();
  while(iter != t.end())
  {
    v.push_back(*iter);
    ++iter;
  }
  ENSURE(v == vt);
}

TEST(d_sibling_iterator)
{
  I3Tree<int> t;
  I3Tree<int>::sibling_iterator iter;
  vector<int> v;
  vector<int> vt(testData_sibling_iterator, testData_sibling_iterator + 2);

  createTree(t);  
  
  iter = t.begin(t.begin());
  while(iter != t.end(t.begin()))
  {
    v.push_back(*iter);
    ++iter;
  }
  ENSURE(v == vt);
}

TEST(e_skip_children)
{
  I3Tree<int> t;
  I3Tree<int>::pre_order_iterator iter;
  vector<int> v;
  vector<int> vt(testData_skip_children, testData_skip_children + 2);

  createTree(t);  
  
  iter = t.begin();
  while(iter != t.end())
  {
    v.push_back(*iter);
    iter.skip_children();
    ++iter;
  }
  ENSURE(v == vt);
}

TEST(f_traversal)
{
  I3Tree<int> t;
  I3Tree<int>::pre_order_iterator iter;
  vector<int> v;
  vector<int> vt(testData_traversal, testData_traversal + 3);

  createTree(t);  
  
  iter = t.begin();
  v.push_back(*iter);
  iter += 3;
  v.push_back(*iter);
  iter -= 2;
  v.push_back(*iter);  
  ENSURE(v == vt);
}

TEST(g_equal_and_flatten)
{
  I3Tree<int> t1;
  I3Tree<int> t2;
  I3Tree<int> t3;
  vector<int> v3;
  vector<int> vt(testData_flatten, testData_flatten + 4);
  
  createTree(t1);
  createTree(t2);
  createTree(t3);
    
  ENSURE(equal(t1.begin(), t1.end(), t2.begin()));
  ENSURE(t1.equal(t1.begin(), t1.end(), t2.begin()));
  ENSURE(equal(t1.begin(), t1.end(), t3.begin()));
  ENSURE(t1.equal(t1.begin(), t1.end(), t3.begin()));
  
  *(t2.begin()) = 111;
  
  ENSURE(!equal(t1.begin(), t1.end(), t2.begin()));
  ENSURE(!t1.equal(t1.begin(), t1.end(), t2.begin()));
  
  t3.flatten(t3.begin());

  ENSURE(equal(t1.begin(), t1.end(), t3.begin()));
  ENSURE(!t1.equal(t1.begin(), t1.end(), t3.begin()));

  I3Tree<int>::sibling_iterator iter = t3.begin();
  while(iter != t3.end())
  {
    v3.push_back(*iter);
    ++iter;
  }
  ENSURE(v3 == vt);  
}

TEST(h_clear_and_erase)
{
  I3Tree<int> t1;
  I3Tree<int> t2;
  I3Tree<int>::pre_order_iterator iter;
  I3Tree<int>::sibling_iterator jter;
  
  createTree(t1);
  createTree(t2);
    
  ENSURE(!t1.empty());
  ENSURE(!t2.empty());
  
  t1.clear();
  t2.erase_children(t2.begin());
  
  ENSURE(t1.empty());
  ENSURE_EQUAL(4, t2.size());
  iter = t2.begin();
  ENSURE_EQUAL(4, *iter);
  ++iter;
  ENSURE_EQUAL(1, *iter);
  ENSURE_EQUAL(2u, iter.number_of_children());
  jter = t2.begin(iter);
  ENSURE_EQUAL(2, *jter);
  ++jter;
  ENSURE_EQUAL(3, *jter);
  
  t2.erase(iter);
  
  ENSURE_EQUAL(1, t2.size());
  ENSURE_EQUAL(4, *(t2.begin()));
}

TEST(i_swap)
{
  I3Tree<int> t;
  I3Tree<int>::sibling_iterator iter;
  vector<int> v;
  vector<int> vt(testData_swap, testData_swap + 2);

  createTree(t);  
  
  t.swap(t.begin());
  iter = t.begin();
  while(iter != t.end())
  {
    v.push_back(*iter);
    ++iter;
  }
  ENSURE(v == vt);
}

TEST(l_merge)
{
  I3Tree<int> t1;
  I3Tree<int> t2;
  I3Tree<int> t3;
  I3Tree<int>::pre_order_iterator iter;
  I3Tree<int>::sibling_iterator jter;

  createTree(t1);
  createTree(t2);
  createTree(t3);
  
  ENSURE(t1.equal(t1.begin(), t1.end(), t2.begin()));
  
  t2.erase(t2.begin());
  ENSURE_EQUAL(3, t2.size());
  iter = t2.begin();
  iter += 2;
  t2.append_child(iter, 7);
  t2.append_child(iter, 8);
  
  t1.merge(t1.begin(), t1.end(), t2.begin(), t2.end());

  ENSURE_EQUAL(6, t3.size());
  iter = t3.begin();
  iter += 5;
  t3.append_child(iter, 7);
  t3.append_child(iter, 8);
  ENSURE_EQUAL(t1.size(), t3.size());
  ENSURE(equal(t1.begin(), t1.end(), t3.begin()));
  ENSURE(t1.equal(t1.begin(), t1.end(), t3.begin()));
}

TEST(m_serialization_of_a_full_tree_using_an_xml_archive)
{
  I3Tree<int> t1;
  I3Tree<int> t2;

  createTree(t1);
  
  ostringstream os;
  {
    icecube::archive::xml_oarchive oa(os);
    oa << icecube::serialization::make_nvp("mytree", t1);
  }
  
  istringstream is;
  is.str(os.str());
  {
    icecube::archive::xml_iarchive ia(is);
    ia >> icecube::serialization::make_nvp("mytree", t2);
  }
  
  ENSURE_EQUAL(t1.size(), t2.size());
  ENSURE(equal(t1.begin(), t1.end(), t2.begin()));    
  ENSURE(t1.equal(t1.begin(), t1.end(), t2.begin()));    
}

TEST(n_nontrivial_constructors_and_assignment)
{
  I3Tree<int> t1;
  
  createTree(t1);
  
  I3Tree<int> t2(*(t1.begin()));
  ENSURE_EQUAL(*(t1.begin()), *(t2.begin()));
  
  t2 = t1;
  ENSURE(equal(t1.begin(), t1.end(), t2.begin()));
  ENSURE(t1.equal(t1.begin(), t1.end(), t2.begin()));
  
  I3Tree<int> t3(t1);
  ENSURE(equal(t1.begin(), t1.end(), t3.begin()));
  ENSURE(t1.equal(t1.begin(), t1.end(), t3.begin()));
  
  I3Tree<int> t4(t1.begin());
  ENSURE(t1.equal_subtree(t1.begin(), t4.begin()));
}
