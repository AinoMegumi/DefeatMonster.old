#include"Class.h"

int Damage::base() {
	int base = (this->attack - (this->defence / 2)) / 2;
	int damage_width = (base / 16);
	std::uniform_int_distribution<int> rand(damage_width * -1, damage_width);
	return base + rand(this->make_mt());
}

int Damage::normal_attack() {
	std::bernoulli_distribution rnd(0.5), rand(static_cast<double>(this->critical_percentage / 100));
	int damage = static_cast<int>(base() * (rand(this->make_mt()) ? 1.3 : 1) * (do_guard ? 0.5 : 1));
	return damage <= 0 ? rnd(this->make_mt()) : std::min(damage, 999);
}

int Damage::skill_attack(int base_power) {
	std::bernoulli_distribution rand(0.5);
	int damage = static_cast<int>((base_power + base()) * (this->do_guard ? 0.5 : 1));
	return damage <= 0 ? rand(this->make_mt()) : std::min(damage, 999);
}

int Damage::calc(attack_type attack_type, int base_power) {
	if (attack_type == attack_type::normal) return normal_attack();
	else if (attack_type == attack_type::skill) return skill_attack(base_power);
	else return 0;
}
