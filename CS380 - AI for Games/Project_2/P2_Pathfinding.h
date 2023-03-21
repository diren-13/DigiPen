#pragma once
#include <numeric>
#include "Misc/PathfindingDetails.hpp"

struct AStarNode
{
    enum class State
    {
        Unexplored  ,
        Open        ,
        Closed
    };

    enum class Neighbour
    {
        Left        ,
        Right       ,
        Top         ,
        Bottom      ,
        TopLeft     ,
        TopRight    ,
        BottomLeft  ,
        BottomRight ,

        Total
    };

    static constexpr unsigned int INVALID_NODE = std::numeric_limits<unsigned int>::max();

    unsigned int    parent  = INVALID_NODE;
    unsigned int    index   = INVALID_NODE;
    unsigned int    neighbours[static_cast<unsigned int>(Neighbour::Total)] = { INVALID_NODE };
    float           totalCost = std::numeric_limits<float>::max();
    float           givenCost = 0.0f;
    State           listState = State::Unexplored;

    float   ComputeGivenCost(Neighbour);
    void    OpenNode    (float gc, float tc, unsigned int parentIdx);
    void    UpdateNode  (float gc, float tc, unsigned int parentIdx);
};

class RubberBandAABB
{
public:
    RubberBandAABB(const GridPos& p0, const GridPos& p1, const GridPos& p2);
    bool Evaluate() const;

private:
    GridPos min;
    GridPos max;
};

class AStarPather
{
public:
    /* 
        The class should be default constructible, so you may need to define a constructor.
        If needed, you can modify the framework where the class is constructed in the
        initialize functions of ProjectTwo and ProjectThree.
    */
    AStarPather();

    /* ************************************************** */
    // DO NOT MODIFY THESE SIGNATURES
    bool        initialize      ();
    void        shutdown        ();
    PathResult  compute_path    (PathRequest& request);
    /* ************************************************** */

    /*
        You should create whatever functions, variables, or classes you need.
        It doesn't all need to be in this header and cpp, structure it whatever way
        makes sense to you.
    */

private:
    using HeuristicCost = float(*)(const GridPos&, const GridPos&);
    static HeuristicCost heuristicJump[static_cast<int>(Heuristic::NUM_ENTRIES)];

    int                             gridWidth;
    int                             gridHeight;
    std::vector<AStarNode>          grid;
    std::vector<AStarNode*>         openList;

    // Setup
    void            setupMap        ();
    void            setupNeighbours (AStarNode&);

    // Pathfinding
    PathResult      astar           (PathRequest& request);
    static float    heuristicCost   (Heuristic heuristic, float weight, const GridPos& start, const GridPos& goal);
    static float    octile          (const GridPos& start, const GridPos& goal);
    static float    chebyshev       (const GridPos& start, const GridPos& goal);
    static float    manhattan       (const GridPos& start, const GridPos& goal);
    static float    euclidean       (const GridPos& start, const GridPos& goal);
    void            computePath     (PathRequest& request, unsigned int startIdx, unsigned int currentIdx) const;

    // Aesthetics
    static void     rubberBand      (PathRequest& request);
    static void     smoothing       (PathRequest& request, bool rubberBanded);

    // Utils
    void            resetGrid       ();
    unsigned int    getGridIndex    (int row, int col) const;
    GridPos         getGridPos      (unsigned int index) const;
};