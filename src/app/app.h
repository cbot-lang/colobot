// * This file is part of the COLOBOT source code
// * Copyright (C) 2001-2008, Daniel ROUX & EPSITEC SA, www.epsitec.ch
// * Copyright (C) 2012, Polish Portal of Colobot (PPC)
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.

/**
 * \file app/app.h
 * \brief CApplication class
 */

#pragma once

#include "common/global.h"
#include "common/singleton.h"
#include "graphics/core/device.h"
#include "graphics/engine/engine.h"
#include "graphics/opengl/gldevice.h"

#include <string>
#include <vector>


class CInstanceManager;
class CEvent;
class CRobotMain;
class CSoundInterface;

/**
 * \struct JoystickDevice
 * \brief Information about a joystick device
 */
struct JoystickDevice
{
    //! Device index (-1 = invalid device)
    int index;
    //! Device name
    std::string name;
    //! Number of axes (only available after joystick opened)
    int axisCount;
    //! Number of buttons (only available after joystick opened)
    int buttonCount;

    JoystickDevice()
        : index(-1), axisCount(0), buttonCount(0) {}
};

/**
 * \enum VideoQueryResult
 * \brief Result of querying for available video resolutions
 */
enum VideoQueryResult
{
    VIDEO_QUERY_ERROR,
    VIDEO_QUERY_NONE,
    VIDEO_QUERY_ALL,
    VIDEO_QUERY_OK
};


/**
 * \enum TrackedKeys
 * \brief Keys (or kmods) whose state (pressed/released) is tracked by CApplication
 */
enum TrackedKey
{
    TRKEY_SHIFT,
    TRKEY_CONTROL,
    TRKEY_NUM_UP,
    TRKEY_NUM_DOWN,
    TRKEY_NUM_LEFT,
    TRKEY_NUM_RIGHT,
    TRKEY_NUM_PLUS,
    TRKEY_NUM_MINUS,
    TRKEY_PAGE_UP,
    TRKEY_PAGE_DOWN,
    TRKEY_MAX
};

/**
 * \enum InputSlot
 * \brief Available slots for input bindings
 */
enum InputSlot
{
    INPUT_SLOT_LEFT    = 0,
    INPUT_SLOT_RIGHT   = 1,
    INPUT_SLOT_UP      = 2,
    INPUT_SLOT_DOWN    = 3,
    INPUT_SLOT_GUP     = 4,
    INPUT_SLOT_GDOWN   = 5,
    INPUT_SLOT_CAMERA  = 6,
    INPUT_SLOT_DESEL   = 7,
    INPUT_SLOT_ACTION  = 8,
    INPUT_SLOT_NEAR    = 9,
    INPUT_SLOT_AWAY    = 10,
    INPUT_SLOT_NEXT    = 11,
    INPUT_SLOT_HUMAN   = 12,
    INPUT_SLOT_QUIT    = 13,
    INPUT_SLOT_HELP    = 14,
    INPUT_SLOT_PROG    = 15,
    INPUT_SLOT_VISIT   = 16,
    INPUT_SLOT_SPEED10 = 17,
    INPUT_SLOT_SPEED15 = 18,
    INPUT_SLOT_SPEED20 = 19,
    INPUT_SLOT_SPEED30 = 20,
    INPUT_SLOT_AIMUP   = 21,
    INPUT_SLOT_AIMDOWN = 22,
    INPUT_SLOT_CBOT    = 23,

    INPUT_SLOT_MAX
};

/**
 * \struct InputBinding
 * \brief Settable binding for user input
 */
struct InputBinding
{
    //! Key
    unsigned int key;
    //! Key modifier (e.g. shift, control)
    unsigned int kmod;
    //! Joystick button
    unsigned int joy;

    inline InputBinding()
    {
        Reset();
    }

    inline void Reset()
    {
        key = kmod = joy = static_cast<unsigned int>(-1);
    }
};

struct ApplicationPrivate;

