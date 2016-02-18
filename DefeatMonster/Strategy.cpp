#include"Class.h"
#include<cmath>

void Strategy::resuscitation(Status& user, Status& target) {
	Message msg(user.name, target.name, this->print_in_kanji, this->message_window);
	msg.magic_attack("リザレクション", this->magic_sound_handle);
	user.mp -= 30;
	std::uniform_int_distribution<int> rand(1, 10);
	std::bernoulli_distribution rnd(0.5);
	if (rnd(this->make_mt())) {
		target.hp = std::min(user.magic_attack + rand(this->make_mt()), target.max_hp);
		user.mp -= 30;
		msg.resuscitation_success();
	}
	else msg.resuscitation_failure();
	this->damage = -1;
}

void Strategy::carel(Status &user, Status &target) {
	Message msg(user.name, target.name, this->print_in_kanji, this->message_window);
	msg.magic_attack("ケアル", this->magic_sound_handle);
	user.mp -= 7;
	std::uniform_int_distribution<int> rand(1, 10);
	int cure_point = 70 + rand(this->make_mt());
	msg.cure(cure_point);
	target.hp = std::min(target.hp + cure_point, target.max_hp);
	this->damage = -1;
}

void Strategy::attack_importance(Status &user, Status &target) {
	Message msg(user.name, target.name, this->print_in_kanji, this->message_window);
	std::uniform_int_distribution<int> rand(1, abs(user.attack - user.magic_attack));
	if ((user.attack + rand(this->make_mt()) < user.magic_attack + rand(this->make_mt())) && user.mp >= 7) {
		msg.magic_attack(magic_name.c_str(), this->magic_sound_handle);
		this->damage = Damage(user.magic_attack, target.magic_defence, 0, false).calc(attack_type::skill, 40);
		user.mp -= 7;
	}
	else {
		msg.attack();
		this->damage = Damage(user.attack, target.defence, 10, false).calc(attack_type::normal, 0);
	}
}

bool Strategy::check_hp_percentage(int hp, int max_hp, int check_percentage) {
	return ((hp * 100 / max_hp) <= check_percentage) && hp != 0;
}

bool Strategy::check_hp_zero(int hp) {
	return hp == 0;
}

void Strategy::balance(Status &player, Status &user, Status &other_partner, Status &enemy) {
	if (this->check_hp_zero(player.hp) && user.mp >= 30) this->resuscitation(user, player);
	else if (this->check_hp_zero(other_partner.hp) && user.mp >= 30) this->resuscitation(user, other_partner);
	else if (this->check_hp_percentage(player.hp, player.max_hp, 20) && user.mp >= 7) this->carel(user, player);
	else if (this->check_hp_percentage(user.hp, user.max_hp, 20) && user.mp >= 7) this->carel(user, user);
	else if (this->check_hp_percentage(other_partner.hp, other_partner.max_hp, 20) && user.mp >= 7) this->carel(user, other_partner);
	else attack_importance(user, enemy);
}

void Strategy::cure_importance(Status &player, Status &user, Status &other_partner, Status &enemy) {
	if (this->check_hp_percentage(player.hp, player.max_hp, 0) && user.mp >= 30) this->resuscitation(user, player);
	else if (this->check_hp_percentage(other_partner.hp, other_partner.max_hp, 0) && user.mp >= 30) this->resuscitation(user, other_partner);
	else if (this->check_hp_percentage(player.hp, player.max_hp, 45) && user.mp >= 7) carel(user, player);
	else if (this->check_hp_percentage(user.hp, user.max_hp, 45) && user.mp >= 7) carel(user, user);
	else if (this->check_hp_percentage(other_partner.hp, other_partner.max_hp, 45) && user.mp >= 7) carel(other_partner, user);
	else attack_importance(user, enemy);
}

void Strategy::partner_command(strategy_type strategy_type, Status &player, Status &user, Status &other_partner, Status &enemy) {
	Message msg(user.name, enemy.name, this->print_in_kanji, this->message_window);
	switch (strategy_type) {
	case strategy_type::attack:
		this->attack_importance(user, enemy);
		msg.damage_to_enemy(this->damage);
		break;
	case strategy_type::balance:
		this->balance(player, user, other_partner, enemy);
		if(this->damage != -1) msg.damage_to_enemy(this->damage);
		break;
	case strategy_type::cure:
		this->cure_importance(player, user, other_partner, enemy);
		if (this->damage != -1) msg.damage_to_enemy(this->damage);
		break;
	default:
		throw std::runtime_error("partner command error");
	}
	if(this->damage != -1) enemy.hp = std::max(enemy.hp - this->damage, 0);
}
