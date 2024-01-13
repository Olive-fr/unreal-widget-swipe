# Why
The goal with this really small library is to allow you to create swipe feature with a widget, like in your photo app. I create it because it's a common feature on Mobile devices.
![Preview gif](Resources/git/gifs/Preview-horizontal.gif)
Also, this component can take full page slot:
![Preview gif](Resources/git/gifs/Adaptability.gif)

# Requirements
Manual installation require an Unreal C++ project and Unreal 5.3.
# Setup
At your root project folder, go to `Plugins` directory (create it if not present). 
Create a folder named `MobileWidgetSwipe` and copy all the files of this project in.
![Directory image](Resources/git/images/PluginsDirectory.png)

Open the project (restart if opened during the process) and go to `Edit/Plugins` and activate the new plugin MobileWidgetSwipe.
![Plugin activation](Resources/git/images/PluginsManager.png)

# Customization
## Orientation
You can use this component Horizontally ou Vertically.
![Tab menu example](Resources/git/gifs/Preview-horizontal.gif) ![Tab menu example](Resources/git/gifs/Preview-vertical.gif)
## Swipe action
### Blend Func
You can change the Blend function of the swipe animation.

![Directory image](Resources/git/images/Customization_SwipeBlendFuncs.png)

As example, see these three preview with 1.0 speed
#### Linear speed 1.0
![Tab menu example](Resources/git/gifs/Customization_linear_speed1.gif)
#### Sinusoidal in speed 1.0
![Tab menu example](Resources/git/gifs/Customization_sin_in_speed1.gif)
#### Sinusoidal out speed 1.0
![Tab menu example](Resources/git/gifs/Customization_sin_out_speed1.gif)

# Blueprint
## Children
It's like any other unreal panel, place the SwipeBox in your hiearchy, and put your elements as children.

![Plugin activation](Resources/git/images/Blueprint_UMG_Structure.png)

## Event
You can register your custom function with the Swipe Event.

![Plugin activation](Resources/git/images/Blueprint_UMG_Event.png)

## Change page
You can modify the page with Blueprint.

![Plugin activation](Resources/git/images/Blueprint_UMG_SetCurrentPage.png)

## Other
You will find many other properties to set in the properties panel and in the event graph.

# Usage Exemples
## Tab menu
If you have a tab menu, it's painful for the user to click to the most upper part of the screen. You can ease the process with swipe feature.

![Tab menu example](Resources/git/gifs/Example_tabmenu.gif)


# Disclaimer
All features in this component aren't fully implemented. The ScrollBox was use as the base of the development so there is some features of ScrollBox that appears in Properties panel but not ready yet.