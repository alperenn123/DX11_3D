#pragma once
#include "Window.h"
#include "Timer.h"
class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window m_wnd;
	Timer m_timer;
};

