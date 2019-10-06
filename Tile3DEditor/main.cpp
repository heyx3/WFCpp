//Redefine "exit" to throw an exception instead of plain quitting,
//    so that destructors can still be called.
//#define exit(code) throw code

#include "RenderLibs.h"
#include "Config.h"


namespace
{
    //Relative to the executable path.
    const char* ConfigFile = "config.json";

    fs::path GetConfigFullPath() { return fs::current_path() / "config.json"; }
}


//The overarching stuff in the app, managed by RAII.
//Also offers some covenience stuff like clearing the window.
class App
{
public:

    static const int OpenGLVersion_Major = 4,
                     OpenGLVersion_Minor = 0;
    static const int MinWindowWidth = 350,
                     MinWindowHeight = 350;

    SDL_Window* Window = nullptr;

    SDL_GLContext OpenGL;

    Config Config;
    ErrorCallback OnError;


    void OnWindowEvent(const SDL_WindowEvent& data)
    {
        //Update config settings about the window.
        Config.WasWindowMaximized = (SDL_GetWindowFlags(Window) & SDL_WINDOW_MAXIMIZED) != 0;
        if (!Config.WasWindowMaximized)
        {
            int wndW, wndH;
            SDL_GetWindowSize(Window, &wndW, &wndH);
            Config.LastWindowWidth = wndW;
            Config.LastWindowHeight = wndH;
        }

        //Make sure the window doesn't get too small.
        if (data.event == SDL_WINDOWEVENT_RESIZED &&
            (data.data1 < MinWindowWidth || data.data2 < MinWindowHeight))
        {
            SDL_SetWindowSize(Window,
                              max(data.data1, MinWindowWidth),
                              max(data.data2, MinWindowHeight));
        }
    }


    #pragma region Initialization/shutdown

    App(const std::string& windowTitle, ErrorCallback onError)
    {
        OnError = onError;

        //Load the config.
        if (!IO::ReadJsonFromFile(GetConfigFullPath(), Config, OnError))
            return;

        //Set up SDL.
        if (!TrySDL(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL"))
            return;
        //Set up the window.
        auto windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        if (Config.WasWindowMaximized)
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        Window = SDL_CreateWindow(windowTitle.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  (int)Config.LastWindowWidth, (int)Config.LastWindowHeight,
                                  windowFlags);
        if (!TrySDL(Window, "Error creating window"))
            return;

        //Initialize OpenGL.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OpenGLVersion_Major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OpenGLVersion_Minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        OpenGL = SDL_GL_CreateContext(Window);
        if (!TrySDL(OpenGL, "Error initializing OpenGL context"))
            return;

        //Initialize GLEW
        glewExperimental = GL_TRUE;
        auto glewError = glewInit();
        if (glewError != GLEW_OK)
        {
            OnError(std::string("Error setting up GLEW: ") + (const char*)glewGetErrorString(glewError));
            return;
        }

        //Set basic OpenGL settings.
        //V-sync:
        if (!TrySDL(SDL_GL_SetSwapInterval(1), "Setting VSync"))
            return;
    }
    ~App()
    {
        //Clean up the window/SDL.
        if (OpenGL != nullptr)
            SDL_GL_DeleteContext(OpenGL);
        if (Window != nullptr)
            SDL_DestroyWindow(Window);
        SDL_Quit();

        //Write out the new config file.
        IO::WriteJsonToFile(GetConfigFullPath(), Config, OnError);
    }

    //No copy or move operator.
    App(const App& cpy) = delete;
    App& operator=(const App& cpy) = delete;

    #pragma endregion

private:

    bool TrySDL(int returnCode, const std::string& msgPrefix)
    {
        if (returnCode != 0)
            OnError(msgPrefix + ": " + SDL_GetError());

        return returnCode == 0;
    }
    bool TrySDL(void* shouldntBeNull, const std::string& msgPrefix)
    {
        if (shouldntBeNull == nullptr)
            return TrySDL(-1, msgPrefix);
        else
            return true;
    }
};


int main(int argc, char* argv[])
{
    std::cout << "Program path: " << fs::current_path() << "\n\n";

    //Set up error management.
    bool quitApp = false;
    auto errorCallback = [&quitApp](const std::string& msg) {
        quitApp = true;
        SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR,
                                 "Error", msg.c_str(),
                                 nullptr);
    };

    //Initialize the app's various modules.
    App app("WFCpp Tile3D Editor", errorCallback);

    //Run the main loop.
    while (!quitApp)
    {
        //Process window events.
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            switch (sdlEvent.type)
            {
                case SDL_EventType::SDL_WINDOWEVENT:
                    std::cout << sdlEvent.window.type << "\n";
                    switch (sdlEvent.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            quitApp = true;
                            break;

                        default: break;
                    }
                    app.OnWindowEvent(sdlEvent.window);
                break;
            
                case SDL_QUIT:
                    quitApp = true;
                break;
                 
                default: break;
            }
        }

        //Update the app.

        //Render the app.
        SDL_GL_SwapWindow(app.Window);
    }

    return 0;
}