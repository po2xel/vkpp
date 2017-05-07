#ifndef __VKPP_SAMPLE_WINDOW_EVENT_H__
#define __VKPP_SAMPLE_WINDOW_EVENT_H__



#include <functional>
#include <cassert>

#include <SDL2/SDL_events.h>

#include <Window/Window.h>
#include <System/Application.h>



namespace vkpp::sample
{



enum UserEvents
{
    USER_WM_REDRAW = SDL_USEREVENT + 1,
    USER_WM_COUNT
};



class CEvent
{
private:
    const CWindow& mWindow;

public:
    explicit CEvent(const CWindow& aWin) : mWindow(aWin)
    {}

    virtual ~CEvent(void) = default;

    Uint32 GetWindowID(void) const
    {
        return mWindow.GetID();
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_WINDOW_EVENT_H__