#include"Class.h"
#include"Common.h"
#include"GameSystem.h"

int end_check(StatusDataList sta) {
	if (sta.player.hp == 0 && sta.partner1.hp == 0 && sta.partner2.hp == 0 && sta.enemy.hp == 0) return 2;
	else if ((sta.player.hp != 0 || sta.partner1.hp != 0 || sta.partner2.hp != 0) && sta.enemy.hp == 0) return 3;
	else if (sta.player.hp == 0 && sta.partner1.hp == 0 && sta.partner2.hp == 0 && sta.enemy.hp != 0) return 1;
	else return 0;
}

void command_message(bool print_in_kanji) {
	DrawBox(1, window_height - 82, 135, window_height - 14, GetColor(128, 128, 128), TRUE);
	DrawBox(1, window_height - 82, 135, window_height - 14, GetColor(255, 255, 255), FALSE);
	DrawString(32, window_height - 80, print_in_kanji ? "攻撃" : "こうげき", GetColor(0, 0, 0));
	DrawString(32, window_height - 64, print_in_kanji ? "魔法" : "まほう", GetColor(0, 0, 0));
	DrawString(32, window_height - 48, print_in_kanji ? "特技" : "とくぎ", GetColor(0, 0, 0));
	DrawString(32, window_height - 32, "ケアルリング", GetColor(0, 0, 0));
	ScreenFlip();
}

int choose_command(bool print_in_kanji) {
	auto screen = dxle::screen::MakeScreen(window_width, window_height);
	screen.drawn_on([print_in_kanji]() { command_message(print_in_kanji); });
	KeyState key;
	return key.cursole(screen, 3, 10, window_height - 80, GetColor(255, 255, 0), GetColor(128, 128, 128));
}

static void safe_degree(int& re, int target) {
	re = std::max(re - target, 0);
}

void Battle::enemy_attack(Status &target, Status &enemy, bool do_guard, COORDINATE coordinate) {
	std::bernoulli_distribution rand(0.5);
	Message msg(enemy.name, target.name, this->print_in_kanji, coordinate);
	int damage = 0;
	bool flag = false;
	if (rand(this->make_mt())) {
		msg.attack();
		damage = Damage(enemy.attack, target.defence, 0, do_guard).calc(attack_type::normal, 0);
	}
	else {
		msg.magic_attack("サンダーボルト", this->magic_sound_handle);
		if (enemy.mp >= 10) {
			damage = Damage(enemy.magic_attack, target.magic_defence, 0, this->player_guard).calc(attack_type::skill, 0);
			enemy.mp -= 10;
		}
		else {
			msg.shortage_magic_power();
			damage = -1;
		}
	}	
	if (damage != -1) {
		msg.damage_from_enemy(damage);
		target.hp = std::max(target.hp - damage, 0);
	}
}

void Battle::enemy_command(StatusDataList &sta, COORDINATE coordinate) {
	std::uniform_int_distribution<int> rand(1, 100);
	bool flag = false;
	int target = rand(this->make_mt()) % 3 + 1;
	do{
		switch (target) {
		case 1:
			if (sta.player.hp != 0) {
				enemy_attack(sta.player, sta.enemy, this->player_guard, coordinate);
				flag = true;
			}
			else target++;
			break;
		case 2:
			if (sta.partner1.hp != 0) {
				enemy_attack(sta.partner1, sta.enemy, false, coordinate);
				flag = true;
			}
			else target++;
			break;
		case 3:
			if (sta.partner2.hp != 0) {
				enemy_attack(sta.partner2, sta.enemy, false, coordinate);
				flag = true;
			}
			else target = 1;
			break;
		default:
			throw std::runtime_error("enemy command error");
		}
	} while (!flag);
}

int calc_cure_average(int cure[], StatusDataList sta) {
	int total = 0;
	int cure_person_num = 0;
	if (sta.player.hp != 0) cure_person_num++;
	if (sta.partner1.hp != 0) cure_person_num++;
	if (sta.partner2.hp != 0) cure_person_num++;
	for (int i = 0; i < 3; i++) {
		total += cure[i];
	}
	return static_cast<int>(total / cure_person_num);
}

