#include <pch.h>
#include "Projects/ProjectTwo.h"
#include "P2_Pathfinding.h"

#pragma region Extra Credit
bool ProjectTwo::implemented_floyd_warshall()
{
    return false;
}

bool ProjectTwo::implemented_goal_bounding()
{
    return false;
}

bool ProjectTwo::implemented_jps_plus()
{
    return false;
}
#pragma endregion

float AStarNode::ComputeGivenCost(Neighbour n)
{
    switch (n)
    {
        case Neighbour::Left:
        case Neighbour::Right:
        case Neighbour::Top:
        case Neighbour::Bottom:
        {
            return 1.0f;
        }
        case Neighbour::TopLeft:
        case Neighbour::TopRight:
        case Neighbour::BottomLeft:
        case Neighbour::BottomRight:
        {
            return std::sqrtf(2.0f);
        }
        default: return std::numeric_limits<float>::max();
    }
}

void AStarNode::OpenNode(float gc, float tc, unsigned int parentIdx)
{
    listState = State::Open;
    givenCost = gc;
    totalCost = tc;
    parent    = parentIdx;
}

void AStarNode::UpdateNode(float gc, float tc, unsigned int parentIdx)
{
    if (listState == State::Unexplored)
        return;

    givenCost = gc;
    totalCost = tc;
    parent    = parentIdx;
}

RubberBandAABB::RubberBandAABB(const GridPos& p0, const GridPos& p1, const GridPos& p2)
: min {0, 0}
, max {0, 0}
{
    min.col = std::min({ p0.col, p1.col, p2.col });
    min.row = std::min({ p0.row, p1.row, p2.row });

    max.col = std::max({ p0.col, p1.col, p2.col });
    max.row = std::max({ p0.row, p1.row, p2.row });
}

bool RubberBandAABB::Evaluate() const
{
    // Check if any pos in AABB in a wall
    for (int r = min.row; r <= max.row; ++r)
    {
        for (int c = min.col; c <= max.col; ++c)
        {
            if (terrain->is_wall(r, c))
                return false;
        }
    }

    return true;
}

AStarPather::HeuristicCost AStarPather::heuristicJump[static_cast<int>(Heuristic::NUM_ENTRIES)] =
{
    octile, chebyshev, manhattan, euclidean
};

AStarPather::AStarPather()
: gridWidth     { 0 }
, gridHeight    { 0 }
{
    const int MAX_GRID_WIDTH    = static_cast<int>(Terrain::maxMapWidth);
    const int MAX_GRID_HEIGHT   = static_cast<int>(Terrain::maxMapHeight);
    grid.resize(MAX_GRID_WIDTH * MAX_GRID_HEIGHT);

    for (auto& n : grid)
    {
        // Set all initial neighbours
        std::for_each(std::begin(n.neighbours), std::end(n.neighbours), [](unsigned int& i){ i = AStarNode::INVALID_NODE; });
    }
}


bool AStarPather::initialize()
{
    // Register grid generation on map change
    Callback mapChangeCB = std::bind(&AStarPather::setupMap, this);
    Messenger::listen_for_message(Messages::MAP_CHANGE, mapChangeCB);

    return true;
}

void AStarPather::shutdown()
{
    
}

