// DepthFieldMixin.hpp
#pragma once

// Implementation-specific headers
#include "NodeRef.hpp"

// Standard library headers
#include <type_traits>



namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  DepthFieldMixin - inherits TNodeRef, overrides movement methods, stores depth
	// =======================================================================
	template <typename TNodeRef, typename TBool>
    class DepthFieldMixin;



	// =======================================================================
	//  DepthFieldMixin<T, std::true_type> - actual implementation
	// =======================================================================
	template <typename TNodeRef>
    class DepthFieldMixin<TNodeRef, std::true_type> :
		public TNodeRef
    {
    public:
        using typename TNodeRef::size_type;
        using TNodeRef::TNodeRef;

    private:
        size_type m_depth{};

    public:
        DepthFieldMixin(TNodeRef base, size_type depth = {}) noexcept :
            TNodeRef(base),
            m_depth(depth)
        {}

        [[nodiscard]] size_type depth() const noexcept
        {
			assert(m_depth > 0);
            return m_depth - 1;
        }

        [[nodiscard]] DepthFieldMixin first() const noexcept
        {
            return { TNodeRef::first(), m_depth + 1 };
        }

        [[nodiscard]] DepthFieldMixin last() const noexcept
        {
            return { TNodeRef::last(), m_depth + 1 };
        }

        [[nodiscard]] DepthFieldMixin parent() const noexcept
        {
            return { TNodeRef::parent(), m_depth - 1 };
        }

        [[nodiscard]] DepthFieldMixin leftmost_deepest() const noexcept
        {
            DepthFieldMixin cur = *this;
            while (cur.has_children()) {
                cur = cur.first();
            }
            return cur;
        }

        [[nodiscard]] DepthFieldMixin rightmost_deepest() const noexcept
        {
            DepthFieldMixin cur = *this;
            while (cur.has_children()) {
                cur = cur.last();
            }
            return cur;
        }

        [[nodiscard]] DepthFieldMixin next() const noexcept
        {
            return { TNodeRef::next(), m_depth };
        }

        [[nodiscard]] DepthFieldMixin prev() const noexcept
        {
            return { TNodeRef::prev(), m_depth };
        }

        [[nodiscard]] DepthFieldMixin end() const noexcept
        {
            return { TNodeRef::end(), m_depth };
        }

    };  // class DepthFieldMixin<TNodeRef, std::true_type>



	// =======================================================================
	//  DepthFieldMixin<T, std::false_type> - dummy implementation
	// =======================================================================
	template <typename TNodeRef>
    class DepthFieldMixin<TNodeRef, std::false_type> : public TNodeRef
    {
    public:
        using typename TNodeRef::size_type;
        using TNodeRef::TNodeRef;

		DepthFieldMixin(TNodeRef base, size_type = {}) noexcept :
			TNodeRef(base)
		{}

		[[nodiscard]] size_type depth() const noexcept
        {
            return TNodeRef::count_depth() - 1;
        }

    };  // class DepthFieldMixin<TNodeRef, std::false_type>


}  // namespace cst::detail::IntrusiveForest



