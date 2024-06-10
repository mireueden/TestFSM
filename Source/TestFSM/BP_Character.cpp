#include "BP_Character.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ABP_Character::ABP_Character()
{
    PrimaryActorTick.bCanEverTick = true;



    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(RootComponent);
    DetectionSphere->InitSphereRadius(500.0f); 
    DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::OnPlayerDetected);

    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);
    AttackRangeSphere->InitSphereRadius(200.0f); 
    AttackRangeSphere->SetCollisionProfileName(TEXT("Trigger"));
    AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::OnPlayerInRange);
}

void ABP_Character::BeginPlay()
{
    Super::BeginPlay();

    TransitionToState(ECharacterState::Find); 
    UE_LOG(LogTemp, Warning, TEXT("Start"));
}

void ABP_Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABP_Character::TransitionToState(ECharacterState NewState)
{
    CurrentState = NewState;

    GetWorldTimerManager().SetTimer(TimerHandle_StateTransition, this, &ABP_Character::HandleStateTransition, 2.0f, false);
}

void ABP_Character::HandleStateTransition()
{
    switch (CurrentState)
    {
    case ECharacterState::Find:
        UE_LOG(LogTemp, Warning, TEXT("Current State: Find"));
        FindState();
        break;
    case ECharacterState::Move:
        UE_LOG(LogTemp, Warning, TEXT("Current State: Move"));
        MoveState();
        break;
    case ECharacterState::Pursuit:
        UE_LOG(LogTemp, Warning, TEXT("Current State: Pursuit"));
        PursuitState();
        break;
    case ECharacterState::Attack:
        UE_LOG(LogTemp, Warning, TEXT("Current State: Attack"));
        AttackState();
        break;
    default:
        break;
    }
}
void ABP_Character::FindState()
{
    bool bEnemyFound = false;
    TArray<AActor*> OverlappingActors;
    DetectionSphere->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->IsA(ACharacter::StaticClass()) && Actor != this)
        {
            bEnemyFound = true;
            ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
            if (PlayerCharacter)
            {
                PlayerLastKnownLocation = PlayerCharacter->GetActorLocation();
            }
            UE_LOG(LogTemp, Warning, TEXT("Player detected."));
            TransitionToState(ECharacterState::Pursuit);
            return;
        }
    }

    // 플레이어를 감지하지 못한 경우에는 Move 상태로 전환
    if (!bEnemyFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player not detected"));
        TransitionToState(ECharacterState::Move);
        return;
    }
}


void ABP_Character::MoveState()
{
    UE_LOG(LogTemp, Warning, TEXT("Move"));

    FVector MovementVector = GetRandomDirection() * MovementSpeed*GetWorld()->GetDeltaSeconds();
    FVector NewLocation = GetActorLocation() + MovementVector;

    SetActorLocation(NewLocation);
    //AddMovementInput(MovementVector);

    TransitionToState(ECharacterState::Find);
    return;
}


void ABP_Character::PursuitState()
{
    if (PlayerLastKnownLocation != FVector::ZeroVector)
    {
        FVector DirectionToPlayer = (PlayerLastKnownLocation - GetActorLocation()).GetSafeNormal();
        FVector NewLocation = PlayerLastKnownLocation;
        SetActorLocation(NewLocation);

        if (IsPlayerWithinAttackRange())
            TransitionToState(ECharacterState::Attack);
        else 
            TransitionToState(ECharacterState::Find);

        return;
    }
    else
    {
        TransitionToState(ECharacterState::Find);
        return;
    }
  
}

void ABP_Character::AttackState()
{
    UE_LOG(LogTemp, Warning, TEXT("Attacking the enemy"));

    GetWorldTimerManager().SetTimer(TimerHandle_AttackCheck, this, &ABP_Character::PerformAttackCheck, 1.0f, false);
    return;
}

void ABP_Character::PerformAttackCheck()
{
    bool bWithinAttackRange = IsPlayerWithinAttackRange();
    if (!bWithinAttackRange)
    {
        TransitionToState(ECharacterState::Pursuit);
        return;
    }
    else
    {
        TransitionToState(ECharacterState::Attack);
        return;
    }
}


FVector ABP_Character::GetRandomDirection()
{
    float RandomAngle = FMath::FRandRange(0.0f, 2.0f * PI); // 랜덤 각도 생성
    float RandomX = FMath::Cos(RandomAngle); // 코사인 값으로 X 성분 계산
    float RandomY = FMath::Sin(RandomAngle); // 사인 값으로 Y 성분 계산

    return FVector(RandomX, RandomY, 0.0f); // Z 성분은 0으로 설정하여 2D 공간에서 움직임
}


void ABP_Character::OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        if (OtherActor->IsA(ACharacter::StaticClass()))
        {
            TransitionToState(ECharacterState::Pursuit);
        }
    }
}

void ABP_Character::OnPlayerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        if (OtherActor->IsA(ACharacter::StaticClass()))
        {
            TransitionToState(ECharacterState::Attack);
        }
    }
}

bool ABP_Character::IsPlayerWithinDetectionRange()
{
    TArray<AActor*> OverlappingActors;
    DetectionSphere->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->IsA(ACharacter::StaticClass()))
        {
            return true;
        }
    }
    return false;
}

bool ABP_Character::IsPlayerWithinAttackRange()
{
    TArray<AActor*> OverlappingActors;
    AttackRangeSphere->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (ACharacter* Character = Cast<ACharacter>(Actor))
        {
            if (Character->IsPlayerControlled())
            {
                return true;
            }
        }
    }
    return false;
}