PathResult AStarPather::compute_path(PathRequest &request)
{
    /*
        This is where you handle pathing requests, each request has several fields:

        start/goal - start and goal world positions
        path - where you will build the path upon completion, path should be
            start to goal, not goal to start
        heuristic - which heuristic calculation to use
        weight - the heuristic weight to be applied
        newRequest - whether this is the first request for this path, should generally
            be true, unless single step is on

        smoothing - whether to apply smoothing to the path
        rubberBanding - whether to apply rubber banding
        singleStep - whether to perform only a single A* step
        debugColoring - whether to color the grid based on the A* state:
            closed list nodes - yellow
            open list nodes - blue

            use terrain->set_color(row, col, Colors::YourColor);
            also it can be helpful to temporarily use other colors for specific states
            when you are testing your algorithms

        method - which algorithm to use: A*, Floyd-Warshall, JPS+, or goal bounding,
            will be A* generally, unless you implement extra credit features

        The return values are:
            PROCESSING - a path hasn't been found yet, should only be returned in
                single step mode until a path is found
            COMPLETE - a path to the goal was found and has been built in request.path
            IMPOSSIBLE - a path from start to goal does not exist, do not add start position to path
    */

    // I did not implement the extra credit, therefore I will not check the method and use A* for all methods.

    const PathResult RESULT = astar(request);

    if (request.settings.debugColoring)
    {
        const GridPos START_POS = terrain->get_grid_position(request.start);
        terrain->set_color(START_POS, Colors::Orange);

        const GridPos GOAL_POS = terrain->get_grid_position(request.goal);
        terrain->set_color(GOAL_POS, Colors::Orange);
    }

    if (RESULT == PathResult::COMPLETE)
    {
        const bool RB = request.settings.rubberBanding;
        const bool SM = request.settings.smoothing;

        if (RB)
        {
            rubberBand(request);
        }

        if (SM)
        {
            smoothing(request, RB);
        }
    }

    return RESULT;
}

void AStarPather::setupMap()
{
    gridWidth   = terrain->get_map_width();
    gridHeight  = terrain->get_map_height();

    // Find all neighbors of each node and store
    // Walls will not be stored

    for (int row = 0; row < gridHeight; ++row)
    {
        for (int col = 0; col < gridWidth; ++col)
        {
            const int GRID_IDX = getGridIndex(row, col);

            AStarNode& node = grid[GRID_IDX];
            node.index = GRID_IDX;

            // Skip walls
            if (terrain->is_wall(row, col))
                continue;

            setupNeighbours(node);
        }
    }
}

void AStarPather::setupNeighbours(AStarNode& node)
{
    const GridPos pos = getGridPos(static_cast<int>(node.index));
    const int ROW = pos.row;
    const int COL = pos.col;

    for (int n = 0; n < static_cast<int>(AStarNode::Neighbour::Total); ++n)
    {
        unsigned int& neighbour = node.neighbours[n];
        neighbour = AStarNode::INVALID_NODE;

        switch (static_cast<AStarNode::Neighbour>(n))
        {
            case AStarNode::Neighbour::Left:
            {
                neighbour = COL == 0 ? AStarNode::INVALID_NODE : 
                            ((terrain->is_wall(ROW, COL - 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW, COL - 1)));
                break;
            }
            case AStarNode::Neighbour::Right:
            {
                neighbour = COL == gridWidth - 1 ? AStarNode::INVALID_NODE : 
                            ((terrain->is_wall(ROW, COL + 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW, COL + 1)));
                break;
            }
            case AStarNode::Neighbour::Top:
            {
                neighbour = ROW == gridHeight - 1 ? AStarNode::INVALID_NODE : 
                            ((terrain->is_wall(ROW + 1, COL) ? AStarNode::INVALID_NODE : getGridIndex(ROW + 1, COL)));
                break;
            }
            case AStarNode::Neighbour::Bottom:
            {
                neighbour = ROW == 0 ? AStarNode::INVALID_NODE : 
                            (terrain->is_wall(ROW - 1, COL) ? AStarNode::INVALID_NODE : getGridIndex(ROW - 1, COL));
                break;
            }
            case AStarNode::Neighbour::BottomLeft:
            {
                const unsigned int BTM      = node.neighbours[static_cast<int>(AStarNode::Neighbour::Bottom)];
                const unsigned int LEFT     = node.neighbours[static_cast<int>(AStarNode::Neighbour::Left)];
                if (BTM != AStarNode::INVALID_NODE && LEFT != AStarNode::INVALID_NODE)
                {
                    neighbour = (terrain->is_wall(ROW - 1, COL - 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW - 1, COL - 1));;
                }
                break;
            }
            case AStarNode::Neighbour::BottomRight:
            {
                const unsigned int BTM      = node.neighbours[static_cast<int>(AStarNode::Neighbour::Bottom)];
                const unsigned int RIGHT    = node.neighbours[static_cast<int>(AStarNode::Neighbour::Right)];
                if (BTM != AStarNode::INVALID_NODE && RIGHT != AStarNode::INVALID_NODE)
                {
                    neighbour = (terrain->is_wall(ROW - 1, COL + 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW - 1, COL + 1));;
                }
                break;
            }
            case AStarNode::Neighbour::TopLeft:
            {
                const unsigned int TOP      = node.neighbours[static_cast<int>(AStarNode::Neighbour::Top)];
                const unsigned int LEFT     = node.neighbours[static_cast<int>(AStarNode::Neighbour::Left)];
                if (TOP != AStarNode::INVALID_NODE && LEFT != AStarNode::INVALID_NODE)
                {
                    neighbour = (terrain->is_wall(ROW + 1, COL - 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW + 1, COL - 1));;
                }
                break;
            }
            case AStarNode::Neighbour::TopRight:
            {
                const unsigned int TOP      = node.neighbours[static_cast<int>(AStarNode::Neighbour::Top)];
                const unsigned int RIGHT    = node.neighbours[static_cast<int>(AStarNode::Neighbour::Right)];
                if (TOP != AStarNode::INVALID_NODE && RIGHT != AStarNode::INVALID_NODE)
                {
                    neighbour = (terrain->is_wall(ROW + 1, COL + 1) ? AStarNode::INVALID_NODE : getGridIndex(ROW + 1, COL + 1));;
                }
                break;
            }
            default: break;
        }
    }
}

