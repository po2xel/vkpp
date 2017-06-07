#ifndef __VKPP_SAMPLE_WINDOW_WINDOW_H__
#define __VKPP_SAMPLE_WINDOW_WINDOW_H__



#include <string>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <System/NonCopyable.h>
#include <System/Exception.h>



namespace vkpp::sample
{



enum class SystemCursorType
{
    eArrow          = SDL_SYSTEM_CURSOR_ARROW,
    eIBeam          = SDL_SYSTEM_CURSOR_IBEAM,
    eWait           = SDL_SYSTEM_CURSOR_WAIT,
    eCrossHair      = SDL_SYSTEM_CURSOR_CROSSHAIR,
    eWaitArrow      = SDL_SYSTEM_CURSOR_WAITARROW,
    eSizeNWSE       = SDL_SYSTEM_CURSOR_SIZENWSE,
    eSizeNESW       = SDL_SYSTEM_CURSOR_SIZENESW,
    eSizeWE         = SDL_SYSTEM_CURSOR_SIZEWE,
    eSizeNS         = SDL_SYSTEM_CURSOR_SIZENS,
    eSizeALL        = SDL_SYSTEM_CURSOR_SIZEALL,
    eNo             = SDL_SYSTEM_CURSOR_NO,
    eHand           = SDL_SYSTEM_CURSOR_HAND
};



class CWindow : public CNonCopyable
{
protected:
    using WindowHandle = SDL_Window*;
    using WindowSurface = SDL_Surface*;
    using CursorType = SDL_Cursor;
    using CursorTypePtr = CursorType*;

    WindowHandle    mWindow;
    CursorTypePtr   mpCursor;

    Uint32          mWinID;

#ifdef SDL_VIDEO_DRIVER_WINDOWS
    HWND            mNativeHandle;
#endif  // SDL_VIDEO_DRIVER_WINDOWS

#ifdef SDL_VIDEO_DRIVER_UIKIT
    UIWindow*       mNativeHandle;
#endif  // SDL_VIDEO_DRIVER_UIKIT

#ifdef SDL_VIDEO_DRIVER_ANDROID
    ANativeWindow* mNativeHandle;
#endif  // SDL_VIDEO_DRIVER_ANDROID

    void ResetCursor(CursorTypePtr apCursor)
    {
        if (mpCursor != nullptr)
            SDL_FreeCursor(mpCursor);

        mpCursor = apCursor;
    }

    void ResetWindow(WindowHandle apWin)
    {
        if (mWindow != nullptr)
            SDL_DestroyWindow(mWindow);

        mWindow = apWin;
    }

public:
    enum WindowFlags
    {
        eFullScreen         = SDL_WINDOW_FULLSCREEN,                    // Fullscreen window
        eFullScreenDesktop  = SDL_WINDOW_FULLSCREEN_DESKTOP,            // Fullscreen window at the current desktop resolution
        eOpenGL             = SDL_WINDOW_OPENGL,                        // Window usable with OpenGL context
        eShown              = SDL_WINDOW_SHOWN,                         // Window is visible
        eHiden              = SDL_WINDOWEVENT_HIDDEN,                   // Window is not visible
        eBorderLess         = SDL_WINDOW_BORDERLESS,                    // No window decoration
        eResizable          = SDL_WINDOW_RESIZABLE,                     // Window can be resized
        eMinimized          = SDL_WINDOW_MINIMIZED,                     // Window is minimized
        eMaximized          = SDL_WINDOW_MAXIMIZED,                     // Window is maximized
        eInputGrabbed       = SDL_WINDOW_INPUT_GRABBED,                 // Window has grabbed input focus
        eInputFocus         = SDL_WINDOW_INPUT_FOCUS,                   // Window has input focus
        eMouseFocus         = SDL_WINDOW_MOUSE_FOCUS,                   // Window has mouse focus
        eForeign            = SDL_WINDOW_FOREIGN,                       // Window not created by SDL
        eAllowHighDPI       = SDL_WINDOW_ALLOW_HIGHDPI,                 // Window should be created in high-DPI mode if supported
        eMouseCapture       = SDL_WINDOW_MOUSE_CAPTURE                  // Window has mouse captured
    };

    enum WindowPos
    {
        eCentered       = SDL_WINDOWPOS_CENTERED,
        eUndefined      = SDL_WINDOWPOS_UNDEFINED
    };

    // Create a window with the specified position, dimensions and flags
    CWindow(const std::string& aTitle, int aPosX, int aPosY, int aWidth, int aHeight, Uint32 aFlags) : mWindow(nullptr), mpCursor(nullptr), mWinID(0), mNativeHandle(nullptr)
    {
        Create(aTitle, aPosX, aPosY, aWidth, aHeight, aFlags);
        mpCursor = SDL_GetCursor();
    }

    virtual ~CWindow(void)
    {
        ResetCursor(nullptr);
        ResetWindow(nullptr);
    }

    auto GetNativeHandle(void) const
    {
        return mNativeHandle;
    }

    Uint32 GetID(void) const
    {
        return mWinID;
    }

    Uint32 GetFlags(void) const
    {
        assert(mWindow != nullptr);

        return SDL_GetWindowFlags(mWindow);
    }

    // Get the SDL surface associated with the window.
    WindowSurface GetSurface(void) const
    {
        return SDL_GetWindowSurface(mWindow);
    }

    // Set the active cursor
    void SetCursor(SystemCursorType aCursorType)
    {
        auto lpCursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(aCursorType));
        if (lpCursor != nullptr)
        {
            SDL_SetCursor(lpCursor);
            ResetCursor(lpCursor);
        }
    }

    // Get the active cursor
    CursorTypePtr GetCursor(void) const
    {
        return mpCursor;
    }

