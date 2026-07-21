// TraversalPolicy.hpp
#pragma once

// Standard library headers
#include <utility>      // exchange, move
#include <type_traits>



/// TraversalPolicy
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  TraversalPolicy - represents a node reference traversal policy
	// =======================================================================
	class TraversalPolicy
	{
		// -- type aliases -------------------------------------------------------
	public:
		using self_type  = TraversalPolicy;

		// -- children traversal -------------------------------------------------
	public:
		struct children_t
		{
			template <typename T> static T prev(T)                                    noexcept;
			template <typename T> static T next(T, [[maybe_unused]] T = nullptr)      noexcept;
			template <typename T> static T begin(T)                                   noexcept;
			template <typename T> static T end(T)                                     noexcept;
			template <typename T> static T parent(T)                                  noexcept;
		};

		// -- preorder traversal -------------------------------------------------
	public:
		struct preorder_t
		{
			template <typename T> static T prev(T)                                    noexcept;
			template <typename T> static T next(T, [[maybe_unused]] T = nullptr)      noexcept;
			template <typename T> static T begin(T)                                   noexcept;
			template <typename T> static T end(T)                                     noexcept;
			template <typename T> static T parent(T)                                  noexcept;
		};

		// -- postorder traversal ------------------------------------------------
	public:
		struct postorder_t
		{
			template <typename T> static T prev(T)                                    noexcept;
			template <typename T> static T next(T, [[maybe_unused]] T = nullptr)      noexcept;
			template <typename T> static T begin(T)                                   noexcept;
			template <typename T> static T end(T)                                     noexcept;
			template <typename T> static T parent(T)                                  noexcept;
		};

	};  // class TraversalPolicy



	/// -- children traversal -------------------------------------------------

	// backward node reference
	template <typename T>
	T TraversalPolicy::children_t::prev(T ref) noexcept
	{
		if (ref.is_sentinel()) {
			return ref.self();  // -> last child
		}
		return ref.prev();  // -> prev sibling
	}  // not applicable for empty container or reverse end sentinel

	// forward node reference
	template <typename T>
	T TraversalPolicy::children_t::next(T ref, T) noexcept
	{
		return ref.next();  // -> next sibling
	}  // not applicable for empty container or any sentinel

	// start node reference
	template <typename T>
	T TraversalPolicy::children_t::begin(T ref) noexcept
	{
		return ref.has_children()
			? ref.first()
			: ref.end();
	}  // not applicable for any sentinel

	// end sentinel reference
	template <typename T>
	T TraversalPolicy::children_t::end(T ref) noexcept
	{
		return ref.end();
	}  // not applicable for any sentinel

	// parent node reference
	template <typename T>
	T TraversalPolicy::children_t::parent(T ref) noexcept
	{
		ref = parent();
		return ref.is_root()
			? ref.end()
			: ref;
	}  // not applicable for any sentinel


	/// -- preorder traversal ------------------------------------------------

	// backward node reference
	template <typename T>
	T TraversalPolicy::preorder_t::prev(T ref) noexcept
	{
		if (ref.is_sentinel()) {
			return ref.self().rightmost_deepest();
		}
		if (ref.has_prev()) {
			return ref.prev().rightmost_deepest();
		}
		return ref.parent();
	}  // not applicable for empty container or reverse end sentinel

	// forward node reference
	template <typename T>
	T TraversalPolicy::preorder_t::next(T ref, T ref_end) noexcept
	{
		if (ref.has_children()) {
			return ref.first();
		}
		for (; !ref.is_root() and ref.end() != ref_end; ref = ref.parent()) {
			if (ref.has_next()) {
				return ref.next();
			}
		}
		return ref.end();
	}  // not applicable for empty container or any sentinel

	// start node reference
	template <typename T>
	T TraversalPolicy::preorder_t::begin(T ref) noexcept
	{
		if (ref.is_root()) {
			return ref.has_children()
				? ref.first()
				: ref.end();
		}
		return ref;
	}  // not applicable for any sentinel

	// end sentinel reference
	template <typename T>
	T TraversalPolicy::preorder_t::end(T ref) noexcept
	{
		return ref.end();
	}  // not applicable for any sentinel

	// parent node reference
	template <typename T>
	T TraversalPolicy::preorder_t::parent(T ref) noexcept
	{
		ref = parent();
		return ref.is_root()
			? ref.end()
			: ref;
	}  // not applicable for any sentinel


	/// -- postorder traversal -----------------------------------------------

	// backward node reference
	template <typename T>
	T TraversalPolicy::postorder_t::prev(T ref) noexcept
	{
		if (ref.is_sentinel()) {
			return ref.self();  // -> last child
		}
		if (ref.has_children()) {
			return ref.last();
		}
		for (; !ref.has_prev(); ref = ref.parent()) {
			assert( !ref.is_root() and "Attempt to decrement begin()");
		};
		return ref.prev();
	}  // not applicable for empty container or reverse end sentinel

	// forward node reference
	template <typename T>
	T TraversalPolicy::postorder_t::next(T ref, T ref_end) noexcept
	{
		if (ref.has_next()) {
			return ref.next().leftmost_deepest();
		}
		ref = ref.parent();
		if (ref.is_root() or ref.end() == ref_end) {
			return ref.end();
		}
		return ref;
	}  // not applicable for empty container or any sentinel

	// start node reference
	template <typename T>
	T TraversalPolicy::postorder_t::begin(T ref) noexcept
	{
		if (ref.is_root()) {
			return ref.has_children()
				? ref.leftmost_deepest()
				: ref.end();
		}
		return ref.leftmost_deepest();
	}  // not applicable for any sentinel

	// end sentinel reference
	template <typename T>
	T TraversalPolicy::postorder_t::end(T ref) noexcept
	{
		return ref.end();
	}  // not applicable for any sentinel

	// parent node reference
	template <typename T>
	T TraversalPolicy::postorder_t::parent(T ref) noexcept
	{
		ref = parent();
		return ref.is_root()
			? ref.end()
			: ref;
	}  // not applicable for any sentinel


}  // namespace cst::detail::IntrusiveForest




