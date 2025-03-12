#include "PixelMario.h"
#include "Modules/ModuleManager.h"

void FShaderModule::StartupModule()
{
    // #if (ENGINE_MINOR_VERSION >= 21)
    FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
    // #endif
}

void FShaderModule::ShutdownModule() {}

IMPLEMENT_PRIMARY_GAME_MODULE(FShaderModule, PixelMario, "PixelMario");
