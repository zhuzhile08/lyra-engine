#include <input/input.h>

namespace lyra {
    
const uint8* Input::m_keyboardState = SDL_GetKeyboardState(NULL);

} // namespace lyra
