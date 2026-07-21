
# IntrusiveForest

A **C++17 header-only** library providing an intrusive forest container –
    a generic tree structure that can own any number of independent root trees (a *forest*).  
Unlike traditional tree implementations that allocate tree nodes separately,
    IntrusiveForest stores the linking pointers directly inside the user’s data types,
    through a configurable traits mechanism.

It is a specialised container for hierarchical, non‑linear data where you want to
    manage node lifetimes yourself or minimise allocations when restructuring subtrees.
It is also well suited for building complex hierarchies intended for
    subsequent processing with standard algorithms.

---

## Motivation

Intrusive containers are useful when:

- **Inline tree construction** – you can define an entire forest hierarchy directly in a single C++ initializer list.
- You frequently restructure subtrees – moving, joining, or splitting – and want to avoid re‑allocation of the nodes themselves.
- You need multiple different orderings/traversals over the same set of nodes without storing extra pointers externally.

If you simply need a flat, sequential collection, stick with the standard containers. If you need to build and manipulate *forests of trees* where the nodes themselves carry the linkage, this library might be a good fit.

---

## Features

- **Intrusive design** – Link fields (parent, child, sibling) are embedded in the user’s value type via a traits class. No separate heap allocations for tree nodes are performed by the container itself.
- **Forest semantics** – The container holds an arbitrary number of root nodes; each root is the top of a tree.
- **Multiple traversal orders** – `children` (iterate over siblings of a node), `preorder`, `postorder`. Each comes with `const`/mutable bidirectional iterators and reverse iterators. Default “STL‑style” iterators (`begin()`/`end()`) traverse in preorder.
- **Rich interface** – Insertion (`push_back`, `insert`, `emplace`), removal (`remove`, `remove_if`), deep copy (`copy`, `copy_single`), comparison (`compare`), subtree move, subtree join/unjoin, and `swap` of subtrees.
- **Policy‑based iteration** – Functions accept a traversal policy template parameter, allowing you to operate on specific views of the forest.
- **Header‑only** – Just include `IntrusiveForest.hpp` in your project.
- **C++17** – Takes advantage of concepts, `noexcept` specifiers, and other modern C++ features. Compiles with any conforming C++17 compiler.

---

## Drawbacks / Trade‑offs

No container suits every use case, and IntrusiveForest has its own costs:

- **Subtree size tracking (`size_tag`) cascades upward.**  
  When you enable the `size_tag` trait (to keep per‑node subtree sizes), every insertion or removal must update the size counters of all ancestors up to the root. For deep trees this becomes **O(depth) per operation**, which can be significant if you mutate the structure often.

- **Direct child count (`child_count_tag`) is cheap.**  
  In contrast, keeping the number of immediate children requires updating only the parent node – no cascade. It is essentially free.

- **Depth tag (`depth_tag`) doubles the iterator size.**  
  Enabling per‑node depth tracking forces every iterator to carry a `pointer + depth` pair instead of a single pointer. This makes iterators twice as heavy and adds overhead in algorithms that maintain depth information during traversal.

---

## Quick example

```cpp
#include "IntrusiveForest.hpp"
#include <iostream>

using Forest = cst::IntrusiveForest<int>;

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

---

## How the intrusive linking works

The library uses the traits to wrap your value type inside an internal node structure (`Node<traits>::data_node`). This wrapper adds the required pointers. The container itself only stores a sentinel base node representing the forest root.

**Node stability and iterator invalidation:**  
The container never moves the memory of tree nodes itself – it only manipulates the intrusive links embedded within them. As a result, the address of a node remains unchanged for its entire lifetime. Iterators and references to a node are **never invalidated** by operations on other parts of the forest (insertions, removals of siblings, subtree moves, etc.). They become invalid only when the specific node they point to is explicitly removed from the container.

---

## API overview

### Template parameters

`template <typename T, typename TTraits> class Container`

- `T` – the value type stored in each tree node.
- `TTraits` – traits class defining pointer/reference types, size/depth tracking tags, and the linking mechanism.

### Main member functions

| Category               | Functions                                                                                    |
|------------------------|----------------------------------------------------------------------------------------------|
| **Capacity**           | `size()`, `empty()`, `clear()`                                                               |
| **Iterators**          | `begin<Policy>()`, `end<Policy>()`, `cbegin()`, `cend()`, `rbegin()`, `rend()`, etc.         |
| **Insertion**          | `push_back<Policy>(value)`, `insert<Policy>(pos, value)`, `emplace<Policy>(pos, args...)`    |
| **Removal**            | `remove<Policy>(it)`, `remove<Policy>(it, value)`, `remove_if<Policy>(it, pred)`             |
| **Subtree operations** | `join<Policy>(pos, other_forest)`, `unjoin<Policy>(it) -> forest`, `move<Policy>(where, it)` |
| **Copy & compare**     | `copy<Policy>(where, it)`, `copy_single<Policy>(where, it)`, `compare<Policy>(...)`          |
| **Swap**               | `swap<Policy>(it1, it2)`                                                                     |

All operation policies default to `preorder_t` where applicable, but you can explicitly use `children_t` or `postorder_t` when you need a different view.

### Construction & assignment

- Default constructor – empty forest.
- Copy/move constructors and assignment operators perform deep copy or move of all nodes.
- Construct from a single value, an initializer list of values, or a root value with a list of child forests.
- Assignment operators match those constructors.

---

## Installation

IntrusiveForest is header‑only. Copy the entire `src/` directory into your project, preserving its internal folder structure. Then include the main header:

Then include `IntrusiveForest.hpp` and make sure to compile with C++17+ enabled (`-std=c++17` for GCC/Clang, `/std:c++17` for MSVC).
```cpp
#include "IntrusiveForest.hpp"
```

---

## Requirements

- A C++17 compliant compiler.
- No external dependencies beyond the standard library.

---

## When not to use this library

- You need a simple flat array – use `std::vector`.
- You want a non‑intrusive tree that owns its nodes and does not require custom traits – consider `std::set` or a third‑party `tree` container.
- You care about cache locality and the nodes are not already laid out contiguously – an intrusive structure will not magically improve cache behaviour.

---

## Licence

MIT

---


## Contributing

Bug reports, suggestions, and pull requests are welcome. Please open an issue to discuss major changes before submitting a PR.
```
