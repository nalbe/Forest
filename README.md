# Forest

A **C++17 header-only** library providing a node-based forest container –
    a generic tree structure that can own any number of independent root trees (a *forest*).  
The container internally manages tree nodes that store both the user value and
    the linking pointers (parent, first child, next sibling).  
No intrusive hooks are required in your data type – just use it as
    you would `std::vector` or `std::list`.

It is a specialised container for hierarchical, non‑linear data where you want to
    manage node lifetimes and minimise allocations when restructuring subtrees.
It is also well suited for building complex hierarchies intended for
    subsequent processing with standard algorithms.

---

## Motivation

A forest container is useful when:

- **Inline tree construction** – you can define an entire forest hierarchy directly in a single C++ initializer list.
- You frequently restructure subtrees – moving, joining, or splitting – and want to avoid re‑allocation of the nodes themselves.
- You need multiple different orderings/traversals over the same set of nodes without storing extra pointers externally.

If you simply need a flat, sequential collection, stick with the standard containers. If you need to build and manipulate *forests of trees* where the container allocates and links the nodes for you, this library might be a good fit.

---

## Features

- **Node-based design** – Tree nodes are allocated and owned by the container. Each node holds the user value and three linking pointers (parent, first child, next sibling). Your data type remains clean and independent.
- **Forest semantics** – The container holds an arbitrary number of root nodes; each root is the top of a tree.
- **Multiple traversal orders** – `children` (iterate over siblings of a node), `preorder`, `postorder`. Each comes with `const`/mutable bidirectional iterators and reverse iterators. Default “STL‑style” iterators (`begin()`/`end()`) traverse in preorder.
- **Rich interface** – Insertion (`push_back`, `insert`, `emplace`), removal (`remove`, `remove_if`), deep copy (`copy`, `copy_single`), comparison (`compare`), subtree move, subtree join/unjoin, and `swap` of subtrees.
- **Policy‑based iteration** – Functions accept a traversal policy template parameter, allowing you to operate on specific views of the forest.
- **Header‑only** – Just include `Forest.hpp` in your project.
- **C++17** – Takes advantage of concepts, `noexcept` specifiers, and other modern C++ features. Compiles with any conforming C++17 compiler.

---

## Drawbacks / Trade‑offs

No container suits every use case, and this forest container has its own costs:

- **Subtree size tracking (`size_tag`) cascades upward.**  
  When you enable the `size_tag` trait (to keep per‑node subtree sizes), every insertion or removal must update the size counters of all ancestors up to the root. For deep trees this becomes **O(depth) per operation**, which can be significant if you mutate the structure often.

- **Direct child count (`child_count_tag`) is cheap.**  
  In contrast, keeping the number of immediate children requires updating only the parent node – no cascade. It is essentially free.

- **Depth tag (`depth_tag`) doubles the iterator size.**  
  Enabling per‑node depth tracking forces every iterator to carry a `pointer + depth` pair instead of a single pointer. This makes iterators twice as heavy and adds overhead in algorithms that maintain depth information during traversal.

---

## Quick example

```cpp
#include "Forest.hpp"
#include <iostream>

using Forest = cst::Forest<int>;

int main() {
    // flat forest (list of root nodes)
    Forest flat = {1, 2, 3, 4, 5};
    flat.print(std::cout) << '\n';   // preorder by default

    // hierarchical tree via nested initializers
    Forest tree {
        Forest{ 1, {
            Forest{ 11, {
                Forest{ 111 } }},
            Forest{ 12 } }},
        Forest{ 2 } };

    // Different traversal orders
    tree.print<Forest::preorder_t>(std::cout)  << "\n";
    /*
        1
        |------ 11
                |------ 111
        |------ 12
        2
    */
    tree.print<Forest::postorder_t>(std::cout) << "\n";
    /*
                |------ 111
        |------ 11
        |------ 12
        1
        2
    */
    tree.print<Forest::children_t>(std::cout) << "\n";
    /*
        1
        2
    */

    // range-based for (default = preorder)
    std::cout << "Preorder traversal: ";
    for (auto& value : tree) std::cout << value << ' ';
    std::cout << '\n';

    // copy and move
    Forest copy = tree;
    Forest moved = std::move(copy);   // copy is now empty

    // remove a root
    auto it = flat.begin();           // points to root '1'
    flat.remove(it);                  // flat now contains {2,3,4,5}
    flat.print(std::cout) << '\n';
}
```



