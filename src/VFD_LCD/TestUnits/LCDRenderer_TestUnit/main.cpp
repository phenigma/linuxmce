#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	LCDRenderer lcd_renderer("/dev/ttyUSB0");

	DisplayState display_state;

	display_state.m_sDescription = "This is a test";
	lcd_renderer.Render(display_state);

	return 0;
}
