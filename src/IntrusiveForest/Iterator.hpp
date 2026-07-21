// Iterator.hpp
#pragma once

// Implementation-specific headers
#include "IteratorVerifier.hpp"
#include "DepthMixin.hpp"

// Standard library headers
#include <iterator>



namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  Iterator - template for traversing the IntrusiveForest
	// =======================================================================
	template <typename TContainer, typename TTPolicy, bool B>
	class Iterator :
		private IteratorVerifier<TContainer, Iterator<TContainer, TTPolicy, B>>
	{
		// -- friend declaration -------------------------------------------------
	private:
		friend class IteratorVerifier<TContainer, Iterator>;
		template <typename, typename, bool> friend class Iterator;
		friend class TContainer::self_type;

		// -- traits type aliases ------------------------------------------------
	public:
		using self_type               = Iterator;
		using container_type          = std::remove_cv_t<typename TContainer::self_type>;
		using traits_type             = typename container_type::traits_type;
		using value_type              = typename traits_type::value_type;
		using reference               = std::conditional_t<B, typename traits_type::const_reference, typename traits_type::reference>;
		using const_reference         = typename traits_type::const_reference;
		using pointer                 = std::conditional_t<B, typename traits_type::const_pointer, typename traits_type::pointer>;
		using const_pointer           = typename traits_type::const_pointer;
		using difference_type         = typename traits_type::difference_type;
		using size_type               = typename traits_type::size_type;
		using iterator_category       = std::bidirectional_iterator_tag;
		using policy_t                = TTPolicy;

		// -- policy type aliases ------------------------------------------------
	private:
		using children_t              = typename container_type::children_t;
		using preorder_t              = typename container_type::preorder_t;
		using postorder_t             = typename container_type::postorder_t;

		// -- iterator types -----------------------------------------------------
	private:
		using iterator                = Iterator<container_type, policy_t, false>;
		using const_iterator          = Iterator<container_type, policy_t, true>;
		using reverse_iterator        = std::reverse_iterator<iterator>;
		using const_reverse_iterator  = std::reverse_iterator<const_iterator>;

		// -- base type aliases --------------------------------------------------
	private:
		using verifier_type           = IteratorVerifier<container_type, self_type>;

		// -- counter type aliases -----------------------------------------------
	private:
		using size_tag                = typename traits_type::size_tag;
		using child_count_tag         = typename traits_type::child_count_tag;
		using depth_tag               = typename traits_type::depth_tag;

		// -- data member --------------------------------------------------------
	private:
		using node_ref_mixin          = DepthFieldMixin<typename container_type::node_ref, depth_tag>;
		node_ref_mixin m_ref;

		// -- private methods ----------------------------------------------------
	private:
		auto _base()                                         const noexcept;
											  
		// -- public operators ---------------------------------------------------
	public:									  
		self_type& operator --()                                   noexcept;
		self_type& operator ++()                                   noexcept;
		self_type  operator --(int)                                noexcept;
		self_type  operator ++(int)                                noexcept;
		self_type& operator -=(size_type)                          noexcept;
		self_type& operator +=(size_type)                          noexcept;
		reference  operator *()                              const noexcept;
		pointer    operator ->()                             const noexcept;

		// -- traversal ----------------------------------------------------------
	public:
		self_type parent()                                   const noexcept;
		self_type prev_sibling()                             const noexcept;
		self_type next_sibling()                             const noexcept;
		self_type prev_preorder()                            const noexcept;
		self_type next_preorder()                            const noexcept;
		self_type prev_postorder()                           const noexcept;
		self_type next_postorder()                           const noexcept;

		// -- basic accessors ----------------------------------------------------
	public:
		bool has_children()                                  const noexcept;
		reference data()                                     const noexcept;

		// -- traits-dependent methods -------------------------------------------
	public:
		size_type size()                                     const noexcept;
		size_type child_count()                              const noexcept;
		size_type depth()                                    const noexcept;

		// -- private constructors -----------------------------------------------
	private:
		template <bool C = B, std::enable_if_t<!C, bool> = 0
		> explicit Iterator(const const_iterator&)                 noexcept;
		explicit Iterator(node_ref_mixin, const container_type*)   noexcept;

		// -- public constructors ------------------------------------------------
	public:
		Iterator(std::nullptr_t = nullptr)                         noexcept;
		Iterator(const self_type&)                                 noexcept;
		Iterator(self_type&&)                                      noexcept;

		// -- public operators ---------------------------------------------------
	public:
		self_type& operator=(std::nullptr_t)                       noexcept;
		self_type& operator=(const self_type&)                     noexcept;
		self_type& operator=(self_type&&)                          noexcept;
		template <bool C = B, typename = std::enable_if_t<!C>
		> operator const_iterator()                                noexcept;

		// -- free operators -----------------------------------------------------
	public:
		template <bool C
		> friend self_type operator -(Iterator<TContainer, TTPolicy, C>, size_type);
		template <bool C
		> friend self_type operator +(Iterator<TContainer, TTPolicy, C>, size_type);
		template <typename T, typename U1, bool C1, typename U2, bool C2
		> friend bool operator ==(const Iterator<T, U1, C1>&, const Iterator<T, U2, C2>&);
		template <typename T, typename U1, bool C1, typename U2, bool C2
		> friend bool operator !=(const Iterator<T, U1, C1>&, const Iterator<T, U2, C2>&);

	};  // class Iterator

}  // namespace cst::detail::IntrusiveForest



