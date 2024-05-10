#pragma once

#include <concepts>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <limits>
#undef max

namespace VT::DependencyGraphImpl
{
	template<typename T>
		requires std::movable<T>
	class DependencyGraphNode;

	/**
	 * storage wrapper for DependencyGraph by type
	 * @tparam T type of node
	 */
	template<typename T>
	struct DependencyGraphNode_T_List
	{
		std::unordered_map<std::string, DependencyGraphNode<T>> T_Map;
		uint32_t MinRefCount{ 0 };
	};

	/**
	 * virtual base class with all necessary DependencyNode methods
	 */
	struct DependencyGraphNodeBase
	{
		DependencyGraphNodeBase(uint32_t& ContainerCounter) : HeaderRef{ ContainerCounter } {}

		/**
		 * add dependency to obj
		 * @return if succesfully added
		 */
		virtual bool addDependency(DependencyGraphNodeBase*) = 0;
		/**
		 * remove dependency from obj
		 * @return if successfully removed
		 */
		virtual bool removeDependency(DependencyGraphNodeBase*) = 0;

		/**
		 * decrease ref count
		 */
		void decreaseRefCount()
		{
			RefCount--;
			if (HeaderRef > RefCount) { HeaderRef = RefCount; }
		}

		virtual ~DependencyGraphNodeBase() = default;

		// number of times referenced by other objects
		uint32_t RefCount{ 0 };

		// reference to head min counter
		uint32_t& HeaderRef;
	};

	/**
	 * DependencyGraphNode by type
	 * @tparam T Type of data the Node is holding
	 */
	template<typename T>
		requires std::movable<T>
	class DependencyGraphNode : public DependencyGraphNodeBase
	{
	public:
		explicit DependencyGraphNode(T&& Obj, uint32_t& ContainerCounter, std::function<void(T&)> Destructor) :
			DependencyGraphNodeBase{ ContainerCounter },
			Delete{ std::move(Destructor) },
			Item{ std::move(Obj) }{}

		/**
		 * create dependency
		 * @return true if successfully added
		 */
		bool addDependency(DependencyGraphNodeBase* Dependency) override
		{
			// trying to add self
			if (Dependency == this) { return false; }

			if (DependencyList.insert(Dependency).second)
			{
				Dependency->RefCount++;
				return true;
			}
			return false;
		}
		/**
		 * remove dependency relation
		 * @return true if removed, false if it doesn't exist (meaning it wasn't a relation to begin with)
		 */
		bool removeDependency(DependencyGraphNodeBase* Dependency) override
		{
			return DependencyList.erase(Dependency) == 1;
		}

		auto getDependencyList() const -> const std::unordered_set<DependencyGraphNodeBase*>&
		{
			return DependencyList;
		}

		T& getItem()
		{
			return Item;
		}

		~DependencyGraphNode() override
		{
			Delete(Item);
			for (auto i : DependencyList)
			{
				i->decreaseRefCount();
			}
		}

	private:
		std::unordered_set<DependencyGraphNodeBase*> DependencyList;
		std::function<void(T&)> Delete;
		T Item;
	};
}

namespace VT
{

	/**
	 * Dependency graph
	 * @cond Type and/or Name must differ, both cannot be the same
	 * 	 *	T "a", T "b" -- legal
	 *	 *	T "a", U "a" -- legal
	 *	 *	T "a", T "a" -- illegal
	 * @tparam T types of obj stored in the dependency graph
	 */
	template<typename ...T>
		requires (std::movable<T> && ...)
	class DependencyGraph
	{
	public:
		/**
		 * insert a new node into graph
		 * @tparam U : target node type
		 * @param NodeName : name
		 * @param Destructor : destructor
		 * @return : pair{ unordered_map<std::string, Target>::const_iterator, bool }
		 */
		template<typename Target>
			requires std::movable<Target>
		std::pair<Target&, bool> insert(Target&& Obj, std::string NodeName, std::function<void(Target&)> Destructor = [](void(Target&)){})
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;

			auto& Head{ std::get<TargetType>(m_Nodes) };

			auto Result = Head.T_Map.try_emplace(std::move(NodeName), std::forward<Target>(Obj), Head.MinRefCount, std::forward<std::function<void(Target&)>>(Destructor));

			return { Result.first->second.getItem(), Result.second };
		}

		/**
		 * add dependency to existing node in graph of U NodeName
		 * @tparam U : target node
		 * @tparam V : dependency
		 * @param NodeName : Name of self
		 * @param Destructor : how to destroy self
		 * @param Dependency : name of dependency
		 * @exception : runtime_exception
		 * @return : if succesfully added dependency
		 */
		template<typename Target, typename Dependent>
		bool addDependency(const std::string& NodeName, const std::string& Dependency)
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;
			using DependentType = DependencyGraphImpl::DependencyGraphNode_T_List<Dependent>;

