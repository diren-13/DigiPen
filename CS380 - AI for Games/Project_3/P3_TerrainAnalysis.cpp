#include <pch.h>
#include "Terrain/TerrainAnalysis.h"
#include "Terrain/MapMath.h"
#include "Agent/AStarAgent.h"
#include "Terrain/MapLayer.h"
#include "Projects/ProjectThree.h"

#include <iostream>
#include <optional>

bool ProjectThree::implemented_fog_of_war() const // extra credit
{
    return false;
}

float distance_to_closest_wall(int row, int col)
{
    /*
        Check the euclidean distance from the given cell to every other wall cell,
        with cells outside the map bounds treated as walls, and return the smallest
        distance.  Make use of the is_valid_grid_position and is_wall member
        functions in the global terrain to determine if a cell is within map bounds
        and a wall, respectively.
    */

    // Check if current cell is valid
    if (!terrain->is_valid_grid_position(row, col))
        return FLT_MAX;

    // Check if current cell is a wall
    if (terrain->is_wall(row, col))
        return 0.0f;

    // Get Map Dimensions
    const int MAP_WIDTH     = terrain->get_map_width() + 1;     //  Take 1 more for outside of the map
    const int MAP_HEIGHT    = terrain->get_map_height() + 1;    //  Take 1 more for outside of the map

    // Find closest wall
    float smallestDist = FLT_MAX;
    for (int r = -1; r < MAP_HEIGHT; ++r)
    {
        for (int c = -1; c < MAP_WIDTH; ++c)
        {
            float dist = 0.0f;

            // Handle out-of-bounds positions (treat as wall)
            if (!terrain->is_valid_grid_position(r, c))
            {
                dist = std::sqrtf(static_cast<float>((row - r) * (row - r)) + static_cast<float>((col - c) * (col - c)));
                if (dist < smallestDist)
                {
                    smallestDist = dist;
                }

                continue;
            }

            auto tmp = terrain->get_world_position(r, c);

            // Skip non walls
            if (!terrain->is_wall(r, c))
                continue;

            dist = std::sqrtf(static_cast<float>((row - r) * (row - r)) + static_cast<float>((col - c) * (col - c)));
            if (dist < smallestDist)
            {
                smallestDist = dist;
            }
        }
    }
    
    return smallestDist;
}