namespace cst::detail::IntrusiveForest
{

	/// -- private methods ----------------------------------------------------

	// returns the underlying node reference
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::_base() const noexcept
	{
		return m_ref;
	}


	/// -- public operators ---------------------------------------------------

	// pre-decrement operator
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator --() noexcept -> self_type&
	{
		m_ref = U::prev(m_ref);
		return *this;
	}

	// pre-increment operator
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator ++() noexcept -> self_type&
	{
		m_ref = U::next(m_ref);
		return *this;
	}

	// post-decrement operator
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator --(int) noexcept -> self_type
	{
		self_type captured(*this);
		--*this;
		return captured;
	}

	// post-increment operator
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator ++(int) noexcept -> self_type
	{
		self_type captured(*this);
		++*this;
		return captured;
	}

	// compound assignment for subtraction
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator -=(size_type value) noexcept -> self_type&
	{
		while (value--) {
			--*this;
		}
		return *this;
	}

	// compound assignment for addition
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator +=(size_type value) noexcept -> self_type&
	{
		while (value--) {
			++*this;
		}
		return *this;
	}

	// returns a reference to the stored value
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator *() const noexcept -> reference
	{
		return data();
	}

	// returns a pointer to the stored value
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator ->() const noexcept -> pointer
	{
		return &data();
	}


	/// -- traversal ----------------------------------------------------------

