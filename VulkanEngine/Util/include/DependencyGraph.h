#pragma once

#include <concepts>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <limits>
#undef max

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
		// Internal impl
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
				Item{ std::move(Obj) } {}

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

			auto getDependencyList() const -> std::unordered_set<DependencyGraphNodeBase*>
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


	public:
		/**
		 * insert item into graph
		 * @tparam Target : type of Obj
		 * @param Obj : Obj of type Target to insert
		 * @param NodeName : Name of obj
		 * @param Destructor : called when obj is removed from graph
		 * @return std::pair<reference to inserted object, true if successfully inserted>
		 */
		template<typename Target>
			requires std::movable<Target>
		std::pair<Target&, bool> insert(Target&& Obj, std::string NodeName, std::function<void(Target&)> Destructor = [](Target&) {})
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			auto& Head{ std::get<TargetType>(m_Nodes) };

			auto Result = Head.T_Map.try_emplace(std::move(NodeName), std::forward<Target>(Obj), Head.MinRefCount, std::forward<std::function<void(Target&)>>(Destructor));

			return { Result.first->second.getItem(), Result.second };
		}

		/**
		 * add dependency between 2 nodes
		 * @tparam Target : base node type
		 * @tparam Dependent : dependent node type
		 * @param NodeName : base node
		 * @param Dependency : node that base node depends on
		 * @return true if successfully added dependency between nodes
		 */
		template<typename Target, typename Dependent>
		bool addDependency(const std::string& NodeName, const std::string& Dependency)
		{
			using TargetType = DependencyGraphNode_T_List<Target>;
			using DependentType = DependencyGraphNode_T_List<Dependent>;

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
		 * remove dependency from node
		 * @tparam Target
		 * @tparam Dependent
		 * @param NodeName
		 * @param Dependency
		 * @return true if successfully removed dependency from node
		 */
		template<typename Target, typename Dependent>
		bool removeDependency(const std::string& NodeName, const std::string& Dependency)
		{
			using TargetType = DependencyGraphNode_T_List<Target>;
			using DependentType = DependencyGraphNode_T_List<Dependent>;

			auto& DependencyList{ std::get<DependentType>(m_Nodes).T_Map };
			auto DependencyPtr{ DependencyList.find(Dependency) };
			// dependency does not exist
			if (DependencyPtr == DependencyList.end()) { return false; }

			auto& Ptr{ std::get<TargetType>(m_Nodes).T_Map.at(NodeName) };

			return Ptr->removeDependency(DependencyPtr->second);
		}

		/**
		 * try to remove node from graph
		 * @tparam Target
		 * @param NodeName
		 * @return true if successfully removed from graph
		 */
		template<typename Target>
		bool try_remove(const std::string& NodeName)
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			auto& Head{ std::get<TargetType>(m_Nodes) };
			auto Node{ Head.T_Map.find(NodeName) };
			if (Node == Head.T_Map.end()) { return false; }

			// It has a dependency, do nothing
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
		 * remove the node and all nodes that depends on it from graph
		 * @time : O(n^2 * m)
		 * @tparam Target
		 * @param NodeName
		 * @param OnlyDependencies : true if do not remove self, default false
		 */
		template<typename Target>
		void remove(const std::string& NodeName, bool OnlyDependencies = false)
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			auto& Head{ std::get<TargetType>(m_Nodes) };
			auto NodeIter{ Head.T_Map.find(NodeName) };
			if (NodeIter == Head.T_Map.end()) { return; }

			if (NodeIter->second.RefCount != 0)
			{
				// get list of dependencies
				std::unordered_set<DependencyGraphNodeBase*> Dependencies;

				auto RemoveNode = [&, Target = static_cast<DependencyGraphNodeBase*>(&NodeIter->second)](std::unordered_set<DependencyGraphNodeBase*>& DL, auto&& Header)
					{
						for (auto it = Header.T_Map.begin(); it != Header.T_Map.end();)
						{
							auto& v = it->second;

							// if depends on target or is in dependency list(list to delete)
							if (DL.contains(&v) || v.getDependencyList().contains(Target))
							{
								// if no ref count, delete
								if (v.RefCount == 0)
								{
									it = Header.T_Map.erase(it);
									DL.erase(&v);
									continue;
								}
								// add to dependency list, and mark for delete
								DL.insert(&v);
							}
							it++;
						}

					};

				while (NodeIter->second.RefCount != 0)
				{
					// iterate through tuple and remove nodes
					std::apply([&](auto&& ...Nodes) { (RemoveNode(Dependencies, Nodes), ...); }, m_Nodes);
				}
			}
			if(!OnlyDependencies)
			{
				// erase the node
				Head.T_Map.erase(NodeIter);
			}

			// update the min ref count
			Head.MinRefCount = std::numeric_limits<uint32_t>::max();
			for (auto it = Head.T_Map.begin(); it != Head.T_Map.end(); it++)
			{
				if (it->second.RefCount < Head.MinRefCount) { Head.MinRefCount = it->second.RefCount; }
			}
		}

		/**
		 * get item from graph
		 * @tparam Target
		 * @param NodeName
		 * @return : reference to item
		 */
		template<typename Target>
		Target& get(const std::string& NodeName)
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			return std::get<TargetType>(m_Nodes).T_Map.at(NodeName).getItem();
		}

		/**
		 * get all items of a certain type
		 * @tparam Target
		 * @return vector of item type
		 */
		template<typename Target>
		std::vector<std::pair<std::string, Target&>> getItemList()
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			std::vector<std::pair<std::string, Target&>> List;
			for (auto& kv : std::get<TargetType>(m_Nodes).T_Map)
			{

				List.emplace_back(kv.first, kv.second.getItem());
			}

			return List;
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
			using TargetType = DependencyGraphNode_T_List<Target>;

			auto& Map{ std::get<TargetType>(m_Nodes).T_Map };
			return Map.find(NodeName) != Map.end();
		}

		/**
		 * get size of type
		 * @tparam Target
		 * @return size of a certain type
		 */
		template<typename Target>
		std::size_t size()
		{
			using TargetType = DependencyGraphNode_T_List<Target>;

			return std::get<TargetType>(m_Nodes).T_Map.size();
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
		std::tuple<DependencyGraphNode_T_List<T> ...> m_Nodes;
	};
}