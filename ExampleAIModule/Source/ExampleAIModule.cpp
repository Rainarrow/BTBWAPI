#include "ExampleAIModule.h"
#include "BehaviorTree.h"
#include "ActionBehaviors.h"
#include "ConditionBehaviors.h"
#include "Blackboard.h"
#include "UnitGroup.h"
#include "BTBuilder.h"
#include <hash_map>
#include <iostream>

using namespace BWAPI;
using namespace Filter;

void ExampleAIModule::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if ( Broodwar->isReplay() )
	{
		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;
		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for(auto p = players.begin(); p != players.end(); ++p)
		{
			// Only print the player if they are not an observer
			if ( !p->isObserver() )
			Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}
	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if ( Broodwar->enemy() ) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}

	m_blackboard = new Blackboard();
	CreateUnitGroups();
}

void ExampleAIModule::onEnd(bool isWinner)
{
	// Called when the game ends
	if ( isWinner )
	{
		// Log your win here!
	}
	SafeDelete(m_blackboard);
	SafeDelete(m_tacticalBehavior);
	for(auto it = m_unitGroups.begin(); it != m_unitGroups.end(); ++it)
		SafeDelete(*it);
	m_unitGroups.clear();
}

void ExampleAIModule::onFrame()
{
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );

	// Return if the game is a replay or is paused
	if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;

	s_blackboard = m_blackboard;
	m_tacticalBehavior->Tick(0.067f);

	for(auto it = m_unitGroups.begin(); it != m_unitGroups.end(); ++it)
		(*it)->Update();

	/////////////////////////////
	// DEBUG DRAW HEALTH BAR
	/////////////////////////////

	Unitset myUnits = Broodwar->self()->getUnits();

	const int hpBarLength = 10;
	for (Unitset::iterator u = myUnits.begin(); u != myUnits.end(); ++u)
	{
		int totalHP = u->getHitPoints() + u->getShields();
		int maxHP = u->getType().maxHitPoints() + u->getType().maxShields();
		float ratio = totalHP / maxHP;
		Broodwar->drawLine(CoordinateType::Map, u->getPosition().x - hpBarLength, u->getPosition().y, u->getPosition().x + (hpBarLength * ratio), u->getPosition().y, Colors::Green);
	}
	/*
	// Iterate through all the units that we own
	Unitset myUnits = Broodwar->self()->getUnits();
	for ( Unitset::iterator u = myUnits.begin(); u != myUnits.end(); ++u )
	{
		UnitInterface * unit = *u;
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if ( !u->exists() )
			continue;

		// Ignore the unit if it has one of the following status ailments
		if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
			continue;

		// Ignore the unit if it is in one of the following states
		if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if ( !u->isCompleted() || u->isConstructing() )
			continue;


		// Finally make the unit do some stuff!
		if(!u->getType().isBuilding())
		{
			if(u->isIdle())
			{
				TILE_SIZE;
				int width = Broodwar->mapWidth();
				int height = Broodwar->mapHeight();
				TilePosition tpos = u->getTilePosition();
				Position pos = u->getPosition();
				pos.x += 200;
				pos.y += 100;
				//Position pos = tpos;
				u->attack(pos);
			}
		}
		// If the unit is a worker unit
		if ( u->getType().isWorker() )
		{
			// if our worker is idle
			if ( u->isIdle() )
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if ( u->isCarryingGas() || u->isCarryingMinerals() )
				{
					u->returnCargo();
				}
				else if ( !u->getPowerUp() )  // The worker cannot harvest anything if it
				{
					// is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
					if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
					{
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}
				} // closure: has no powerup
			} // closure: if idle

		}
		else if ( u->getType().isResourceDepot() ) // A resource depot is a Command Center, Nexus, or Hatchery
		{

			// Order the depot to construct more workers! But only when it is idle.
			if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) )
			{
				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame
				// so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				Broodwar->registerEvent([pos,lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
										nullptr,    // condition
										Broodwar->getLatencyFrames());  // frames to run

				// Retrieve the supply provider type in the case that we have run out of supplies
				UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (  lastErr == Errors::Insufficient_Supply &&
						lastChecked + 400 < Broodwar->getFrameCount() &&
						Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 )
				{
					lastChecked = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed
					Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
															(IsIdle || IsGatheringMinerals) &&
															IsOwned);
					// If a unit was found
					if ( supplyBuilder )
					{
						if ( supplyProviderType.isBuilding() )
						{
							TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
							if ( targetBuildLocation )
							{
								// Register an event that draws the target build location
								Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
														{
															Broodwar->drawBoxMap( Position(targetBuildLocation),
																				Position(targetBuildLocation + supplyProviderType.tileSize()),
																				Colors::Blue);
														},
														nullptr,  // condition
														supplyProviderType.buildTime() + 100 );  // frames to run

								// Order the builder to construct the supply structure
								supplyBuilder->build( supplyProviderType, targetBuildLocation );
							}
						}
						else
						{
							// Train the supply provider (Overlord) if the provider is not a structure
							supplyBuilder->train( supplyProviderType );
						}
					} // closure: supplyBuilder is valid
				} // closure: insufficient supply
			} // closure: failed to train idle unit
		}
	} // closure: unit iterator
	*/
}