PathResult AStarPather::astar(PathRequest& request)
{
    static auto minTotalCost = [](const AStarNode* a, const AStarNode* b)->bool { return a->totalCost < b->totalCost; };

    const GridPos START_GRID_POS = terrain->get_grid_position(request.start);
    AStarNode& startNode = grid[getGridIndex(START_GRID_POS.row, START_GRID_POS.col)];

    const GridPos GOAL_GRID_POS         = terrain->get_grid_position(request.goal);
    AStarNode& goalNode = grid[getGridIndex(GOAL_GRID_POS.row, GOAL_GRID_POS.col)];

    // EDGE CASE: start == goal
    if (START_GRID_POS == GOAL_GRID_POS)
        return PathResult::COMPLETE;

    // EDGE CASE: start or goal is wall
    if (terrain->is_wall(START_GRID_POS) || terrain->is_wall(GOAL_GRID_POS))
        return PathResult::IMPOSSIBLE;

    // Do not push start if not a new request
    if (request.newRequest)
    {
        openList.clear();
        resetGrid();

        // Compute start node total cost
        startNode.totalCost = startNode.givenCost + heuristicCost(request.settings.heuristic, request.settings.weight, START_GRID_POS, GOAL_GRID_POS);
        startNode.listState = AStarNode::State::Open;

        // Place start node onto open list
        openList.emplace_back(&startNode);
    }

    while (!openList.empty())
    {
        // Sort in order of cheapest to most expensive node
        std::sort(openList.begin(), openList.end(), minTotalCost);

        // Pop off front node
        const AStarNode* CHEAPEST_NODE = openList.front();
        openList.begin() = openList.erase(openList.begin());

        const GridPos CURRENT_GRID_POS = getGridPos(CHEAPEST_NODE->index);
        if (CURRENT_GRID_POS == GOAL_GRID_POS)
        {
            computePath(request, startNode.index, goalNode.index);
            return PathResult::COMPLETE;
        }

        // Neighbour nodes
        for (int i = 0; i < static_cast<int>(AStarNode::Neighbour::Total); ++i)
        {
            // Skip invalid neighbours (walls, off-map)
            if (CHEAPEST_NODE->neighbours[i] == AStarNode::INVALID_NODE)
                continue;

            AStarNode& neighbour = grid[CHEAPEST_NODE->neighbours[i]];
            //Skip any node on the closed list
            if (neighbour.listState == AStarNode::State::Closed)
                continue;

            // Compute total cost
            const GridPos NEIGHBOUR_GRID_POS = getGridPos(neighbour.index);
            const float NGS = CHEAPEST_NODE->givenCost + neighbour.ComputeGivenCost(static_cast<AStarNode::Neighbour>(i));
            const float NTS = NGS + heuristicCost(request.settings.heuristic, request.settings.weight, NEIGHBOUR_GRID_POS, GOAL_GRID_POS);

            // Set neighbour's parent to current node
            if (neighbour.listState == AStarNode::State::Unexplored)
            {
                neighbour.OpenNode(NGS, NTS, CHEAPEST_NODE->index);
                if (request.settings.debugColoring)
                {
                    terrain->set_color(getGridPos(neighbour.index), Colors::Blue);
                }
                openList.emplace_back(&neighbour);
            }
            else if (NTS < neighbour.totalCost)
            {
                // Update node
                neighbour.UpdateNode(NGS, NTS, CHEAPEST_NODE->index);
            }
        }

        grid[CHEAPEST_NODE->index].listState = AStarNode::State::Closed;
        if (request.settings.debugColoring)
        {
            terrain->set_color(getGridPos(CHEAPEST_NODE->index), Colors::Yellow);
        }

        if (request.settings.singleStep)
        {
            // Colour nodes on the grid
            return PathResult::PROCESSING;
        }
    }

    return PathResult::IMPOSSIBLE;
}

