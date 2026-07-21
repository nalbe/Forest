// NodeRef.hpp
#pragma once

// Implementation-specific headers
#include "../IntegralProxy/IntegralProxy.hpp"
#include "TraversalPolicy.hpp"

// Standard library headers
#include <type_traits>
#include <cstddef>
#include <utility>      // exchange, move
#include <cassert>
#include <functional>   // plus, minus


namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  NodeRef - wrapper for TNode providing core functionality
	// =======================================================================
	template <typename TNode>
	class NodeRef
	{
		// -- friend declaration -------------------------------------------------
	private:
		friend class TraversalPolicy;

		// -- traits type aliases ------------------------------------------------
	protected:
		using self_type             = NodeRef;
		using traits_type           = typename TNode::traits_type;
		using value_type            = typename traits_type::value_type;
		using size_type             = typename traits_type::size_type;
		using difference_type       = typename traits_type::difference_type;
		using reference             = typename traits_type::reference;
		using const_reference       = typename traits_type::const_reference;
		using pointer               = typename traits_type::pointer;
		using const_pointer         = typename traits_type::const_pointer;
		
		// -- node type aliases --------------------------------------------------
	protected:
		using node_type             = typename TNode::self_type;
		using mutable_node_pointer  = node_type*;
		using const_node_pointer    = const node_type*;
		using mutable_node_handle   = node_type**;
		using const_node_handle     = const node_type* const*;

		// -- counter type aliases -----------------------------------------------
	protected:
		using size_tag              = typename traits_type::size_tag;
		using child_count_tag       = typename traits_type::child_count_tag;
		using depth_tag             = typename traits_type::depth_tag;

		// -- traverse policy aliases --------------------------------------------
	public:
		using children_t            = typename TraversalPolicy::children_t;
		using preorder_t            = typename TraversalPolicy::preorder_t;
		using postorder_t           = typename TraversalPolicy::postorder_t;

		// -- data member --------------------------------------------------------
	private:
		mutable_node_handle m_handle;

		// -- base setters -------------------------------------------------------
	protected:
		self_type& set_end(self_type)                                   noexcept;
		self_type& set_rend(self_type)                                  noexcept;
		self_type& set_parent(self_type)                                noexcept;
		self_type& set_prev(self_type)                                  noexcept;
		self_type& set_next(self_type)                                  noexcept;

		// -- base accessors -----------------------------------------------------
	protected:
		[[nodiscard]] self_type self()                            const noexcept;
		[[nodiscard]] self_type end()                             const noexcept;
		[[nodiscard]] self_type rend()                            const noexcept;
		[[nodiscard]] self_type parent()                          const noexcept;
		[[nodiscard]] self_type first()                           const noexcept;
		[[nodiscard]] self_type last()                            const noexcept;
		[[nodiscard]] self_type prev()                            const noexcept;
		[[nodiscard]] self_type next()                            const noexcept;
		[[nodiscard]] self_type leftmost_deepest()                const noexcept;
		[[nodiscard]] self_type rightmost_deepest()               const noexcept;

		// -- common traverse algorithms -----------------------------------------
	protected:
		template <typename TPolicy, typename Op
		> static bool for_each(self_type, self_type, Op&&)
			noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>())));
		template <typename TPolicy, typename Op
		> static bool for_each(self_type, self_type, self_type, self_type, Op&&)
			noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>(), std::declval<self_type&>())));
		template <typename TPolicy, typename Op
		> static bool for_each_reverse(self_type, self_type, Op&&)
			noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>())));
		template <typename TPolicy, typename Op
		> static bool for_each_reverse(self_type, self_type, self_type, self_type, Op&&)
			noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>(), std::declval<self_type&>())));

		// -- access data --------------------------------------------------------
	public:
		[[nodiscard]] reference       data()                      const noexcept;
		[[nodiscard]] const_reference cdata()                     const noexcept;

		// -- cached counters (optional) -----------------------------------------
	public:
		[[nodiscard]] auto size()                                 const noexcept;
		[[nodiscard]] auto child_count()                          const noexcept;

		// -- iterative counters -------------------------------------------------
	public:
		[[nodiscard]] size_type count_depth()                     const noexcept;
		[[nodiscard]] size_type count_size()                      const noexcept;
		[[nodiscard]] size_type count_children()                  const noexcept;

		// -- node checks --------------------------------------------------------
	public:
		[[nodiscard]] bool valid()                                const noexcept;
		[[nodiscard]] bool invalid()                              const noexcept;
		[[nodiscard]] bool is_root()                              const noexcept;
		[[nodiscard]] bool is_top()                               const noexcept;
		[[nodiscard]] bool is_real()                              const noexcept;
		[[nodiscard]] bool is_begin()                             const noexcept;
		[[nodiscard]] bool is_rbegin()                            const noexcept;
		[[nodiscard]] bool has_prev()                             const noexcept;
		[[nodiscard]] bool has_next()                             const noexcept;
		[[nodiscard]] bool has_children()                         const noexcept;
		template <typename Pred
		> bool [[nodiscard]] is_equal(self_type, Pred&&)
			const noexcept(noexcept(std::declval<Pred>()(std::declval<const value_type&>(), std::declval<const value_type&>())));
		template <typename Pred
		> bool [[nodiscard]] is_subtree_equal(self_type, Pred&&)
			const noexcept(noexcept(std::declval<Pred>()(std::declval<const value_type&>(), std::declval<const value_type&>())));

		// -- sentinel checks ----------------------------------------------------
	public:
		[[nodiscard]] bool is_sentinel()                          const noexcept;
		[[nodiscard]] bool is_end_of_empty_subtree()              const noexcept;
		[[nodiscard]] bool is_rend_of_empty_subtree()             const noexcept;
		[[nodiscard]] bool is_sentinel_of_empty_subtree()         const noexcept;
		[[nodiscard]] bool is_end_of_non_empty_subtree()          const noexcept;
		[[nodiscard]] bool is_rend_of_non_empty_subtree()         const noexcept;
		[[nodiscard]] bool is_sentinel_of_non_empty_subtree()     const noexcept;
		[[nodiscard]] bool is_end()                               const noexcept;
		[[nodiscard]] bool is_rend()                              const noexcept;

		// -- non-modifiers ------------------------------------------------------
	public:
		self_type copy_single()                                   const;
		self_type copy()                                          const;
		self_type copy_subtree(self_type)                         const;

		// -- modifiers ----------------------------------------------------------
	public:
		self_type  remove()                                             noexcept;
		self_type& remove_subtree()                                     noexcept;
		self_type& move(self_type)                                      noexcept;
		self_type& move_subtree(self_type)                              noexcept;
		void       swap(self_type)                                      noexcept;
		template <typename Pred
		> size_type remove_if(Pred&&)                                   noexcept
			(noexcept(std::declval<Pred>()(std::declval<const value_type&>())));

		// -- linkage ------------------------------------------------------------
	public:
		self_type& unlink()                                             noexcept;
		self_type& link(self_type)                                      noexcept;

		// -- helpers ------------------------------------------------------------
	private:
		self_type& unlink_impl()                                        noexcept;
		self_type& link_impl(self_type)                                 noexcept;
		self_type& bind_as_prev(self_type)                              noexcept;
		self_type& bind_as_next(self_type)                              noexcept;
		self_type  parent_for_linkage()                           const noexcept;
		self_type  prev_for_linkage()                             const noexcept;
		self_type  next_for_linkage()                             const noexcept;
		template <typename Op
		> self_type& update_sizes_upwards(size_type, Op&&)              noexcept;

		// -- public constructors ------------------------------------------------
	public:
		NodeRef(const self_type&)                                       noexcept;
		NodeRef(self_type&&)                                            noexcept;
		explicit NodeRef(mutable_node_handle)                           noexcept;
		explicit NodeRef(mutable_node_pointer)                          noexcept;
		NodeRef(std::nullptr_t)                                         noexcept;

		// -- public operators ---------------------------------------------------
	public:
		self_type& operator=(const self_type&)                          noexcept;
		self_type& operator=(self_type&&)                               noexcept;
		self_type& operator=(mutable_node_handle)                       noexcept;
		self_type& operator=(mutable_node_pointer)                      noexcept;
		self_type& operator=(std::nullptr_t)                            noexcept;

		explicit operator mutable_node_handle()                   const noexcept;
		explicit operator mutable_node_pointer()                  const noexcept;

		// -- private constructors -----------------------------------------------
	protected:
		explicit NodeRef(const_node_handle)                             noexcept;
		explicit NodeRef(const_node_pointer)                            noexcept;

		// -- free operators -----------------------------------------------------
	protected:
		template <typename T> friend bool operator==(const NodeRef<T>&, const NodeRef<T>&);
		template <typename T> friend bool operator!=(const NodeRef<T>&, const NodeRef<T>&);
		template <typename T> friend bool operator==(const NodeRef<T>&, const typename NodeRef<T>::const_node_handle&);
		template <typename T> friend bool operator==(const typename NodeRef<T>::const_node_handle&, const NodeRef<T>&);
		template <typename T> friend bool operator!=(const NodeRef<T>&, const typename NodeRef<T>::const_node_handle&);
		template <typename T> friend bool operator!=(const typename NodeRef<T>::const_node_handle&, const NodeRef<T>&);

	};  // class NodeRef



	/// -- base setters -------------------------------------------------------

	// set end sentinel to new node pointer
	template <typename T>
	auto NodeRef<T>::set_end(self_type ref) noexcept -> self_type&
	{
		assert(is_real());
		(**m_handle).pEnd = *ref.m_handle;
		return *this;
	}

	// set reverse-end sentinel to new node pointer
	template <typename T>
	auto NodeRef<T>::set_rend(self_type ref) noexcept -> self_type&
	{
		assert(is_real());
		(**m_handle).pREnd = *ref.m_handle;
		return *this;
	}

	// set parent to new node handle 
	template <typename T>
	auto NodeRef<T>::set_parent(self_type ref) noexcept -> self_type&
	{
		assert(is_real());
		(**m_handle).pParent = ref.m_handle;
		return *this;
	}

	// set previous-sibling to new node handle
	template <typename T>
	auto NodeRef<T>::set_prev(self_type ref) noexcept -> self_type&
	{
		assert(is_real());
		(**m_handle).pPrev = ref.m_handle;
		return *this;
	}

	// set next-sibling to new node handle
	template <typename T>
	auto NodeRef<T>::set_next(self_type ref) noexcept -> self_type&
	{
		assert(is_real());
		(**m_handle).pNext = ref.m_handle;
		return *this;
	}


	/// -- base accessors -----------------------------------------------------

	// raw access to the self node reference
	template <typename T>
	auto NodeRef<T>::self() const noexcept -> self_type
	{
		return self_type{ &(**m_handle).pSelf };
	}

	// raw access to the end sentinel reference
	template <typename T>
	auto NodeRef<T>::end() const noexcept -> self_type
	{
		return self_type{ &(**m_handle).pEnd };
	}

	// raw access to the reverse end sentinel reference
	template <typename T>
	auto NodeRef<T>::rend() const noexcept -> self_type
	{
		return self_type{ &(**m_handle).pREnd };
	}

	// raw access to the parent node reference
	template <typename T>
	auto NodeRef<T>::parent() const noexcept -> self_type
	{
		return self_type{ (**m_handle).pParent };
	}

	// raw access the first child node reference
	template <typename T>
	auto NodeRef<T>::first() const noexcept -> self_type
	{
		return self_type{ &(**m_handle).pREnd->pSelf };
	}

	// raw access the last child node reference
	template <typename T>
	auto NodeRef<T>::last() const noexcept -> self_type
	{
		return self_type{ &(**m_handle).pEnd->pSelf };
	}

	// raw access the previous sibling node reference
	template <typename T>
	auto NodeRef<T>::prev() const noexcept -> self_type
	{
		return self_type{ (**m_handle).pPrev };
	}

	// raw access the next sibling node reference
	template <typename T>
	auto NodeRef<T>::next() const noexcept -> self_type
	{
		return self_type{ (**m_handle).pNext };
	}

	// access the leftmost deepest child node reference
	template <typename T>
	auto NodeRef<T>::leftmost_deepest() const noexcept -> self_type
	{
		assert(is_real());
		self_type it{ *this };
		for (; it.has_children(); it = it.first());
		return it;
	}

	// access the rightmost deepest child node reference
	template <typename T>
	auto NodeRef<T>::rightmost_deepest() const noexcept -> self_type
	{
		assert(is_real());
		self_type it{ *this };
		for (; it.has_children(); it = it.last());
		return it;
	}


	/// -- common traverse algorithms -----------------------------------------

	// traverses a range of nodes, applying a operation
	template <typename T>
	template <typename U, typename Op>
	bool NodeRef<T>::for_each(self_type ref, self_type ref_end, Op&& op)
		noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>())))
	{
		while (ref != ref_end) {
			if (!op(ref)) {
				return false;
			}
			ref = U::next(ref, ref_end);
		}
		return true;
	}

	// traverses two ranges of nodes in parallel, applying a binary operation
	template <typename T>
	template <typename U, typename Op>
	bool NodeRef<T>::for_each(self_type lhs, self_type lhs_end, self_type rhs, self_type rhs_end, Op&& op)
		noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>(), std::declval<self_type&>())))
	{
		while (lhs != lhs_end and rhs != rhs_end) {
			if (!op(lhs, rhs)) {
				return false;
			}
			lhs = U::next(lhs, lhs_end);
			rhs = U::next(rhs, rhs_end);
		}
		return lhs == lhs_end and rhs == rhs_end;
	}

	// reverse-preorder traversal of subtree in lockstep, capturing node and applying operation one step behind
	template <typename T>
	template <typename U, typename Op>
	bool NodeRef<T>::for_each_reverse(self_type ref_end, self_type ref, Op&& op)
		noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>())))
	{
		ref = U::prev(ref);
		while (ref != ref_end) {
			if (!op(
				std::exchange(ref, U::prev(ref))
			)) {
				return false;
			}
		}
		return op(ref);
	}

	// reverse-preorder traversal of two subtrees in lockstep, capturing nodes and applying binary operation one step behind
	template <typename T>
	template <typename U, typename Op>
	bool NodeRef<T>::for_each_reverse(self_type lhs_end, self_type lhs, self_type rhs_end, self_type rhs, Op&& op)
		noexcept(noexcept(std::declval<Op&>()(std::declval<self_type&>(), std::declval<self_type&>())))
	{
		lhs = U::prev(lhs);
		rhs = U::prev(rhs);
		while (lhs != lhs_end and rhs != rhs_end) {
			if (!op(
				std::exchange(lhs, U::prev(lhs)),
				std::exchange(rhs, U::prev(rhs))
			)) {
				return false;
			}
		}
		return (lhs == lhs_end and rhs == rhs_end)
			? op(lhs, rhs)
			: false;
	}


	/// -- cached counters (optional) -----------------------------------------

	// total number of elements
	template <typename T>
	auto NodeRef<T>::size() const noexcept
	{
		assert(is_real());
		if constexpr (size_tag::value) {
			return IntegralProxy<size_type, std::true_type>( (**m_handle).nSize );
		}
		else {
			return IntegralProxy<size_type, std::false_type>( 1 );
		}
	}

	// number of direct children
	template <typename T>
	auto NodeRef<T>::child_count() const noexcept
	{
		assert(is_real());
		if constexpr (child_count_tag::value) {
			return IntegralProxy<size_type, std::true_type>( (**m_handle).nChildCount );
		}
		else {
			return IntegralProxy<size_type, std::false_type>( 0 );
		}
	}


	/// -- access data --------------------------------------------------------

	// access the data reference
	template <typename T>
	auto NodeRef<T>::data() const noexcept -> reference
	{
		assert(is_real());
		return reinterpret_cast<reference>((**m_handle).data);
	}

	// access the data reference
	template <typename T>
	auto NodeRef<T>::cdata() const noexcept -> const_reference
	{
		return data();
	}


	/// -- iterative counters -------------------------------------------------

	// count depth iteratively (slow)
	template <typename T>
	auto NodeRef<T>::count_depth() const noexcept -> size_type
	{
		assert(is_real());
		size_type cnt{};
		for (self_type it{ parent() }; it.valid(); it = it.parent()) {
			++cnt;
		}
		return cnt;  // hidden root has depth 0
	}

	// count size iteratively (slow)
	template <typename T>
	auto NodeRef<T>::count_size() const noexcept -> size_type
	{
		assert(is_real());
		size_type cnt{};
		bool result = for_each<preorder_t>(
			preorder_t::begin(*this), end(),
			[&cnt]([[maybe_unused]] self_type) {
				++cnt;
				return true;
			}
		);
		assert(result);
		return cnt;
	}

	// count children iteratively (slow)
	template <typename T>
	auto NodeRef<T>::count_children() const noexcept -> size_type
	{
		assert(is_real());
		size_type cnt{};
		bool result = for_each<children_t>(
			children_t::begin(*this), end(),
			[&cnt]([[maybe_unused]] self_type) {
				++cnt;
				return true;
			}
		);
		assert(result);
		return cnt;
	}


	/// -- node checks --------------------------------------------------------

	// validation for node operations
	template <typename T>
	bool NodeRef<T>::valid() const noexcept
	{
		return m_handle and *m_handle;  // sentinel is valid as well
	}

	// invalidation for node operations
	template <typename T>
	bool NodeRef<T>::invalid() const noexcept
	{
		return !valid();
	}

	// check if the node is the hidden root
	template <typename T>
	bool NodeRef<T>::is_root() const noexcept
	{
		assert(valid());
		assert(is_real());
		return parent().invalid();
	}

	// check if the most top valid node
	template <typename T>
	bool NodeRef<T>::is_top() const noexcept
	{
		assert(valid());
		return parent().is_root();
	}

	// checks if the node is real
	template <typename T>
	bool NodeRef<T>::is_real() const noexcept
	{
		assert(valid());
		return m_handle == self();
	}

	// check if the node is the first node of child list
	template <typename T>
	bool NodeRef<T>::is_begin() const noexcept
	{
		assert(valid());
		return m_handle == prev().self();
	}

	// check if the node is the last node of child list
	template <typename T>
	bool NodeRef<T>::is_rbegin() const noexcept
	{
		assert(valid());
		return m_handle == next().self();
	}

	// checks if the node has left sibling
	template <typename T>
	bool NodeRef<T>::has_prev() const noexcept
	{
		assert(is_real());
		return !is_begin();  // prev().is_real() -- false-positive for rend sentinel itself
	}

	// checks if the node has right sibling
	template <typename T>
	bool NodeRef<T>::has_next() const noexcept
	{
		assert(is_real());
		return !is_rbegin();  // next().is_real() -- false-positive for end sentinel itself
	}

	// checks if the node has any children
	template <typename T>
	bool NodeRef<T>::has_children() const noexcept
	{
		assert(is_real());
		return *m_handle != (**m_handle).pEnd;
	}

	// compare pair of nodes
	template <typename T>
	template <typename Pred>
	bool NodeRef<T>::is_equal(self_type other, Pred&& pred)
		const noexcept(noexcept(std::declval<Pred>()(std::declval<const value_type&>(), std::declval<const value_type&>())))
	{
		assert(is_real());
		assert(other.is_real());
		if (this == other) {
			return true;
		}
		return for_each<preorder_t>(
			preorder_t::begin(*this), preorder_t::end(*this),
			preorder_t::begin(other), preorder_t::end(other),
			[&pred](auto lhs, auto rhs) {
				return lhs.size() == rhs.size()
					and lhs.child_count() == rhs.child_count()
					and pred( lhs.data(), rhs.data() );
			}
		);
	}

	// compare sub-trees
	template <typename T>
	template <typename Pred>
	bool NodeRef<T>::is_subtree_equal(self_type other, Pred&& pred)
		const noexcept(noexcept(std::declval<Pred>()(std::declval<const value_type&>(), std::declval<const value_type&>())))
	{
		assert(is_real());
		assert(other.is_real());
		if (this == other) {
			return true;
		}
		return for_each<preorder_t>(
			children_t::begin(*this), children_t::end(*this),
			children_t::begin(other), children_t::end(other),
			[&pred](auto lhs, auto rhs) {
				return lhs.size() == rhs.size()
					and lhs.child_count() == rhs.child_count()
					and pred( lhs.data(), rhs.data() );
			}
		);
	}


	/// -- sentinel checks ----------------------------------------------------

	// check if node is sentinel (not real)
	template <typename T>
	bool NodeRef<T>::is_sentinel() const noexcept
	{
		return !is_real();  // any sentinel
	}

	// is a rend sentinel that indicating an empty list
	template <typename T>
	bool NodeRef<T>::is_rend_of_empty_subtree() const noexcept
	{
		return m_handle == rend();
	}

	// is an end sentinel that indicating an empty list
	template <typename T>
	bool NodeRef<T>::is_end_of_empty_subtree() const noexcept
	{
		return m_handle == end();
	}

	// is a rend sentinel that indicating a non-empty list
	template <typename T>
	bool NodeRef<T>::is_rend_of_non_empty_subtree() const noexcept
	{
		return m_handle == prev();
		// m_ptr == parent().rend(); -- false-positive for empty root node
	}

	// is an end sentinel that indicating a non-empty list
	template <typename T>
	bool NodeRef<T>::is_end_of_non_empty_subtree() const noexcept
	{
		return m_handle == next();
		// m_ptr == parent().end(); -- false-positive for empty root node
		// m_ptr == next(); -- false-positive for top-end sentinel
	}

	// is a end/rend sentinel that indicating an empty list
	template <typename T>
	bool NodeRef<T>::is_sentinel_of_empty_subtree() const noexcept
	{
		return is_rend_of_empty_subtree() or is_end_of_empty_subtree();
	}

	// is a end/rend sentinel that indicating a non-empty list
	template <typename T>
	bool NodeRef<T>::is_sentinel_of_non_empty_subtree() const noexcept
	{
		return is_rend_of_non_empty_subtree() or is_end_of_non_empty_subtree();
	}

	// check if the node is the end sentinel
	template <typename T>
	bool NodeRef<T>::is_end() const noexcept
	{
		assert(valid());
		return is_end_of_empty_subtree() or is_end_of_non_empty_subtree();
	}

	// check if the node is the rend sentinel
	template <typename T>
	bool NodeRef<T>::is_rend() const noexcept
	{
		assert(valid());
		return is_rend_of_empty_subtree() or is_rend_of_non_empty_subtree();
	}


	/// -- non-modifiers ------------------------------------------------------

	// copy node
	template <typename T>
	auto NodeRef<T>::copy_single() const -> self_type
	{
		assert(is_real());
		return self_type{ new node_type(data()) };
	}

	// deep-copy node
	template <typename T>
	auto NodeRef<T>::copy() const -> self_type
	{
		assert(is_real());
		self_type copied{ copy_single() };
		for_each<preorder_t>(
			copied, copied.end(),
			*this, end(),
			[](self_type lhs, self_type rhs) {
				for (self_type it{ children_t::begin(rhs) }; it != rhs.end(); it = it.next()) {
					it.copy_single().link_impl( lhs.end() );
				}
				lhs.size() = rhs.size();
				return true;
			}
		);
		return copied;
	}

	// deep-copies sub-tree to the indicated position; returns reference to the first copied node
	template <typename T>
	auto NodeRef<T>::copy_subtree(self_type where) const -> self_type
	{
		assert(is_real());
		self_type new_parent{ where.parent_for_linkage() };
		self_type new_left{ where.prev_for_linkage() };
		bool result = for_each<children_t>(
			children_t::begin(*this), end(),
			[&where](self_type ref) {
				ref.copy().link_impl(where);
				return true;
			}
		);
		assert(result);
		new_parent.update_sizes_upwards( size() - 1, std::plus<>{} );
		return new_left.next_for_linkage();
	}


	/// -- modifiers ----------------------------------------------------------

	// removes the node with its subtree and returns reference to node that followed it
	template <typename T>
	auto NodeRef<T>::remove() noexcept -> self_type
	{
		assert(is_real());
		self_type following = next();
		unlink();
		bool result = for_each_reverse<preorder_t>(
			*this, end(),  // should begins at self, stops at end
			[](self_type ref) {
				delete *ref.m_handle;
				return true;
			});
		assert(result);
		return following;
	}  // not applicable for linked nodes

	// destroy sub-tree
	template <typename T>
	auto NodeRef<T>::remove_subtree() noexcept -> self_type&
	{
		assert(is_real());
		bool result = for_each_reverse<preorder_t>(
			children_t::begin(*this), end(),
			[](self_type ref) {
				delete *ref.m_handle;
				return true;
			}
		);
		assert(result);
		update_sizes_upwards( size() - 1, std::minus<>{} );
		return *this;
	}

	// move node before the position indicated by where
	template <typename T>
	auto NodeRef<T>::move(self_type where) noexcept -> self_type&
	{
		assert(is_real());
		if (where == *this) {
			return *this;
		}
		parent().update_sizes_upwards( size(), std::minus<>{} );
		unlink_impl();
		link_impl(where);
		parent().update_sizes_upwards( size(), std::plus<>{} );
		return *this;
	}

	// moves sub-tree to the new empty parent
	template <typename T>
	auto NodeRef<T>::move_subtree(self_type where) noexcept -> self_type&
	{
		assert(is_real());
		if (!has_children()) {
			return *this;
		}
		self_type new_parent{ where.parent_for_linkage() };
		self_type new_left{ where.prev_for_linkage() };
		self_type new_right{ where };
		for_each<children_t>(
			children_t::begin(*this), end(),
			[&new_parent](self_type ref) {
				ref.set_parent(new_parent);
				return true;
			});
		first().set_prev( new_left );
		last().set_next( new_right );
		new_left.bind_as_next( first() );
		new_right.bind_as_prev( last() );
		new_parent.child_count() = child_count();
		new_parent.update_sizes_upwards( size() - 1, std::plus<>{} );
		update_sizes_upwards( size() - 1, std::minus<>{} );
		set_end( *this );
		set_rend( *this );
		child_count().reset();
		return *this;
	}

	// swap nodes
	template <typename T>
	void NodeRef<T>::swap(self_type other) noexcept
	{
		assert(is_real());
		assert(other.is_real());
		if (*this == other) {
			return;
		}
		if (next() == other) {
			unlink().link(other.next());
		}  // adjacent nodes (a <-> b)
		else if (other.next() == *this) {
			other.swap(*this);
		}  // reverse adjacency
		else {
			auto pos = next();
			unlink().link(other.next());
			other.unlink().link(pos);
		}  // non-adjacent nodes
	}

	// removes all nodes for which predicate returns true
	template <typename T>
	template <typename Pred>
	auto NodeRef<T>::remove_if(Pred&& pred)
		noexcept(noexcept(std::declval<Pred>()(std::declval<const value_type&>())))	-> size_type
	{
		assert(is_real());
		size_type cnt{};
		bool result = for_each_reverse<preorder_t>(
			preorder_t::begin(*this), end(),
			[&cnt, &pred](self_type ref) {
				if (pred( ref.cdata() )) {
					cnt += ref.size();
					ref.remove();
				}
				return true;
			}
		);
		assert(result);
		return cnt;
	}


	/// -- linkage ------------------------------------------------------------

	// unlink node from its parent's sibling list
	template <typename T>
	auto NodeRef<T>::unlink() noexcept -> self_type&
	{
		parent().update_sizes_upwards( size(), std::minus<>{} );
		unlink_impl();
		set_parent(nullptr);
		set_prev(nullptr);
		set_next(nullptr);
		return *this;
	}

	// link a node to new parent's sibling list
	template <typename T>
	auto NodeRef<T>::link(self_type where) noexcept -> self_type&
	{
		if (where == *this) {
			return *this;
		}
		link_impl(where);
		parent().update_sizes_upwards( size(), std::plus<>{} );
		return *this;
	}


	/// -- helpers----------------------------------------------------------------

	// unlink node from its parent's sibling list
	template <typename T>
	auto NodeRef<T>::unlink_impl() noexcept -> self_type&
	{
		--parent().child_count();
		prev_for_linkage().bind_as_next( next_for_linkage() );
		next_for_linkage().bind_as_prev( prev_for_linkage() );
		return *this;
	}

	// link a node to new parent's sibling list
	template <typename T>
	auto NodeRef<T>::link_impl(self_type where) noexcept -> self_type&
	{
		set_parent( where.parent_for_linkage() );
		set_prev( where.prev_for_linkage() );
		set_next( where );

		prev().bind_as_next( self() );
		where.bind_as_prev( self() );

		++parent().child_count();
		return *this;
	}

	// set previous-sibling node reference
	template <typename T>
	auto NodeRef<T>::bind_as_prev(self_type ref) noexcept -> self_type&
	{
		if (is_real()) {
			return set_prev(ref);
		}
		auto pfl{ parent_for_linkage() };
		pfl.set_end(ref.is_real() ? ref : pfl);
		return *this;
	}

	// set next-sibling node reference
	template <typename T>
	auto NodeRef<T>::bind_as_next(self_type ref) noexcept -> self_type&
	{
		if (is_real()) {
			return set_next(ref);
		}
		auto pfl{ parent_for_linkage() };
		pfl.set_rend(ref.is_real() ? ref : pfl);
		return *this;
	}

	// retrieve the parent node reference
	template <typename T>
	auto NodeRef<T>::parent_for_linkage() const noexcept -> self_type
	{
		if (is_sentinel_of_empty_subtree()) {
			return self();
		}
		return parent();
	}

	// retrieve the previous sibling node reference
	template <typename T>
	auto NodeRef<T>::prev_for_linkage() const noexcept -> self_type
	{
		if (is_real()) {
			return prev();
		}
		if (is_end_of_non_empty_subtree()) {
			return self();  // last <- end
		}
		return rend();  // rend <- end
	}  // not applicable for rend

	// retrieve the next sibling node reference
	template <typename T>
	auto NodeRef<T>::next_for_linkage() const noexcept -> self_type
	{
		if (is_real()) {
			return next();
		}
		if (is_rend_of_non_empty_subtree()) {
			return self();  // rend -> first
		}
		return end();  // rend -> end
	}  // not applicable for end

	// updates the total subtree count upwards to the root
	template <typename T>
	template <typename Op>
	auto NodeRef<T>::update_sizes_upwards(size_type value, Op&& op) noexcept -> self_type&
	{
		if constexpr (size_tag::value) {
			for (auto it{ *this }; it.valid(); it = it.parent()) {
				it.size().set(
					op(it.size(), value)
				);
			}
		}
		return *this;
	}


	/// -- public constructors ---------------------------------------------------

	// nullptr -> self (construct)
	template <typename T>
	NodeRef<T>::NodeRef(std::nullptr_t) noexcept :
		m_handle(nullptr)
	{}

	// self -> self (copy construct)
	template <typename T>
	NodeRef<T>::NodeRef(const self_type& other) noexcept :
		m_handle(other.m_handle)
	{}

	// self -> self (move construct)
	template <typename T>
	NodeRef<T>::NodeRef(self_type&& other) noexcept :
		m_handle(other.m_handle)
	{}

	// non-const handle -> self (construct)
	template <typename T>
	NodeRef<T>::NodeRef(mutable_node_handle handle) noexcept :
		m_handle(handle)
	{}

	// non-const pointer -> self (construct)
	template <typename T>
	NodeRef<T>::NodeRef(mutable_node_pointer ptr) noexcept :
		m_handle(&ptr->pSelf)
	{}


	/// -- public operators ------------------------------------------------------

	// self -> self (copy assign)
	template <typename T>
	auto NodeRef<T>::operator=(const self_type& other) noexcept -> self_type&
	{
		m_handle = other.m_handle;
		return *this;
	}

	// self -> self (move assign)
	template <typename T>
	auto NodeRef<T>::operator=(self_type&& other) noexcept -> self_type&
	{
		m_handle = other.m_handle;
		return *this;
	}

	// handle -> self (assign, const_cast)
	template <typename T>
	auto NodeRef<T>::operator=(mutable_node_handle handle) noexcept -> self_type&
	{
		m_handle = const_cast<mutable_node_handle>(handle);
		return *this;
	}

	// pointer -> self (assign, const_cast)
	template <typename T>
	auto NodeRef<T>::operator=(mutable_node_pointer ptr) noexcept -> self_type&
	{
		m_handle = const_cast<mutable_node_handle>(&ptr->pSelf);
		return *this;
	}

	// nullptr -> self (assign)
	template <typename T>
	auto NodeRef<T>::operator=(std::nullptr_t) noexcept -> self_type&
	{
		m_handle = nullptr;
		return *this;
	}


	// self -> non-const handle (explicit conversion)
	template <typename T>
	NodeRef<T>::operator mutable_node_handle() const noexcept
	{
		return m_handle;
	}

	// self -> non-const pointer (explicit conversion)
	template <typename T>
	NodeRef<T>::operator mutable_node_pointer() const noexcept
	{
		return *m_handle;
	}


	/// -- private constructors ----------------------------------------------

	// const handle -> self (construct, const_cast)
	template <typename T>
	NodeRef<T>::NodeRef(const_node_handle handle) noexcept :
		m_handle(static_cast<mutable_node_handle>(handle))
	{}

	// const pointer -> self (construct, const_cast)
	template <typename T>
	NodeRef<T>::NodeRef(const_node_pointer ptr) noexcept :
		m_handle(static_cast<mutable_node_handle>(&ptr->pSelf))
	{}


	/// -- free operators -----------------------------------------------------

	// self == self
	template <typename U>
	bool operator==(const NodeRef<U>& lhs, const NodeRef<U>& rhs)
	{
		return lhs.m_handle == rhs.m_handle;
	}

	// self != self
	template <typename U>
	bool operator!=(const NodeRef<U>& lhs, const NodeRef<U>& rhs)
	{
		return !(lhs == rhs);
	}

	// self == handle
	template <typename U>
	bool operator==(const NodeRef<U>& lhs, const typename NodeRef<U>::const_node_handle& rhs)
	{
		return lhs.m_handle == rhs;
	}

	// handle == self
	template <typename U>
	bool operator==(const typename NodeRef<U>::const_node_handle& lhs, const NodeRef<U>& rhs)
	{
		return lhs == rhs.m_handle;
	}

	// self != handle
	template <typename U>
	bool operator!=(const NodeRef<U>& lhs, const typename NodeRef<U>::const_node_handle& rhs)
	{
		return !(lhs == rhs);
	}

	// handle != self
	template <typename U>
	bool operator!=(const typename NodeRef<U>::const_node_handle& lhs, const NodeRef<U>& rhs)
	{
		return !(lhs == rhs);
	}


}  // namespace cst::detail::IntrusiveForest



