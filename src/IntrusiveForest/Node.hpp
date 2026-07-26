// Node.hpp
#pragma once

// Standard library headers
#include <cstddef>
#include <utility>
#include <type_traits>  // conditional_t



/// class Node
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  Node - common node representation class
	// =======================================================================
	template <typename TTraits>
	class Node
	{
		// -- forward declarations -----------------------------------------------
	private:
		template <typename T
		> struct Dummy;
		struct SizeField;
		struct ChildCountField;

		// -- internal type aliases ----------------------------------------------
	private:
		using SizeBase        = std::conditional_t<TTraits::size_tag        ::value, SizeField,       Dummy<SizeField>>;
		using ChildCountBase  = std::conditional_t<TTraits::child_count_tag ::value, ChildCountField, Dummy<ChildCountField>>;

		// -- nested classes -----------------------------------------------------
	private:
		template <typename TDerived
		> class LinkNode;
		class DataNode;

		// -- type aliases -------------------------------------------------------
	public:
		using data_node = DataNode;
		using base_node = LinkNode<DataNode>;

	};  // class Node

}  // namespace cst::detail::IntrusiveForest



/// struct Node::Dummy
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  Dummy - dummy structure
	// =======================================================================
	template <typename TTraits>
	template <typename T>
	struct Node<TTraits>::Dummy
	{};

}  // namespace cst::detail::IntrusiveForest



/// struct Node::SizeField
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  SizeField - optional size field
	// =======================================================================
	template <typename TTraits>
	struct Node<TTraits>::SizeField
	{
		typename TTraits::size_type nSize{ 1 };

	};  // struct Node::SizeField

}  // namespace cst::detail::IntrusiveForest



/// struct Node::ChildCountField
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  ChildCountField - optional direct children counter field
	// =======================================================================
	template <typename TTraits>
	struct Node<TTraits>::ChildCountField
	{
		typename TTraits::size_type nChildCount {};

	};  // struct Node::ChildCountField

}  // namespace cst::detail::IntrusiveForest



/// struct Node::LinkNode
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  LinkNode - base node linkage class
	// =======================================================================
	template <typename TTraits>
	template <typename TDerived>
	class Node<TTraits>::LinkNode :
		public SizeBase,
		public ChildCountBase
	{
		// -- type aliases -------------------------------------------------------
	public:
		using self_type  = TDerived;  // mimic the derived type

		// -- data members -------------------------------------------------------
	public:
		self_type* pSelf{ static_cast<self_type*>(this) };
		self_type* pREnd{ pSelf };
		self_type* pEnd{ pSelf };

		self_type** pParent{};
		self_type** pPrev{};
		self_type** pNext{};

		// -- lifecycle ----------------------------------------------------------
	public:
		~LinkNode()                           = default;
		LinkNode()                            = default;
		LinkNode(const LinkNode&)              = delete;
		LinkNode(LinkNode&&)                   = delete;
		LinkNode& operator =(const LinkNode&)  = delete;
		LinkNode& operator =(LinkNode&&)       = delete;

	};  // class Node::LinkNode

}  // namespace cst::detail::IntrusiveForest



/// struct Node::DataNode
namespace cst::detail::IntrusiveForest
{

	// =======================================================================
	//  DataNode - derived class holding the value
	// =======================================================================
	template <typename TTraits>
	class Node<TTraits>::DataNode final :
		public LinkNode<DataNode>
	{
		// -- type aliases -------------------------------------------------------
	public:
		using self_type        = DataNode;
		using base_type        = LinkNode<self_type>;
		using traits_type      = TTraits;
		using value_type       = typename traits_type::value_type;
		using pointer          = typename traits_type::pointer;
		using const_pointer    = typename traits_type::const_pointer;
		using reference        = typename traits_type::reference;
		using const_reference  = typename traits_type::const_reference;

		// -- members ------------------------------------------------------------
	public:
		alignas(value_type) unsigned char data[sizeof(value_type)];

#ifdef _DEBUG
		pointer pData = reinterpret_cast<pointer>(&data);
#endif // _DEBUG

		// -- lifecycle ----------------------------------------------------------
	public:
		~DataNode();
		DataNode(const_reference);
		DataNode(value_type&&) noexcept;
		template <typename... Args
		> DataNode(Args&&...);
		DataNode(const self_type&)               = delete;
		DataNode(self_type&&)                    = delete;
		self_type& operator =(const self_type&)  = delete;
		self_type& operator =(self_type&&)       = delete;

	};  // class DataNode


		
	/// -- lifecycle ----------------------------------------------------------

	// destroy
	template <typename TTraits>
	Node<TTraits>::DataNode::~DataNode()
	{
		reinterpret_cast<reference>(data).~value_type();
	}

	// copy constructor
	template <typename TTraits>
	Node<TTraits>::DataNode::DataNode(const_reference value)
	{
		new(data) value_type(value);
	}

	// move constructor
	template <typename TTraits>
	Node<TTraits>::DataNode::DataNode(value_type&& value) noexcept
	{
		new(data) value_type(std::move(value));
	}

	// emplace
	template <typename TTraits>
	template <typename... Args>
	Node<TTraits>::DataNode::DataNode(Args&&... args)
	{
		new(data) value_type(std::forward<Args>(args)...);
	}


}  // namespace cst::detail::IntrusiveForest



