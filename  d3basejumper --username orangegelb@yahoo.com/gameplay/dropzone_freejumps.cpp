
#include "headers.h"
#include "dropzone.h"
#include "airplane.h"
#include "hud.h"
#include "npc.h"
#include "npccameraman.h"
#include "affinstructor.h"
#include "smokejet.h"
#include "equip.h"

/**
 * common methods
 */

void castSkydivers(Mission* mission, Airplane* airplane, unsigned int numSkydivers)
{
    assert( mission->getPlayer() );

    // detect player level
    float playerLevel = 0.25f * (
        mission->getPlayer()->getVirtues()->getEnduranceSkill() + 
        mission->getPlayer()->getVirtues()->getPerceptionSkill() +
        mission->getPlayer()->getVirtues()->getRiggingSkill() +
        mission->getPlayer()->getVirtues()->getTrackingSkill()
    );

    // detect player is a LICENSED_CHAR
    bool playerIsLicensed = mission->getScene()->getCareer()->getLicensedFlag();

    // build list of same level npc    
    std::vector<unsigned int> npcs;
    database::NPCInfo::select( playerLevel, 0.126f, !playerIsLicensed, npcs );

    for( unsigned i=0; i<numSkydivers; i++ )
    {
        if( npcs.size() )
        {
            // select NPC
            unsigned int index = getCore()->getRandToolkit()->getUniformInt() % npcs.size();
            unsigned int npcId = npcs[index];
            npcs.erase( &npcs[index] );
            // create NPC
            NPC* npc = new NPC( mission, npcId, airplane, NULL, CatToy::wrap( mission->getPlayer() ) );
            // setup cameraman behaviour
            npc->setProgram( new NPCCameraman( npc ) );
            // setup brief signature
            npc->getJumper()->setSignatureType( stBrief );
        }
    }
}

void castFreejump(Actor* parent, float altitude)
{
    Mission* mission = dynamic_cast<Mission*>( parent ); assert( mission );

    // setup audible altimeter
    Altimeter::setAudibleAltimeter( mission->getScene()->getCareer(), true, 70000.0f );

    // cast airplane object
    AirplaneDesc airplaneDesc;
    airplaneDesc.templateClump = parent->getScene()->findClump( "Airplane01" ); assert( airplaneDesc.templateClump );
    airplaneDesc.propellerFrame = "PropellerSound";
    airplaneDesc.propellerSound = "./res/sounds/aircrafts/airplane.ogg";
    airplaneDesc.exitPointFrames.push_back( "LeftDoor01" );
    airplaneDesc.exitPointFrames.push_back( "RightDoor01" );
    airplaneDesc.exitPointFrames.push_back( "RightDoor02" );
    airplaneDesc.cameraFrames.push_back( "Cesna_Cam" );
    airplaneDesc.cameraFrames.push_back( "Cesna_Cam1" );
    airplaneDesc.cameraFrames.push_back( "Cesna_Cam2" );
    airplaneDesc.animationSpeed = 0.5f;
    airplaneDesc.initAltitude  = altitude;
    airplaneDesc.lastAltitude  = 5000.0f;
    airplaneDesc.loweringSpeed = 500.0f;
    Airplane* airplane = new Airplane( mission, &airplaneDesc );

    // cast player on airplane
    mission->setPlayer( new Jumper( mission, airplane, NULL, NULL, NULL, NULL ) );

    // setup full signature for player
    mission->getPlayer()->setSignatureType( stFull );

    // cast NPCs
    castSkydivers( mission, airplane, 2 );

    // cast goals
    new GoalDropzone( mission->getPlayer() );
    new GoalLanding( mission->getPlayer() );
    new GoalStateOfHealth( mission->getPlayer() );
    new GoalStateOfGear( mission->getPlayer() );
    new GoalExperience( mission->getPlayer() );

    // play original music for this mission
    Gameplay::iGameplay->playSoundtrack( "./res/sounds/music/dirty_moleculas_action.ogg" );
}

/**
 * startup callbacks
 */

void castingCallback_Freejump_1000(Actor* parent)
{
    castFreejump( parent, 100000.0f );
}

void castingCallback_Freejump_1500(Actor* parent)
{
    castFreejump( parent, 150000.0f );
}

void castingCallback_Freejump_2000(Actor* parent)
{
    castFreejump( parent, 200000.0f );
}

void castingCallback_Freejump_2500(Actor* parent)
{
    castFreejump( parent, 250000.0f );
}

void castingCallback_Freejump_3000(Actor* parent)
{
    castFreejump( parent, 300000.0f );
}

/**
 * equip callback
 */

bool equipCallback_Freejump(Career* career, float windAmbient, float windBlast, database::MissionInfo* missionInfo)
{
    // equip best rig and canopy
    if( !equipBestSkydivingEquipment( career, windAmbient, windBlast ) ) return false;

    // set slider up and 36' pilotchute 
    career->getVirtues()->equipment.sliderOption = ::soUp;
    career->getVirtues()->equipment.pilotchute   = 0;

    return true;
}

bool equipCallback_FreejumpCommonSuit(Career* career, float windAmbient, float windBlast, database::MissionInfo* missionInfo)
{
    // equip best rig and canopy
    if( !equipBestSkydivingEquipment( career, windAmbient, windBlast ) ) return false;

    // equip best common suit
    if( !equipBestSuit( career, windAmbient, windBlast ) ) return false;

    // set slider up and 36' pilotchute 
    career->getVirtues()->equipment.sliderOption = ::soUp;
    career->getVirtues()->equipment.pilotchute   = 0;

    return true;
}

bool equipCallback_FreejumpWingsuit(Career* career, float windAmbient, float windBlast, database::MissionInfo* missionInfo)
{
    // equip best rig and canopy
    if( !equipBestSkydivingEquipment( career, windAmbient, windBlast ) ) return false;

    // equip best wingsuit
    if( !equipBestWingsuit( career, windAmbient, windBlast ) ) return false;

    // set slider up and 36' pilotchute 
    career->getVirtues()->equipment.sliderOption = ::soUp;
    career->getVirtues()->equipment.pilotchute   = 0;

    return true;
}