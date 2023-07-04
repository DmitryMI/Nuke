// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimeHUD.h"

void APlaytimeHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bSelectionBoxActive)
	{
		double
			x1 = selectionBoxStart.X,
			y1 = selectionBoxStart.Y,

			x2 = selectionBoxEnd.X,
			y2 = selectionBoxStart.Y,

			x3 = selectionBoxEnd.X,
			y3 = selectionBoxEnd.Y,

			x4 = selectionBoxStart.X,
			y4 = selectionBoxEnd.Y;

		bool srgb = true;

		DrawLine(x1, y1, x2, y2, selectionBoxColor, selectionBoxLineWidth);
		DrawLine(x2, y2, x3, y3, selectionBoxColor, selectionBoxLineWidth);
		DrawLine(x3, y3, x4, y4, selectionBoxColor, selectionBoxLineWidth);
		DrawLine(x4, y4, x1, y1, selectionBoxColor, selectionBoxLineWidth);
	}
}

void APlaytimeHUD::StartDrawSelectionBox(FVector2D startingPoint)
{
	bSelectionBoxActive = true;
	selectionBoxStart = startingPoint;
	selectionBoxEnd = startingPoint;
}

void APlaytimeHUD::UpdateSelectionBox(FVector2D endingPoint)
{
	selectionBoxEnd = endingPoint;
}

void APlaytimeHUD::EndDrawSelectionBox()
{
	bSelectionBoxActive = false;
}