/// TraversalProxy
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  TraversalProxy - wraps a node reference
	// =======================================================================
	template <typename TPolicy, typename T>
	class TraversalProxy
	{
		// -- type aliases -------------------------------------------------------
	private:
		using self_type = TraversalProxy;

		// -- members ------------------------------------------------------------
	private:
		T m_ref;

		// -- traversal ----------------------------------------------------------
	public:
		self_type begin()            const noexcept;
		self_type end()              const noexcept;
		self_type next(T = nullptr)  const noexcept;
		self_type prev()             const noexcept;
		self_type parent()           const noexcept;

		// -- accessors ----------------------------------------------------------
	public:
		operator T()                 const noexcept;

		// -- lifecycle ----------------------------------------------------------
	public:
		explicit TraversalProxy(T)   noexcept;

	};  // class TraversalProxy



	/// -- traversal ----------------------------------------------------------

	template <typename U, typename T>
	auto TraversalProxy<U, T>::begin() const noexcept -> self_type
	{
		return self_type{ U::begin(m_ref) };
	}

	template <typename U, typename T>
	auto TraversalProxy<U, T>::end() const noexcept -> self_type
	{
		return self_type{ U::end(m_ref) };
	}

	template <typename U, typename T>
	auto TraversalProxy<U, T>::next(T ref_end) const noexcept -> self_type
	{
		return self_type{ U::next(m_ref, ref_end) }; 
	}

	template <typename U, typename T>
	auto TraversalProxy<U, T>::prev() const noexcept -> self_type
	{
		return self_type{ U::prev(m_ref) };
	}

	template <typename U, typename T>
	auto TraversalProxy<U, T>::parent() const noexcept -> self_type
	{
		return self_type{ U::parent(m_ref) };
	}


	/// -- accessors ----------------------------------------------------------

	template <typename U, typename T>
	TraversalProxy<U, T>::operator T() const noexcept
	{
		return m_ref;
	}


	/// -- lifecycle ----------------------------------------------------------

	template <typename U, typename T>
	TraversalProxy<U, T>::TraversalProxy(T ref) noexcept :
		m_ref(ref)
	{}


}  // namespace cst::detail::IntrusiveForest



