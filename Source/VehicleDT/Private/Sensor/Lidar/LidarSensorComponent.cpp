#include "Sensor/Lidar/LidarSensorComponent.h"
#include "Sensor/Lidar/LidarScanner.h"
#include "Sensor/Lidar/LidarBevRenderer.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogLidarSensor, Log, All);

ULidarSensorComponent::ULidarSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULidarSensorComponent::BeginPlay()
{
	Super::BeginPlay();
	SetPreset(Preset);
	InitSensor();
}

void ULidarSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopTimer();
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void ULidarSensorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ULidarSensorComponent, Preset))
		SetPreset(Preset);
}
#endif

void ULidarSensorComponent::InitSensor()
{
	BevConfig.ViewRange = Config.MaxRange;

	Scanner = NewObject<ULidarScanner>(this, TEXT("Scanner"));
	Scanner->Initialize(Config, Preset);

	BevRenderer = NewObject<ULidarBevRenderer>(this, TEXT("BevRenderer"));
	BevRenderer->Initialize(BevConfig);

	bEnabled = false;

	UE_LOG(LogLidarSensor, Log,
		TEXT("[LidarSensor] Ready — %d ch x %d pts @ %.0f Hz, range %.0f m"),
		Config.NumChannels, Config.PointsPerChannel,
		Config.RotationRate, Config.MaxRange / 100.f);
}

void ULidarSensorComponent::StartTimer()
{
	if (!GetWorld()) return;

	const float Interval = 1.0f / FMath::Max(Config.RotationRate, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, this, &ULidarSensorComponent::OnTimer, Interval, true);
}

void ULidarSensorComponent::StopTimer()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void ULidarSensorComponent::OnTimer()
{
	if (!bEnabled || !Scanner) return;

	Scanner->PerformScan(GetWorld(), GetComponentTransform(), GetOwner());
	++ScanCount;

	const FLidarScanResult& Scan = Scanner->GetLastScan();

	if (BevRenderer)
		BevRenderer->Render(Scan, GetComponentTransform());

	if (bExportEnabled && Scan.PointCount > 0)
		ExportScan();
}

void ULidarSensorComponent::ExportScan()
{
}

void ULidarSensorComponent::StartScan()
{
	bEnabled = true;
	StartTimer();
}

void ULidarSensorComponent::StopScan()
{
	bEnabled = false;
	StopTimer();
}

FLidarScanResult ULidarSensorComponent::GetLastScan() const
{
	return Scanner ? Scanner->GetLastScan() : FLidarScanResult{};
}

UTexture2D* ULidarSensorComponent::GetBevRenderTarget() const
{
	return BevRenderer ? BevRenderer->GetRenderTarget() : nullptr;
}

void ULidarSensorComponent::SetRotationRate(float Hz)
{
	Config.RotationRate = FMath::Clamp(Hz, 1.0f, 30.0f);
	StopTimer();
	if (bEnabled)
		StartTimer();
}

void ULidarSensorComponent::ApplySensorConfig()
{
	BevConfig.ViewRange = Config.MaxRange;

	if (Scanner)
		Scanner->UpdateConfig(Config, Preset);
	if (BevRenderer)
		BevRenderer->UpdateConfig(BevConfig);

	StopTimer();
	if (bEnabled)
		StartTimer();
}

void ULidarSensorComponent::SetPreset(ELidarPreset NewPreset)
{
	Preset = NewPreset;

	switch (NewPreset)
	{
	case ELidarPreset::VelodyneVLP16:
		Config = { 16, 1800, 10.f, 10000.f, 50.f, 15.f, -15.f, 360.f, 2.f };
		break;
	case ELidarPreset::VelodyneVLP32:
		Config = { 32,  900, 10.f, 20000.f, 50.f, 15.f, -25.f, 360.f, 2.f };
		break;
	case ELidarPreset::OusterOS1_64:
		Config = { 64,  512, 10.f, 12000.f, 50.f, 22.5f, -22.5f, 360.f, 1.5f };
		break;
	case ELidarPreset::Livox_Mid360:
		Config = {  8,  450, 10.f,  7000.f, 100.f, 52.f, -7.f, 360.f, 3.f };
		break;
	case ELidarPreset::Custom:
	default:
		break;
	}

	if (Scanner)
		Scanner->UpdateConfig(Config, Preset);
}
