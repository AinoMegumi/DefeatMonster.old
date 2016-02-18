#pragma once
#include<vector>
std::vector<status_data> read_xml_player_status_data(std::string player_name, bool is_male);
std::vector<status_data> read_xml_partner_status_data();
std::vector<status_data> read_xml_enemy_status_data();