void ExampleAIModule::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if ( target )
	{
	// if so, print the location of the nuclear strike target
	Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else 
	{
	// Otherwise, ask other players where the nuke is!
	Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
	//Broodwar->sendText("onUnitDiscover %s!", unit->getType().getName().c_str());
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
	//Broodwar->sendText("onUnitEvade %s!", unit->getType().getName().c_str());
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
	//Broodwar->sendText("onUnitShow %s!", unit->getType().getName().c_str());
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
	//Broodwar->sendText("onUnitHide %s!", unit->getType().getName().c_str());
}

void ExampleAIModule::CreateUnitGroups()
{
	{
		BTBuilder BT(new Selector);
		BT
			.AddSequence()
				.AddAction(new FindCriticalTarget)
				.AddAction(new Delay(2))
			.End()
			.AddAction(new Delay(2));

		m_tacticalBehavior = BT.GetTree();
	}

	{
		for(int i = 0; i < 8; ++i)
		{
			BTBuilder BT(new ActiveSelector);
			BT
				.AddSequence()
					.AddAction(new IsLowHp)
					.AddAction(new IsBeingAttacked)
					.AddDecorator(new Negate)
						.AddAction(new IsReadyToAttack)
					.End()
					.AddSequence()
						.AddAction(new CalculateFallbackPos)
						.AddAction(new MoveTo)
					.End()
				.End()
				.AddSequence()
					.AddAction((new FindTarget))
					.AddAction(new Attack)
				.End();
				//.AddSequence()
					//.AddAction(new Delay(2))
				//.End();

			// Own unit detected, create group
			UnitGroup * group = new UnitGroup(UnitTypes::Enum::Protoss_Dragoon, 1, BT.GetTree(), m_blackboard);
			m_unitGroups.push_back(group);
		}
	}
	{
		BTBuilder BT(new ActiveSelector);
		BT
			.AddSequence()
				.AddAction(new CheckCriticalTarget)
				.AddAction(new Attack)
			.End()
			.AddSequence()
				.AddAction((new FindGroupMoveToEnemy))
				.AddAction(new GroupMoveTo)
				.AddAction(new FindCenterAttack)
				.AddAction(new GroupMoveTo)
			.End()
			.AddSequence()
				.AddAction(new Delay(2))
			.End();

		// Own unit detected, create group
		UnitGroup * group = new UnitGroup(UnitTypes::Enum::Protoss_Zealot, 3, BT.GetTree(), m_blackboard);
		m_unitGroups.push_back(group);
	}
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
	//Broodwar->sendText("onUnitCreate %s!", unit->getType().getName().c_str());
	if(unit->getPlayer() == Broodwar->self())
	{
		// Own unit detected, add to group
		auto it = m_unitGroups.begin();
		for( ; it != m_unitGroups.end(); ++it)
		{
			UnitGroup * group = *it;
			if(group->AddUnit(unit))
				break;
		}

		if(it == m_unitGroups.end())
		{
			Broodwar->sendText("No group: %s!", unit->getType().getName().c_str());
		}
	}

	/*
	if ( Broodwar->isReplay() )
	{
		// if we are in a replay, then we will print out the build order of the structures
		if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
		{
			int seconds = Broodwar->getFrameCount()/24;
			int minutes = seconds/60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
	*/
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
	for(auto it = m_unitGroups.begin(); it != m_unitGroups.end(); ++it)
	{
		(*it)->RemoveUnit(unit);
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
	if ( Broodwar->isReplay() )
	{
	// if we are in a replay, then we will print out the build order of the structures
	if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
	{
		int seconds = Broodwar->getFrameCount()/24;
		int minutes = seconds/60;
		seconds %= 60;
		Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
	}
	}
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
}
