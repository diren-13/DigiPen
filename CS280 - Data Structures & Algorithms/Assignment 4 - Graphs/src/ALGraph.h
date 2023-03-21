/************************************************************************************//*!
 @file    ALGraph.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the interface for the ALGraph, and BSTree objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef ALGRAPH_H
#define ALGRAPH_H

// Standard Libraries
#include <vector>
#include <list>
#include <utility>      // std::pair

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
 @brief     Encapsulates information for running Djikstra's Algorithm
*//*************************************************************************************/
struct DijkstraInfo
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    unsigned                cost;
    std::vector<unsigned>   path;
};

/************************************************************************************//*!
 @brief     Encapsulates the information of an edge in an adjacency list.
*//*************************************************************************************/
struct AdjacencyInfo
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    unsigned id;
    unsigned weight;
};

/************************************************************************************//*!
 @brief     A vector of Adjacency List Information structs.
*//*************************************************************************************/
typedef std::vector<std::vector<AdjacencyInfo>> ALIST;

/************************************************************************************//*!
 @brief     Encapsulates a Graph represented as an Adjacency List
*//*************************************************************************************/
class ALGraph
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    using DijkstraResult = std::vector<DijkstraInfo>;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Constructor for an ALGraph.

    @param      size
        The size of the ALGraph
    *//*********************************************************************************/
    ALGraph     (unsigned int size);
    /********************************************************************************//*!
    @brief      Destructor for an ALGraph.
    *//*********************************************************************************/
    ~ALGraph    ();

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Adds a directed edge to the graph.

    @param      source
        The source node.
    @param      destination
        The destination node.
    @param      weight
        The of the edge between the source and destination nodes
    *//*********************************************************************************/
    void            AddDEdge    (unsigned int source, unsigned int destination, unsigned int weight);
    /********************************************************************************//*!
    @brief      Adds a undirected edge to the graph.

    @param      source
        The source node.
    @param      destination
        The destination node.
    @param      weight
        The of the edge between the source and destination nodes
    *//*********************************************************************************/
    void            AddUEdge    (unsigned int source, unsigned int destination, unsigned int weight);
    /********************************************************************************//*!
    @brief      Finds the shortest path from a given node to all the other nodes in
                the graph.

    @param      startNode
        The source node.

    @returns    A vector of DijkstraInfo structs.
    *//*********************************************************************************/
    DijkstraResult  Dijkstra    (unsigned int startNode) const;
    /********************************************************************************//*!
    @brief      Gets the adjacency list representation of the graph

    @returns    The adjacency list representation of the graph.
    *//*********************************************************************************/
    ALIST           GetAList    () const;
        
private:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Encapsulates an attached node and the edge weight.
    *//*********************************************************************************/
    struct Node
    {
        unsigned int destination;
        unsigned int cost;

        /****************************************************************************//*!
        @brief      Default Constructor for a Node
        *//*****************************************************************************/
        Node();
        /****************************************************************************//*!
        @brief      Constructor for a Node

        @param      dst
            The destination of the edge.
        @param      weight
            The weight of the edge.
        *//*****************************************************************************/
        Node(unsigned int dst, unsigned int weight);

        /****************************************************************************//*!
        @brief      Compares two nodes.

        @param      rhs
            The other node to compare with.

        @returns    True if the cost of this edge is smaller than the other edge.
        *//*****************************************************************************/
        bool operator<(const Node& rhs) const;
        /****************************************************************************//*!
        @brief      Compares two nodes.

        @param      rhs
            The other node to compare with.

        @returns    True if the cost of this edge is larger than the other edge.
        *//*****************************************************************************/
        bool operator>(const Node& rhs) const;
    };

    using AdjacencyList  = std::vector<std::vector<Node>>;

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    static constexpr unsigned   INF             = static_cast<unsigned>(-1);

    const unsigned int          NUM_VERTEX;
    AdjacencyList               adjacencyList;
};

#endif
