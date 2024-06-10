#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "BP_Character.generated.h"

// Enum to represent different states
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    None UMETA(DisplayName = "None"),
    Find UMETA(DisplayName = "Find"),
    Move UMETA(DisplayName = "Move"),
    Pursuit UMETA(DisplayName = "Pursuit"),
    Attack UMETA(DisplayName = "Attack")
};

UCLASS()
class TESTFSM_API ABP_Character : public ACharacter
{
    GENERATED_BODY()

public:
    ABP_Character();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    ECharacterState CurrentState;

    void HandleStateTransition();

    void FindState();
    void MoveState();
    void PursuitState();
    void AttackState();

    void PerformAttackCheck();

    FVector GetRandomDirection();
    void TransitionToState(ECharacterState NewState);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* DetectionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* AttackRangeSphere;

    UFUNCTION()
    void OnPlayerDetected(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerInRange(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    bool IsPlayerWithinDetectionRange();

    bool IsPlayerWithinAttackRange();

    float DelaySeconds = 2.0f;
    float MovementSpeed = 20000.0f;

    FVector PlayerLastKnownLocation;

    FTimerHandle TimerHandle_StateTransition;
    FTimerHandle TimerHandle_AttackCheck;
};
