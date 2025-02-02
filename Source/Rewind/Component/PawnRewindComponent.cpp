// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnRewindComponent.h"

// Sets default values for this component's properties
UPawnRewindComponent::UPawnRewindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UPawnRewindComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UPawnRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

