// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"

void AFighter::BeginPlay()
{
	Super::BeginPlay();

	fighterRadar = GetComponentByClass<URadarConeComponent>();
	ensure(fighterRadar);

	fighterRadar->SetRadarRange(radarRange);
	fighterRadar->SetConeAngleDeg(radarConeAngle);
}

AFighter::AFighter() : Super()
{

}