/**
 * \class CApplication
 * \brief Main application
 *
 * This class is responsible for main application execution, including creating
 * and handling main application window, receiving events, etc.
 *
 * It is a singleton class with only one instance that can be created.
 *
 * \section Creation Creation of other main objects
 *
 * The class creates the only instance of CInstanceManager, CEventQueue, CEngine,
 * CRobotMain and CSound classes.
 *
 * \section Window Window management
 *
 * The class is responsible for creating app window, setting and changing the video mode,
 * joystick management, grabbing input and changing the system mouse cursor
 * position and visibility.
 * ("System mouse cursor" means the cursor displayed by the OS in constrast to the cursor
 * displayed by CEngine).
 *
 * \section Events Events
 *
 * Events are taken from SDL event queue, translated to common events from src/common.h
 * and pushed to global event queue CEventQueue.
 *
 * Joystick events are generated somewhat differently, by running a separate timer,
 * polling the device for changes and synthesising events on change. It avoids flooding
 * the event queue with too many joystick events and the granularity of the timer can be
 * adjusted.
 *
 * The events are passed to ProcessEvent() of classes in this order: CApplication, CEngine
 * and CRobotMain. CApplication and CEngine's ProcessEvent() functions return bool, which
 * means whether to pass the event on, or stop the chain. This is to enable handling some
 * events which are internal to CApplication or CEngine.
 *
 * \section Portability Portability
 *
 * Currently, the class only handles OpenGL devices. SDL can be used with DirectX, but
 * for that to work, video initialization and video setting must be done differently.
 *
 */
class CApplication : public CSingleton<CApplication>
{
public:
    //! Constructor (can only be called once!)
    CApplication();
    //! Destructor
    ~CApplication();

public:
    //! Parses commandline arguments
    bool        ParseArguments(int argc, char *argv[]);
    //! Initializes the application
    bool        Create();
    //! Main event loop
    int         Run();
    //! Returns the code to be returned at main() exit
    int         GetExitCode();

    //! Returns the message of error (set to something if exit code is not 0)
    const std::string& GetErrorMessage();

    //! Cleans up before exit
    void        Destroy();

    //! Returns a list of possible video modes
    VideoQueryResult GetVideoResolutionList(std::vector<Math::IntPoint> &resolutions,
                                            bool fullScreen, bool resizeable);

    //! Returns the current video mode
    Gfx::GLDeviceConfig GetVideoConfig();

    //! Change the video mode to given mode
    bool        ChangeVideoConfig(const Gfx::GLDeviceConfig &newConfig);

    //! Suspends animation (time will not be updated)
    void        SuspendSimulation();
    //! Resumes animation
    void        ResumeSimulation();
    //! Returns whether simulation is suspended
    bool        GetSimulationSuspended();

    //! Updates the simulation state
    void        StepSimulation();

    //@{
    //! Management of simulation speed
    void            SetSimulationSpeed(float speed);
    float           GetSimulationSpeed();
    //@}

    //! Returns the absolute time counter [seconds]
    float       GetAbsTime();
    //! Returns the exact absolute time counter [nanoseconds]
    long long   GetExactAbsTime();

    //! Returns the exact absolute time counter disregarding speed setting [nanoseconds]
    long long   GetRealAbsTime();

    //! Returns the relative time since last update [seconds]
    float       GetRelTime();
    //! Returns the exact realative time since last update [nanoseconds]
    long long   GetExactRelTime();

    //! Returns the exact relative time since last update disregarding speed setting [nanoseconds]
    long long   GetRealRelTime();

    //! Returns a list of available joystick devices
    std::vector<JoystickDevice> GetJoystickList();

    //! Returns info about the current joystick
    JoystickDevice GetJoystick();

    //! Change the current joystick device
    bool        ChangeJoystick(const JoystickDevice &newJoystick);

    //! Management of joystick enable state
    //@{
    void        SetJoystickEnabled(bool enable);
    bool        GetJoystickEnabled();
    //@}

    //! Polls the state of joystick axes and buttons
    void        UpdateJoystick();

    //! Updates the mouse position explicitly
    void        UpdateMouse();

    //! Returns the current key modifiers
    int         GetKmods();
    //! Returns whether the given kmod is active
    bool        GetKmodState(int kmod);

    //! Returns whether the tracked key is pressed
    bool        GetTrackedKeyState(TrackedKey key);

    //! Returns whether the mouse button is pressed
    bool        GetMouseButtonState(int index);

    //! Resets tracked key states, modifiers and motion vectors
    void        ResetKeyStates();


    // TODO move input binding and motion vectors to CRobotMain

    //! Sets the default input bindings
    void        SetDefaultInputBindings();