bool is_clear_path(int row0, int col0, int row1, int col1)
{
    /*
        Two cells (row0, col0) and (row1, col1) are visible to each other if a line
        between their centerpoints doesn't intersect the four boundary lines of every
        wall cell.  You should puff out the four boundary lines by a very tiny amount
        so that a diagonal line passing by the corner will intersect it.  Make use of the
        line_intersect helper function for the intersection test and the is_wall member
        function in the global terrain to determine if a cell is a wall or not.
    */

    static constexpr float EXPANSION = 0.001f;

    if (terrain->is_wall(row0, col0))
        return false;

    if (terrain->is_wall(row1, col1))
        return false;

    const float CELL_HALFWIDTH  = terrain->mapSizeInWorld / static_cast<float>(terrain->get_map_width()) * 0.5f;
    const float CELL_HALFHEIGHT = terrain->mapSizeInWorld / static_cast<float>(terrain->get_map_height()) * 0.5f;

    // Build line
    const Vec3 WORLD_POS_0 = terrain->get_world_position(row0, col0);
    const Vec3 WORLD_POS_1 = terrain->get_world_position(row1, col1);
    const Vec2 RAY_START    { WORLD_POS_0.x, WORLD_POS_0.z };
    const Vec2 RAY_END      { WORLD_POS_1.x, WORLD_POS_1.z };

    const auto RAYCAST_TO_CELL = [&](int r, int c)->bool
    {
        const Vec3 WALL_WORLDPOS = terrain->get_world_position(r, c);
        // Get 4 Vertices
        const std::vector VERTICES =
        {
            Vec2{ WALL_WORLDPOS.x + (CELL_HALFWIDTH + EXPANSION), WALL_WORLDPOS.z + (CELL_HALFHEIGHT + EXPANSION) },   // TR
            Vec2{ WALL_WORLDPOS.x + (CELL_HALFWIDTH + EXPANSION), WALL_WORLDPOS.z - (CELL_HALFHEIGHT + EXPANSION) },   // BR
            Vec2{ WALL_WORLDPOS.x - (CELL_HALFWIDTH + EXPANSION), WALL_WORLDPOS.z - (CELL_HALFHEIGHT + EXPANSION) },   // BL
            Vec2{ WALL_WORLDPOS.x - (CELL_HALFWIDTH + EXPANSION), WALL_WORLDPOS.z + (CELL_HALFHEIGHT + EXPANSION) }    // TL
        };

        for (size_t i = 0; i < VERTICES.size(); ++i)
        {
            const Vec2 P0 = VERTICES[i];
            const Vec2 P1 = VERTICES[(i + 1) % VERTICES.size()];

            if (line_intersect(RAY_START, RAY_END, P0, P1))
                return true;
        }

        return false;
    };

    // Build bounding box around two points
    const int MIN_ROW = std::min(row0, row1);   const int MIN_COL = std::min(col0, col1);
    const int MAX_ROW = std::max(row0, row1);   const int MAX_COL = std::max(col0, col1);

    for (int r = MIN_ROW; r <= MAX_ROW; ++r)
    {
        for (int c = MIN_COL; c <= MAX_COL; ++c)
        {
            // Skip non walls
            if (!terrain->is_wall(r, c))
                continue;

            if (RAYCAST_TO_CELL(r, c))
                return false;
        }
    }

    return true;
}

void analyze_openness(MapLayer<float> &layer)
{
    /*
        Mark every cell in the given layer with the value 1 / (d * d),
        where d is the distance to the closest wall or edge.  Make use of the
        distance_to_closest_wall helper function.  Walls should not be marked.
    */

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (!terrain->is_valid_grid_position(r, c))
                continue;

            if (terrain->is_wall(r, c))
                continue;

            const float DIST = distance_to_closest_wall(r, c);
            layer.set_value(r, c, 1.0f / (DIST * DIST));
        }
    }
}

void analyze_visibility(MapLayer<float> &layer)
{
    /*
        Mark every cell in the given layer with the number of cells that
        are visible to it, divided by 160 (a magic number that looks good). Make sure
        to cap the value at 1.0 as well.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell. Make use of the is_clear_path
        helper function.
    */

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r0 = 0; r0 < MAP_HEIGHT; ++r0)
    {
        for (int c0 = 0; c0 < MAP_WIDTH; ++c0)
        {
            int numVisible = 0;

            for (int r1 = 0; r1 < MAP_HEIGHT; ++r1)
            {
                for (int c1 = 0; c1 < MAP_WIDTH; ++c1)
                {
                    if (r0 == r1 && c0 == c1)
                        continue;

                    numVisible += is_clear_path(r0, c0, r1, c1) ? 1 : 0;
                }
            }

            float value = static_cast<float>(numVisible) / 160.0f;
            value = std::min(value, 1.0f);

            layer.set_value(r0, c0, value);
        }
    }
}

