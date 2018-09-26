--------
I3MCTree
--------

The I3MCTree is an STL-like n-ary tree of I3Particles. There is also
a mapping of I3ParticleID -> I3Particle for fast lookups.

Users Guide
===========

.. caution::

   Double insertion of I3Particles is not allowed. Because of the mapping
   relying on I3ParticleID, no two things with the same I3ParticleID can
   be in the tree at the same time.

boost::optional
---------------

Boost provides an optional template type which can optionally store a value,
or not store anything. This is similar to pointers being NULL. The I3MCTree
uses boost::optional for many function return values, so it can return a
particle or the absense of one.

Example Usage
^^^^^^^^^^^^^

An optional can be converted directly to an I3Particle if you're sure it
should exist:

.. code-block:: c++

    I3Particle p = tree.get_head();

If you are unsure about existance in the tree, it is best to check:

.. code-block:: c++

    boost::optional<I3Particle> ret = tree.next_sibling(p);
    if (ret) {
        I3Particle sibling(ret);
    }


C++
---

Everything is const-correct, so be sure to follow the rules.
If something isn't compiling, this may be why.

Iterators
^^^^^^^^^

.. _pre_order_iterator:

  **pre_order_iterator (default)**
  
    Iterates through a tree in pre-order.
    
    .. only:: html
      
      .. raw:: html
          
        <a href="http://en.wikipedia.org/wiki/Tree_traversal#Pre-order" title="Pre-order">
          <img src="http://upload.wikimedia.org/wikipedia/commons/d/d4/Sorted_binary_tree_preorder.svg" alt="Pre-order traversal image" />
        </a>

    #. Visit the root
    #. Traverse the left subtree
    #. Traverse the right subtree

.. _post_order_iterator:

  **post_order_iterator**

    Iterates through a tree in post-order.
    
    .. only:: html
      
      .. raw:: html
          
        <a href="http://en.wikipedia.org/wiki/Tree_traversal#Post-order" title="Post-order">
          <img src="http://upload.wikimedia.org/wikipedia/commons/9/9d/Sorted_binary_tree_postorder.svg" alt="Post-order traversal image" />
        </a>

    #. Traverse the left subtree
    #. Visit the root
    #. Traverse the right subtree

.. _sibling_iterator:

  **sibling_iterator**

    Iterates through all siblings to the right of the given node.

.. _fast_iterator:

  **fast_iterator**

    No guarantees on order of node. Uses underlying datastructures to
    give fastest iterator possible.

.. _leaf_iterator:

  **leaf_iterator**

    Uses a fast_iterator, but only stops at leaf nodes.

