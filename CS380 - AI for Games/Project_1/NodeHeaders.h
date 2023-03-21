#pragma once

// Include all node headers in this file

// Example Control Flow Nodes
#include "ControlFlow/C_ParallelSequencer.h"
#include "ControlFlow/C_RandomSelector.h"
#include "ControlFlow/C_Selector.h"
#include "ControlFlow/C_Sequencer.h"

// Student Control Flow Nodes
#include "ControlFlow/C_IfElse.h"
#include "ControlFlow/C_Switch.h"


// Example Decorator Nodes
#include "Decorator/D_Delay.h"
#include "Decorator/D_InvertedRepeater.h"
#include "Decorator/D_RepeatFourTimes.h"

// Student Decorator Nodes
#include "Decorator/D_IfSpawnAvailable.h"
#include "Decorator/D_IfSpawnerEnabled.h"
#include "Decorator/D_IfSpawnerDisabled.h"
#include "Decorator/D_IfRobotInForwardRange.h"
#include "Decorator/D_IfTargetSizeHit.h"

// Example Leaf Nodes
#include "Leaf/L_CheckMouseClick.h"
#include "Leaf/L_Idle.h"
#include "Leaf/L_MoveToFurthestAgent.h"
#include "Leaf/L_MoveToMouseClick.h"
#include "Leaf/L_MoveToRandomPosition.h"

// Student Leaf Nodes
/// Spawner
#include "Leaf/L_SpawnRobot.h"
#include "Leaf/L_TurnOffSpawner.h"
#include "Leaf/L_ChangeAgentColour.h"
/// Builder & Programmer
#include "Leaf/L_FindClosestRobot.h"
#include "Leaf/L_BuildRobot.h"
#include "Leaf/L_NextSubTree.h"
#include "Leaf/L_FinishBuilding.h"
#include "Leaf/L_PrevSubTree.h"
#include "Leaf/L_IncreaseRobotSize.h"
#include "Leaf/L_ProgramRobot.h"
#include "Leaf/L_SetRobotRandomColour.h"
#include "Leaf/L_FinishProgramming.h"
/// Robot
#include "Leaf/L_IsRobotBuilding.h"
#include "Leaf/L_MoveAlongZ.h"