void analyze_visible_to_cell(MapLayer<float> &layer, int row, int col)
{
    /*
        For every cell in the given layer mark it with 1.0 if it is visible to the given cell,
        0.5 if it isn't visible but is next to a visible cell,
        or 0.0 otherwise.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    const auto CHECK_ADJ_CELLS_VISIBLE = [&](int r, int c)->bool
    {
        const GridPos TOP{ r + 1, c };  const bool TOP_VALID = terrain->is_valid_grid_position(TOP) && !terrain->is_wall(TOP);
        const GridPos RGT{ r, c + 1};   const bool RGT_VALID = terrain->is_valid_grid_position(RGT) && !terrain->is_wall(RGT);
        const GridPos BTM{ r - 1, c };  const bool BTM_VALID = terrain->is_valid_grid_position(BTM) && !terrain->is_wall(BTM);
        const GridPos LFT{ r, c - 1 };  const bool LFT_VALID = terrain->is_valid_grid_position(LFT) && !terrain->is_wall(LFT);
        

        // TOP
        if (TOP_VALID && is_clear_path(row, col, TOP.row, TOP.col))
            return true;

        // RIGHT
        if (RGT_VALID && is_clear_path(row, col, RGT.row, RGT.col))
                return true;

        // BOTTOM
        if (BTM_VALID && is_clear_path(row, col, BTM.row, BTM.col))
                return true;

        // LEFT
        if (LFT_VALID && is_clear_path(row, col, LFT.row, LFT.col))
                return true;

        // TOP RIGHT
        if (TOP_VALID && RGT_VALID && is_clear_path(row, col, TOP.row, RGT.col))
            return true;

        // BOTTOM RIGHT
        if (BTM_VALID && RGT_VALID && is_clear_path(row, col, BTM.row, RGT.col))
            return true;

        // BOTTOM LEFT
        if (BTM_VALID && LFT_VALID && is_clear_path(row, col, BTM.row, LFT.col))
            return true;

        // TOP LEFT
        if (TOP_VALID && LFT_VALID && is_clear_path(row, col, TOP.row, LFT.col))
            return true;

        return false;
    };

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            // Walls are 0
            if (terrain->is_wall(r, c))
            {
                layer.set_value(r, c, 0.0f);
                continue;
            }

            const float VALUE = is_clear_path(row, col, r, c) ? 1.0f : (CHECK_ADJ_CELLS_VISIBLE(r, c) ? 0.5f : 0.0f);
            layer.set_value(r, c, VALUE);
        }
    }
}

void analyze_agent_vision(MapLayer<float> &layer, const Agent *agent)
{
    /*
        For every cell in the given layer that is visible to the given agent,
        mark it as 1.0, otherwise don't change the cell's current value.

        You must consider the direction the agent is facing.  All of the agent data is
        in three dimensions, but to simplify you should operate in two dimensions, the XZ plane.

        Take the dot product between the view vector and the vector from the agent to the cell,
        both normalized, and compare the cosines directly instead of taking the arccosine to
        avoid introducing floating-point inaccuracy (larger cosine means smaller angle).

        Give the agent a field of view slighter larger than 180 degrees.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell. Make use of the is_clear_path
        helper function.
    */

    static const float COS = std::cosf(DirectX::XMConvertToRadians(92.0f));

    const Vec2 AGENT_WORLD_POS{ agent->get_position().x, agent->get_position().z };
    const GridPos AGENT_POS{ terrain->get_grid_position(agent->get_position()) };

    Vec2 viewVector{ agent->get_forward_vector().x, agent->get_forward_vector().z };
    viewVector.Normalize();

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            const Vec2 CELL_WORLD_POS{ terrain->get_world_position(r, c).x, terrain->get_world_position(r, c).z };
            Vec2 agentToCell = CELL_WORLD_POS - AGENT_WORLD_POS;
            agentToCell.Normalize();

            const float DOT = viewVector.Dot(agentToCell);
            if (COS <= DOT)
            {
                if (!is_clear_path(AGENT_POS.row, AGENT_POS.col, r, c))
                    continue;

                layer.set_value(r, c, 1.0f);
            }
        }
    }
}

