#pragma once
#include "Shared/Utilities/vec2.hpp"

namespace DEVIOUSMUD 
{
	namespace PATH 
	{
		/// <summary>
		/// Basic Dijkstra Algorithm implementation.
		/// </summary>
		class AStar
		{
		public:
			/// <summary>
			/// Returns the optimal path from start to end.
			/// </summary>
			/// <param name="startPoint"></param>
			/// <param name="endPoint"></param>
			/// <returns></returns>
			static std::vector<Utilities::ivec2> find_path(const Utilities::ivec2& _startPoint, const Utilities::ivec2& _endPoint);

		private:
			/// <summary>
			/// Returns all adjacent nodes relative to the given node.
			/// </summary>
			/// <param name="node"></param>
			/// <returns></returns>
			static std::vector<Utilities::ivec2> get_neighbours(const Utilities::ivec2& _node);

			/// <summary>
			/// Generates a grid.
			/// </summary>
			static const bool vector_contains_node(const Utilities::ivec2& _target, std::vector<Utilities::ivec2> _nodes);

			/// <summary>
			/// Calculate the cost to get to the end point.
			/// </summary>
			/// <param name="_pos"></param>
			/// <param name="_end"></param>
			/// <returns></returns>
			static int get_cost(const Utilities::ivec2& _pos, 
				                const Utilities::ivec2& _end);

		};
	}
}
