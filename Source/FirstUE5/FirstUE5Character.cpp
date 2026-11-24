#include "FirstUE5Character.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "UObject/ConstructorHelpers.h"

AFirstUE5Character::AFirstUE5Character()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Side view: the character turns to face the way it moves; the controller doesn't rotate it.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.f, 720.f, 0.f);
	MoveComp->JumpZVelocity = 650.f;
	MoveComp->AirControl = 0.6f;
	MoveComp->MaxWalkSpeed = 500.f;

	// The camera's world transform is computed every frame in CalcCamera, so it ignores its parent.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetUsingAbsoluteLocation(true);
	Camera->SetUsingAbsoluteRotation(true);
	Camera->bUsePawnControlRotation = false;
	Camera->SetFieldOfView(SideCameraFOV);

	// Grey-box body built from the engine's basic shapes (100 x 100 x 100 units, centered pivot).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeScale3D(FVector(0.84f, 0.84f, 1.92f));
	if (CylinderMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderMesh.Object);
	}

	FacingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacingMesh"));
	FacingMesh->SetupAttachment(GetCapsuleComponent());
	FacingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FacingMesh->SetRelativeLocation(FVector(45.f, 0.f, 50.f));
	FacingMesh->SetRelativeScale3D(FVector(0.3f, 0.25f, 0.2f));
	if (CubeMesh.Succeeded())
	{
		FacingMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void AFirstUE5Character::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
	EnterSideScrollMode();
}

// ---------------------------------------------------------------------------------------------
// Perspective modes
// ---------------------------------------------------------------------------------------------

void AFirstUE5Character::EnterSideScrollMode()
{
	Perspective = ECameraPerspective::SideScroll;
	TransitionElapsed = 0.f;
	bSideCameraInitialized = false;

	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	// Lock movement to the vertical plane through the spawn point (the side-scrolling "track").
	MoveComp->SetPlaneConstraintEnabled(true);
	MoveComp->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	MoveComp->SetPlaneConstraintOrigin(SpawnTransform.GetLocation());

	SetBodyVisibleToOwner(true);
}

void AFirstUE5Character::StartFirstPersonTransition()
{
	if (Perspective != ECameraPerspective::SideScroll)
	{
		return;
	}

	Perspective = ECameraPerspective::Transitioning;
	TransitionElapsed = 0.f;

	// Stop walking; the player has no control during the swing. Vertical velocity is kept so a
	// mid-air character still lands naturally.
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->Velocity.X = 0.f;
	MoveComp->Velocity.Y = 0.f;
	MoveComp->bOrientRotationToMovement = false;

	// The first-person view ends up looking the way the character is facing, level with the horizon.
	if (Controller)
	{
		Controller->SetControlRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
	}
}

void AFirstUE5Character::EnterFirstPersonMode()
{
	Perspective = ECameraPerspective::FirstPerson;
	TransitionElapsed = TransitionDuration;

	// Free 3D movement; the body turns with the mouse/stick.
	bUseControllerRotationYaw = true;
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->SetPlaneConstraintEnabled(false);
	MoveComp->bOrientRotationToMovement = false;

	SetBodyVisibleToOwner(false);
}

void AFirstUE5Character::ResetToSideScroll()
{
	TeleportTo(SpawnTransform.GetLocation(), SpawnTransform.Rotator());
	GetCharacterMovement()->StopMovementImmediately();
	if (Controller)
	{
		Controller->SetControlRotation(SpawnTransform.Rotator());
	}
	EnterSideScrollMode();
}

void AFirstUE5Character::SetBodyVisibleToOwner(bool bVisible)
{
	// "Owner no see" hides the meshes from this player's camera only; other views still see them.
	BodyMesh->SetOwnerNoSee(!bVisible);
	FacingMesh->SetOwnerNoSee(!bVisible);
}

void AFirstUE5Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Perspective == ECameraPerspective::Transitioning)
	{
		TransitionElapsed += DeltaSeconds;
		const float Alpha = FMath::Clamp(TransitionElapsed / TransitionDuration, 0.f, 1.f);

		if (Alpha >= HideBodyAtAlpha)
		{
			SetBodyVisibleToOwner(false);
		}
		if (Alpha >= 1.f)
		{
			EnterFirstPersonMode();
		}
	}
}

// ---------------------------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------------------------

FTransform AFirstUE5Character::ComputeSideViewTransform(float DeltaTime)
{
	const FVector Target = GetActorLocation() + SideCameraOffset;

	if (!bSideCameraInitialized || SideCameraLagSpeed <= 0.f)
	{
		SmoothedSideCameraLocation = Target;
		bSideCameraInitialized = true;
	}
	else
	{
		SmoothedSideCameraLocation =
			FMath::VInterpTo(SmoothedSideCameraLocation, Target, DeltaTime, SideCameraLagSpeed);
	}

	// Look straight across at the track (toward +Y), tilted slightly down.
	const FRotator Rotation(SideCameraPitch, 90.f, 0.f);
	return FTransform(Rotation, SmoothedSideCameraLocation);
}

FTransform AFirstUE5Character::ComputeFirstPersonTransform() const
{
	return FTransform(GetControlRotation(), GetPawnViewLocation());
}

