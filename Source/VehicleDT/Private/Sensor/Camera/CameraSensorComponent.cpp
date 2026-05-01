#include "Sensor/Camera/CameraSensorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogCameraSensor, Log, All);

UCameraSensorComponent::UCameraSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(this);
}

void UCameraSensorComponent::OnRegister()
{
	Super::OnRegister();

	if (SceneCapture && !SceneCapture->IsRegistered())
	{
		SceneCapture->SetupAttachment(this);
		SceneCapture->RegisterComponent();
	}
}

void UCameraSensorComponent::BeginPlay()
{
	Super::BeginPlay();
	SetPreset(Preset);
	InitCapture();
}

void UCameraSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopTimer();
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UCameraSensorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UCameraSensorComponent, Preset))
	{
		SetPreset(Preset);
	}
}
#endif

void UCameraSensorComponent::InitCapture()
{
	if (!GetOwner())
	{
		UE_LOG(LogCameraSensor, Error, TEXT("[CameraSensor] No owning actor."));
		return;
	}

	BuildRenderTarget();
	ConfigureCapture();
	ApplyPostProcess();
	ApplyDistortion();

	if (bEnabled)
		StartTimer();

	UE_LOG(LogCameraSensor, Log, TEXT("[CameraSensor] Ready — %dx%d @ %.0f Hz, FOV %.0f deg"),
		Spec.Width, Spec.Height, Spec.FrameRate, Spec.FOV);
}

void UCameraSensorComponent::BuildRenderTarget()
{
	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("CameraRT"));
	RenderTarget->InitAutoFormat(Spec.Width, Spec.Height);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RenderTarget->bAutoGenerateMips  = false;
	RenderTarget->UpdateResourceImmediate(true);
}

void UCameraSensorComponent::ConfigureCapture()
{
	if (!SceneCapture || !RenderTarget)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("[CameraSensor] SceneCapture or RenderTarget missing."));
		return;
	}

	SceneCapture->TextureTarget               = RenderTarget;
	SceneCapture->FOVAngle                    = Spec.FOV;
	SceneCapture->bCaptureEveryFrame          = false;
	SceneCapture->bCaptureOnMovement          = false;
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->CaptureSource               = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->PostProcessBlendWeight      = 1.0f;
}

void UCameraSensorComponent::ApplyPostProcess()
{
	if (!SceneCapture) return;

	FPostProcessSettings& PP = SceneCapture->PostProcessSettings;

	PP.bOverride_VignetteIntensity    = true;
	PP.VignetteIntensity              = PostProcess.VignetteIntensity;

	PP.bOverride_BloomIntensity       = true;
	PP.BloomIntensity                 = PostProcess.BloomIntensity;

	PP.bOverride_MotionBlurAmount     = true;
	PP.MotionBlurAmount               = PostProcess.MotionBlurAmount;

	PP.bOverride_SceneFringeIntensity = true;
	PP.SceneFringeIntensity           = PostProcess.ChromaticAberration;

	PP.bOverride_LensFlareIntensity   = true;
	PP.LensFlareIntensity             = PostProcess.LensFlareIntensity;

	if (Exposure.bEnabled)
	{
		PP.bOverride_AutoExposureMethod       = true;
		PP.AutoExposureMethod                 = EAutoExposureMethod::AEM_Histogram;
		PP.bOverride_AutoExposureMinBrightness = true;
		PP.AutoExposureMinBrightness          = Exposure.MinEV;
		PP.bOverride_AutoExposureMaxBrightness = true;
		PP.AutoExposureMaxBrightness          = Exposure.MaxEV;
		PP.bOverride_AutoExposureSpeedUp      = true;
		PP.AutoExposureSpeedUp                = Exposure.SpeedUp;
		PP.bOverride_AutoExposureSpeedDown    = true;
		PP.AutoExposureSpeedDown              = Exposure.SpeedDown;
	}
	else
	{
		PP.bOverride_AutoExposureMethod = true;
		PP.AutoExposureMethod           = EAutoExposureMethod::AEM_Manual;
	}
}

void UCameraSensorComponent::ApplyDistortion()
{
	if (!Distortion.IsNonTrivial() || !DistortionMaterial) return;
}