void Battle::player_carel(StatusDataList &sta, COORDINATE message_window, Message msg) {
	std::uniform_int_distribution<int> rand(1, 20);
	int cure[3] = { 150 + rand(this->make_mt()), 150 + rand(this->make_mt()), 150 + rand(this->make_mt()) };
	msg.magic_attack("ケアルリング", magic_sound_handle);
	if (sta.player.mp >= 15) {
		if (this->print_in_kanji) DrawFormatString(message_window.x + 5, message_window.y + 21, message_window.string_color, "%sたちは平均%d回復した！", sta.player.name.c_str(), calc_cure_average(cure, sta));
		else DrawFormatString(message_window.x + 5, message_window.y + 21, message_window.string_color, "%sたちはへいきん%dかいふくした！", sta.player.name.c_str(), calc_cure_average(cure, sta));
		ScreenFlip();
		if (sta.player.hp != 0) sta.player.hp = std::min(sta.player.hp + cure[0], sta.player.max_hp);
		if (sta.partner1.hp != 0) sta.partner1.hp = std::min(sta.partner1.hp + cure[1], sta.partner1.max_hp);
		if (sta.partner2.hp != 0) sta.partner2.hp = std::min(sta.partner2.hp + cure[2], sta.partner2.max_hp);
		sta.player.mp -= 15;
	}
	else msg.shortage_magic_power();
}

void Battle::player_turn_command(StatusDataList &sta, Damage player_damage, COORDINATE message_window) {
	Message msg(sta.player.name, sta.enemy.name, print_in_kanji, message_window);
	switch (command_cursole) {
		case 0:
			msg.attack();
			damage = player_damage.calc(attack_type::normal, 0);
			break;
		case 1:
			msg.magic_attack("アイスニードル", this->magic_sound_handle);
			
			if (sta.player.mp >= 5) {
				damage = player_damage.calc(attack_type::skill, 40);
				sta.player.mp -= 5;
			}
			else {
				msg.shortage_magic_power();
				damage = -1;
			}
			break;
		case 2:
			msg.skill_attack("かえんぎり");
			damage = player_damage.calc(attack_type::skill, 40);
			break;
		case 3:
			player_carel(sta, message_window, msg);
			damage = -1;
			break;
		default:
			throw std::runtime_error("player command error");
	}
	if (damage != -1) {
		msg.damage_to_enemy(damage);
		sta.enemy.hp = std::max(sta.enemy.hp - damage, 0);
	}
}

int Battle::battle_main(StatusDataList &sta, COORDINATE Status_graph) {
	COORDINATE message_window = { message_window_x ,message_window_y, GetColor(255, 255, 255) };
	this->partner1_strategy = partner2_strategy = strategy_type::balance;
	if (turn == command) {
		this->turn_count = 0;
		this->command_cursole = 0;
		this->player_guard = false;
	}
	auto& mt = this->make_mt();
	print_status(sta, Status_graph);
	switch (turn) {
		case command:
			if(sta.player.hp != 0) this->command_cursole = choose_command(print_in_kanji);
			std::sort(this->turn_arr.begin(), this->turn_arr.end());
			turn_arr.pop_back();
			std::shuffle(this->turn_arr.begin(), this->turn_arr.end(), this->make_mt());
			turn_arr.emplace_back(check);
			this->turn = turn_arr[turn_count];
			break;
		case player_turn:
			if (sta.player.hp != 0 && sta.enemy.hp != 0) {
				player_turn_command(sta, Damage(sta.player.attack, sta.enemy.defence, 10, false), message_window);
				WaitKey();
			}
			turn_count++;
			this->turn = this->turn = turn_arr[turn_count];
			break;
		case partner1_turn:
			if (sta.partner1.hp != 0 && sta.enemy.hp != 0) {
				Strategy(print_in_kanji, message_window, "フレイムショット", this->magic_sound_handle).partner_command(this->partner1_strategy, sta.player, sta.partner1, sta.partner2, sta.enemy);
				WaitKey();
			}
			turn_count++;
			this->turn = this->turn = turn_arr[turn_count];
			break;
		case partner2_turn:
			if (sta.partner2.hp != 0 && sta.enemy.hp != 0) {
				Strategy(this->print_in_kanji, message_window, "フレイムショット", this->magic_sound_handle).partner_command(this->partner2_strategy, sta.player, sta.partner2, sta.partner1, sta.enemy);
				WaitKey();
			}
			turn_count++;
			this->turn = this->turn = turn_arr[turn_count];
			break;
		case enemy_turn:
			if (sta.enemy.hp != 0) {
				this->enemy_command(sta, message_window);
				WaitKey();
			}
			turn_count++;
			this->turn = this->turn = turn_arr[turn_count];
			break;
		case check:
			if (0 == end_check(sta)) {
				this->turn = command;
				break;
			}
			else return end_check(sta);
		default: return -1;
	}
	return 0;
}