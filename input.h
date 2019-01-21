int input[25];
enum InputLabels { Up, Down, Left, Right, Camera, FlyUp, FlyDown, Next, Prev, Add, Remove, Height, Pause, MouseX, MouseY, Click, AltClick, FinishTrack, Boost, ChainLift };

void initInput(void);

void	keyPress(unsigned char key, int x, int y);
void	keyRelease(unsigned char key, int x, int y);
void	specialKeyPress(int key, int x, int y);
void	specialKeyRelease(int key, int x, int y);
void	mouseMovement(int x, int y);
void	mouseClick(int button, int state, int x, int y);
void	consumeMouseInput(void);
