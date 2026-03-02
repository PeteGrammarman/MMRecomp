#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "segment_symbols.h"
#include "playermodelmanager_api.h"
#include "gDryBonesSkel.h"
#include "gFatDryBonesSkel.h"
#include "strongbad64.h"
#include "fgGoronRoll.h"
#include "gFishBonesSkel.h"
#include "gDryNutSkel.h"
#include "paulinia.h"
#include "z64actor.h"
#include "strongbad64_fat.h"

// Mask positioning constants
#define DB_MASK_SCALE_MODIFIER 1.2f
#define DB_MASK_DOWNWARDS_OFFSET -14.f
#define DB_MASK_FORWARDS_OFFSET 650.f
#define DB_MASK_LATERAL_OFFSET 0.f


//Empty Matrix, for removing shield
Mtx gEmptyMtx;


// Matrix stack tracking flags
u8 gPushedMatrixSheath;
u8 gPushedMatrixHead;

// Dependencies for height adjustment
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];
extern LinkAnimationHeader gPlayerAnim_cl_setmask;

// Mask DL
extern LinkAnimationHeader gPlayerAnim_cl_setmask;
extern Gfx* D_801C0B20[];  // Mask display lists (if needed)

//Height stuff
Player *gOriginalPlayer;
u8 gOriginalPlayerTransformation;
static Mtx DBMaskMtx; //Strong Bad mask matrix for positioning
static Mtx DBSheathdMtx; //Dry Bones sheath matrix for positioning

static Mtx sSBMaskMtx; //Strong Bad mask matrix for positioning
static Mtx sSBShieldMtx; //Strong Bad shield matrix for positioning
static Mtx FGDrumMtx; //Drum matrix for resizing

extern Gfx GoronPlane[];
extern void* sDryBonesEyes[];
extern Gfx gDryBonesElegyShellHuman[];
extern Gfx gDryBonesElegyShellGoron[];
extern Gfx gDryBonesElegyShellDeku[];

//gElegyShellHumanDL



PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerModels(void) {
    //DRY BONES (human) h
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheSexGod", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(h, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(h, "Dry Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(h, &gDryBonesSkel);
    PlayerModelManager_setMatrix(h, PMM_MTX_SHIELD_HERO_BACK, &gEmptyMtx);
    PlayerModelManager_setMatrix(h, PMM_MTX_SHIELD_MIRROR_BACK, &gEmptyMtx);
    PlayerModelManager_setEyesTextures(h, sDryBonesEyes);
    PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, gDryBonesElegyShellHuman);

    
    //Mask poistioning 
    guPosition(&DBMaskMtx, 0.0f, 0.0f, 0.0f, DB_MASK_SCALE_MODIFIER, DB_MASK_FORWARDS_OFFSET, DB_MASK_DOWNWARDS_OFFSET, 0.0f);  //?,?,?,Scale, Forwards, Downwards, Lateral
    PlayerModelManager_setMatrix(h, PMM_MTX_MASKS, &DBMaskMtx);
    //Sheath positioning
    //guPosition(&DBSheathMtx, 0.0f, 0.0f, 0.0f, 1.0f, 4.0f, - 12.0f, 0.0f); //?,?,?,Scale, Forwards, Downwards, Lateral
    //PlayerModelManager_setMatrix(h, PMM_DL_SWORD_FIERCE_DEITY_BLADE, &DBSheathMtx;

    //DRY BONES FAT (human) f
    PlayerModelManagerHandle f = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheFatGod", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(f, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(f, "Fat Dry Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(f, &gFatDryBonesSkel);
    PlayerModelManager_setMatrix(f, PMM_MTX_SHIELD_HERO_BACK, &gEmptyMtx);
    PlayerModelManager_setMatrix(f, PMM_MTX_SHIELD_MIRROR_BACK, &gEmptyMtx);
    
    //ELEGY DRY BONES STATUES
    //PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, gDryBonesElegyShellHuman); //Replace DL for Goron curled
    //PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, gDryBonesElegyShellHuman); //Replace DL for Goron curled


    //DRY BONES GORON
    PlayerModelManagerHandle fg = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheFatGoron", PMM_MODEL_TYPE_GORON);
    PlayerModelManager_setAuthor(fg, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(fg, "Fat Goron Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(fg, &gFatDryBonesSkel);
    PlayerModelManager_setDisplayList(fg, PMM_DL_CURLED, GoronPlane); //Replace DL for Goron curled
    PlayerModelManager_setDisplayList(fg, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_GORON, gDryBonesElegyShellGoron);

    //Resize Drums
    /*
    guPosition(&FGDrumMtx, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f); //?,?,?,Scale, Forwards, Downwards, Lateral
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_STRAP, &FGDrumMtx);
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_LEFT, &FGDrumMtx);
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_RIGHT, &FGDrumMtx);
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_DOWN, &FGDrumMtx);
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_UP, &FGDrumMtx);
    PlayerModelManager_setMatrix(fg, PMM_DL_DRUM_A, &FGDrumMtx);    
    */


    //FISH BONES ZORA
    PlayerModelManagerHandle fz = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheFishBones", PMM_MODEL_TYPE_ZORA);
    PlayerModelManager_setAuthor(fz, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(fz, "Fish Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(fz, &gFishBonesSkel);

    //DRY NUT DEKU
    PlayerModelManagerHandle dn = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheDryNut", PMM_MODEL_TYPE_DEKU);
    PlayerModelManager_setAuthor(dn, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(dn, "Dry Nut"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(dn, &gDryNutSkel);
    PlayerModelManager_setDisplayList(dn, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_DEKU, gDryBonesElegyShellDeku);


    
    //STRONG BAD
    PlayerModelManagerHandle sb = PLAYERMODELMANAGER_REGISTER_MODEL("StrongBadTheWrestleMan", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(sb, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(sb, "Strong Bad"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(sb, &StrongBadSkel);
   // PlayerModelManager_setEyesTextures(sb, gLinkOniEyeTextures);
   // PlayerModelManager_setMouthTextures(sb, gLinkOniMouthTextures);

    //STRONG BAD FAT
    PlayerModelManagerHandle sbf = PLAYERMODELMANAGER_REGISTER_MODEL("StrongBadTheFatMan", PMM_MODEL_TYPE_GORON);
    PlayerModelManager_setAuthor(sbf, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(sbf, "Strong Bad Fat"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(sbf, &StrongBadFatSkel);
  //  PlayerModelManager_setEyesTextures(sbf, gLinkOniEyeTextures);
   // PlayerModelManager_setMouthTextures(sbf, gLinkOniMouthTextures);

    //MASKS
    guPosition(&sSBMaskMtx, 0.0f, 0.0f, 0.0f, 1.5f, 12.0f, -18.0f, 0.0f);  //?,?,?,Scale, Forwards, Downwards, Lateral
    PlayerModelManager_setMatrix(sb, PMM_MTX_MASKS, &sSBMaskMtx);

    //SHIELD
    guPosition(&sSBShieldMtx, 0.0f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 0.0f);  //?,?,?,Scale, Forwards, Downwards, Lateral
    PlayerModelManager_setMatrix(sb, PMM_MTX_SHIELD1_ITEM, &sSBShieldMtx);
    
}


//CREMIA PAULINE REPLACE
extern FlexSkeletonHeader gPauliniaSkel;
extern FlexSkeletonHeader gCremiaSkel;
extern FlexSkeletonHeader gPaulineWagon;
extern FlexSkeletonHeader gUmSkel;

//Replace Cremia model
RECOMP_HOOK("EnMaYto_Init") void on_EnMaYto_Init(Actor* thix, PlayState* play) {
    *(FlexSkeletonHeader*)Lib_SegmentedToVirtual(&gCremiaSkel) = gPauliniaSkel;
}

//Replace Cremia in cart model
RECOMP_HOOK("ObjUm_Init") void on_ObjUm_Init(Actor* thix, PlayState* play) {
    *(FlexSkeletonHeader*)Lib_SegmentedToVirtual(&gUmSkel) = gPaulineWagon;
}