Functions
^^^^^^^^^

  Functions that are built-in to the tree. Most functions support
  either I3ParticleID / I3Particle or Iterator arguments.

  **Navigating the Tree**

  .. only:: html
    
    .. raw:: html
    
      <style>span.var{color:blue} span.func{color:green}</style>
      <div class="highlight"><pre>
      <span class="var">Particle</span> <span class="func">get_head</span>()
      <span class="var">vector&lt;Particle&gt;</span> <span class="func">get_heads</span>()
      <span class="var">Particle</span> <span class="func">at</span>(<span class="var">ID</span>)
      <span class="var">Particle</span> <span class="func">parent</span>(<span class="var">ID</span>)
      <span class="var">Particle</span> <span class="func">previous_sibling</span>(<span class="var">ID</span>)
      <span class="var">Particle</span> <span class="func">next_sibling</span>(<span class="var">ID</span>)
      <span class="var">int</span> <span class="func">number_of_siblings</span>(<span class="var">ID</span>)
      <span class="var">Particle</span> <span class="func">first_child</span>(<span class="var">ID</span>)
      <span class="var">vector&lt;Particle&gt;</span> <span class="func">children</span>(<span class="var">ID</span>)
      <span class="var">int</span> <span class="func">number_of_children</span>(<span class="var">ID</span>)
      <span class="var">int</span> <span class="func">depth</span>(<span class="var">ID</span>)
      </pre></div>
  
  .. only:: text
    
    * Particle get_head()
    * vector<Particle> get_heads()
    * Particle at(ID)
    * Particle parent(ID)
    * Particle previous_sibling(ID)
    * Particle next_sibling(ID)
    * int number_of_siblings(ID)
    * Particle first_child(ID)
    * vector<Particle> children(ID)
    * int number_of_children(ID)
    * int depth(ID)

  **Adding to the Tree**

  .. only:: html
    
    .. raw:: html

      <div class="highlight"><pre>
      <span class="var">void</span> <span class="func">insert</span>(<span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">insert_after</span>(<span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">insert</span>(<span class="var">ID</span>, <span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">insert_after</span>(<span class="var">ID</span>, <span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">insert_subtree</span>(<span class="var">ID</span>, <span class="var">Tree</span>, <span class="var">newID</span>)
      <span class="var">void</span> <span class="func">insert_subtree_after</span>(<span class="var">ID</span>, <span class="var">Tree</span>, <span class="var">newID</span>)
      <span class="var">void</span> <span class="func">append_child</span>(<span class="var">ID</span>, <span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">append_child</span>(<span class="var">ID</span>, <span class="var">Tree</span>, <span class="var">newID</span>)
      <span class="var">void</span> <span class="func">append_children</span>(<span class="var">ID</span>, <span class="var">vector&lt;Particle&gt;</span>)
      </pre></div>

  .. only:: text
  
    * void insert(Particle)
    * void insert_after(Particle)
    * void insert(ID, Particle)
    * void insert_after(ID, Particle)
    * void insert_subtree(ID, Tree, newID)
    * void insert_subtree_after(ID, Tree, newID)
    * void append_child(ID, Particle)
    * void append_child(ID, Tree, newID)
    * void append_children(ID, Vector)

  **Modifying the Tree**

  .. only:: html
    
    .. raw:: html
    
      <div class="highlight"><pre>
      <span class="var">void</span> <span class="func">clear</span>()
      <span class="var">void</span> <span class="func">erase</span>(<span class="var">ID</span>)
      <span class="var">void</span> <span class="func">erase_children</span>(<span class="var">ID</span>)
      <span class="var">void</span> <span class="func">flatten</span>(<span class="var">ID</span>)
      <span class="var">void</span> <span class="func">reparent</span>(<span class="var">toID</span>, <span class="var">fromID</span>)
      <span class="var">void</span> <span class="func">replace</span>(<span class="var">ID</span>, <span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">replace</span>(<span class="var">ID</span>, <span class="var">Tree</span>, <span class="var">newID</span>)
      <span class="var">void</span> <span class="func">merge</span>(<span class="var">Tree</span>)
      <span class="var">void</span> <span class="func">swap</span>(<span class="var">Tree</span>)
      </div></span>

  .. only:: text
  
    * void clear()
    * void erase(ID)
    * void erase_children(ID)
    * void flatten(ID)
    * void reparent(toID, fromID)
    * void replace(ID, Particle)
    * void replace(ID, Tree, newID)
    * void merge(Tree)
    * void swap(Tree)

  **General Tree Actions**

  .. only:: html
    
    .. raw:: html

      <div class="highlight"><pre>
      <span class="var">int</span> <span class="func">size</span>()
      <span class="var">bool</span> <span class="func">empty</span>()
      <span class="var">bool</span> <span class="func">is_in_subtree</span>(<span class="var">ID</span>, <span class="var">ID</span>)
      <span class="var">bool</span> <span class="func">subtree_in_tree</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      </pre></div>

  .. only:: text

    * int size()
    * bool empty()
    * bool is_in_subtree(ID, ID)
    * bool subtree_in_tree(Tree, ID)


I3MCTreeUtils
^^^^^^^^^^^^^

  Functions that do not need to know about tree internals to work.

  **General Functions**
  
  .. only:: html
    
    .. raw:: html

      <div class="highlight"><pre>
      <span class="var">void</span> <span class="func">AddPrimary</span>(<span class="var">Tree</span>, <span class="var">Particle</span>)
      <span class="var">void</span> <span class="func">AppendChild</span>(<span class="var">Tree</span>, <span class="var">ID</span>, <span class="var">Particle</span>)
      <span class="var">Particle</span> <span class="func">GetParticle</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">Particle</span> <span class="func">GetPrimary</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">vector&lt;Particle&gt;</span> <span class="func">GetPrimaries</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetParent</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">vector&lt;Particle&gt;</span> <span class="func">GetDaughters</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">bool</span> <span class="func">Has</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">bool</span> <span class="func">HasParent</span>(<span class="var">Tree</span>, <span class="var">ID</span>)
      <span class="var">string</span> <span class="func">Dump</span>(<span class="var">Tree</span>)
      <span class="var">Tree</span> <span class="func">Get</span>(<span class="var">Frame</span>, <span class="var">Key</span>)
      </pre></div>

  .. only:: text

    * void AddPrimary(Tree, Particle)
    * void AppendChild(Tree, ID, Particle)
    * Particle GetParticle(Tree, ID)
    * Particle GetPrimary(Tree, ID)
    * vector<Particle> GetPrimaries(Tree)
    * Particle GetParent(Tree, ID)
    * vector<Particle> GetDaughters(Tree, ID)
    * bool Has(Tree, ID)
    * bool HasParent(Tree, ID)
    * string Dump(Tree)
    * Tree Get(Frame, Key)

  **Filtering / Selection Functions**
  
    Use either a function from the Physics Library or your own.
  
  * GetBest(Tree, Function)
    
    Returns the single particle that best matches a comparison function.
    Function is a callable which takes two I3Particles, compares them,
    and returns true/false (true = first is less than second).
    
  * GetFilter(Tree, Function)
    
    Returns all partiles that pass the filtering function.
    Function is a callable which takes an I3Particle and returns true/false.
    
  * GetBestFilter(Tree, FilterFunction, CmpFunction)
    
    Returns the single particle that best matches a comparison function
    and also passes a filtering function.
    FilterFunction is a callable which takes an I3Particle and returns
    true/false.
    CmpFunction is a callable which takes two I3Particles, compares them,
    and returns true/false (true = first is less than second).

For more C++ details, see the 
`doxygen docs <../../doxygen/dataclasses/classTreeBase_1_1Tree.html>`_.

I3MCTreePhysicsLibrary
^^^^^^^^^^^^^^^^^^^^^^

The following functions comprise a set of very thin convenience wrappers 
around GetBest, GetFilter, and GetBestFilter in I3MCTreeUtils specialized 
to extract analysis-level quantities.  Again, this is a convenience.  Feel 
free to use the I3MCTreeUtils::Get* functions yourself instead.  Currently 
these are limited to pulling the "MostEnergetic" particles of particular 
types out of tree, but there's no reason this library needs to be limited 
to that.  Anything that pulls information out of the tree of general physics 
interest can potentially be added to this library.

The function names were chosen to be self-documenting, for example, 
GetMostEnergeticPrimary, well...it gets the most energetic primary.  
GetMostEnergetic gets the most energetic particle of the type 
(i.e. I3Particle::ParticleType) passed.

  **General Functions**
  
  .. only:: html
    
    .. raw:: html

      <div class="highlight"><pre>
      <span class="var">Particle</span> <span class="func">GetMostEnergeticPrimary</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticInIce</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergetic</span>(<span class="var">Tree</span>, <span class="var">ParticleType</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticTrack</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticCascade</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticInIceCascade</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticNeutrino</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticMuon</span>(<span class="var">Tree</span>)
      <span class="var">Particle</span> <span class="func">GetMostEnergeticNucleus</span>(<span class="var">Tree</span>)
      </pre></div>

  .. only:: text

    * Particle GetMostEnergeticPrimary(Tree)
    * Particle GetMostEnergeticInIce(Tree)
    * Particle GetMostEnergetic(Tree, ParticleType)
    * Particle GetMostEnergeticTrack(Tree)
    * Particle GetMostEnergeticCascade(Tree)
    * Particle GetMostEnergeticInIceCascade(Tree)
    * Particle GetMostEnergeticNeutrino(Tree)
    * Particle GetMostEnergeticMuon(Tree)
    * Particle GetMostEnergeticNucleus(Tree)

All of the functions above have an extra bool in their signature, whose value is **true** by default.  The parameter name is *safe_mode*.
When set to true these functions will return boost::none if there are more than one "best" matches, leaving it up to the caller to decide
how to handle this situation.  This check requires an extra pass over the tree (to collect any matching candidates) and an iteration over
those candidates, so will be slower than having no check.  If you need the extra speed set *safe_mode* to false and pray for the best.

**NB**: On the python side the above functions have the corresponding python-style names, e.g. "get_most_energetic_primary" and live in the dataclasses module.  So the usage is as follows :

::
 Python 2.7.6 (default, Mar 22 2014, 22:59:56) 
 Type "copyright", "credits" or "license" for more information.
 
 IPython 1.2.1 -- An enhanced Interactive Python.
 ?         -> Introduction and overview of IPython's features.
 %quickref -> Quick reference.
 help      -> Python's own help system.
 object?   -> Details about 'object', use 'object??' for extra details.
 
 In [1]: from icecube import dataclasses
 In [2]: t = dataclasses.I3MCTree()
 In [3]: p = dataclasses.get_most_energetic_cascade(t)


Python
------

All C++ functions (both built-in to the tree and in I3MCTreeUtils)
are available in python.

Iterators are simulated through python generators.

Several special functions are included:

* __str__

  Dump the tree to a string::
  
    print tree

* __nonzero__

  Test for empty tree::
  
    if tree:
      print 'the tree is not empty'
    else:
      print 'empty tree'

* __eq__

  Test for equality::
  
    if tree1 == tree2:
      print 'the trees are equal'

* __len__

  Number of particles in the tree::
  
    print 'tree has',len(tree),'particles'

* __contains__

  Does the tree contain an I3ParticleID? ::
  
    if particle in tree:
      print 'found particle'
    else:
      print 'particle not in tree'

* __getitem__

  Get a particle from the tree::
  
    particle = tree[particleID]

* __delitem__

  Erase a particle (and all children) from the tree. Calls the erase() method::
  
    del tree[particleID]

* __iter__

  Gives you the default iterator (pre_order_iterator)::
  
    for particle in tree:
      print particle

For more details, see :ref:`mctree-python-bindings`.


Tree Internals
==============

N-ary Tree
----------

The n-ary tree is formed by making a binary tree representation.

N-ary Tree:

.. only:: html

  .. raw:: html

    <div style="white-space: pre;font-family: 'Consolas', 'Deja Vu Sans Mono',
                'Bitstream Vera Sans Mono', monospace;margin:-1em 0 2em">
              P
           ---^---
          P       P
         / \      /\
        P   P    P  P
       /\   |   /\   \
      P  P  P  P  P   P
    </div>

.. only:: not html

  |         P
  |      ---^---
  |     P       P
  |    / \      /\
  |   P   P    P  P
  |  /\   |   /\   \
  | P  P  P  P  P   P

Binary Representation:

.. only:: html

  .. raw:: html

    <div style="white-space: pre;font-family: 'Consolas', 'Deja Vu Sans Mono',
                'Bitstream Vera Sans Mono', monospace;margin:-1em 0 2em">
              P
            /
          P-------P
         /       /
        P---P    P--P
       /    |   /    \
      P--P  P  P--P   P
    </div>

.. only:: not html

  |         P
  |       /
  |     P-------P
  |    /       /
  |   P---P    P--P
  |  /    |   /    \
  | P--P  P  P--P   P

Storage of I3Particles
----------------------

Each particle is stored in a node which has the following structure:

* parent node pointer
* next sibling node pointer
* first child node pointer
* I3Particle

This allows a compact storage of the binary tree representation above.

Nodes are themselves stored in a hash_map with the hash being the
I3ParticleID of each I3Particle. This allows constant time access
to each I3Particle in the tree. One major constraint is that the I3ParticleID
must be unique, so inserting a second particle with the same ID does not work
(you'll just write over the first copy).

Iterator Invalidation
^^^^^^^^^^^^^^^^^^^^^

The implementation of hash_map only stores pointers to each node, so changing
the shape of the hash_map does not invalidate those pointers, leaving all
references to nodes as valid (though note that the child/sibling/parent
node pointer is not valid if that node is deleted). Moreover, inserting an
element in the hash_map invalidates no hash_map iterators, and removing an
element invalidates only those iterators which point at the removed element.

Thus, operations which add nodes to the tree structure do not invalidate
any iterators. Operators which modify or remove nodes from the tree structure
invalidate all fast_iterator_ and leaf_iterator_, but only invalidate
pre_order_iterator_, post_order_iterator_ and sibling_iterator_ where
the iterator is a child/sibling/parent of the modified or removed nodes.

Iterator Internals
------------------

Iterators use boost::iterator_facade to generate most of the STL syntax
for the iterators. This makes things simpler, but there is still some
complexity.

There are two general types of iterators, with several implementations
of each:

* Tree Iterators

  * pre_order_iterator (default)
  * post_order_iterator
  * sibling_iterator

* hash_map Iterators

  * fast_iterator
  * leaf_iterator

Additionally, each iterator can be constant as well. To present a unified
iterator class to functions, iterator traits were used.

When modifying the tree, iterators are not invalidated unless that node or
a neighboring node is deleted, or the tree structure changes around the node.

Iterator Traits
^^^^^^^^^^^^^^^

Using partial template specialization we can define two different trait
types: 

Const Traits
""""""""""""

  This controls the const-ness of the iterator.

Storage Traits
""""""""""""""

  This contains the actual storage type underlying the iterator. It is a
  node pointer to the current node, and either the I3ParticleID to the
  current node or the hash_map iterator to the current node.

Static Binding of Derived Methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Derived iterators use a static cast to the derived type to call derived
methods. This allows compile-type binding and verification instead of
runtime binding of derived methods, which is mainly of benefit to detect
errors sooner.

The secret code::

  *static_cast<Derived*>(this)

This requires the parent class template to contain the Derived type, but
this was required by boost::iterator_facade anyway.

Compilers
---------

Or, why this doesn't compile with older versions of GCC.

The main reason is `bug 14032 <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=14032>`_.
This has to do with template specialization within a templated class (basing
the inner template off of the outer template). It's important to the Tree
class, and difficult to make everything work without it.

So, GCC 4.3+ is supported. No support for anything lower. Clang is also good.

.. _mctree-python-bindings:

Python Bindings
===============

.. autoclass:: icecube.dataclasses.I3MCTree
    :members:
    :noindex:


