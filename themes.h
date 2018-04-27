
struct Theme{
  uint16_t background;
  uint16_t cell_frame;
  uint16_t cell_background;
  uint16_t editable_numbers;
  uint16_t noneditable_numbers;
  uint16_t cursor_border;
};
#define NUM_THEMES 6

#define defaulttheme {BLACK, YELLOW, WHITE_SMOKE, BLACK, RED, DARK_GREEN}
#define greyscale {WHITE,BLACK,WHITE,BLACK,GREY,LIGHT_GREY}
#define cutetheme {PINK, DEEP_PINK, PEACH_PUFF, FUCHSIA, ORANGE, TURQUOISE}
#define primaryColors {WHITE,RED,YELLOW,BLUE,GREEN,BLACK}
#define banana {LEMON_CHIFFON,YELLOW,PALE_GOLDENROD,BROWN, BLACK,LIGHT_GOLDENROD_YELLOW}
#define minimal {BLACK, BLACK, BLACK, WHITE, GRAY, WHITE}



const struct Theme themeList[NUM_THEMES] = {defaulttheme,greyscale, cutetheme,primaryColors,banana,minimal};