			auto& DependencyList{ std::get<DependentType>(m_Nodes).T_Map };
			auto DependencyPtr{ DependencyList.find(Dependency) };
			// dependency does not exist
			if (DependencyPtr == DependencyList.end()) { return false; }

			auto& Head{ std::get<TargetType>(m_Nodes).T_Map };
			auto Ptr{ Head.find(NodeName) };
			// self does not exist, or try to depend on itself
			if (Ptr == Head.end()) { return false; }

			// check for circular dependency
			if (DependencyPtr->second.getDependencyList().contains(&Ptr->second))
			{
				return false;
			}

			return Ptr->second.addDependency(&DependencyPtr->second);
		}

		/**
		 * remove dependency from NodeName
		 * @tparam U : target node
		 * @tparam V : dependency
		 * @param NodeName : name of self
		 * @param Dependency : name for dependency
		 * @exception : runtime_exception
		 * @return : if successfully removed
		 */
		template<typename Target, typename Dependent>
		bool removeDependency(const std::string& NodeName, const std::string& Dependency)
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;
			using DependentType = DependencyGraphImpl::DependencyGraphNode_T_List<Dependent>;

			auto& DependencyList{ std::get<DependentType>(m_Nodes).T_Map };
			auto DependencyPtr{ DependencyList.find(Dependency) };
			// dependency does not exist
			if (DependencyPtr == DependencyList.end()) { return false; }

			auto& Ptr{ std::get<TargetType>(m_Nodes).T_Map.at(NodeName) };

			return Ptr->removeDependency(DependencyPtr->second);
		}

		/**
		 * remove node from graph
		 * @tparam U : target node
		 * @param NodeName : name of node
		 * @exception : runtime_exception
		 * @return if successfully removed from graph
		 */
		template<typename Target>
		bool remove(const std::string& NodeName)
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;

			auto& Head{ std::get<TargetType>(m_Nodes) };
			auto Node{ Head.T_Map.find(NodeName) };
			if (Node == Head.T_Map.end()) { return false; }

			// It is a dependency, do nothing
			if (Node->second.RefCount != 0) { return false; }

			Head.T_Map.erase(Node);

			// update the min ref count
			Head.MinRefCount = std::numeric_limits<uint32_t>::max();
			for (auto it = Head.T_Map.begin(); it != Head.T_Map.end(); it++)
			{
				if (it->second.RefCount < Head.MinRefCount) { Head.MinRefCount = it->second.RefCount; }
			}

			return true;
		}

		/**
		 * get item from graph
		 * @tparam Target 
		 * @param NodeName 
		 * @return : item
		 */
		template<typename Target>
		Target& get(const std::string& NodeName)
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;

			return std::get<TargetType>(m_Nodes).T_Map.at(NodeName).getItem();
		}

		/**
		 * check if item exists in graph
		 * @tparam Target 
		 * @param NodeName 
		 * @return true if exists
		 */
		template<typename Target>
		bool has(const std::string& NodeName)
		{
			using TargetType = DependencyGraphImpl::DependencyGraphNode_T_List<Target>;

			auto& Map{ std::get<TargetType>(m_Nodes).T_Map };
			return Map.find(NodeName) != Map.end();
		}

		~DependencyGraph()
		{
			bool Empty{ false };

			const auto Delete = [](bool& Empty, auto&& Node)
				{
					// Empty is assumed true, until proven false

					if (Node.T_Map.empty()) { return; }
					if (Node.MinRefCount > 0) { Empty = false; return; }

					// while its not empty and has min ref count 0
					while (Node.MinRefCount == 0 && !Node.T_Map.empty())
					{
						Node.MinRefCount = std::numeric_limits<uint32_t>::max();

						for (auto it = Node.T_Map.begin(); it != Node.T_Map.end();)
						{
							uint32_t RefCount = it->second.RefCount;
							// erase all with 0 ref
							if (RefCount == 0) { it = Node.T_Map.erase(it); }

							else
							{
								if (RefCount < Node.MinRefCount) { Node.MinRefCount = RefCount; }

								it++;
							}
						}
					}

					if (!Node.T_Map.empty()) { Empty = false; }
				};
			// while there is a map that is not empty
			while (!Empty)
			{
				// assume empty
				Empty = true;

				// iterate through tuple and get all min ref counts
				std::apply([&](auto&& ...Nodes) {(Delete(Empty, Nodes), ...); }, m_Nodes);
			}
		}
	private:
		std::tuple<DependencyGraphImpl::DependencyGraphNode_T_List<T> ...> m_Nodes;
	};
}