#include"Class.h"
#ifdef max
#undef max
#endif
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

KeyState::KeyState() noexcept : KeyStateBuf() {
	this->fllush_stream();
}

bool KeyState::update() noexcept {
	char buf[keybufsize];
	auto re = GetHitKeyStateAll(buf);
	if (0 != re) return false;
	for (size_t i = 0; i < keybufsize; ++i) {
		if (buf[i] && std::numeric_limits<int>::max() != this->KeyStateBuf[i])++this->KeyStateBuf[i];
		else this->KeyStateBuf[i] = 0;
	}
	return true;
}

bool KeyState::fllush() {
	if (this->fllush_stream()) return false;
	this->KeyStateBuf.fill(0);
	return true;
}

bool KeyState::fllush_stream() {
	char buf[2][keybufsize] = {};
	for (size_t i = 0; i < this->KeyStateBuf.size(); ++i) buf[0][i] = 0 != this->KeyStateBuf[i];
	char* first_p;
	char* last_p;
	size_t i;
	for (first_p = buf[0], last_p = buf[1]; 0 == DxLib::GetHitKeyStateAll(last_p); std::swap(first_p, last_p)) {
		for (i = 0; i < keybufsize && !first_p[i] && !last_p[i]; ++i);
		if (i == keybufsize) break;
	}
	return (i == keybufsize);
}

int KeyState::operator[](size_t n) const noexcept {
	return this->KeyStateBuf[n];
}

int KeyState::at(size_t n) const {
	return this->KeyStateBuf.at(n);
}

bool KeyState::esc() const noexcept {
	return 0 != this->KeyStateBuf[KEY_INPUT_ESCAPE];
}

bool KeyState::up() const noexcept {
	return 0 != this->KeyStateBuf[KEY_INPUT_W] || 0 != this->KeyStateBuf[KEY_INPUT_UP];
}

bool KeyState::down() const noexcept {
	return 0 != this->KeyStateBuf[KEY_INPUT_S] || 0 != this->KeyStateBuf[KEY_INPUT_DOWN];
}

bool KeyState::decide() const noexcept {
	return 0 != this->KeyStateBuf[KEY_INPUT_SPACE] || 0 != this->KeyStateBuf[KEY_INPUT_Z];
}

bool operator!=(const KeyState& l, size_t r) {
	return 0 == l[r];
}

int KeyState::cursole(const dxle::graph2d::screen& back, cint choise_size, cint x, cint y, uint cursole_color, uint back_color) {
	auto normal_con_f = []() ->bool {
		bool re = -1 != ProcessMessage() && 0 == ScreenFlip() && 0 == ClearDrawScreen();
		if (!re) throw std::runtime_error("ProcessMessage() return -1.");
		return re;
	};
	bool is_normal_state = normal_con_f();
	std::vector<dxle::graph2d::screen> screens;
	screens.reserve(choise_size);
	for (int i = 0; i < choise_size; ++i) {
		screens.push_back(std::move(dxle::MakeScreen(window_width, window_height).drawn_on([&back, choise_size, i, x, y, cursole_color, back_color]() {
			back.DrawGraph({}, true);
			for (int j = 0; j < choise_size; ++j) {
				DxLib::DrawBox(x, j * 16 + y, x + 16, (j + 1) * 16 + y, (i == j) ? cursole_color : back_color, true);
			}
		})));
	}
	assert(screens.size() == choise_size);
	int cur;
	screens.front().DrawGraph({}, true);
	is_normal_state = normal_con_f();
	for (cur = 0; (is_normal_state = -1 != ProcessMessage()) && update() && !decide();) {
		if (up() || down()) {
			if (up()) cur--;
			if (down()) cur++;
			fllush();
			cur = std::max(0, std::min(choise_size - 1, cur));
			screens[cur].DrawGraph({}, true);
			is_normal_state = normal_con_f();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	if (!is_normal_state) throw std::runtime_error("");
	return cur;
}

