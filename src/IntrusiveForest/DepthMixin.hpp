// DepthMixin.hpp
#pragma once

// Implementation-specific headers
#include "NodeRef.hpp"

// Standard library headers
#include <type_traits>



/// DepthMixin
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  DepthMixin - inherits TNodeRef, overrides movement methods, stores depth
	// =======================================================================
	template <typename TNodeRef, typename TBool>
	class DepthMixin;

}  // namespace cst::detail::IntrusiveForest



/// DepthMixin<T, std::true_type>
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  DepthMixin<T, std::true_type> - actual implementation
	// =======================================================================
	template <typename TNodeRef>
	class DepthMixin<TNodeRef, std::true_type> :
		public TNodeRef
	{
		// -- type aliases -------------------------------------------------------
	public:
		using base_type = TNodeRef;
		using typename base_type::size_type;

		// -- members ------------------------------------------------------------
	private:
		size_type m_depth{};

		// -- non-modifiers ------------------------------------------------------
	public:
		[[nodiscard]] size_type  depth()                  const noexcept;
		[[nodiscard]] DepthMixin first()                  const noexcept;
		[[nodiscard]] DepthMixin last()                   const noexcept;
		[[nodiscard]] DepthMixin parent()                 const noexcept;
		[[nodiscard]] DepthMixin leftmost_deepest()       const noexcept;
		[[nodiscard]] DepthMixin rightmost_deepest()      const noexcept;
		[[nodiscard]] DepthMixin next()                   const noexcept;
		[[nodiscard]] DepthMixin prev()                   const noexcept;
		[[nodiscard]] DepthMixin end()                    const noexcept;

		// -- lifecycle ----------------------------------------------------------
	public:
		using base_type::base_type;
		DepthMixin(base_type, size_type = {})                   noexcept;

	};  // class DepthMixin<TNodeRef, std::true_type>



	/// -- non-modifiers ------------------------------------------------------

	// depth
	template <typename T>
	auto DepthMixin<T, std::true_type>::depth() const noexcept -> size_type
	{
		assert(m_depth > 0);
		return m_depth;
	}

	// first child
	template <typename T>
	auto DepthMixin<T, std::true_type>::first() const noexcept -> DepthMixin
	{
		return { base_type::first(), m_depth + 1 };
	}

	// last child
	template <typename T>
	auto DepthMixin<T, std::true_type>::last() const noexcept -> DepthMixin
	{
		return { base_type::last(), m_depth + 1 };
	}

	// parent
	template <typename T>
	auto DepthMixin<T, std::true_type>::parent() const noexcept -> DepthMixin
	{
		return { base_type::parent(), m_depth - 1 };
	}

	// leftmost deepest child
	template <typename T>
	auto DepthMixin<T, std::true_type>::leftmost_deepest() const noexcept -> DepthMixin
	{
		DepthMixin cur = *this;
		while (cur.has_children()) {
			cur = cur.first();
		}
		return cur;
	}

	// rightmost deepest child
	template <typename T>
	auto DepthMixin<T, std::true_type>::rightmost_deepest() const noexcept -> DepthMixin
	{
		DepthMixin cur = *this;
		while (cur.has_children()) {
			cur = cur.last();
		}
		return cur;
	}

	// next sibling
	template <typename T>
	auto DepthMixin<T, std::true_type>::next() const noexcept -> DepthMixin
	{
		return { base_type::next(), m_depth };
	}

	// previous sibling
	template <typename T>
	auto DepthMixin<T, std::true_type>::prev() const noexcept -> DepthMixin
	{
		return { base_type::prev(), m_depth };
	}

	// end
	template <typename T>
	auto DepthMixin<T, std::true_type>::end() const noexcept -> DepthMixin
	{
		return { base_type::end(), m_depth + 1 };
	}


	/// -- lifecycle ----------------------------------------------------------

	// construct from base and depth
	template <typename T>
	DepthMixin<T, std::true_type>::DepthMixin(base_type base, size_type depth) noexcept :
		base_type(base),
		m_depth(depth)
	{}


}  // namespace cst::detail::IntrusiveForest



/// DepthMixin<T, std::false_type>
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  DepthMixin<T, std::false_type> - dummy implementation
	// =======================================================================
	template <typename TNodeRef>
    class DepthMixin<TNodeRef, std::false_type> :
		public TNodeRef
    {
		// -- type aliases -------------------------------------------------------
	public:
		using base_type  = TNodeRef;
		using typename base_type::size_type;

		// -- non-modifiers ------------------------------------------------------
	public:
		[[nodiscard]] size_type  depth()                  const noexcept;
		[[nodiscard]] DepthMixin first()                  const noexcept;
		[[nodiscard]] DepthMixin last()                   const noexcept;

 		// -- lifecycle ----------------------------------------------------------
	public:
        using base_type::base_type;
		DepthMixin(base_type, [[maybe_unused]] size_type = {}) noexcept;

   };  // class DepthMixin<TNodeRef, std::false_type>



	/// -- non-modifiers ------------------------------------------------------

	// current depth of node
	template <typename T>
	auto DepthMixin<T, std::false_type>::depth() const noexcept -> size_type
	{
		return 0;
	}

	// first child
	template <typename T>
	auto DepthMixin<T, std::false_type>::first() const noexcept -> DepthMixin
	{
		return base_type::first();
	}

	// last child
	template <typename T>
	auto DepthMixin<T, std::false_type>::last() const noexcept -> DepthMixin
	{
		return base_type::last();
	}


	/// -- lifecycle ----------------------------------------------------------

	// construct from base
	template <typename T>
	DepthMixin<T, std::false_type>::DepthMixin(base_type base, size_type) noexcept :
		base_type(base)
	{}


}  // namespace cst::detail::IntrusiveForest



