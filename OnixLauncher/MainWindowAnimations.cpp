#include "Animations.h"

void UIAnimations::init() {



#pragma region Default
	{
		LayoutAnimation& animation = DEFAULT;
		animation.name = "DEFAULT";
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)47.468;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)306.962;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)78.059;
			animationData.posY = (float)152.717;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)34.6;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)79.114;
			animationData.posY = (float)192.364;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)373.418;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)180.38;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)401.471;
			animationData.posY = (float)164.211;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)32.272;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region LaunchHovered 
	{
		LayoutAnimation& animation = LAUNCH_HOVERED;
		animation.name = "LAUNCH_HOVERED";

		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)31.915;
			animationData.posY = (float)124.113;
			animationData.sizeX = (float)338.652;
			animationData.sizeY = (float)127.66;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)67.376;
			animationData.posY = (float)148.936;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)37.796;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)67.376;
			animationData.posY = (float)191.489;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)19.325;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)388.298;
			animationData.posY = (float)134.752;
			animationData.sizeX = (float)163.121;
			animationData.sizeY = (float)108.156;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)413.121;
			animationData.posY = (float)166.667;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)29;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region SettingsHovered
	{
		LayoutAnimation& animation = SETTINGS_HOVERED;
		animation.name = "SETTINGS_HOVERED";
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)67.376;
			animationData.posY = (float)136.525;
			animationData.sizeX = (float)278.369;
			animationData.sizeY = (float)106.383;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)95.745;
			animationData.posY = (float)157.801;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)31.199;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)95.745;
			animationData.posY = (float)193.262;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)15.806;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)363.475;
			animationData.posY = (float)124.113;
			animationData.sizeX = (float)196.809;
			animationData.sizeY = (float)129.433;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)393.617;
			animationData.posY = (float)163.121;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)35.45;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region LAUNCHERINFO_HOVERED
	{
		LayoutAnimation& animation = LAUNCHERINFO_HOVERED;
		animation.name = "LAUNCHERINFO_HOVERED";
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)47.468;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)306.962;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)78.059;
			animationData.posY = (float)152.717;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)34.6;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)79.114;
			animationData.posY = (float)192.364;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)373.418;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)180.38;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)401.471;
			animationData.posY = (float)164.211;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)32.272;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072 - 4;
			animationData.posY = (float)286.617 - 4;
			animationData.sizeX = (float)129.747 + 8;
			animationData.sizeY = (float)34.81 + 8;
			animationData.fontSize = (float)18.885;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)63.83;
			animationData.posY = (float)289.007;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)19.618;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region DISCORDINVITE_HOVERED
	{
		LayoutAnimation& animation = DISCORDINVITE_HOVERED;
		animation.name = "DISCORDINVITE_HOVERED";

		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)47.468;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)306.962;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)78.059;
			animationData.posY = (float)152.717;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)34.6;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)79.114;
			animationData.posY = (float)192.364;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)373.418;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)180.38;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)401.471;
			animationData.posY = (float)164.211;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)32.272;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)501.773;
			animationData.posY = (float)280.142;
			animationData.sizeX = (float)56.738;
			animationData.sizeY = (float)42.553;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region CLOSEBUTTON_HOVERED
	{
		LayoutAnimation& animation = CLOSEBUTTON_HOVERED;
		animation.name = "CLOSEBUTTON_HOVERED";
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)47.468;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)306.962;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)78.059;
			animationData.posY = (float)152.717;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)34.6;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)79.114;
			animationData.posY = (float)192.364;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)373.418;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)180.38;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)401.471;
			animationData.posY = (float)164.211;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)32.272;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)553.191;
			animationData.posY = (float)14.184;
			animationData.sizeX = (float)30.142;
			animationData.sizeY = (float)30.142;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion


#pragma region HIDEBUTTON_HOVERED
	{
		LayoutAnimation& animation = HIDEBUTTON_HOVERED;
		animation.name = "HIDEBUTTON_HOVERED";
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixLogo";
			animationData.posX = (float)47.468;
			animationData.posY = (float)40.78;
			animationData.sizeX = (float)66.456;
			animationData.sizeY = (float)66.456;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientText";
			animationData.posX = (float)118.143f;
			animationData.posY = (float)31.646;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)45.197;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncButton";
			animationData.posX = (float)47.468;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)306.962;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.508;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LaunchButtonText";
			animationData.posX = (float)78.059;
			animationData.posY = (float)152.717;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)34.6;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinecraftVersionText";
			animationData.posX = (float)79.114;
			animationData.posY = (float)192.364;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsButton";
			animationData.posX = (float)373.418;
			animationData.posY = (float)129.747;
			animationData.sizeX = (float)180.38;
			animationData.sizeY = (float)117.089;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "SettingsText";
			animationData.posX = (float)401.471;
			animationData.posY = (float)164.211;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)32.272;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoButton";
			animationData.posX = (float)59.072;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)129.747;
			animationData.sizeY = (float)34.81;
			animationData.fontSize = (float)18.834;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "LauncherInfoText";
			animationData.posX = (float)70.588;
			animationData.posY = (float)291.169;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)17.542;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "DiscordInvite";
			animationData.posX = (float)506.329;
			animationData.posY = (float)286.617;
			animationData.sizeX = (float)47.468;
			animationData.sizeY = (float)34.81;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "OnixClientVersionText";
			animationData.posX = (float)122.307;
			animationData.posY = (float)88.608;
			animationData.sizeX = (float)0;
			animationData.sizeY = (float)0;
			animationData.fontSize = (float)16.766;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "CloseButton";
			animationData.posX = (float)556.962;
			animationData.posY = (float)18.987;
			animationData.sizeX = (float)22.152;
			animationData.sizeY = (float)22.152;

			animation.actions.emplace_back(animationData);
		}
		{
			LayoutAnimation::Data animationData;
			animationData.elementName = "MinimizeButton";
			animationData.posX = (float)522.152 - 3.f;
			animationData.posY = (float)18.987 - 3.f;
			animationData.sizeX = (float)22.152 + 6.f;
			animationData.sizeY = (float)22.152 + 6.f;

			animation.actions.emplace_back(animationData);
		}
	}
#pragma endregion
}
