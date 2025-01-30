// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindableStaticMesh.h"

#include "Rewind/Component/RewindComponent.h"

// Sets default values
ARewindableStaticMesh::ARewindableStaticMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RewindableObject = CreateDefaultSubobject<UStaticMeshComponent>("RewindableStaticMesh");
	RewindComponent = CreateDefaultSubobject<URewindComponent>("RewindComponent");
	RewindComponent->SetRewindableObject(RewindableObject);

}

// Called when the game starts or when spawned
void ARewindableStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARewindableStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

