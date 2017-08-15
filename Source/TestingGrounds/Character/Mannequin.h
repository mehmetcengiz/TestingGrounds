#pragma once

#include "GameFramework/Character.h"
#include "Mannequin.generated.h"

UENUM()
enum class EFiringState : uint8 {
	READY,
	FIRING,
	NOT_READY,
	GETTING_READY,
	OUT_OF_AMMO,
	RELOADING
};


UCLASS()
class TESTINGGROUNDS_API AMannequin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMannequin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Death")
	void Death();

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AGun> GunBlueprint;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PullTrigger();

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	AGun* Gun;

	void ReleaseTrigger();

	UPROPERTY()
	bool bIsPlayerShooting;
	
	EFiringState FiringState;
	
	UPROPERTY(EditdefaultsOnly, Category = "Firing")
	float PrimaryFiringRate = 0.15;

	void MakeReadyGunToNextShot();
	void HandleFiringRate();
	void ShootPrimary();

};