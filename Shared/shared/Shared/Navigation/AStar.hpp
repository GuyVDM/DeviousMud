#pragma once
#include <vector>
#include <unordered_map>
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

#pragma region IMPLEMENTATION_DETAILS

using namespace DEVIOUSMUD;
using namespace PATH;

inline const bool AStar::vector_contains_node(const Utilities::ivec2& _target, std::vector<Utilities::ivec2> _nodes)
{
	for (Utilities::ivec2& node : _nodes)
	{
		if (node == _target)
			return true;
	}

	return false;
}

inline int DEVIOUSMUD::PATH::AStar::get_cost(const Utilities::ivec2& _pos, const Utilities::ivec2& _end)
{
	return abs(_pos.x - _end.x) + abs(_pos.y - _end.y);
}

inline std::vector<Utilities::ivec2> AStar::get_neighbours(const Utilities::ivec2& _node)
{
	std::vector<Utilities::ivec2> neighbours;

	//Horizontal & Verticals
	neighbours.emplace_back(_node.x + 1, _node.y); //Right
	neighbours.emplace_back(_node.x - 1, _node.y); //Left
	neighbours.emplace_back(_node.x, _node.y - 1); //Up
	neighbours.emplace_back(_node.x, _node.y + 1); //Down

	//Diagonals
	neighbours.emplace_back(_node.x - 1, _node.y + 1); //Topleft 
	neighbours.emplace_back(_node.x + 1, _node.y + 1); //Topright
	neighbours.emplace_back(_node.x - 1, _node.y - 1); //Bottomleft
	neighbours.emplace_back(_node.x + 1, _node.y - 1); //Bottomright

	return neighbours;
}

inline std::vector<Utilities::ivec2> AStar::find_path(const Utilities::ivec2& _startPoint, const Utilities::ivec2& _endPoint)
{
	std::unordered_map<Utilities::ivec2, Utilities::ivec2> parents;

	std::vector<Utilities::ivec2> open_list;
	std::vector<Utilities::ivec2> closed_list;

	open_list.push_back(_startPoint);

	while (open_list.size() > 0)
	{
		Utilities::ivec2 current = *std::min_element(open_list.begin(), open_list.end(), [_endPoint](const Utilities::ivec2& lhs, const Utilities::ivec2& rhs)
			{
				return get_cost(lhs, _endPoint) < get_cost(rhs, _endPoint);
			});

		closed_list.push_back(current);

		open_list.clear();

		for (Utilities::ivec2& node : get_neighbours(current))
		{
			if (!vector_contains_node(node, closed_list))
			{
				open_list.push_back(node);

				parents[node] = current;
			}

			//Check if we found the destination tile.
			if (node == _endPoint)
			{
				closed_list.push_back(_endPoint);

				//Jump to the part where we trace and create a inverse of the path.
				//Only reason i'm using goto is to break out of the nested loop, don't hate me.
				goto calculate_path;
			}
		}
	}

	//Start tracing and reversing the path so it's linear from the starting point.
calculate_path:
	std::vector<Utilities::ivec2> path;

	Utilities::ivec2 current = _endPoint;
	while (current != _startPoint)
	{
		path.push_back(current);

		if (parents.find(current) != parents.end())
		{
			current = parents[current];
			continue;
		}

		break;
	}

	std::reverse(path.begin(), path.end());

	return path;
}

#pragma endregion
