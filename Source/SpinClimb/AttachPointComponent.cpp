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
	PhysicsConstraint->SetDisableCollision(true);

	nearAttachmentPoints.Init(nullptr,0);
}


// Called when the game starts
void UAttachPointComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

//add any nearAttachmentPoints that this object is now in range of
void UAttachPointComponent::ObjectEntersRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<USceneComponent*> parents;
	OtherComp->GetParentComponents(parents);

	for (int i = 0; i < parents.Num(); i++) {
		UAttachPointComponent* attachComponent = Cast<UAttachPointComponent>(parents[i]);
		if (attachComponent != nullptr && attachComponent != this) {
			nearAttachmentPoints.Add(attachComponent);
			break;
		}
	}
	
}

//Remove any attachment points from nearAttachmentPoints that this one has moved out of range of
void UAttachPointComponent::ObjectExitsRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<USceneComponent*> parents;
	OtherComp->GetParentComponents(parents);

	for (int i = 0; i < parents.Num(); i++) {
		UAttachPointComponent* attachComponent = Cast<UAttachPointComponent>(parents[i]);
		if (attachComponent != nullptr && attachComponent != this) {
				nearAttachmentPoints.Remove(attachComponent);
		}
	}
}


// Called every frame
void UAttachPointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//returns if this point is eligble to be attached to another point (isn't already attached)
bool UAttachPointComponent::canAttach()
{
	return ConnectedAttachment == nullptr;
}

//attaches 2 attachpointcomponents via physics constrains
void UAttachPointComponent::attach(UAttachPointComponent* otherAttachmentPointComponent, bool isParent)
{
		ConnectedAttachment = otherAttachmentPointComponent;
		if (isParent) {
			PhysicsConstraint->SetConstrainedComponents(Cast<UPrimitiveComponent>(this->GetAttachParentActor()->GetRootComponent()), TEXT("None"), Cast<UPrimitiveComponent>(otherAttachmentPointComponent->GetAttachParentActor()->GetRootComponent()), TEXT("None"));
			otherAttachmentPointComponent->attach(this, false);
		}
}


//Blueprint called for player to try to attach this component to its near one, will select the 1st in nearAttachmentPoints array is multiple are viable. Returns if attachment was successful
bool UAttachPointComponent::attemptAttach()
{
	if (!canAttach()) { return false; }
	for (UAttachPointComponent* other : nearAttachmentPoints) {
		if (other->canAttach()) {
			attach(nearAttachmentPoints[0], true);
			return true;
		}
	}
	return false;
}

//Blueprint called to detach the components
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


