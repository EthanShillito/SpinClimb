// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachPointComponent.h"
#include "Components/StaticMeshComponent.h"

#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, x);

// Sets default values for this component's properties
UAttachPointComponent::UAttachPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	AttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attachment Mesh"));
	AttachmentMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	AttachmentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ConnectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Connection Radius"));
	ConnectionRadius->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ConnectionRadius->OnComponentBeginOverlap.AddDynamic(this, &UAttachPointComponent::ObjectEntersRange);
	ConnectionRadius->OnComponentEndOverlap.AddDynamic(this, &UAttachPointComponent::ObjectExitsRange);
	ConnectionRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Physics Constraint"));
	PhysicsConstraint->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked,0);

}


// Called when the game starts
void UAttachPointComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAttachPointComponent::ObjectEntersRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<USceneComponent*> parents;
	OtherComp->GetParentComponents(parents);

	for (int i = 0; i < parents.Num(); i++) {
		UAttachPointComponent* attachComponent = Cast<UAttachPointComponent>(parents[i]);
		if (attachComponent != nullptr && attachComponent != this) {
			if (canAttach() && attachComponent->canAttach()) {
				attach(attachComponent, OverlappedComponent->GetAttachParentActor()->GetRootComponent(), OtherComp->GetAttachParentActor()->GetRootComponent(), true);

				break;
			}
		}
	}
	
}

void UAttachPointComponent::ObjectExitsRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


// Called every frame
void UAttachPointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAttachPointComponent::canAttach()
{
	return ConnectedAttachment == nullptr;
}

void UAttachPointComponent::attach(UAttachPointComponent* otherAttachmentPointComponent, USceneComponent* ownAttachedComp, USceneComponent* otherAttachedComp, bool isParent)
{
	if (Active && otherAttachmentPointComponent->Active) {
		ConnectedAttachment = otherAttachmentPointComponent;
		if (isParent) {
			PhysicsConstraint->SetConstrainedComponents(Cast<UPrimitiveComponent>(ownAttachedComp), TEXT("None"), Cast<UPrimitiveComponent>(otherAttachedComp), TEXT("None"));
		}
	}
	
}

void UAttachPointComponent::detach(bool isParent)
{
	if (ConnectedAttachment != nullptr) {
		PhysicsConstraint->BreakConstraint();
		if (isParent) { 
			ConnectedAttachment->detach(false); 
		}
		ConnectedAttachment = nullptr;
	}
}


