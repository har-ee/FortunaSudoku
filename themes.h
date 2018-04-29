
struct Theme{
  uint16_t background;
  uint16_t cell_frame;
  uint16_t cell_background;
  uint16_t editable_numbers;
  uint16_t noneditable_numbers;
  uint16_t cursor_border;
  char theme_name[10]; /* Length should be <= 10 */
};

#define NUM_THEMES 7 /* Update when a new theme is added to the list */

#define defaulttheme {BLACK, YELLOW, WHITE_SMOKE, BLACK, RED, DARK_GREEN, "Default"}
#define greyscale {WHITE,BLACK,WHITE,BLACK,GREY,LIGHT_GREY, "Greyscale"}
#define cutetheme {PINK, DEEP_PINK, PEACH_PUFF, FUCHSIA, ORANGE, TURQUOISE, "Cute Theme"}
#define primaryColors {WHITE,RED,YELLOW,BLUE,GREEN,BLACK, "Primary"}
#define banana {RAYNA_YELLOW,GOLD,RAYNA_BANANA,DARK_ORANGE, GREEN,RED, "Banana"}
#define minimal {BLACK, BLACK, BLACK, WHITE, GRAY, WHITE, "Minimal"}
#define wavey {RAYNA_DARK_PINK, DARK_TURQUOISE, BLACK, LIME, WHITE, YELLOW, "Wavey"}



const struct Theme themeList[NUM_THEMES] = {defaulttheme,greyscale, cutetheme,primaryColors,banana,minimal,wavey};