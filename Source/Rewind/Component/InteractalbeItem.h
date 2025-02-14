// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "InteractalbeItem.generated.h"


class UWidgetComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteracted);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class REWIND_API UInteractalbeItem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractalbeItem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	void SetWidget(UWidgetComponent* Widget);
	UFUNCTION(BlueprintCallable)
	void SetBox(UBoxComponent* Box);

	UPROPERTY(BlueprintAssignable)
	FOnInteracted OnInteracted;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Interactalbe, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractableWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Interactalbe, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TryInteract();
	bool CanInteract = false;

	

		
};
