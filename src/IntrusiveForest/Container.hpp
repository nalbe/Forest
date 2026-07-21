// Container.hpp
#pragma once

// Implementation-specific headers
#include "Node.hpp"
#include "NodeRef.hpp"
#include "Iterator.hpp"

// Standard library headers
#include <utility>  // move



namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  Container - intrusive forest container
	// =======================================================================
	template <typename T, typename TTraits>
	class Container
	{
		// -- friend declaration -------------------------------------------------
	private:
		template <typename, typename, bool> friend class Iterator;

		// -- traits type aliases ------------------------------------------------
	public:
		using self_type             = Container;
		using traits_type           = TTraits;
		using value_type            = typename traits_type::value_type;
		using pointer               = typename traits_type::pointer;
		using const_pointer         = typename traits_type::const_pointer;
		using reference             = typename traits_type::reference;
		using const_reference       = typename traits_type::const_reference;
		using difference_type       = typename traits_type::difference_type;
		using size_type             = typename traits_type::size_type;

		// -- counter type aliases -----------------------------------------------
	private:
		using size_tag              = typename traits_type::size_tag;
		using child_count_tag       = typename traits_type::child_count_tag;
		using depth_tag             = typename traits_type::depth_tag;

		// -- node type aliases --------------------------------------------------
	private:
		using node_base             = typename Node<traits_type>::base_node;
		using node_type             = typename Node<traits_type>::data_node;
		using mutable_node_pointer  = node_type*;
		using const_node_pointer    = const node_type*;
		using node_ref              = NodeRef<node_type>;
		using const_node_ref        = NodeRef<const node_type>;
		using node_ref_mixin        = DepthFieldMixin<node_ref, depth_tag>;
		using const_node_ref_mixin  = DepthFieldMixin<const_node_ref, depth_tag>;

		// -- traverse policy aliases --------------------------------------------
	public:
		using children_t            = typename TraversalPolicy::children_t;
		using preorder_t            = typename TraversalPolicy::preorder_t;
		using postorder_t           = typename TraversalPolicy::postorder_t;

		// -- generic iterator type aliases --------------------------------------
	private:
		template <typename U, bool C> using generic_iterator       = Iterator<self_type, U, C>;
		template <typename U> using const_policy_iterator          = generic_iterator<U, true>;
		template <typename U> using policy_iterator                = generic_iterator<U, false>;
		template <typename U> using reverse_policy_iterator        = std::reverse_iterator<policy_iterator<U>>;
		template <typename U> using const_reverse_policy_iterator  = std::reverse_iterator<const_policy_iterator<U>>;

		// -- sibling iterator type aliases --------------------------------------
	public:
		using const_children_iterator           = const_policy_iterator<children_t>;
		using children_iterator                 = policy_iterator<children_t>;
		using const_reverse_children_iterator   = std::reverse_iterator<const_children_iterator>;
		using reverse_children_iterator         = std::reverse_iterator<children_iterator>;

		// -- preorder iterator type aliases -------------------------------------
	public:
		using const_preorder_iterator           = const_policy_iterator<preorder_t>;
		using preorder_iterator                 = policy_iterator<preorder_t>;
		using const_reverse_preorder_iterator   = std::reverse_iterator<const_preorder_iterator>;
		using reverse_preorder_iterator         = std::reverse_iterator<preorder_iterator>;

		// -- postorder iterator type aliases ------------------------------------
	public:
		using const_postorder_iterator          = const_policy_iterator<postorder_t>;
		using postorder_iterator                = policy_iterator<postorder_t>;
		using const_reverse_postorder_iterator  = std::reverse_iterator<const_postorder_iterator>;
		using reverse_postorder_iterator        = std::reverse_iterator<postorder_iterator>;

		// -- stl convenience iterator type aliases ------------------------------
	public:
		using const_iterator                    = const_preorder_iterator;
		using iterator                          = preorder_iterator;
		using const_reverse_iterator            = const_reverse_preorder_iterator;
		using reverse_iterator                  = reverse_preorder_iterator;

		// -- data member --------------------------------------------------------
	private:
		node_base m_base{};

		// -- private methods ----------------------------------------------------
	private:
		template <typename TPolicy
		> TraversalProxy<TPolicy, node_ref_mixin> root()           const noexcept;
		node_ref_mixin root()                                      const noexcept;

		// -- private constructors -----------------------------------------------
	private:
		explicit Container(node_ref_mixin)                               noexcept;

		// -- public constructors ------------------------------------------------
	public:
		~Container()                                                     noexcept;
		Container(self_type&&)                                           noexcept;
		Container(const self_type&);
		Container()                                                     = default;
		explicit Container(const_reference);
		explicit Container(value_type&&);
		explicit Container(std::initializer_list<value_type>);
		Container(std::initializer_list<self_type>);
		Container(const_reference, std::initializer_list<self_type>);

		// -- public operators ---------------------------------------------------
	public:
		self_type& operator =(self_type&&)                               noexcept;
		self_type& operator =(const self_type&);
		self_type& operator =(const_reference);
		self_type& operator =(value_type&&);
		self_type& operator =(std::initializer_list<value_type>);
		template <typename T, typename V> friend bool operator ==(const Container<T, V>&, const Container<T, V>&);
		template <typename T, typename V> friend bool operator !=(const Container<T, V>&, const Container<T, V>&);

		// -- basic operations ---------------------------------------------------
	public:
		void      clear()                                                noexcept;
		size_type size()                                           const noexcept;
		bool      empty()                                          const noexcept;
		template <typename TPolicy = preorder_t, typename TStream
		> TStream& print(TStream&)                                 const;

		// -- element insertion --------------------------------------------------
	public:
		template <typename U> policy_iterator<U> push_back(const_reference);
		template <typename U> policy_iterator<U> push_back(value_type&&);
		template <typename U> policy_iterator<U> insert(const_policy_iterator<U>, const_reference);
		template <typename U> policy_iterator<U> insert(const_policy_iterator<U>, value_type&&);
		template <typename U> policy_iterator<U> insert(const_policy_iterator<U>, std::initializer_list<value_type>);
		template <typename U, typename... Args> policy_iterator<U> emplace(const_policy_iterator<U>, Args&&...);

		// -- non-modifiers ------------------------------------------------------
	public:
		template <typename U> policy_iterator<U> copy_single(const_policy_iterator<U> where, const_policy_iterator<U>)  const;
		template <typename U> policy_iterator<U> copy(const_policy_iterator<U> where, const_policy_iterator<U>)         const;
		template <typename U, typename Pred = std::equal_to<>
		> bool compare(const_policy_iterator<U>, const_policy_iterator<U>, Pred&& = {})                                 const noexcept
			(noexcept(std::declval<Pred>()(std::declval<const T&>(), std::declval<const T&>())));

		// -- modifiers ----------------------------------------------------------
	public:
		template <typename U> policy_iterator<U> join(const_policy_iterator<U>, self_type)                       noexcept;
		template <typename U> policy_iterator<U> join(const_policy_iterator<U>, self_type&&)                     noexcept;
		template <typename U> self_type unjoin(const_policy_iterator<U>)                                         noexcept;
		template <typename U, bool С> void swap(const_policy_iterator<U>, const_policy_iterator<U>)              noexcept;
		template <typename U> policy_iterator<U> remove(policy_iterator<U>)                                      noexcept;
		template <typename U> size_type remove(policy_iterator<U>, const_reference)                              noexcept;
		template <typename U> policy_iterator<U> move(const_policy_iterator<U> where, const_policy_iterator<U>)  noexcept;
		template <typename U, typename Pred> size_type remove_if(const_policy_iterator<U>, Pred&&)
			noexcept(noexcept(std::declval<Pred>()(std::declval<const T&>())));

		// -- constant iterators -----------------------------------------------
	public:
		template <typename U = preorder_t> const_policy_iterator<U> cend()              const noexcept;
		template <typename U = preorder_t> const_policy_iterator<U> cbegin()            const noexcept;
		template <typename U = preorder_t> const_policy_iterator<U> end()               const noexcept;
		template <typename U = preorder_t> const_policy_iterator<U> begin()             const noexcept;

		// -- mutable iterators ------------------------------------------------
	public:
		template <typename U = preorder_t> policy_iterator<U> end()                           noexcept;
		template <typename U = preorder_t> policy_iterator<U> begin()                         noexcept;

		// -- constant reverse iterators ---------------------------------------
	public:
		template <typename U = preorder_t> const_reverse_policy_iterator<U> crend()     const noexcept;
		template <typename U = preorder_t> const_reverse_policy_iterator<U> crbegin()   const noexcept;
		template <typename U = preorder_t> const_reverse_policy_iterator<U> rend()      const noexcept;
		template <typename U = preorder_t> const_reverse_policy_iterator<U> rbegin()    const noexcept;

		// -- mutable reverse iterators ----------------------------------------
	public:
		template <typename U = preorder_t> reverse_policy_iterator<U> rbegin()                noexcept;
		template <typename U = preorder_t> reverse_policy_iterator<U> rend()                  noexcept;

	};  // class Container



	/// -- private methods ----------------------------------------------------

	// returns mutable reference to the root (const_cast)
	template <typename T, typename V>
	auto Container<T, V>::root() const noexcept -> node_ref_mixin
	{
		return node_ref_mixin{
			static_cast<node_type*>( const_cast<node_base*>(&m_base) )
		};
	}

	// returns the root of the tree
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::root() const noexcept -> TraversalProxy<U, node_ref_mixin>
	{
		return TraversalProxy<U, node_ref_mixin>{ root() };
	}


	/// -- private constructors -----------------------------------------------

	// сonstructor from mutable node reference
	template <typename T, typename V>
	Container<T, V>::Container(node_ref_mixin ref) noexcept
	{
		ref.link(
			root<children_t>().end()
		);
	}


	// -- public constructors -----------------------------------------------

	// clears the entire tree
	template <typename T, typename V>
	Container<T, V>::~Container() noexcept
	{
		clear();
	}

	// transfers ownership of the other tree's resources
	template <typename T, typename V>
	Container<T, V>::Container(self_type&& other) noexcept
	{
		*this = std::move(other);
	}

	// copy of the other tree
	template <typename T, typename V>
	Container<T, V>::Container(const self_type& other)
	{
		*this = other;
	}

	// сonstructor from const reference
	template <typename T, typename V>
	Container<T, V>::Container(const_reference value)
	{
		*this = value;
	}

	// constructor from rvalue type
	template <typename T, typename V>
	Container<T, V>::Container(value_type&& value)
	{
		*this = std::move(value);
	}

	// constructor from initializer list of values
	template <typename T, typename V>
	Container<T, V>::Container(std::initializer_list<value_type> init)
	{
		*this = init;
	}

	// constructor from initializer list of trees
	template <typename T, typename V>
	Container<T, V>::Container(std::initializer_list<self_type> forest)
	{
		for (auto& it : forest) {
			it.root().copy_subtree(
				root<preorder_t>().end()
			);
		}
	}

	// constructor from value and initializer list of trees
	template <typename T, typename V>
	Container<T, V>::Container(const_reference value, std::initializer_list<self_type> forest) :
		Container(value)
	{
		for (auto& it : forest) {
			it.root().copy_subtree(
				root<children_t>().begin().end()
			);
		}
	}


	// -- public operators ---------------------------------------------------

	// clears the current container and transfers ownership from the other
	template <typename T, typename V>
	auto Container<T, V>::operator =(self_type&& other) noexcept -> self_type&
	{
		if (this == &other) {
			return *this;
		}
		clear();
		other.root().move_subtree(
			root<children_t>().end()
		);
		return *this;
	}

	// deep copy the other tree
	template <typename T, typename V>
	auto Container<T, V>::operator =(const self_type& other) -> self_type&
	{
		if (this == &other) {
			return *this;
		}
		clear();
		if (other.empty()) {
			return *this;
		}
		other.root().copy_subtree(
			root<children_t>().end()
		);
		return *this;
	}

	// assigment-copy operator from a value
	template <typename T, typename V>
	auto Container<T, V>::operator =(const_reference value) -> self_type&
	{
		node_ref{ new node_type(value) }
			.link( root<children_t>().end() );
		return *this;
	}

	// assigment-move operator from a value
	template <typename T, typename V>
	auto Container<T, V>::operator =(value_type&& value) -> self_type&
	{
		node_ref{ new node_type(std::move(value)) }
			.link( root<children_t>().end() );
		return *this;
	}

	// assignment operator from an initializer list of values
	template <typename T, typename V>
	auto Container<T, V>::operator =(std::initializer_list<value_type> init) -> self_type&
	{
		clear();
		if (init.size() == 0) {
			return *this;
		}
		for (auto it{ std::begin(init) }; it != std::end(init); ++it) {
			node_ref{ new node_type(*it) }
				.link( root<children_t>().end() );
		}
		return *this;
	}

	// equality operator
	template <typename T, typename V>
	bool operator ==(const Container<T, V>& lhs, const Container<T, V>& rhs)
	{
		if (&lhs == &rhs) {
			return true;
		}
		if (lhs.empty() or rhs.empty()) {
			return lhs.empty() and rhs.empty();
		}
		return lhs.root().is_subtree_equal(
			rhs.root(), std::equal_to<>()
		);
	}

	// inequality operator
	template <typename T, typename V>
	bool operator !=(const Container<T, V>& lhs, const Container<T, V>& rhs)
	{
		return !(lhs == rhs);
	}


	/// -- basic operations -------------------------------------------------

	// clears the entire container
	template <typename T, typename V>
	void Container<T, V>::clear() noexcept
	{
		if (!empty()) {
			root().remove_subtree();
		}
	}

	// returns the total number of nodes in the container
	template <typename T, typename V>
	auto Container<T, V>::size() const noexcept -> size_type
	{
		return root().size() - 1;
	}

	// checks if the container is empty
	template <typename T, typename V>
	bool Container<T, V>::empty() const noexcept
	{
		return !root().has_children();
	}

	template <typename T, typename V>
	template <typename U, typename TStream>
	TStream& Container<T, V>::print(TStream& os) const
	{
		for (auto it = begin<U>(); it != end<U>(); ++it) {
			const auto depth = it.depth();
			if (depth > 0) {
				for (size_type i{}; i + 1 < depth; ++i) {
					os << "        ";
				}
				os << "|------ ";
			}
#ifdef _DEBUG
			os << "[" << depth << "] ";
#endif
			os << *it << "\n";
		}
#ifdef _DEBUG
		os << "size: " << size() << "\n";
#endif
		return os;
	}


	/// -- element insertion ------------------------------------------------

	// inserts a single value by copy before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::push_back(const_reference value) -> policy_iterator<U>
	{
		return insert<U>(end<U>(), value);
	}

	// inserts a single value by move before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::push_back(value_type&& value) -> policy_iterator<U>
	{
		return insert<U>(end<U>(), std::move(value));
	}

	// inserts a single value by copy before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::insert(const_policy_iterator<U> where, const_reference value) -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		node_ref_mixin mixin{
			node_ref{ new node_type(value) }.link( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// inserts a single value by move before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::insert(const_policy_iterator<U> where, value_type&& value) -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		node_ref_mixin mixin{
			node_ref{ new node_type(std::move(value)) }.link( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// inserts a range of values before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::insert(const_policy_iterator<U> where, std::initializer_list<value_type> init) -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		if (init.size() == 0) {
			return where;
		}
		node_ref ref = where._base();
		for (auto it{ std::rbegin(init) }; it != std::rend(init); ++it) {
			ref = node_ref{ new node_type(*it) }.link(ref);
		}
		node_ref_mixin mixin{ ref, where.depth() };
		return policy_iterator<U>(mixin, this);
	}

	// constructs an element in place before the indicated position
	template <typename T, typename V>
	template <typename U, typename... Args>
	auto Container<T, V>::emplace(const_policy_iterator<U> where, Args&&... args) -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		node_ref_mixin mixin{
			node_ref{ new node_type(std::forward<Args>(args)...) }.link(where._base()),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}


	/// -- non-modifiers ----------------------------------------------------

	// copy a single node and insert before the indicated position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::copy_single(const_policy_iterator<U> where, const_policy_iterator<U> it) const -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		it._check_dereference();
		node_ref_mixin mixin{
			it._base().copy_single().link( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// deep copy a single node and insert it before the indicating position
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::copy(const_policy_iterator<U> where, const_policy_iterator<U> it) const -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		it._check_dereference();
		node_ref_mixin mixin{
			it._base().copy().link( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// compares two nodes (and their respective subtrees)
	template <typename T, typename V>
	template <typename U, typename Pred>
	bool Container<T, V>::compare(const_policy_iterator<U> first, const_policy_iterator<U> second, Pred&& equal)
		const noexcept(noexcept(std::declval<Pred>()(std::declval<const T&>(), std::declval<const T&>())))
	{
		first._check_belongs(this);
		first._check_compatible(second);
		first._check_dereference();
		second._check_dereference();
		return first._base()
			.is_equal( second._base(), std::forward<Pred>(equal) );
	}


	/// -- modifiers --------------------------------------------------------
	
	// join-copy the contents of other container into this
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::join(const_policy_iterator<U> where, self_type other) noexcept -> policy_iterator<U>
	{
		return join( where, std::move(other) );
	}

	// join-move the contents of other container into this
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::join(const_policy_iterator<U> where, self_type&& other) noexcept -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		if (this == &other) {
			return policy_iterator<U>{ where };
		}
		if (other.empty()) {
			return policy_iterator<U>{ where };
		}
		node_ref_mixin mixin{
			other.root().move_subtree( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// unjoins the subtree rooted at the indicated node
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::unjoin(const_policy_iterator<U> it) noexcept -> self_type
	{
		it._check_belongs(this);
		it._check_remove();
		return self_type{
			it._base().unlink()
		};
	}

	// swaps the contents of two containers
	template <typename T, typename V>
	template <typename U, bool С>
	void Container<T, V>::swap(const_policy_iterator<U> first, const_policy_iterator<U> second) noexcept
	{
		first._check_belongs(this);
		first._check_compatible(second);
		first._check_remove();
		second._check_remove();
		first._base().swap( second._base() );
	}

	// removes the indicated node
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::remove(policy_iterator<U> it) noexcept -> policy_iterator<U>
	{
		it._check_belongs(this);
		it._check_remove();
		node_ref_mixin mixin{
			it._base().remove(),
			it.depth()
		};
		return policy_iterator<U>(mixin, this);
	}

	// removes elements from the container based on a value
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::remove(policy_iterator<U> it, const_reference value) noexcept -> size_type
	{
		it._check_belongs(this);
		it._check_remove();
		return it._base().remove_if(
			[&value](const_reference stored_value) {
				return value == stored_value;
			}
		);
	}

	// remove elements from the container based on provided predicate
	template <typename T, typename V>
	template <typename U, typename Pred>
	auto Container<T, V>::remove_if(const_policy_iterator<U> it, Pred&& pred)
		noexcept(noexcept(std::declval<Pred>()(std::declval<const T&>()))) -> size_type
	{
		it._check_belongs(this);
		it._check_remove();
		return it._base().remove_if(
			std::forward<Pred>(pred)
		);
	}

	// moves a single node to a new position within the containers of same type
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::move(const_policy_iterator<U> where, const_policy_iterator<U> it) noexcept -> policy_iterator<U>
	{
		where._check_belongs(this);
		where._check_insert();
		it._check_dereference();
		node_ref_mixin mixin{
			it._base().move( where._base() ),
			where.depth()
		};
		return policy_iterator<U>(mixin, this);
	}


	/// -- constant iterators -----------------------------------------------

	// returns a constant iterator to the end sentinel for traversal order
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::cend() const noexcept -> const_policy_iterator<U>
	{
		return const_policy_iterator<U>{ U::end( root() ), this };
	}

	// returns a constant iterator to the first element in the given traversal order
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::cbegin() const noexcept -> const_policy_iterator<U>
	{
		return const_policy_iterator<U>{ U::begin( root() ), this };
	}

	// returns a constant iterator to the end sentinel
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::end() const noexcept -> const_policy_iterator<U>
	{
		return cend<U>();
	}

	// returns a constant iterator to the first element
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::begin() const noexcept -> const_policy_iterator<U>
	{
		return cbegin<U>();
	}


	/// -- mutable iterators ------------------------------------------------

	// returns a mutable iterator to the end sentinel
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::end() noexcept -> policy_iterator<U>
	{
		return policy_iterator<U>{
			static_cast<const self_type*>(this)->cend<U>()
		};
	}

	// returns a mutable iterator to the first element
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::begin() noexcept -> policy_iterator<U>
	{
		return policy_iterator<U>{
			static_cast<const self_type*>(this)->cbegin<U>()
		};
	}


	/// -- constant reverse iterators ---------------------------------------

	// returns a constant reverse iterator to the end sentinel of the reversed traversal
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::crend() const noexcept -> const_reverse_policy_iterator<U>
	{
		return const_reverse_policy_iterator<U>{ cbegin<U>() };
	}

	// returns a constant reverse iterator to the first element of the reversed traversal
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::crbegin() const noexcept -> const_reverse_policy_iterator<U>
	{
		return const_reverse_policy_iterator<U>{ cend<U>() };
	}

	// returns a constant reverse iterator to the reversed end
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::rend() const noexcept -> const_reverse_policy_iterator<U>
	{
		return const_reverse_policy_iterator<U>{ begin<U>() };
	}

	// returns a constant reverse iterator to the reversed beginning
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::rbegin() const noexcept -> const_reverse_policy_iterator<U>
	{
		return const_reverse_policy_iterator<U>{ end<U>() };
	}


	/// -- mutable reverse iterators ----------------------------------------

	// returns a mutable reverse iterator to the first element of the reversed travers
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::rbegin() noexcept -> reverse_policy_iterator<U>
	{
		return reverse_policy_iterator<U>{ end<U>() };
	}

	// returns a mutable reverse iterator to the end sentinel of the reversed traversal
	template <typename T, typename V>
	template <typename U>
	auto Container<T, V>::rend() noexcept -> reverse_policy_iterator<U>
	{
		return reverse_policy_iterator<U>{ begin<U>() };
	}


}  // namespace cst::detail::IntrusiveForest