void propagate_solo_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        For every cell in the given layer:

            1) Get the value of each neighbor and apply decay factor
            2) Keep the highest value from step 1
            3) Linearly interpolate from the cell's current value to the value from step 2
               with the growing factor as a coefficient.  Make use of the lerp helper function.
            4) Store the value from step 3 in a temporary layer.
               A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */
    float tmpLayer[40][40] = { 0.0f };

    const auto DECAY_NEIGHBOURS = [&](int row, int col)->float
    {
        const GridPos TOP{ row + 1, col };  const bool TOP_VALID = terrain->is_valid_grid_position(TOP) && !terrain->is_wall(TOP);
        const GridPos RGT{ row, col + 1 };  const bool RGT_VALID = terrain->is_valid_grid_position(RGT) && !terrain->is_wall(RGT);
        const GridPos BTM{ row - 1, col };  const bool BTM_VALID = terrain->is_valid_grid_position(BTM) && !terrain->is_wall(BTM);
        const GridPos LFT{ row, col - 1 };  const bool LFT_VALID = terrain->is_valid_grid_position(LFT) && !terrain->is_wall(LFT);
        

        float highestValue = 0.0f;

        // TOP
        if (TOP_VALID && is_clear_path(row, col, TOP.row, TOP.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, TOP.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // RIGHT
        if (RGT_VALID && is_clear_path(row, col, RGT.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(RGT.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM
        if (BTM_VALID && is_clear_path(row, col, BTM.row, BTM.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, BTM.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // LEFT
        if (LFT_VALID && is_clear_path(row, col, LFT.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(LFT.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // TOP RIGHT
        if (TOP_VALID && RGT_VALID && is_clear_path(row, col, TOP.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM RIGHT
        if (BTM_VALID && RGT_VALID && is_clear_path(row, col, BTM.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM LEFT
        if (BTM_VALID && LFT_VALID && is_clear_path(row, col, BTM.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // TOP LEFT
        if (TOP_VALID && LFT_VALID && is_clear_path(row, col, TOP.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        return highestValue;
    };
    
    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (terrain->is_wall(r, c))
                continue;

            const float HIGH_VAL = DECAY_NEIGHBOURS(r, c);
            tmpLayer[c][r] = lerp(layer.get_value(r, c), HIGH_VAL, growth);
        }
    }

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            layer.set_value(r, c, tmpLayer[c][r]);
        }
    }
}

void propagate_dual_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        Similar to the solo version, but the values range from -1.0 to 1.0, instead of 0.0 to 1.0

        For every cell in the given layer:

        1) Get the value of each neighbor and apply decay factor
        2) Keep the highest ABSOLUTE value from step 1
        3) Linearly interpolate from the cell's current value to the value from step 2
           with the growing factor as a coefficient.  Make use of the lerp helper function.
        4) Store the value from step 3 in a temporary layer.
           A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */

    float tmpLayer[40][40] = { -1.0f };

    const auto DECAY_NEIGHBOURS = [&](int row, int col)->float
    {
        const GridPos TOP{ row + 1, col };  const bool TOP_VALID = terrain->is_valid_grid_position(TOP) && !terrain->is_wall(TOP);
        const GridPos RGT{ row, col + 1 };  const bool RGT_VALID = terrain->is_valid_grid_position(RGT) && !terrain->is_wall(RGT);
        const GridPos BTM{ row - 1, col };  const bool BTM_VALID = terrain->is_valid_grid_position(BTM) && !terrain->is_wall(BTM);
        const GridPos LFT{ row, col - 1 };  const bool LFT_VALID = terrain->is_valid_grid_position(LFT) && !terrain->is_wall(LFT);
        

        float highestValue = 0.0f;

        // TOP
        if (TOP_VALID && is_clear_path(row, col, TOP.row, TOP.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, TOP.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // RIGHT
        if (RGT_VALID && is_clear_path(row, col, RGT.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(RGT.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM
        if (BTM_VALID && is_clear_path(row, col, BTM.row, BTM.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, BTM.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // LEFT
        if (LFT_VALID && is_clear_path(row, col, LFT.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(LFT.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // TOP RIGHT
        if (TOP_VALID && RGT_VALID && is_clear_path(row, col, TOP.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM RIGHT
        if (BTM_VALID && RGT_VALID && is_clear_path(row, col, BTM.row, RGT.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, RGT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // BOTTOM LEFT
        if (BTM_VALID && LFT_VALID && is_clear_path(row, col, BTM.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(BTM.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        // TOP LEFT
        if (TOP_VALID && LFT_VALID && is_clear_path(row, col, TOP.row, LFT.col))
        {
            const float DECAY_VALUE = layer.get_value(TOP.row, LFT.col) * (1.0f - decay);
            highestValue = std::max(highestValue, DECAY_VALUE);
        }

        return highestValue;
    };
    
    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (terrain->is_wall(r, c))
                continue;

            const float HIGH_VAL = DECAY_NEIGHBOURS(r, c);
            tmpLayer[c][r] = lerp(layer.get_value(r, c), HIGH_VAL, growth);
        }
    }

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            layer.set_value(r, c, tmpLayer[c][r]);
        }
    }
}

void normalize_solo_occupancy(MapLayer<float> &layer)
{
    /*
        Determine the maximum value in the given layer, and then divide the value
        for every cell in the layer by that amount. This will keep the values in the
        range of [0, 1]. Negative values should be left unmodified.
    */
    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    float maxValue = -FLT_MAX;
    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            maxValue = std::max(maxValue, layer.get_value(r, c));
        }
    }

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            const float CURRENT_VALUE = layer.get_value(r, c);
            if (CURRENT_VALUE <= 0.0f)
                continue;

            const float NEW_VALUE = CURRENT_VALUE / maxValue;
            layer.set_value(r, c, NEW_VALUE);
        }
    }
}

void normalize_dual_occupancy(MapLayer<float> &layer)
{
    /*
        Similar to the solo version, but you need to track greatest positive value AND 
        the least (furthest from 0) negative value.

        For every cell in the given layer, if the value is currently positive divide it by the
        greatest positive value, or if the value is negative divide it by -1.0 * the least negative value
        (so that it remains a negative number).  This will keep the values in the range of [-1, 1].
    */

    float maxPositive = 0.0f;
    float minNegative = 0.0f;

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            const float CURRENT_VALUE = layer.get_value(r, c);
            if (CURRENT_VALUE > 0.0f)
            {
                maxPositive = std::max(maxPositive, CURRENT_VALUE);
            }
            else
            {
                minNegative = std::min(minNegative, CURRENT_VALUE);
            }
        }
    }

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            const float CURRENT_VALUE = layer.get_value(r, c);
            float newValue = 0.0f;
            if (CURRENT_VALUE > 0.0f)
            {
                newValue = CURRENT_VALUE / maxPositive;
            }
            else if (CURRENT_VALUE < 0.0f)
            {
                newValue = CURRENT_VALUE / (-1.0f * minNegative);
            }

            layer.set_value(r, c, newValue);
        }
    }
}

void enemy_field_of_view(MapLayer<float> &layer, float fovAngle, float closeDistance, float occupancyValue, AStarAgent *enemy)
{
    /*
        First, clear out the old values in the map layer by setting any negative value to 0.
        Then, for every cell in the layer that is within the field of view cone, from the
        enemy agent, mark it with the occupancy value. Take the dot product between the view
        vector and the vector from the agent to the cell, both normalized, and compare the
        cosines directly instead of taking the arccosine to avoid introducing floating-point
        inaccuracy (larger cosine means smaller angle).

        If the tile is close enough to the enemy (less than closeDistance),
        you only check if it's visible to enemy.  Make use of the is_clear_path
        helper function.  Otherwise, you must consider the direction the enemy is facing too.
        This creates a radius around the enemy that the player can be detected within, as well
        as a fov cone.
    */

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    const float COS = std::cosf(DirectX::XMConvertToRadians(fovAngle * 0.5f));

    const Vec2 ENEMY_WORLD_POS{ enemy->get_position().z, enemy->get_position().x };
    const GridPos ENEMY_POS{ terrain->get_grid_position(enemy->get_position()) };
     const GridPos ENEMY_CELL = terrain->get_grid_position(enemy->get_position());

    Vec2 viewVector{ enemy->get_forward_vector().z, enemy->get_forward_vector().x };
    viewVector.Normalize();

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (layer.get_value(r, c) >= 0.0f)
                continue;

            layer.set_value(r, c, 0.0f);
        }
    }

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (terrain->is_wall(r, c))
                continue;

            const Vec2 CELL_WORLD_POS{ terrain->get_world_position(r, c).z, terrain->get_world_position(r, c).x };
            Vec2 enemyToCell = CELL_WORLD_POS - ENEMY_WORLD_POS;

            bool setValue = false;

            const float CELL_DIFF = static_cast<float>(((ENEMY_POS.row - r) * (ENEMY_POS.row - r)) + ((ENEMY_POS.col - c) * (ENEMY_POS.col - c)));
            if (CELL_DIFF < closeDistance * closeDistance)
            {
                setValue = is_clear_path(ENEMY_POS.row, ENEMY_POS.col, r, c);
            }
            else
            {
                enemyToCell.Normalize();
                const float DOT = viewVector.Dot(enemyToCell);
                setValue = COS <= DOT && is_clear_path(ENEMY_POS.row, ENEMY_POS.col, r, c);;

            }

            if (setValue)
                layer.set_value(r, c, occupancyValue);
        }
    }
}

bool enemy_find_player(MapLayer<float> &layer, AStarAgent *enemy, Agent *player)
{
    /*
        Check if the player's current tile has a negative value, ie in the fov cone
        or within a detection radius.
    */

    const auto &playerWorldPos = player->get_position();

    const auto playerGridPos = terrain->get_grid_position(playerWorldPos);

    // verify a valid position was returned
    if (terrain->is_valid_grid_position(playerGridPos) == true)
    {
        if (layer.get_value(playerGridPos) < 0.0f)
        {
            return true;
        }
    }

    // player isn't in the detection radius or fov cone, OR somehow off the map
    return false;
}

bool enemy_seek_player(MapLayer<float> &layer, AStarAgent *enemy)
{
    /*
        Attempt to find a cell with the highest nonzero value (normalization may
        not produce exactly 1.0 due to floating point error), and then set it as
        the new target, using enemy->path_to.

        If there are multiple cells with the same highest value, then pick the
        cell closest to the enemy.

        Return whether a target cell was found.
    */

    const int MAP_WIDTH     = terrain->get_map_width();
    const int MAP_HEIGHT    = terrain->get_map_height();

    const Vec2 ENEMY_WORLD_POS{ enemy->get_position().z, enemy->get_position().x };
    const GridPos ENEMY_CELL{ terrain->get_grid_position(enemy->get_position()) };

    std::optional<Vec3> closestPoint;
    float closestDist = FLT_MAX;
    float highestValue = -FLT_MAX;

    for (int r = 0; r < MAP_HEIGHT; ++r)
    {
        for (int c = 0; c < MAP_WIDTH; ++c)
        {
            if (ENEMY_CELL.row == r && ENEMY_CELL.col == c)
                continue;

            if (terrain->is_wall(r, c))
                continue;

            const float CELL_VALUE = layer.get_value(r, c);
            const Vec2 CELL_V2{ terrain->get_world_position(r, c).z, terrain->get_world_position(r, c).x };
            const float D_SQ = Vec2::DistanceSquared(ENEMY_WORLD_POS, CELL_V2);
            if (std::abs(CELL_VALUE) < 0.001f && CELL_VALUE > highestValue || (CELL_VALUE == highestValue && D_SQ < closestDist))
            {
                highestValue    = CELL_VALUE;
                closestDist     = D_SQ;
                closestPoint    = terrain->get_world_position(r, c); 
            }
        }
    }

    if (!closestPoint.has_value())
        return false;

    enemy->path_to(closestPoint.value());
    return true; 
}
