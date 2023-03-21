#include <pch.h>
#include "Projects/ProjectOne.h"
#include "Agent/CameraAgent.h"

void createSpawner();
void createBuilder();
void createProgrammer();

void ProjectOne::setup()
{
    // Create your inital agents
    createSpawner();
    createBuilder();
    createProgrammer();

    // you can technically load any map you want, even create your own map file,
    // but behavior agents won't actually avoid walls or anything special, unless you code that yourself
    // that's the realm of project 2 though
    terrain->goto_map(0);

    // you can also enable the pathing layer and set grid square colors as you see fit
    // works best with map 0, the completely blank map
    terrain->pathLayer.set_enabled(true);
    terrain->pathLayer.set_value(0, 0, Colors::Red);

    // camera position can be modified from this default as well
    auto camera = agents->get_camera_agent();
    camera->set_position(Vec3(-70.0f, 100.0f, terrain->mapSizeInWorld * 0.5f));
    camera->set_pitch(0.610865); // 35 degrees
}

void createSpawner()
{
    BehaviorAgent* spawner = agents->create_behavior_agent("Spawner", BehaviorTreeTypes::Spawner);

    // Set transform
    spawner->set_position(Vec3{ 1.0f, 0.0f, 5.0f });
    spawner->set_yaw(PI * 0.5f);
    spawner->set_color(Vec3{ 1.0f, 1.0f, 0.0f });
     
    // Set blackboard variables
    Blackboard& spawnerBlackboard = spawner->get_blackboard();

    spawnerBlackboard.set_value<std::string>("Role", "Spawner");
    spawnerBlackboard.set_value<int>("SpawnCount", 0);
    spawnerBlackboard.set_value<int>("SpawnLimit", 25);
    spawnerBlackboard.set_value<bool>("SpawnState", true);
    spawnerBlackboard.set_value<Vec3>("TargetColour", Vec3{ 1.0f, 0.0f, 0.0f });
}

void createBuilder()
{
    BehaviorAgent* builder = agents->create_behavior_agent("Builder", BehaviorTreeTypes::Builder);

    // Set transform
    builder->set_position(Vec3{ 1.0f, 0.0f, 45.0f });
    builder->set_yaw(PI * 0.5f);
    builder->set_color(Vec3{ 1.0f, 1.0f, 0.0f });

    // Set blackboard variables
    Blackboard& builderBlackboard = builder->get_blackboard();
    builderBlackboard.set_value<std::string>("Role", "Builder");
    builderBlackboard.set_value<int>("SubTreeCount", 1);
    builderBlackboard.set_value<int>("SubTreeIndex", 0);
    builderBlackboard.set_value<int>("TargetID", -1);
    builderBlackboard.set_value<float>("ForwardRange", 10.0f);
}

void createProgrammer()
{
    BehaviorAgent* programmer = agents->create_behavior_agent("Programmer", BehaviorTreeTypes::Programmer);

    // Set transform
    programmer->set_position(Vec3{ 1.0f, 0.0f, 90.0f });
    programmer->set_yaw(PI * 0.5f);
    programmer->set_color(Vec3{ 1.0f, 1.0f, 0.0f });

    // Set blackboard variables
    Blackboard& programmerBlackboard = programmer->get_blackboard();
    programmerBlackboard.set_value<std::string>("Role", "Programmer");
    programmerBlackboard.set_value<int>("SubTreeCount", 1);
    programmerBlackboard.set_value<int>("SubTreeIndex", 0);
    programmerBlackboard.set_value<int>("TargetID", -1);
    programmerBlackboard.set_value<float>("ForwardRange", 10.0f);
}