float AStarPather::heuristicCost(Heuristic heuristic, float weight, const GridPos& start, const GridPos& goal)
{
    return heuristicJump[static_cast<int>(heuristic)](start, goal) * weight;
}

float AStarPather::octile(const GridPos& start, const GridPos& goal)
{
    const float START_X = static_cast<float>(start.col);
    const float START_Y = static_cast<float>(start.row);

    const float GOAL_X = static_cast<float>(goal.col);
    const float GOAL_Y = static_cast<float>(goal.row);

    const float X_DIFF = std::fabs(GOAL_X - START_X);
    const float Y_DIFF = std::fabs(GOAL_Y - START_Y);

    return std::min(X_DIFF, Y_DIFF) * std::sqrtf(2.0) + std::max(X_DIFF, Y_DIFF) - std::min(X_DIFF, Y_DIFF);
}

float AStarPather::chebyshev(const GridPos& start, const GridPos& goal)
{
    const float START_X = static_cast<float>(start.col);
    const float START_Y = static_cast<float>(start.row);

    const float GOAL_X = static_cast<float>(goal.col);
    const float GOAL_Y = static_cast<float>(goal.row);

    const float X_DIFF = std::fabs(GOAL_X - START_X);
    const float Y_DIFF = std::fabs(GOAL_Y - START_Y);

    return std::max(X_DIFF, Y_DIFF);
}

float AStarPather::manhattan(const GridPos& start, const GridPos& goal)
{
    const float START_X = static_cast<float>(start.col);
    const float START_Y = static_cast<float>(start.row);

    const float GOAL_X = static_cast<float>(goal.col);
    const float GOAL_Y = static_cast<float>(goal.row);

    const float X_DIFF = std::fabs(GOAL_X - START_X);
    const float Y_DIFF = std::fabs(GOAL_Y - START_Y);

    return (X_DIFF + Y_DIFF);
}

float AStarPather::euclidean(const GridPos& start, const GridPos& goal)
{
    const float START_X = static_cast<float>(start.col);
    const float START_Y = static_cast<float>(start.row);

    const float GOAL_X = static_cast<float>(goal.col);
    const float GOAL_Y = static_cast<float>(goal.row);

    const float X_DIFF = std::fabs(GOAL_X - START_X);
    const float Y_DIFF = std::fabs(GOAL_Y - START_Y);

    return std::sqrtf((X_DIFF * X_DIFF) + (Y_DIFF * Y_DIFF));
}

void AStarPather::computePath(PathRequest& request, unsigned int startIdx, unsigned int currentIdx) const
{
    while (currentIdx != startIdx)
    {
        const GridPos GP = getGridPos(static_cast<int>(currentIdx));
        request.path.emplace_front(terrain->get_world_position(GP));

        currentIdx = grid[currentIdx].parent;
    }
    request.path.emplace_front(request.start);
}