    // Set the title of the window in UTF-8 format.
    void SetTitle(const std::string& aTitle) const
    {
        SDL_SetWindowTitle(mWindow, aTitle.c_str());
    }

    // Return the title of the window in UTF-8 format or "" if there is no title.
    std::string GetTitle(void) const
    {
        return SDL_GetWindowTitle(mWindow);
    }

    // Set the size of a window's client area.
    void SetSize(int aWidth, int aHeight) const
    {
        SDL_SetWindowSize(mWindow, aWidth, aHeight);
    }

    // Get the size of a window's client area
    void GetSize(int& aWidth, int& aHeight) const
    {
        SDL_GetWindowSize(mWindow, &aWidth, &aHeight);
    }
    
    // Set the minimum size of a window's client area.
    void SetMinimumSize(int aMinWidth, int aMinHeight) const
    {
        SDL_SetWindowMinimumSize(mWindow, aMinWidth, aMinHeight);
    }

    // Get the minimum size of a window's client area.
    void GetMinimumSize(int& aWidth, int& aHeight) const
    {
        SDL_GetWindowMinimumSize(mWindow, &aWidth, &aHeight);
    }

    // Set the maximum size of a window's client area.
    void SetMaximumSize(int aMaxWidth, int aMaxHeight) const
    {
        SDL_SetWindowMaximumSize(mWindow, aMaxWidth, aMaxHeight);
    }

    // Get the maximum size of a window's client area.
    void GetMaximumSize(int& aWidth, int& aHeight) const
    {
        SDL_GetWindowMaximumSize(mWindow, &aWidth, &aHeight);
    }

    // Set the position of a window.
    void SetPosition(int aPosX, int aPosY) const
    {
        SDL_SetWindowPosition(mWindow, aPosX, aPosY);
    }

    // Get the position of a window.
    void GetPosition(int& aPosX, int& aPosY) const
    {
        SDL_GetWindowPosition(mWindow, &aPosX, &aPosY);
    }

    // Set a window's input grab mode.
    // When input is grabbed, the mouse is confined to the window.
    // If the caller enables a grab while another window is currently grabbed, the other window loses its grab
    // in favor of the caller's window.
    void SetGrabMode(bool aIsGrabbed) const
    {
        SDL_SetWindowGrab(mWindow, aIsGrabbed ? SDL_TRUE : SDL_FALSE);
    }

    // Return true if input is in grabbed mode, othersize false.
    bool IsGrabbed(void) const
    {
        return SDL_TRUE == SDL_GetWindowGrab(mWindow);
    }

    bool SetFullscreen(void) const
    {
        return 0 == SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);
    }

    bool SetWindowedMode(void) const
    {
        return 0 == SDL_SetWindowFullscreen(mWindow, 0);
    }

    // Set the brightness (gamma multiplier) for the display that owns the window
    bool SetBrightness(float aBrightness) const
    {
        return 0 == SDL_SetWindowBrightness(mWindow, aBrightness);
    }

    // Add or remove the border form the window.
    void SetBordered(bool aIsBordered) const
    {
        SDL_SetWindowBordered(mWindow, aIsBordered ? SDL_TRUE : SDL_FALSE);
    }

    // Restore the size and position of a minimized or maximized window.
    void Restore(void) const
    {
        SDL_RestoreWindow(mWindow);
    }

    // Raise a window above other windows and set the input focus.
    void Raise(void) const
    {
        SDL_RaiseWindow(mWindow);
    }

    // Minimize a window to an iconic representation.
    void Minimize(void) const
    {
        SDL_MinimizeWindow(mWindow);
    }

    // Maximize a window as large as possible.
    void Maximize(void) const
    {
        SDL_MaximizeWindow(mWindow);
    }

    void Show(void) const
    {
        SDL_ShowWindow(mWindow);
    }

    void Hide(void) const
    {
        SDL_HideWindow(mWindow);
    }

    void Close(void)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    void Create(const std::string& aTitle, int aPosX, int aPosY, int aWidth, int aHeight, Uint32 aFlags)
    {
        if (mWindow != nullptr)
            Close();

        mWindow = SDL_CreateWindow(aTitle.c_str(), aPosX, aPosY, aWidth, aHeight, aFlags);

        if (nullptr == mWindow)
            throw CWindowCreationException();

        mWinID = SDL_GetWindowID(mWindow);

        SDL_SysWMinfo lInfo;
        SDL_VERSION(&lInfo.version);

        if (SDL_TRUE == SDL_GetWindowWMInfo(mWindow, &lInfo))
        {
#ifdef SDL_VIDEO_DRIVER_WINDOWS
            mNativeHandle = lInfo.info.win.window;
#endif  // SDL_VIDEO_DRIVER_WINDOWS

#ifdef SDL_VIDEO_DRIVER_UIKIT
            mNativeHandle = lInfo.info.uikit.window;
#endif  // SDL_VIDEO_DRIVER_UIKIT

#ifdef SDL_VIDEO_DRIVER_ANDROID
            mNativeHandle = lInfo.info.android.window;
#endif  // SDL_VIDEO_DRIVER_ANDROID
        }
    }

    // Prevent the screen from being blanked by a screen saver.
    static void DisableScreenSaver(void)
    {
        SDL_DisableScreenSaver();
    }

    // Allow the screen to be blanked by a screen saver.
    static void EnableScreenSaver(void)
    {
        SDL_EnableScreenSaver();
    }

    // Return whether the screensaver is currently enabled.
    // The screensaver is disabled by default.
    static bool IsScreenSaverEnabled(void)
    {
        return SDL_TRUE == SDL_IsScreenSaverEnabled();
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_WINDOW_WINDOW_H__