void AFirstUE5Character::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	// Called once per frame by the camera manager after movement has been applied, so the
	// camera never lags a frame behind the character.
	FVector Location;
	FQuat Rotation;
	float FOV;

	switch (Perspective)
	{
	case ECameraPerspective::SideScroll:
	{
		const FTransform Side = ComputeSideViewTransform(DeltaTime);
		Location = Side.GetLocation();
		Rotation = Side.GetRotation();
		FOV = SideCameraFOV;
		break;
	}
	case ECameraPerspective::Transitioning:
	{
		const FTransform Side = ComputeSideViewTransform(DeltaTime);
		const FTransform FirstPerson = ComputeFirstPersonTransform();
		const float Linear = FMath::Clamp(TransitionElapsed / TransitionDuration, 0.f, 1.f);
		const float Alpha = FMath::InterpEaseInOut(0.f, 1.f, Linear, TransitionEaseExponent);

		Location = FMath::Lerp(Side.GetLocation(), FirstPerson.GetLocation(), Alpha);
		Rotation = FQuat::Slerp(Side.GetRotation(), FirstPerson.GetRotation(), Alpha);
		FOV = FMath::Lerp(SideCameraFOV, FirstPersonFOV, Alpha);
		break;
	}
	case ECameraPerspective::FirstPerson:
	default:
	{
		const FTransform FirstPerson = ComputeFirstPersonTransform();
		Location = FirstPerson.GetLocation();
		Rotation = FirstPerson.GetRotation();
		FOV = FirstPersonFOV;
		break;
	}
	}

	Camera->SetWorldLocationAndRotation(Location, Rotation);
	Camera->SetFieldOfView(FOV);

	Super::CalcCamera(DeltaTime, OutResult);
}

// ---------------------------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------------------------

void AFirstUE5Character::CreateInputObjects()
{
	if (InputContext)
	{
		return;
	}

	InputContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Prototype"));

	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;

	LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
	LookAction->ValueType = EInputActionValueType::Axis2D;

	JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
	JumpAction->ValueType = EInputActionValueType::Boolean;

	ResetAction = NewObject<UInputAction>(this, TEXT("IA_DebugReset"));
	ResetAction->ValueType = EInputActionValueType::Boolean;

	auto Swizzle = [this](FEnhancedActionKeyMapping& Mapping)
	{
		Mapping.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(InputContext));
	};
	auto Negate = [this](FEnhancedActionKeyMapping& Mapping, bool bX, bool bY)
	{
		UInputModifierNegate* Mod = NewObject<UInputModifierNegate>(InputContext);
		Mod->bX = bX;
		Mod->bY = bY;
		Mod->bZ = false;
		Mapping.Modifiers.Add(Mod);
	};
	auto DeadZone = [this](FEnhancedActionKeyMapping& Mapping)
	{
		Mapping.Modifiers.Add(NewObject<UInputModifierDeadZone>(InputContext));
	};

	// Move: X = right/left, Y = forward/back (same layout as Epic's templates).
	Swizzle(InputContext->MapKey(MoveAction, EKeys::W));
	{
		FEnhancedActionKeyMapping& S = InputContext->MapKey(MoveAction, EKeys::S);
		Swizzle(S);
		Negate(S, true, true);
	}
	Negate(InputContext->MapKey(MoveAction, EKeys::A), true, true);
	InputContext->MapKey(MoveAction, EKeys::D);
	DeadZone(InputContext->MapKey(MoveAction, EKeys::Gamepad_Left2D));

	// Look (first person only).
	Negate(InputContext->MapKey(LookAction, EKeys::Mouse2D), false, true);
	{
		FEnhancedActionKeyMapping& Stick = InputContext->MapKey(LookAction, EKeys::Gamepad_Right2D);
		DeadZone(Stick);
		Negate(Stick, false, true);
	}

	InputContext->MapKey(JumpAction, EKeys::SpaceBar);
	InputContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);

	// Debug: replay the transition.
	InputContext->MapKey(ResetAction, EKeys::BackSpace);
}

void AFirstUE5Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	CreateInputObjects();
	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContext, 0);
		}
	}
}

void AFirstUE5Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CreateInputObjects();
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstUE5Character::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstUE5Character::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AFirstUE5Character::JumpPressed);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFirstUE5Character::JumpReleased);
		Input->BindAction(ResetAction, ETriggerEvent::Started, this, &AFirstUE5Character::ResetToSideScroll);
	}
}

void AFirstUE5Character::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	switch (Perspective)
	{
	case ECameraPerspective::SideScroll:
		// Left/right along the track only.
		AddMovementInput(FVector::ForwardVector, Axis.X);
		break;

	case ECameraPerspective::FirstPerson:
	{
		const FRotator YawOnly(0.f, GetControlRotation().Yaw, 0.f);
		const FRotationMatrix Basis(YawOnly);
		AddMovementInput(Basis.GetUnitAxis(EAxis::X), Axis.Y);
		AddMovementInput(Basis.GetUnitAxis(EAxis::Y), Axis.X);
		break;
	}

	case ECameraPerspective::Transitioning:
	default:
		break; // No control during the swing.
	}
}

void AFirstUE5Character::Look(const FInputActionValue& Value)
{
	if (Perspective != ECameraPerspective::FirstPerson)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X * LookSensitivity);
	AddControllerPitchInput(Axis.Y * LookSensitivity);
}

void AFirstUE5Character::JumpPressed()
{
	if (Perspective != ECameraPerspective::Transitioning)
	{
		Jump();
	}
}

void AFirstUE5Character::JumpReleased()
{
	StopJumping();
}