void UCameraSensorComponent::StartTimer()
{
	if (!GetWorld()) return;

	const float Interval = 1.0f / FMath::Max(Spec.FrameRate, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, this, &UCameraSensorComponent::OnTimer, Interval, true);
}

void UCameraSensorComponent::StopTimer()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UCameraSensorComponent::OnTimer()
{
	if (!bEnabled || !SceneCapture) return;

	SceneCapture->CaptureScene();
	CaptureCount++;

	if (bExportEnabled && RenderTarget)
		ExportFrame();
}

void UCameraSensorComponent::ExportFrame()
{
}

void UCameraSensorComponent::TriggerCapture()
{
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
		CaptureCount++;
	}
}

void UCameraSensorComponent::SetFrameRate(float Hz)
{
	Spec.FrameRate = FMath::Clamp(Hz, 1.0f, 120.0f);
	StopTimer();
	if (bEnabled)
		StartTimer();
}

void UCameraSensorComponent::ApplySensorConfig()
{
	if (!SceneCapture) return;

	SceneCapture->FOVAngle = Spec.FOV;

	if (RenderTarget && (RenderTarget->SizeX != Spec.Width || RenderTarget->SizeY != Spec.Height))
	{
		RenderTarget->InitAutoFormat(Spec.Width, Spec.Height);
		RenderTarget->UpdateResourceImmediate(true);
	}

	SceneCapture->PostProcessSettings.WeightedBlendables.Array.Empty();
	ApplyPostProcess();
	ApplyDistortion();

	StopTimer();
	if (bEnabled)
		StartTimer();
}

void UCameraSensorComponent::SetPreset(ECameraPreset NewPreset)
{
	Preset = NewPreset;

	switch (NewPreset)
	{
	case ECameraPreset::TeslaHW3_Wide:
		Spec      = { 1280, 960, 36.0f, 120.0f, 1.5f };
		Distortion = { -0.3f,  0.10f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.4f;  PostProcess.ChromaticAberration = 0.5f;
		PostProcess.MotionBlurAmount  = 0.5f;  PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.1f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise    = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraPreset::TeslaHW3_Main:
		Spec       = { 1280, 960, 36.0f, 50.0f, 5.0f };
		Distortion = { -0.1f,  0.02f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.3f;  PostProcess.ChromaticAberration = 0.3f;
		PostProcess.MotionBlurAmount  = 0.5f;  PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.05f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise    = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraPreset::TeslaHW3_Narrow:
		Spec       = { 1280, 960, 36.0f, 35.0f, 8.0f };
		Distortion = { -0.05f, 0.01f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.2f;  PostProcess.ChromaticAberration = 0.2f;
		PostProcess.MotionBlurAmount  = 0.5f;  PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.05f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise    = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraPreset::TeslaHW4:
		Spec       = { 2560, 1920, 24.0f, 130.0f, 1.3f };
		Distortion = { -0.35f, 0.12f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.35f; PostProcess.ChromaticAberration = 0.4f;
		PostProcess.MotionBlurAmount  = 0.4f;  PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.08f;
		Exposure = { true, 1.0f, 15.0f, 3.5f, 1.2f };
		Noise    = { true, 0.0f, 0.015f, 0.008f };
		break;

	case ECameraPreset::DroneFPV:
		Spec       = { 1920, 1080, 60.0f, 150.0f, 2.5f };
		Distortion = { -0.5f,  0.20f, -0.05f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.5f;  PostProcess.ChromaticAberration = 0.7f;
		PostProcess.MotionBlurAmount  = 0.3f;  PostProcess.BloomIntensity = 0.1f;
		PostProcess.LensFlareIntensity = 0.15f;
		Exposure = { true, 3.0f, 16.0f, 4.0f, 2.0f };
		Noise    = { true, 0.0f, 0.025f, 0.015f };
		break;

	case ECameraPreset::Waymo:
		Spec       = { 1920, 1280, 30.0f, 50.0f, 6.0f };
		Distortion = { -0.08f, 0.015f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.2f;  PostProcess.ChromaticAberration = 0.15f;
		PostProcess.MotionBlurAmount  = 0.3f;  PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.03f;
		Exposure = { true, 1.0f, 16.0f, 4.0f, 1.5f };
		Noise    = { true, 0.0f, 0.01f, 0.005f };
		break;

	case ECameraPreset::Custom:
	default:
		break;
	}
}
