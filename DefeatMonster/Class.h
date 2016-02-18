#pragma once
#include"Base.h"
#include<string>
#include<random>
#include<algorithm>
#include<array>

class status_data {
public:
	status_data(std::string name, std::string graph_name, bool is_male, int hp, int mp, int attack, int defence, int magic_attack, int magic_defence, int speed)
		: name(name), graph_name(graph_name), is_male(is_male), hp(hp), mp(mp), attack(attack), defence(defence), magic_attack(magic_attack),
		magic_defence(magic_defence), speed(speed) {}
	std::string name, graph_name;
	int hp, mp, attack, defence, magic_attack, magic_defence, speed;
	bool is_male;
};

class COORDINATE {
public:
	int x, y;
	uint string_color;
	COORDINATE() = default;
	COORDINATE(int x, int y, uint string_color) {
		this->x = x;
		this->y = y;
		this->string_color = string_color;
	}
};

class IniStream {
public:
	IniStream(std::string const& filepath_or_name);
	bool is_now_created() const;
	void edit(std::string const& section, std::string const& key, std::string const& str);
	int get_int(std::string const& section, std::string const& key, const int default_num);
	std::string get_string(std::string const& section, std::string const& key, std::string const& default_str);
private:
	void create_UTF16_file();
	std::string filefullpath;
	bool was_no_exist;
};

class Status {
public:
	Status(std::vector<status_data> arr, int i, bool is_player_setting);
	std::string name;
	int hp, max_hp, mp, max_mp, attack, defence, magic_attack, magic_defence, speed, graph_handle;
	bool is_male;
};

typedef struct {
	Status player, partner1, partner2, enemy;
}StatusDataList;

class Random {
public:
	static std::mt19937& make_mt();
};

class Damage : public Random {
private:
	int attack, defence, critical_percentage;
	bool do_guard;
	int base();
	int normal_attack();
	int skill_attack(int base_power);
public:
	Damage(int attack, int defence, cint critical_percentage, bool do_guard) {
		this->attack = attack;
		this->defence = defence;
		this->critical_percentage = critical_percentage;
		this->do_guard = do_guard;
	}
	int calc(int attack_type, int base_power);
};

class Message {
private:
	bool print_in_kanji;
	std::string attacker, target;
	COORDINATE message_window;
	void miss();
	void window_box();
public:
	Message(std::string attacker, std::string target, bool print_in_kanji, COORDINATE coordinate) {
		this->attacker = attacker;
		this->target = target;
		this->print_in_kanji = print_in_kanji;
		this->message_window = coordinate;
	}
	void attack();
	void magic_attack(const char* chant_magic, cint magic_sound_handle);
	void skill_attack(const char* use_skill);
	void cure(int cure_point);
	void resuscitation_success();
	void resuscitation_failure();
	void shortage_magic_power();
	void damage_to_enemy(int damage);
	void damage_from_enemy(int damage);
};

class Strategy : public Random {
private:
	COORDINATE message_window;
	int damage, magic_sound_handle;
	bool print_in_kanji;
	std::string magic_name;
	void carel(Status &user, Status &target);
	bool check_hp_percentage(int hp, int max_hp, int check_percentage);
	bool check_hp_zero(int hp);
	void attack_importance(Status &user, Status &target);
	void balance(Status &player, Status &user, Status &other_partner, Status &enemy);
	void cure_importance(Status &player, Status &user, Status &other_partner, Status &enemy);
	void resuscitation(Status& user, Status& target);
public:
	Strategy(bool print_in_kanji, COORDINATE coordinate, std::string magic_name, int magic_sound_handle) {
		this->print_in_kanji = print_in_kanji;
		this->message_window = coordinate;
		this->magic_name = magic_name;
		this->magic_sound_handle = magic_sound_handle;
	}
	void partner_command(int strategy_type, Status &player, Status &user, Status &other_partner, Status &enemy);
};

enum TURN {
	command, player_turn, partner1_turn, partner2_turn, enemy_turn, check
};

class Battle : public Random {
private:
	int command_cursole, partner1_strategy, partner2_strategy;
	int magic_sound_handle;
	bool print_in_kanji, player_guard;
	int damage;
	TURN turn;
	short turn_count;
	void player_turn_command(StatusDataList &sta, Damage player_damage, COORDINATE message_window);
	void player_carel(StatusDataList &sta, COORDINATE message_window, Message msg);
	void enemy_attack(Status &target, Status &enemy, bool do_guard, COORDINATE coordinate);
	void enemy_command(StatusDataList &sta, COORDINATE coordinate);public:
	Battle() = default;
	Battle(TURN turn, bool print_in_kanji, int magic_sound_handle) {
		this->turn = turn;
		this->print_in_kanji = print_in_kanji;
		this->magic_sound_handle = magic_sound_handle;
	}
	std::vector<TURN> turn_arr;
	int battle_main(StatusDataList &sta, COORDINATE Status_graph);
};

class KeyState {
public:
	KeyState() noexcept;
	KeyState(const KeyState&) = delete;
	KeyState(KeyState&&) = delete;
	KeyState& operator=(const KeyState&) = delete;
	KeyState& operator=(KeyState&&) = delete;
	bool update() noexcept;
	bool fllush();
	int operator[](size_t n) const noexcept;
	int at(size_t n) const;
	bool esc() const noexcept;
	bool up() const noexcept;
	bool down() const noexcept;
	bool decide() const noexcept;
	static constexpr size_t keybufsize = 256;
	void cursole(cint max, cint x, cint y, int &cursole_point, uint cursole_color, uint back_color);
private:
	bool fllush_stream();
	std::array<int, 256> KeyStateBuf;
};
bool operator!=(const KeyState& l, size_t r);
inline bool operator!=(size_t l, const KeyState& r) {
	return r != l;
}
inline bool operator==(const KeyState& l, size_t r) {
	return !(l != r);
}
inline bool operator==(size_t l, const KeyState& r) {
	return !(l != r);
}