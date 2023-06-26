// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nuke.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Nuke, "Nuke" );

TAutoConsoleVariable<bool> CVarShowInvisibleUnits(
    TEXT("fow.ShowInvisibleUnits"),
    false,
    TEXT("Enables of disables rendering of units hidden by UFogOfWarComponent. Does not affect Fog of War logic.\n")
    TEXT("\ttrue: units in fog of war will always be rendered\n")
    TEXT("\tfalse: units in fog of war will not be rendered\n"),
    ECVF_Cheat
);

TAutoConsoleVariable<bool> CVarRadarDrawLos(
    TEXT("radar.DrawLos"),
    false,
    TEXT("Enables of disables rendering of debug lines that represent line trace calculations.\n")
    TEXT("\ttrue: drawing enabled\n")
    TEXT("\tfalse: drawing disabled\n"),
    ECVF_Cheat
);

TAutoConsoleVariable<bool> CVarRadarDrawShapes(
    TEXT("radar.DrawShapes"),
    false,
    TEXT("Enables of disables rendering of debug shapes that represent radar irradiation areas.\n")
    TEXT("\ttrue: drawing enabled\n")
    TEXT("\tfalse: drawing disabled\n"),
    ECVF_Cheat
);


TAutoConsoleVariable<int> CVarRadarDrawShapesSegments(
    TEXT("radar.DrawShapesSegments"),
    16,
    TEXT("Number of segments in a sphere, used with radar.DrawShapes.\n"),
    ECVF_Cheat
);