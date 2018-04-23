
struct Theme{
  uint16_t background;
  uint16_t cell_frame;
  uint16_t cell_background;
  uint16_t editable_numbers;
  uint16_t noneditable_numbers;
  uint16_t cursor_border;
};

#define defaulttheme {BLACK      , YELLOW     , WHITE_SMOKE, BLACK, RED, DARK_GREEN}
#define cutetheme {PINK, DEEP_PINK, PEACH_PUFF, FUCHSIA, GREEN, TURQUOISE}