    //! Management of input bindings
    //@{
    void        SetInputBinding(InputSlot slot, const InputBinding& binding);
    const InputBinding& GetInputBinding(InputSlot slot);
    //@}


    //! Management of the grab mode for input (keyboard & mouse)
    //@{
    void        SetGrabInput(bool grab);
    bool        GetGrabInput();
    //@}

    //! Management of the visiblity of system mouse cursor
    //@{
    void        SetSystemMouseVisible(bool visible);
    bool        GetSystemMouseVisibile();
    //@}

    //! Management of the position of system mouse cursor (in interface coords)
    //@{
    void        SetSystemMousePos(Math::Point pos);
    Math::Point GetSystemMousePos();
    //@}

    //! Management of debug mode (prints more info in logger)
    //@{
    void        SetDebugMode(bool mode);
    bool        GetDebugMode();
    //@}

    //! Returns the full path to a file in data directory
    std::string GetDataFilePath(const std::string &dirName, const std::string &fileName);

    //! Management of language
    //@{
    Language    GetLanguage();
    void        SetLanguage(Language language);
    //@}

protected:
    //! Creates the window's SDL_Surface
    bool CreateVideoSurface();

    //! Processes the captured SDL event to Event struct
    Event       ParseEvent();
    //! Handles some incoming events
    bool        ProcessEvent(const Event &event);
    //! Renders the image in window
    void        Render();

    //! Opens the joystick device
    bool OpenJoystick();
    //! Closes the joystick device
    void CloseJoystick();

protected:
    //! Instance manager
    CInstanceManager*       m_iMan;
    //! Private (SDL-dependent data)
    ApplicationPrivate*     m_private;
    //! Global event queue
    CEventQueue*            m_eventQueue;
    //! Graphics engine
    Gfx::CEngine*           m_engine;
    //! Graphics device
    Gfx::CDevice*           m_device;
    //! Sound subsystem
    CSoundInterface*                 m_sound;
    //! Main class of the proper game engine
    CRobotMain*             m_robotMain;

    //! Code to return at exit
    int             m_exitCode;
    //! Whether application window is active
    bool            m_active;
    //! Whether debug mode is enabled
    bool            m_debugMode;

    //! Message to be displayed as error to the user
    std::string     m_errorMessage;

    //! Current configuration of OpenGL display device
    Gfx::GLDeviceConfig m_deviceConfig;
    //! Previous configuration of OpenGL display device
    Gfx::GLDeviceConfig m_lastDeviceConfig;

    //! Text set as window title
    std::string     m_windowTitle;

    //! Animation time stamps, etc.
    //@{
    SystemTimeStamp* m_baseTimeStamp;
    SystemTimeStamp* m_lastTimeStamp;
    SystemTimeStamp* m_curTimeStamp;

    long long       m_realAbsTimeBase;
    long long       m_realAbsTime;
    long long       m_realRelTime;

    long long       m_absTimeBase;
    long long       m_exactAbsTime;
    long long       m_exactRelTime;

    float           m_absTime;
    float           m_relTime;

    float           m_simulationSpeed;
    bool            m_simulationSuspended;
    //@}

    //! Current state of key modifiers (mask of SDLMod)
    unsigned int    m_kmodState;
    //! Current state of some tracked keys (mask of TrackedKey)
    bool            m_trackedKeysState[TRKEY_MAX];
    //! Current state of mouse buttons (mask of button indexes)
    unsigned int    m_mouseButtonsState;

    //! Bindings for user inputs
    InputBinding    m_inputBindings[INPUT_SLOT_MAX];

    //! Motion vector set by keyboard
    Math::Vector    m_keyMotion;
    //! Motion vector set by joystick
    Math::Vector    m_joyMotion;

    //! Current system mouse position
    Math::Point     m_systemMousePos;

    //! Info about current joystick device
    JoystickDevice  m_joystick;
    //! Whether joystick is enabled
    bool            m_joystickEnabled;
    //! Current state of joystick axes; may be updated from another thread
    std::vector<int> m_joyAxeState;
    //! Current state of joystick buttons; may be updated from another thread
    std::vector<bool> m_joyButtonState;

    //! Path to directory with data files
    std::string     m_dataPath;

    //! Application language
    Language        m_language;
};

