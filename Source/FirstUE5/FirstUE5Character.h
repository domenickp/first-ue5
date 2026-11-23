// Player character for the camera-perspective prototype.
//
// Starts in a side-scrolling view (a 3D world seen from the side, movement locked to a line),
// and can swing smoothly into first person when StartFirstPersonTransition() is called
// (e.g. by an APerspectiveSwitchTrigger placed in the level).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstUE5Character.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECameraPerspective : uint8
{
	SideScroll,
	Transitioning,
	FirstPerson
};

UCLASS()
class FIRSTUE5_API AFirstUE5Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFirstUE5Character();

	/** Begin the swing from side view into first person. Ignored unless currently in side view. */
	UFUNCTION(BlueprintCallable, Category = "Perspective")
	void StartFirstPersonTransition();

	/** Debug helper: return to the spawn point in side view so the transition can be replayed. */
	UFUNCTION(BlueprintCallable, Category = "Perspective")
	void ResetToSideScroll();

	UFUNCTION(BlueprintPure, Category = "Perspective")
	ECameraPerspective GetPerspective() const { return Perspective; }

	virtual void Tick(float DeltaSeconds) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	/** Grey placeholder body (stands in for a real character model). */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** Small block on the front of the body so you can see which way the character faces. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FacingMesh;

	// ---- Side view ----

	/** Camera position relative to the character in side view (world space). Negative Y = toward the viewer. */
	UPROPERTY(EditAnywhere, Category = "Perspective|Side View")
	FVector SideCameraOffset = FVector(0.f, -900.f, 150.f);

	/** Side camera tilt in degrees (negative looks slightly down). */
	UPROPERTY(EditAnywhere, Category = "Perspective|Side View")
	float SideCameraPitch = -8.f;

	UPROPERTY(EditAnywhere, Category = "Perspective|Side View", meta = (ClampMin = "10.0", ClampMax = "170.0"))
	float SideCameraFOV = 60.f;

	/** How quickly the side camera catches up with the character. Higher = tighter. 0 = no lag. */
	UPROPERTY(EditAnywhere, Category = "Perspective|Side View", meta = (ClampMin = "0.0"))
	float SideCameraLagSpeed = 8.f;

	// ---- First person ----

	UPROPERTY(EditAnywhere, Category = "Perspective|First Person", meta = (ClampMin = "10.0", ClampMax = "170.0"))
	float FirstPersonFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Perspective|First Person", meta = (ClampMin = "0.01"))
	float LookSensitivity = 1.f;

	// ---- Transition ----

	/** Length of the swing from side view into first person, in seconds. */
	UPROPERTY(EditAnywhere, Category = "Perspective|Transition", meta = (ClampMin = "0.1"))
	float TransitionDuration = 2.5f;

	/** Easing strength: 1 = constant speed, higher = slower start and finish. */
	UPROPERTY(EditAnywhere, Category = "Perspective|Transition", meta = (ClampMin = "1.0"))
	float TransitionEaseExponent = 2.5f;

	/** Point in the transition (0-1) at which the character's own body is hidden from the camera. */
	UPROPERTY(EditAnywhere, Category = "Perspective|Transition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HideBodyAtAlpha = 0.85f;

private:
	void CreateInputObjects();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpPressed();
	void JumpReleased();

	void EnterSideScrollMode();
	void EnterFirstPersonMode();
	void SetBodyVisibleToOwner(bool bVisible);

	FTransform ComputeSideViewTransform(float DeltaTime);
	FTransform ComputeFirstPersonTransform() const;

	ECameraPerspective Perspective = ECameraPerspective::SideScroll;
	float TransitionElapsed = 0.f;

	FVector SmoothedSideCameraLocation = FVector::ZeroVector;
	bool bSideCameraInitialized = false;

	FTransform SpawnTransform;

	// Input is defined in code for the prototype, so no input assets need to be created in the editor.
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ResetAction;
};
