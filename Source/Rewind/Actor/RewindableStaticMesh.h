#pragma once

#include "CoreMinimal.h"
#include "Rewind/Component/RewindComponent.h"
#include "GameFramework/Actor.h"
#include "RewindableStaticMesh.generated.h"



class URewindComponent;

UCLASS()
class REWIND_API ARewindableStaticMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewindableStaticMesh();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RewindableObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URewindComponent* RewindComponent;
	

};
