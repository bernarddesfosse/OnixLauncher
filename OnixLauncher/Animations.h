#pragma once
#include "UIManager.h"



struct UIAnimations {
	static inline LayoutAnimation DEFAULT;
	static inline LayoutAnimation LAUNCH_HOVERED;
	static inline LayoutAnimation SETTINGS_HOVERED;
	static inline LayoutAnimation LAUNCHERINFO_HOVERED;
	static inline LayoutAnimation DISCORDINVITE_HOVERED;
	static inline LayoutAnimation CLOSEBUTTON_HOVERED;
	static inline LayoutAnimation HIDEBUTTON_HOVERED;

	static void init();
};