#ifndef PIX_RENDERER_HPP
#define PIX_RENDERER_HPP

#include <SDL2/SDL.h>
#include <memory>

class Renderer {
public:
    Renderer();

    ~Renderer();

    void init();

    int process_events();

    void draw_frame(char const *data);

private:
    SDL_Window *m_window;

    SDL_Renderer *m_renderer;

    SDL_Texture *m_texture;

    std::unique_ptr<Uint32[]> m_pixels;

    bool m_initialized;
};

#endif
