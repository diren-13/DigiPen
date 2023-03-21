/************************************************************************************//*!
 @file    ALGraph.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the interface for the BSTException, and BSTree objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <algorithm>
#include <queue>
// Primary Header
#include "ALGraph.h"

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Default Constructor for an ALGraph.

 @param     size
    The size of the ALGraph
*//*************************************************************************************/
ALGraph::ALGraph(unsigned int size)
: NUM_VERTEX    { size }
, adjacencyList { NUM_VERTEX }
{}

/************************************************************************************//*!
 @brief     Default Constructor for an Node.
*//*************************************************************************************/
ALGraph::Node::Node()
: destination   { 0 }
, cost          { INF }
{}

/************************************************************************************//*!
 @brief     Constructor for a Node

 @param     dst
    The destination of the edge.
 @param     weight
    The weight of the edge.
*//*************************************************************************************/
ALGraph::Node::Node(unsigned int dst, unsigned int weight)
: destination   { dst }
, cost          { weight }
{}

/************************************************************************************//*!
 @brief      Destructor for an ALGraph.
*//*************************************************************************************/
ALGraph::~ALGraph()
{
    // Do nothing
}

/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Adds a directed edge to the graph.

 @param     source
    The source node.
 @param     destination
    The destination node.
 @param     weight
    The of the edge between the source and destination nodes
*//*************************************************************************************/
void ALGraph::AddDEdge(unsigned int source, unsigned int destination, unsigned int weight)
{
    --source;
    Node srcToDest{ destination, weight };
    adjacencyList[source].emplace_back(srcToDest);

    // Sort the adjacency list for the source node
    std::sort(adjacencyList[source].begin(), adjacencyList[source].end(), [](const Node& lhs, const Node& rhs)->bool
    {
        if (lhs.cost < rhs.cost)
            return true;
        else if (lhs.cost == rhs.cost && lhs.destination < rhs.destination)
            return true;

        return false;
    });
}
/************************************************************************************//*!
 @brief     Adds a undirected edge to the graph.

 @param     source
    The source node.
 @param     destination
    The destination node.
 @param     weight
    The of the edge between the source and destination nodes
*//*************************************************************************************/
void ALGraph::AddUEdge(unsigned int source, unsigned int destination, unsigned int weight)
{
    AddDEdge(source, destination, weight);
    AddDEdge(destination, source, weight);
}
/************************************************************************************//*!
 @brief     Finds the shortest path from a given node to all the other nodes in
            the graph.

 @param     startNode
    The source node.

 @returns   A vector of DijkstraInfo structs.
*//*************************************************************************************/
ALGraph::DijkstraResult ALGraph::Dijkstra(unsigned int startNode) const
{
    DijkstraResult result{ NUM_VERTEX };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    Node start{ startNode - 1, 0 };
    pq.emplace(start);

    // Set distances in result
    for (unsigned int i = 0; i < NUM_VERTEX; ++i)
    {
        result[i].cost = (i == startNode - 1 ? 0 : INF);
    }

    // To prevent duplicate checks
    std::vector<bool> evaluated;
    evaluated.resize(NUM_VERTEX);
    for (unsigned int i = 0; i < NUM_VERTEX; ++i)
    {
        evaluated[i] = false;
    }

    while (!pq.empty())
    {
        const unsigned int START = pq.top().destination;
        pq.pop();

        // If node has already been evaluated, skip
        if (!evaluated[START])
        {
            evaluated[START] = true;
        }
        else
            continue;

        const DijkstraInfo& START_INFO = result[START];

        for (auto it = adjacencyList[START].begin(); it != adjacencyList[START].end(); ++it)
        {
            const unsigned int DST          = it->destination;
            const unsigned int COST         = it->cost;
            const unsigned int CURRENT_COST = COST + (START_INFO.cost == INF ? 0 : START_INFO.cost);

            DijkstraInfo& DST_INFO = result[DST-1];

            if (DST_INFO.cost > CURRENT_COST)
            {
                // Update distance
                DST_INFO.cost = CURRENT_COST;
                DST_INFO.path = START_INFO.path;
                DST_INFO.path.emplace_back(DST);

                Node next{ DST-1, DST_INFO.cost };
                pq.emplace(next);
            }
        }
    }

    // Insert start node in front of all paths except paths with -1 cost
    for (auto& r : result)
    {
        if (r.cost == INF)
            continue;

        r.path.insert(r.path.begin(), startNode);
    }

    return result;
}
/************************************************************************************//*!
 @brief     Gets the adjacency list representation of the graph

 @returns   The adjacency list representation of the graph.
*//*************************************************************************************/
ALIST ALGraph::GetAList() const
{
    ALIST adjList{ NUM_VERTEX };

    for (unsigned int i = 0; i < NUM_VERTEX; ++i)
    {
        const auto& NODE_LIST   = adjacencyList[i];
        auto&       aList       = adjList[i];

        for (const auto EDGE : NODE_LIST)
        {
            AdjacencyInfo info{ EDGE.destination, EDGE.cost };
            aList.emplace_back(info);
        }
    }

    return adjList;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Compares two nodes.

 @param     rhs
    The other node to compare with.

 @returns   True if the cost of this edge is smaller than the other edge.
*//*************************************************************************************/
bool ALGraph::Node::operator<(const Node& rhs) const
{
    if (cost < rhs.cost)
        return true;
    else if (cost == rhs.cost && destination < rhs.destination)
        return true;

    return false;
}
/************************************************************************************//*!
 @brief     Compares two nodes.

 @param     rhs
    The other node to compare with.

 @returns   True if the cost of this edge is larger than the other edge.
*//*************************************************************************************/
bool ALGraph::Node::operator>(const Node& rhs) const
{
    if (cost > rhs.cost)
        return true;
    else if (cost == rhs.cost && destination > rhs.destination)
        return true;

    return false;
}

