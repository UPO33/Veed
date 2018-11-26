enum class EEventHandleResult
{
	Handled,
	NotHandled,
	
};
enum class EPaiKey
{
	Left, Right, Any
};
enum class EKeyEvent
{
	Pressed, Released, DoubleClick
};
struct VInputButtonEvent
{
	EKeyCode Key;
	EKeyEvent Event;
	
	float mPressTime;
	bool IsShiftDown() const;
	bool IsCtrlDown() const;
	float GetHoldTime() const;
};
struct VRect2D
{
	VInt2 mTopLeft;
	VInt2 mBottomRight;
};

struct VWidget
{
	VRect2D mRect;
	
	virtual EEventHandleResult OnInputButton(const VInputButtonEvent& event) = 0;
	//is called when cursor enter this widget
	virtual EEventHandleResult OnCursorEnter() =  0;
	//is called when cursor left this widget
	virtual EEventHandleResult OnCursorLeave() = 0;
	//is called when cursor moved on this widget
	virtual EEventHandleResult OnCursourMove() = 0;
	//is called when this widget lost the focus
	virtual void OnLostFocus() = 0;
	//get the next widget we must focus to when user pressed tab
	virtual VWidget* GetFrontFocus() = 0;
	//get the widget that we must focus to when user pressed ctrl + tab
	virtual VWidget* GetBackFocus() = 0;
	
	virtual VWidget* GetParentWidget() const = 0;
	virtual VWidget* GetChild(int index) const = 0;
};

struct VTextButton : VWidget
{
	VTextRenderBObject mButtonText;
};

struct VVertialBoxLayout
{
	
	VArray<VWidget*> Children;
	
	void ArrangeChildren()
	{
		VInt2 vSize(0,0);
		for(VWidget* pChild : mChildren)
		{
			vSize = VMax pChild->CalcSize();
		}
	}
};

struct VGuiSys
{
	//mouse position in screen space
	VInt2 MousePosition;
	//
	VGuiWidget* CuurentHoverWidget;
	//widget that has keyboard focus at the moment
	VGuiWidget* KeyboardFocusWidget;
	//current window that has focus
	VGuiWindow* CurrentWidnow;
	
	
	
};