	// returns iterator to the parent node
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::parent() const noexcept -> self_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real(),
			!m_ref.parent().is_root()
		);
		return U::parent(m_ref);
	}

	// returns iterator to the previous sibling node
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::prev_sibling() const noexcept -> self_type
	{
		verifier_type::_check_conditions(
			!m_ref.is_begin(),
			!m_ref.is_rend()
		);
		return m_ref.children().prev();
	}

	// returns iterator to the next sibling node
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::next_sibling() const noexcept -> self_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		return m_ref.children().next();
	}

	// returns iterator to the previous node in pre-order traversal
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::prev_preorder() const noexcept -> self_type
	{
		verifier_type::_check_conditions(
			!(m_ref.is_first() and m_ref.parent().is_root()),
			!m_ref.is_rend()
		);
		return m_ref.preorder().prev();
	}

	// returns iterator to the next node in pre-order traversal
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::next_preorder() const noexcept -> self_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		return m_ref.preorder().next();
	}

	// returns iterator to the previous node in post-order traversal
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::prev_postorder() const noexcept -> self_type
	{
//		verifier_type::_check_conditions(
//			!(m_ref.is_first() and m_ref.parent().is_root()),
//			!m_ref.is_rend()
//		);
		return m_ref.postorder().prev();
	}

	// returns iterator to the next node in post-order traversal
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::next_postorder() const noexcept -> self_type
	{
//		verifier_type::_check_conditions(
//			m_ref.is_real()
//		);
		return m_ref.postorder().next();
	}


	/// -- basic accessors ----------------------------------------------------

	// checks if the node has any children
	template <typename T, typename U, bool B>
	bool Iterator<T, U, B>::has_children() const noexcept
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		return m_ref.has_children();
	}

	// returns a reference to the data
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::data() const noexcept -> reference
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		return m_ref.data();
	}


	/// -- traits-dependent methods -------------------------------------------

	// returns the total number of elements of the node
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::size() const noexcept -> size_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		if constexpr (size_tag::value) {
			return m_ref.size();
		}
		else {
			return m_ref.count_size();
		}
	}

	// returns the number of direct children
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::child_count() const noexcept -> size_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		if constexpr (child_count_tag::value) {
			return m_ref.child_count();
		}
		else {
			return m_ref.count_children();
		}
	}

	// returns the current depth
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::depth() const noexcept -> size_type
	{
		verifier_type::_check_conditions(
			m_ref.is_real()
		);
		return m_ref.depth();
	}


	/// -- private constructors -----------------------------------------------

	// const self -> non-const self (construct, const_cast)
	template <typename T, typename U, bool B>
	template <bool C, std::enable_if_t<!C, bool>>
	Iterator<T, U, B>::Iterator(const const_iterator& other) noexcept :
		m_ref(other.m_ref)
	{
		this->_register_container(other._get_container());
	}

	// const node reference -> non-const self (construct, const_cast)
	template <typename T, typename U, bool B>
	Iterator<T, U, B>::Iterator(node_ref_mixin ref, const container_type* container) noexcept :
		m_ref(ref)
	{
		this->_register_container(container);
	}


	/// -- public constructors ------------------------------------------------

	// nullptr -> self (construct)
	template <typename T, typename U, bool B>
	Iterator<T, U, B>::Iterator(std::nullptr_t) noexcept :
		verifier_type(),
		m_ref(nullptr)
	{}

	// non-const self -> non-const self (construct)
	template <typename T, typename U, bool B>
	Iterator<T, U, B>::Iterator(const self_type& other) noexcept :
		verifier_type(other),
		m_ref(other.m_ref)
	{}

	// const self -> const self (construct)
	template <typename T, typename U, bool B>
	Iterator<T, U, B>::Iterator(self_type&& other) noexcept :
		verifier_type(other),
		m_ref(std::exchange(other.m_ref, nullptr))
	{}


	/// -- public operators ---------------------------------------------------

	// nullptr -> self (assign)
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator=(nullptr_t) noexcept -> self_type&
	{
		verifier_type::operator=();
		m_ref = nullptr;
		return *this;
	}


	// non-const self -> non-const self (assign)
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator=(const self_type& other) noexcept -> self_type&
	{
		verifier_type::operator=(other);
		m_ref = other.m_ref;
		return *this;
	}

	// const self -> const self (assign)
	template <typename T, typename U, bool B>
	auto Iterator<T, U, B>::operator=(self_type&& other) noexcept -> self_type&
	{
		verifier_type::operator=(other);
		m_ref = other.m_ref;
		return *this;
	}


	// non-const self -> const self (conversion)
	template <typename T, typename U, bool B>
	template <bool C, typename>
	Iterator<T, U, B>::operator const_iterator() noexcept
	{
		return const_iterator{ m_ref };
	}


	// -- free operators -------------------------------------------------------

	// subtraction operator (returns a new iterator)
	template <typename T, typename U, bool B>
	auto operator -(Iterator<T, U, B> lhs, typename Iterator<T, U, B>::size_type value) -> Iterator<T, U, B>
	{
		return lhs -= value;
	}

	// addition operator (returns a new iterator)
	template <typename T, typename U, bool B>
	auto operator +(Iterator<T, U, B> lhs, typename Iterator<T, U, B>::size_type value) -> Iterator<T, U, B>
	{
		return lhs += value;
	}

	// equality operator for iterators
	template < typename T, typename U1, bool C1, typename U2, bool C2 >
	bool operator ==(const Iterator<T, U1, C1>& lhs, const Iterator<T, U2, C2>& rhs)
	{
		lhs._check_compatible(rhs);
		return lhs.m_ref == rhs.m_ref;
	}

	// inequality operator for iterators
	template < typename T, typename U1, bool C1, typename U2, bool C2 >
	bool operator !=(const Iterator<T, U1, C1>& lhs, const Iterator<T, U2, C2>& rhs)
	{
		return !(lhs == rhs);
	}


}  // namespace cst::detail::IntrusiveForest



