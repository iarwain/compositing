//! Includes
#include "compositing.h"


//! Variables
orxOBJECT *pstScene = orxNULL;


//! Code
void InitTextures()
{
  orxU32      u32X, u32Y, u32Size;
  orxFLOAT    fScreenWidth, fScreenHeight, fSpotRadius;
  orxTEXTURE *pstTexture;
  orxBITMAP  *pstBitmap;
  orxU8      *au8Data;

  // Gets screen size
  orxDisplay_GetScreenSize(&fScreenWidth, &fScreenHeight);

  // Creates background texture
  pstBitmap  = orxDisplay_CreateBitmap(256, 256);
  pstTexture = orxTexture_Create();
  orxTexture_LinkBitmap(pstTexture, pstBitmap, "BackgroundTexture");

  // Allocates pixel buffer
  au8Data = (orxU8 *)orxMemory_Allocate(256 * 256 * sizeof(orxRGBA), orxMEMORY_TYPE_VIDEO);

  // For all pixels
  for(u32Y = 0; u32Y < 256; u32Y++)
  {
    for(u32X = 0; u32X < 256; u32X++)
    {
      orxU32 u32Index, u32Value;

      // Gets pixel value
      u32Value = u32X ^ u32Y;

      // Gets pixel index
      u32Index = (u32Y * 256 + u32X) * sizeof(orxRGBA);

      // Stores pixel channels
      au8Data[u32Index]     =
      au8Data[u32Index + 1] =
      au8Data[u32Index + 2] = u32Value;
      au8Data[u32Index + 3] = 0xFF;
    }
  }

  // Updates bitmap content
  orxDisplay_SetBitmapData(pstBitmap, au8Data, 256 * 256 * sizeof(orxRGBA));

  // Frees pixel buffer
  orxMemory_Free(au8Data);

  // Creates lightmap texture
  pstBitmap   = orxDisplay_CreateBitmap(orxF2U(fScreenWidth), orxF2U(fScreenHeight));
  pstTexture  = orxTexture_Create();
  orxTexture_LinkBitmap(pstTexture, pstBitmap, "LightMapTexture");

  // Pushes main config section
  orxConfig_PushSection("Main");

  // Gets spot size & inner radius
  u32Size     = orxConfig_GetU32("SpotLightSize");
  fSpotRadius = orxConfig_GetFloat("SpotLightRadius");

  // Pops config section
  orxConfig_PopSection();

  // Creates spotlight texture
  pstBitmap   = orxDisplay_CreateBitmap(u32Size, u32Size);
  pstTexture  = orxTexture_Create();
  orxTexture_LinkBitmap(pstTexture, pstBitmap, "SpotLightTexture");

  // Allocates pixel buffer
  au8Data = (orxU8 *)orxMemory_Allocate(u32Size * u32Size * sizeof(orxRGBA), orxMEMORY_TYPE_VIDEO);

  // For all pixels
  for(u32Y = 0; u32Y < u32Size; u32Y++)
  {
    for(u32X = 0; u32X < u32Size; u32X++)
    {
      orxU32    u32Index, u32Alpha;
      orxFLOAT  fDistance;

      // Gets pixel distance from center
      fDistance = orxMath_Sqrt(orxMath_Pow(orxU2F(u32X) - (orx2F(0.5f) * orxU2F(u32Size)), orx2F(2.0)) + orxMath_Pow(orxU2F(u32Y) - (orx2F(0.5f) * orxU2F(u32Size)), orx2F(2.0)));

      // Gets pixel alpha
      u32Alpha = orxF2U(orx2F(255.0f) * orxMath_SmoothStep(orx2F(0.5f) * orxU2F(u32Size), fSpotRadius, fDistance));

      // Gets pixel index
      u32Index = (u32Y * u32Size + u32X) * sizeof(orxRGBA);

      // Stores pixel channels
      au8Data[u32Index]     =
      au8Data[u32Index + 1] =
      au8Data[u32Index + 2] = 0xFF;
      au8Data[u32Index + 3] = u32Alpha;
    }
  }

  // Updates bitmap content
  orxDisplay_SetBitmapData(pstBitmap, au8Data, u32Size * u32Size * sizeof(orxRGBA));

  // Frees pixel buffer
  orxMemory_Free(au8Data);
}

orxSTATUS orxFASTCALL Init()
{
  orxS32    i;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Creates and inits textures
  InitTextures();

  // Pushes main config section
  orxConfig_PushSection("Main");

  // Creates all viewports
  for(i = 0; i < orxConfig_GetListCounter("ViewportList"); i++)
  {
    orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
  }

  // Pops config section
  orxConfig_PopSection();

  // Creates scene
  pstScene = orxObject_CreateFromConfig("Scene");

  // Done!
  return eResult;
}

orxSTATUS orxFASTCALL Run()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Screenshot?
  if(orxInput_IsActive("Screenshot") && orxInput_HasNewStatus("Screenshot"))
  {
    // Captures it
    orxScreenshot_Capture();
  }
  // Quitting?
  if(orxInput_IsActive("Quit"))
  {
    // Updates result
    eResult = orxSTATUS_FAILURE;
  }

  // Done!
  return eResult;
}

void orxFASTCALL Exit()
{
  // We could delete everything we created here but orx will do it for us anyway
}

int main(int argc, char **argv)
{
  // Executes orx
  orx_Execute(argc, argv, Init, Run, Exit);

  // Done!
  return EXIT_SUCCESS;
}

#ifdef __orxWINDOWS__

#include "windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // Executes orx
  orx_WinExecute(Init, Run, Exit);

  // Done!
  return EXIT_SUCCESS;
}

#endif // __orxWINDOWS__
