// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "AttachPointComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPINCLIMB_API UAttachPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttachPointComponent();

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* AttachmentMesh;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* ConnectionRadius;

	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* PhysicsConstraint;

	UAttachPointComponent* ConnectedAttachment;

	UPROPERTY(BlueprintReadWrite)
	bool Active = true;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION()
	void ObjectEntersRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ObjectExitsRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool canAttach();

	void attach(UAttachPointComponent* otherAttachmentPointComponent, USceneComponent* ownAttachedComp, USceneComponent* otherAttachedComp, bool isParent);

	UFUNCTION(BlueprintCallable)
	void bpDetach() { detach(true); }

	void detach(bool isParent);
};
