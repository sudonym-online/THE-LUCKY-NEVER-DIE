#include "debug.h"
#include "raylib.h"
#include <cstdio>
#include <cstring>

Debug::Debug() : count(0) {}

void Debug::Log(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buf[MSG_LEN];
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (count < MAX_LOGS) count++;

	for (int i = count - 1; i > 0; i--)
		strncpy(messages[i], messages[i - 1], MSG_LEN);

	strncpy(messages[0], buf, MSG_LEN - 1);
	messages[0][MSG_LEN - 1] = '\0';
}

void Debug::Draw()
{
	int fontSize = 14;
	int lineHeight = fontSize + 4;
	int xPad = 10;
	int yStart = 10;
	int screenW = GetScreenWidth();

	for (int i = 0; i < count; i++) {
		int textW = MeasureText(messages[i], fontSize);
		int x = screenW - xPad - textW;
		int y = yStart + i * lineHeight;
		DrawText(messages[i], x, y, fontSize, BLACK);
	}
}
