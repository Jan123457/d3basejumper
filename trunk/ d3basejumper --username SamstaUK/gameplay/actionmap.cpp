
#include "headers.h"
#include "actionmap.h"
#include "gameplay.h"

const wchar_t* ActionChannel::getInputActionDescription(void)
{
    switch( _inputAction )
    {
    case iaLeft : return Gameplay::iLanguage->getUnicodeString(127);
    case iaRight : return Gameplay::iLanguage->getUnicodeString(128);
    case iaForward : return Gameplay::iLanguage->getUnicodeString(129);
    case iaBackward : return Gameplay::iLanguage->getUnicodeString(130);
    case iaLeftWarp : return Gameplay::iLanguage->getUnicodeString(131);
    case iaRightWarp : return Gameplay::iLanguage->getUnicodeString(132);
    case iaHeadLeft : return Gameplay::iLanguage->getUnicodeString(133);
    case iaHeadRight : return Gameplay::iLanguage->getUnicodeString(134);
    case iaHeadUp : return Gameplay::iLanguage->getUnicodeString(135);
    case iaHeadDown : return Gameplay::iLanguage->getUnicodeString(136);
    case iaZoomIn : return Gameplay::iLanguage->getUnicodeString(137);
    case iaZoomOut : return Gameplay::iLanguage->getUnicodeString(138);
    case iaCameraMode0 : return Gameplay::iLanguage->getUnicodeString(139);
    case iaCameraMode1 : return Gameplay::iLanguage->getUnicodeString(140);
    case iaCameraMode2 : return L"iaCameraMode2<unassigned>"; // flyby camera
    case iaCameraMode3 : return L"iaCameraMode3<unassigned>"; // free camera
    case iaPhase : return Gameplay::iLanguage->getUnicodeString(141);
    case iaModifier : return Gameplay::iLanguage->getUnicodeString(142);
    case iaGlobalDeceleration: return Gameplay::iLanguage->getUnicodeString(148);
    case iaGlobalAcceleration: return Gameplay::iLanguage->getUnicodeString(149);
    case iaAltimeterMode: return Gameplay::iLanguage->getUnicodeString(220);
    case iaIncreaseWarningAltitude: return Gameplay::iLanguage->getUnicodeString(221);
    case iaDecreaseWarningAltitude: return Gameplay::iLanguage->getUnicodeString(222);
    case iaSwitchHUDMode: return Gameplay::iLanguage->getUnicodeString(364);
    case iaWLO: return Gameplay::iLanguage->getUnicodeString(143);
    case iaHook: return Gameplay::iLanguage->getUnicodeString(144);
    case iaAccelerateFlightTime: return Gameplay::iLanguage->getUnicodeString(149);
    case iaDecelerateFlightTime: return Gameplay::iLanguage->getUnicodeString(148);
    }
    return NULL;
}