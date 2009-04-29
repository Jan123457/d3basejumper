
#include "headers.h"
#include "hud.h"

/**
 * actor abstracts
 */

void Variometer::onUpdateActivity(float dt)
{
    _timeout -= dt;
    if( _timeout < 0 ) _timeout = 0;

    if( _timeout > 0 )
    {
        resetIndicator( _window->getPanel()->find( "Digit4" ) );
        resetIndicator( _window->getPanel()->find( "Digit3" ) );
        resetIndicator( _window->getPanel()->find( "Digit2" ) );
        resetIndicator( _window->getPanel()->find( "Digit1" ) );
        resetIndicator( _window->getPanel()->find( "Digit0" ) );
    }
    else
    {
        // retrieve parent actor altitude
        Vector3f parentVel = _parent->getVel();
        float fall = parentVel[1];

        // draw signum digit
        setSignumIndicator( _window->getPanel()->find( "Digit4" ), fall );    

        // absolute units conversion
        float meters = fabs( fall ) * 0.01f * 10;
        int units = int( meters );

        // digits pickup
        int digit = units % 10;
        setIndicator( _window->getPanel()->find( "Digit0" ), digit );
    
        units = units / 10;
        digit = units % 10;
        setIndicator( _window->getPanel()->find( "Digit1" ), digit );

        units = units / 10;
        digit = units % 10;
        setIndicator( _window->getPanel()->find( "Digit2" ), digit );

        units = units / 10;
        digit = units % 10;
        setIndicator( _window->getPanel()->find( "Digit3" ), digit );
    }
}

void Variometer::onEvent(Actor* initiator, unsigned int eventId, void* eventData)
{
    if( eventId == EVENT_HUD_HIDE )
    {
        if( _window->getPanel()->getParent() )
        {
            _window->getPanel()->getParent()->removePanel( _window->getPanel() );
        }
    }
}

/**
 * class implemetation
 */
   
Variometer::Variometer(Actor* parent) : HUD( parent )
{
    // window
    _window = Gameplay::iGui->createWindow( "Variometer" ); assert( _window );
    Gameplay::iGui->getDesktop()->insertPanel( _window->getPanel() );
    _window->align( gui::atTop, 0, gui::atRight, 0 );

    // timeout
    _timeout = getCore()->getRandToolkit()->getUniform( 1, 3 );
}

Variometer::~Variometer()
{
    Gameplay::iGui->getDesktop()->removePanel( _window->getPanel() );
    _window->getPanel()->release();
}