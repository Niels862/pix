#ifndef PIX_RENDERER_HPP
#define PIX_RENDERER_HPP

#include <SDL2/SDL.h>
#include <memory>

class Renderer {
public:
    Renderer(int window_width, int window_height, 
             int mem_width, int mem_height);

    ~Renderer();

    int process_events();

    void draw_frame(char const *data);

private:
    SDL_Window *m_window;

    SDL_Renderer *m_renderer;

    SDL_Texture *m_texture;

    std::unique_ptr<Uint32[]> m_pixels;

    int m_window_width, m_window_height;

    int m_mem_width, m_mem_height;
};

#endif
