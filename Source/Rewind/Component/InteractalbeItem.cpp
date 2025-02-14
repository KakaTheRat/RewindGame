// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractalbeItem.h"

#include "Components/WidgetComponent.h"
#include "Rewind/RewindCharacter.h"

// Sets default values for this component's properties
UInteractalbeItem::UInteractalbeItem()
{

	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UInteractalbeItem::BeginPlay()
{
	Super::BeginPlay();

	if (ARewindCharacter* Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		Player->OnPlayerInteracted.AddDynamic(this, &ThisClass::TryInteract);
	}

}


// Called every frame
void UInteractalbeItem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractalbeItem::SetBox(UBoxComponent* Box)
{
	TriggerBox = Box;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UInteractalbeItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "In Zone");
	CanInteract = true;
}

void UInteractalbeItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Out Zone");
	CanInteract = false;
}

void UInteractalbeItem::TryInteract()
{
	if (CanInteract)
	{
		OnInteracted.Broadcast();
	}
}

