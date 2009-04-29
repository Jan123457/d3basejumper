
#include "headers.h"
#include "jumper.h"
#include "gameplay.h"

void SpinalCord::mapActionChannels(void)
{
    left      = Gameplay::iGameplay->getActionChannel( iaLeft )->getAmplitude();
    right     = Gameplay::iGameplay->getActionChannel( iaRight )->getAmplitude();
    up        = Gameplay::iGameplay->getActionChannel( iaForward )->getAmplitude();
    down      = Gameplay::iGameplay->getActionChannel( iaBackward )->getAmplitude();
    leftWarp  = Gameplay::iGameplay->getActionChannel( iaLeftWarp )->getAmplitude();
    rightWarp = Gameplay::iGameplay->getActionChannel( iaRightWarp )->getAmplitude();
    phase     = Gameplay::iGameplay->getActionChannel( iaPhase )->getTrigger();
    modifier  = Gameplay::iGameplay->getActionChannel( iaModifier )->getTrigger();
    wlo       = Gameplay::iGameplay->getActionChannel( iaWLO )->getTrigger();
    hook      = Gameplay::iGameplay->getActionChannel( iaHook )->getTrigger();
}

void SpinalCord::reset(void)
{
    left      = 0.0f;
    right     = 0.0f;
    up        = 0.0f;
    down      = 0.0f;
    leftWarp  = 0.0f;
    rightWarp = 0.0f;
    phase     = false;
    modifier  = false;
    wlo       = false;
    hook      = false;
}