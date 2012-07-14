#ifdef MOUSE_EXPORTS
#define MOUSE_API __declspec(dllexport)
#else
#define MOUSE_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251)

#include <glm\glm.hpp>

// A simple mouse wrapping class.
class MOUSE_API Mouse
{
private:
	glm::ivec2 currentPosition;
	glm::ivec2 lastPosition;

	bool isLeftButtonPressed;
	bool isRightButtonPressed;

	bool isMouseWheelUp;
	bool isMouseWheelDown;

public:
	Mouse();

	
	void SetCurrentPosition(glm::ivec2 newCurrentPosition);
	glm::ivec2 GetCurrentPosition();
	glm::ivec2 GetLastPosition();

	void OverrideLastPosition(glm::ivec2 newLastPosition);


	bool IsLeftButtonDown();
	bool IsRightButtonDown();
	
	bool IsMouseWheelUp();
	bool IsMouseWheelDown();

	void PressLeftButton();
	void ReleaseLeftButton();
	void PressRightButton();
	void ReleaseRightButton();

	void MouseWheelUp();
	void MouseWheelNotUp();
	void MouseWheelDown();
	void MouseWheelNotDown();
};