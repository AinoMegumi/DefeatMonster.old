#pragma once
int check_print_in_kanji();
std::string get_player_name(bool print_in_kanji, uint draw_string_color);
bool check_male(bool print_in_kanji, uint string_color);
void choose_partner(std::vector<status_data> arr, int &partner1, int &partner2, const DWORD string_color, bool print_in_kanji);