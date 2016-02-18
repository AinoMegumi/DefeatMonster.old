#include"Class.h"
#include<limits>
#ifdef max
#undef max
#endif

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

void KeyState::cursole(cint max, cint x, cint y, int &cursole_point, uint cursole_color, uint back_color) {
	int cur = 0;
	auto normal_con_f = []() ->bool { return -1 != ProcessMessage(); };
	volatile bool is_normal_state;
	while (is_normal_state = normal_con_f()) {
		DrawBox(x, y, x + 16, (max + 1) * 16 + y, back_color, TRUE);
		DrawBox(x, cur * 16 + y, x + 16, (cur + 1) * 16 + y, cursole_color, TRUE);
		ScreenFlip();
		if (up() && cur != 0) cur--;
		if (down() && cur != max) cur++;
		if (decide()) break;
		Sleep(20);
	}
	if (!is_normal_state) throw std::runtime_error("");
	cursole_point = cur;
}

void cursole(cint max, cint x, cint y, int &cursole_point, uint cursole_color, uint back_color) {
	int cur = 0;
	auto normal_con_f = []() ->bool { return -1 != ProcessMessage(); };
	volatile bool is_normal_state;
	while (is_normal_state = normal_con_f()) {
		DrawBox(x, y, x + 16, (max + 1) * 16 + y, back_color, TRUE);
		DrawBox(x, cur * 16 + y, x + 16, (cur + 1) * 16 + y, cursole_color, TRUE);
		ScreenFlip();
		while (ProcessMessage() == 0 && CheckHitKeyAll() != 0) {}
		while (ProcessMessage() == 0 && CheckHitKeyAll() == 0) {}
		if (CheckHitKey(KEY_INPUT_W) == 1 && cur != 0) cur--;
		if (CheckHitKey(KEY_INPUT_S) == 1 && cur != max) cur++;
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) break;
	}
	if (!is_normal_state) throw std::runtime_error("");
	cursole_point = cur;
}