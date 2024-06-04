#include <map>
#include <string>

#include "../vendor/SDL2/SDL_scancode.h"

using CharacterMap = std::map<SDL_Scancode, std::string>;

static const CharacterMap characterMap()
{
    static const CharacterMap charactermap =
    {
        /* All letters */
         { SDL_SCANCODE_A,  "a"},
         { SDL_SCANCODE_B,  "b"},
         { SDL_SCANCODE_C,  "c"},
         { SDL_SCANCODE_D,  "d"},
         { SDL_SCANCODE_E,  "e"},
         { SDL_SCANCODE_F,  "f"},
         { SDL_SCANCODE_G,  "g"},
         { SDL_SCANCODE_H,  "h"},
         { SDL_SCANCODE_I,  "i"},
         { SDL_SCANCODE_J,  "j"},
         { SDL_SCANCODE_K,  "k"},
         { SDL_SCANCODE_L,  "l"},
         { SDL_SCANCODE_M,  "m"},
         { SDL_SCANCODE_N,  "n"},
         { SDL_SCANCODE_O,  "o"},
         { SDL_SCANCODE_P,  "p"},
         { SDL_SCANCODE_Q,  "q"},
         { SDL_SCANCODE_R,  "r"},
         { SDL_SCANCODE_S,  "s"},
         { SDL_SCANCODE_T,  "t"},
         { SDL_SCANCODE_U,  "u"},
         { SDL_SCANCODE_V,  "v"},
         { SDL_SCANCODE_W,  "w"},
         { SDL_SCANCODE_X,  "x"},
         { SDL_SCANCODE_Y,  "y"},
         { SDL_SCANCODE_Z,  "z"},



         /* All numbers */
         { SDL_SCANCODE_0,  "0"},
         { SDL_SCANCODE_1,  "1"},
         { SDL_SCANCODE_2,  "2"},
         { SDL_SCANCODE_3,  "3"},
         { SDL_SCANCODE_4,  "4"},
         { SDL_SCANCODE_5,  "5"},
         { SDL_SCANCODE_6,  "6"},
         { SDL_SCANCODE_7,  "7"},
         { SDL_SCANCODE_8,  "8"},
         { SDL_SCANCODE_9,  "9"},
    };

    return charactermap;
}