void AStarPather::rubberBand(PathRequest& request)
{
    // List must be >= 3
    if (request.path.size() < 3)
        return;

    auto p0It = request.path.begin();
    auto p1It = request.path.begin(); std::advance(p1It, 1);
    auto p2It = request.path.begin(); std::advance(p2It, 2);
    while (p2It != request.path.end())
    {
        const GridPos P0 = terrain->get_grid_position(*p0It);
        const GridPos P1 = terrain->get_grid_position(*p1It);
        const GridPos P2 = terrain->get_grid_position(*p2It);

        RubberBandAABB aabb{ P0, P1, P2 };
        if (aabb.Evaluate())
        {
            // remove middle node
            p1It = request.path.erase(p1It);
            ++p2It;
            continue;
        }

        ++p0It; ++p1It; ++p2It;
    }
}

void AStarPather::smoothing(PathRequest& request, bool rubberBanded)
{
    if (rubberBanded)
    {
        static constexpr float DISTANCE_THRESHOLD = 1.5f;
        // Add middle points for rubber banding
        auto p0It = request.path.begin();
        auto p1It = request.path.begin(); std::advance(p1It, 1);

        const GridPos GOAL_POS = terrain->get_grid_position(request.goal);

        while (p1It != request.path.end())
        {
            const GridPos P0_POS = terrain->get_grid_position(*p0It);
            const GridPos P1_POS = terrain->get_grid_position(*p1It);

            const float ROW_DIFF    = static_cast<float>((P1_POS.row - P0_POS.row) * (P1_POS.row - P0_POS.row));
            const float COL_DIFF    = static_cast<float>((P1_POS.col - P0_POS.col) * (P1_POS.col - P0_POS.col));
            const float DIST        = std::sqrtf(ROW_DIFF + COL_DIFF);
            if (DIST > DISTANCE_THRESHOLD)
            {
                const Vec3 MID_POINT = Vec3::Lerp(*p0It, *p1It, 0.5f);
                p1It = request.path.insert(p1It, MID_POINT);
                continue;
            }

            p0It = p1It;
            ++p1It;
        }
    }

    // List must be >= 3
    if (request.path.size() < 3)
        return;

    auto p0It = request.path.begin();
    auto p1It = request.path.begin();
    auto p2It = request.path.begin(); std::advance(p2It, 1);
    auto p3It = request.path.begin(); std::advance(p3It, 2);

    int numIterations = static_cast<int>(request.path.size()) - 1;
    while (numIterations > 0)
    {
        // Last iteration
        if (numIterations == 1)
        {
            p3It = p2It;
        }

        // Get spline
        Vec3 r1{ Vec3::Zero };  Vec3::CatmullRom(*p0It, *p1It, *p2It, *p3It, 0.25f, r1);
        Vec3 r2{ Vec3::Zero };  Vec3::CatmullRom(*p0It, *p1It, *p2It, *p3It, 0.5f, r2);
        Vec3 r3{ Vec3::Zero };  Vec3::CatmullRom(*p0It, *p1It, *p2It, *p3It, 0.75f, r3);

        auto insertedIt = p2It;

        // Move to next points
        p0It = p1It;
        p1It = p2It;
        p2It = p3It;
        ++p3It;

        insertedIt = request.path.insert(insertedIt, r3);
        insertedIt = request.path.insert(insertedIt, r2);
        insertedIt = request.path.insert(insertedIt, r1);

        --numIterations;
    }
}

void AStarPather::resetGrid()
{
    for (auto& n : grid)
    {
        n.listState     = AStarNode::State::Unexplored;
        n.parent        = AStarNode::INVALID_NODE;
        n.givenCost     = 0.0f;
        n.totalCost     = std::numeric_limits<float>::max();
    }
}


unsigned int AStarPather::getGridIndex(int row, int col) const
{
    assert(row >= 0 && row < gridHeight && col >= 0 && col < gridWidth);
    return static_cast<unsigned int>(row * gridHeight + col);
}

GridPos AStarPather::getGridPos(unsigned int index) const 
{
    assert(index >= 0);
    GridPos gridPos;
    gridPos.col = index % gridWidth;
    gridPos.row = (index - gridPos.col) / gridHeight;
    return gridPos;
}

