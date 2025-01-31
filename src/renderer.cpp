#include "renderer.hpp"

Renderer::Renderer()
        : m_window{nullptr}, m_renderer{nullptr},
          m_texture{nullptr},
          m_pixels{}, 
          m_window_width{}, m_window_height{},
          m_mem_width{}, m_mem_height{},
          m_initialized{false} {}

void Renderer::init(int window_width, int window_height, 
                    int mem_width, int mem_height) {
    m_window_width = window_width;
    m_window_height = window_height;
    m_mem_width = mem_width;
    m_mem_height = mem_height;

    m_pixels = std::make_unique<Uint32[]>(m_mem_width * m_mem_height);

    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow("<Title>", 
                                SDL_WINDOWPOS_CENTERED, 
                                SDL_WINDOWPOS_CENTERED, 
                                m_window_width,
                                m_window_height, 
                                SDL_WINDOW_RESIZABLE);

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

    m_texture = SDL_CreateTexture(m_renderer, 
                                  SDL_PIXELFORMAT_RGB888, 
                                  SDL_TEXTUREACCESS_STREAMING, 
                                  m_mem_width, m_mem_height);
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

    m_initialized = true;
}

Renderer::~Renderer() {
    if (m_initialized) {
        SDL_DestroyTexture(m_texture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }
}

int Renderer::process_events() {
    if (!m_initialized) return 0;

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;
        }
    }

    return 0;
}

void Renderer::draw_frame(char const *data) {
    if (!m_initialized) return;

    for (int i = 0; i < m_mem_width * m_mem_height; i++) {
        uint32_t r = ((data[i] >> 5) & 0x7) << 5;
        uint32_t g = ((data[i] >> 3) & 0x3) << 6;
        uint32_t b = ((data[i] >> 0) & 0x7) << 5;

        m_pixels[i] = (r << 16) | (g << 8) | b;
    }

    SDL_UpdateTexture(m_texture, NULL, m_pixels.get(), 
                      m_mem_width * sizeof(Uint32));

    SDL_RenderClear(m_renderer);
    SDL_Rect dst = {0, 0, m_window_width, m_window_height};
    SDL_RenderCopy(m_renderer, m_texture, NULL, &dst);
    SDL_RenderPresent(m_renderer);

    SDL_Delay(2);
}
