// Fill out your copyright notice in the Description page of Project Settings.


#include "Attackable.h"

// Add default functionality here for any IAttackable functions that are not pure virtual.

void IAttackable::ReceiveDamage(float damageAmount)
{
}

bool IAttackable::IsAlive() const
{
    return false;
}

EMobilityEnvironmentType IAttackable::GetMobilityEnvironmentType() const
{
    return EMobilityEnvironmentType::MET_None;
}

float IAttackable::GetStealthFactor() const
{
    return 0.0f;
}
