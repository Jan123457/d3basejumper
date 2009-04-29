#ifndef ACTION_MAPPING_INCLUDED
#define ACTION_MAPPING_INCLUDED

#include "headers.h"
#include "../shared/ccor.h"
#include "../shared/input.h"
#include "tinyxml.h"

using namespace ccor;

/**
 * input actions
 */

enum InputAction
{
    iaLeft,
    iaRight,
    iaForward,
    iaBackward,
    iaLeftWarp,
    iaRightWarp,
    iaHeadLeft,
    iaHeadRight,
    iaHeadUp,
    iaHeadDown,
    iaZoomIn,
    iaZoomOut,
    iaCameraMode0,
    iaCameraMode1,
    iaCameraMode2,
    iaCameraMode3,
    iaPhase,
    iaModifier,  
    iaGlobalDeceleration,
    iaGlobalAcceleration,
    iaAltimeterMode,
    iaIncreaseWarningAltitude,
    iaDecreaseWarningAltitude,
    iaSwitchHUDMode,
    iaWLO,
    iaHook,
    iaAccelerateFlightTime,
    iaDecelerateFlightTime,
    iaIncreaseMusicVolume,
    iaDecreaseMusicVolume
};

/**
 * abstract action channel
 */

class ActionChannel
{
protected:
    InputAction _inputAction;
public:
    ActionChannel(InputAction inputAction) : _inputAction(inputAction) {}
    virtual ~ActionChannel() {}
public:
    InputAction getInputAction(void);
    const wchar_t* getInputActionDescription(void);
public:
    virtual const wchar_t* getInputActionHint(void) = 0;
    virtual void setup(input::MouseState* mouseState, input::KeyboardState* keyboardState) = 0;
    virtual void update(float dt, input::MouseState* mouseState, input::KeyboardState* keyboardState) = 0;
    virtual float getAmplitude(void) = 0;
    virtual void setAmplitude(float amplitude) = 0;
    virtual bool getTrigger(void) = 0;
    virtual void reset(void) = 0;
};

/**
 * action channel container
 */

typedef std::pair<InputAction,ActionChannel*> ActionChannelT;
typedef std::map<InputAction,ActionChannel*> ActionChannelM;
typedef ActionChannelM::iterator ActionChannelI;

/**
 * button channel
 */

class ButtonChannel : public ActionChannel
{
private:
    unsigned int _deviceCode; // 0 is keyboard 1 is mouse
    unsigned int _keyCode;
    bool         _smoothMode;
    float        _amplitude;
    float        _ascendingVel;
    float        _descendingVel;
private:
    void setupDefault(void);
public:
    // class implementation
    ButtonChannel(InputAction inputAction);
    ButtonChannel(InputAction inputAction, float ascendingVel, float descendingVel);
    // ActionChannel
    virtual const wchar_t* getInputActionHint(void);
    virtual void setup(input::MouseState* mouseState, input::KeyboardState* keyboardState);
    virtual void update(float dt, input::MouseState* mouseState, input::KeyboardState* keyboardState);
    virtual float getAmplitude(void);
    virtual void setAmplitude(float amplitude);
    virtual bool getTrigger(void);
    virtual void reset(void);
    // ButtonChannel specifics
    void setup(unsigned int deviceCode, unsigned int keyCode);
};

/**
 * mouse channel
 */

enum MouseAxis
{
    xAxis,
    yAxis,
    zAxis
};

class MouseChannel : public ActionChannel
{
private:
    MouseAxis _axis;
    int       _direction;
    float     _amplitude;
public:
    static float mouseSensitivityX;
    static float mouseSensitivityY;
    static float mouseSensitivityZ;
private:
    void setupDefault(void);
public:
    // class implementation
    MouseChannel(InputAction inputAction);
    // ActionChannel
    virtual const wchar_t* getInputActionHint(void);
    virtual void setup(input::MouseState* mouseState, input::KeyboardState* keyboardState);
    virtual void update(float dt, input::MouseState* mouseState, input::KeyboardState* keyboardState);
    virtual float getAmplitude(void);
    virtual void setAmplitude(float amplitude);
    virtual bool getTrigger(void);
    virtual void reset(void);
};

#endif