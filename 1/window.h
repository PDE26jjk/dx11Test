#pragma once
#include "h.h"
#include "keyboard.h" 
#include "mouse.h"
#include <optional>
#include "Graphics.h"
#include <memory>
class Window {
private:
	class WindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
		
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;
		static constexpr const char* wndClassName = "kawaiiWindow";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	static int wins;//窗口数量，如果只有一个窗口则没啥卵用，所有框框被关了才退出程序。待扩展
	Window(int width,int height,const char* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator= (const Window&) = delete;
	void SetTitle(const char* title);
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard kbd;// 键盘输入
	Mouse mouse;// 鼠标输入
	
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;// 智能唯一指针指向图形
};