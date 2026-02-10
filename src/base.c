#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "playermodelmanager_api.h"
#include "gDryBonesSkel.h"
#include "gFatDryBonesSkel.h"
#include "strongbad64.h"

// Mask positioning constants
#define DB_MASK_SCALE_MODIFIER 1.2f
#define DB_MASK_DOWNWARDS_OFFSET -14.f
#define DB_MASK_FORWARDS_OFSSEET 650.f
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


PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerModels(void) {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheSexGod", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(h, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(h, "Dry Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(h, &gDryBonesSkel);
    PlayerModelManager_setMatrix(h, PMM_MTX_SHIELD_HERO_BACK, &gEmptyMtx);

    PlayerModelManagerHandle f = PLAYERMODELMANAGER_REGISTER_MODEL("DryBonesTheFatGod", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(f, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(f, "Fat Dry Bones"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(f, &gFatDryBonesSkel);
    PlayerModelManager_setMatrix(f, PMM_MTX_SHIELD_HERO_BACK, &gEmptyMtx);

    PlayerModelManagerHandle sb = PLAYERMODELMANAGER_REGISTER_MODEL("StrongBadTheWrestleMan", PMM_MODEL_TYPE_CHILD);
    PlayerModelManager_setAuthor(sb, "PeteGrammarman"); // optional, will show up in a later version of PMM
    PlayerModelManager_setDisplayName(sb, "Strong Bad"); // name that shows up in the menu
    PlayerModelManager_setSkeleton(sb, &gStrongBadSkel_001);
}


RECOMP_HOOK("Player_PostLimbDrawGameplay") void on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    Player* player = (Player*)actor;

    // Handle mask positioning
    if (limbIndex == PLAYER_LIMB_HEAD) {
        if (((*dList1 != NULL) && ((u32)player->currentMask != PLAYER_MASK_NONE)) &&
            (((player->transformation == PLAYER_FORM_HUMAN) &&
              ((player->skelAnime.animation != &gPlayerAnim_cl_setmask) || (player->skelAnime.curFrame >= 12.0f))) ||
             ((((player->transformation != PLAYER_FORM_HUMAN) && (player->currentMask >= PLAYER_MASK_FIERCE_DEITY)) &&
               ((player->transformation + PLAYER_MASK_FIERCE_DEITY) != player->currentMask)) &&
              (player->skelAnime.curFrame >= 10.0f)))) {   
                s32 maskMinusOne = player->currentMask - 1;
                OPEN_DISPS(play->state.gfxCtx);
                Matrix_Push();
                gPushedMatrixHead = 1;
                Matrix_Scale(DB_MASK_SCALE_MODIFIER, DB_MASK_SCALE_MODIFIER, DB_MASK_SCALE_MODIFIER, MTXMODE_APPLY);
                Matrix_Translate(DB_MASK_FORWARDS_OFSSEET, DB_MASK_DOWNWARDS_OFFSET, DB_MASK_LATERAL_OFFSET, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                CLOSE_DISPS(play->state.gfxCtx);
        } else {
            gPushedMatrixHead = 0;
        }    
    } else {
        gPushedMatrixSheath = 0;
        gPushedMatrixHead = 0;
    }
}

RECOMP_HOOK_RETURN("Player_PostLimbDrawGameplay") void return_Player_PostLimbDrawGameplay(void) {
    if (gPushedMatrixHead) {
        Matrix_Pop();
        gPushedMatrixHead = 0;
    }
}
