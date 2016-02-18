#pragma once
void all_music_setting(IniStream ini, Music &music);
int opening(IniStream ini) noexcept(false);
StatusDataList all_settings(COORDINATE &GSize, bool print_in_kanji, int setting_music_handle);
void print_status(StatusDataList sta, COORDINATE status_graph);
void graph_enemy_and_background(COORDINATE enemy_graph, int bg_graphic_handle, std::string player_name, Status &enemy);
void result(int battle_result, bool print_in_kanji, int win, int lose);
int opening(IniStream ini) noexcept(false);
