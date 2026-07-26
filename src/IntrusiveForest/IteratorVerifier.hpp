// IteratorVerifier.hpp
#pragma once

// Standard library headers
#include <cassert>
#include <type_traits>



namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  IteratorVerifier - isolated debug validation layer
	// =======================================================================

/// release
#ifdef NDEBUG
	template <typename T, typename U>
	class IteratorVerifier
	{
		// -- type aliases -------------------------------------------------------
	private:
		using self_type       = IteratorVerifier;
		using container_type  = std::remove_cv_t<T>;

		// -- internals ----------------------------------------------------------
	protected:
		void _register_container([[maybe_unused]] const container_type*)        noexcept {}
		auto _get_node_ref()                                              const noexcept { return nullptr; }
		auto _get_container()                                             const noexcept { return nullptr; }

		// -- validation ---------------------------------------------------------
	protected:
		void _check_belongs([[maybe_unused]] const container_type*)       const noexcept {}
		void _check_compatible([[maybe_unused]] const self_type&)         const noexcept {}
		void _check_initialized()                                         const noexcept {}
		void _check_dereference()                                         const noexcept {}
		void _check_increment()                                           const noexcept {}
		void _check_decrement()                                           const noexcept {}
		void _check_insert()                                              const noexcept {}
		void _check_remove()                                              const noexcept {}
		template <typename... B> void _check_conditions(B...)             const noexcept {}

		// -- lifecycle ----------------------------------------------------------
	protected:
		constexpr IteratorVerifier()                                            noexcept {}
		constexpr IteratorVerifier(const self_type&)                            noexcept {}
		constexpr IteratorVerifier(self_type&&)                                 noexcept {}
		self_type& operator=(const self_type&)                                  noexcept { return *this; }
		self_type& operator=(self_type&&)                                       noexcept { return *this; }

	};  // class IteratorVerifier



/// debug
#else
	template <typename TContainer, typename TIterator>
	class IteratorVerifier
	{
		// -- type aliases -------------------------------------------------------
	private:
		using self_type       = IteratorVerifier;
		using container_type  = std::remove_cv_t<TContainer>;

		// -- members ------------------------------------------------------------
	private:
		const container_type* m_container{};

		// -- internals ----------------------------------------------------------
	protected:
		void _register_container(const container_type* container) noexcept
		{
			m_container = container;
		}

		auto _get_node_ref() const noexcept
		{
			return static_cast<const TIterator*>(this)->_base();
		}

		auto _get_container() const noexcept
		{
			return m_container;
		}

		// -- validation ---------------------------------------------------------
	protected:
		void _check_belongs(const container_type* owner) const noexcept
		{
			assert(m_container == owner and "Iterator doesn't belong to this container!");
		}

		void _check_compatible(const self_type& other) const noexcept
		{
			assert(m_container == other.m_container and "Iterators belong to different containers!");
		}

		void _check_initialized() const noexcept
		{
			auto ref = _get_node_ref();
			assert(m_container != nullptr && "Iterator is not bound to a container!");
			assert(ref.valid() && "Attempt to use null policy_iterator!");
		}

		void _check_dereference() const noexcept
		{
			auto ref = _get_node_ref();
			_check_initialized();
			assert(ref.is_real() && "Attempt to dereference sentinel!");
		}

		void _check_increment() const noexcept
		{
			auto ref = _get_node_ref();
			_check_initialized();
			assert(!ref.is_end() && "Attempt to increment end() policy_iterator!");
		}

		void _check_decrement() const noexcept
		{
			auto ref = _get_node_ref();
			_check_initialized();
			assert(!ref.is_first() && "Attempt to decrement first() policy_iterator!");
		}

		void _check_insert() const noexcept
		{
			auto ref = _get_node_ref();
			_check_initialized();
			assert(ref.valid() && "Attempted to insert at invalid position.");
			assert(!(m_container->empty() && ref != m_container->end()._base()) &&
				"Attempted to insert at invalid position.");
		}

		void _check_remove() const noexcept
		{
			auto ref = _get_node_ref();
			_check_dereference();
			assert(!m_container->empty() && "Attempted to remove from empty container.");
		}

		template <typename... Bools
		> void _check_conditions(Bools... conditions) const noexcept
		{
			auto ref = _get_node_ref();
			assert(m_container != nullptr && "Iterator is not bound to a container!");
			assert(ref.valid() && "Attempt to use null policy_iterator!");
			assert((... && conditions) && "Attempted to access invalid element.");
		}

		// -- lifecycle ----------------------------------------------------------
	protected:
		constexpr IteratorVerifier() noexcept = default;

		constexpr IteratorVerifier(const self_type& other) noexcept :
			m_container(other.m_container)
		{}

		constexpr IteratorVerifier(self_type&& other) noexcept :
			m_container(std::exchange(other.m_container, nullptr))
		{}

		self_type& operator=(const self_type& other) noexcept
		{
			m_container = other.m_container;
			return *this;
		}

		self_type& operator=(self_type&& other) noexcept
		{
			m_container = std::exchange(other.m_container, nullptr);
			return *this;
		}

	};  // class IteratorVerifier
#endif

}  // namespace cst::detail::IntrusiveForest



