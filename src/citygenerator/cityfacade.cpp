#include "generator.h"


#define FIRST_PHASE_SPLITS 10
#define SECOND_PHASE_MAX_SPLITS 3

#define LEFT_FACADE_OFFSET -15.f
#define RIGHT_FACADE_OFFSET 15.f
#define BACK_FACADE_OFFSET 100.f


/*
 * Initialize shape grammar with progenitor block (in pseudo-object space)
*/
void CityFacade::InitShapeGrammar() {
    Block progenitor = Block{
                .height=PROGENITOR_HEIGHT,
                .zStart=0.f,
                .zEnd=PROGENITOR_LENGTH,
                .xStart=0.f,
                .xEnd=PROGENITOR_WIDTH,
                .xTranslate = 0.f,
                .xScale = 1.f
            };
    CityFacade::shapeGrammarData.emplace_back(progenitor);
}


/*
 * First subdivision phase.
 * Split progenitor block along z-axis until sufficient splits have been made.
 * Go through each block and scale the height
*/
void CityFacade::SubdividePhaseZAxis() {
    for (int i = 0; i < FIRST_PHASE_SPLITS; ++i) {
        // grab first thing in the blocks list
        auto toSplit = CityFacade::shapeGrammarData[0];
        // remove it from the
        CityFacade::shapeGrammarData.erase(shapeGrammarData.begin());
    }
}


/*
 * Second subdivision phase.
 * For every block,
 *      Create new vector to hold blocks
 *      Split it along x-axis 0 to SECOND_PHASE_MAX_SPLITS times
 *      Go through each newly block and scale the height
 *      Accumulate newly split blocks.
*/
void CityFacade::SubdividePhaseXAxis() {

}


/*
 * Perturbation phase for x-axis.
 * For every block,
 *      Create new vector to hold blocks
 *      Split it along x-axis 0 to SECOND_PHASE_MAX_SPLITS times
 *      Go through each newly block and scale the height
 *      Accumulate newly split blocks.
 * This should happen in between the two subdivision phases.
*/
void CityFacade::PerturbationPhase() {

}


// EXTRA, pin random blocks onto walls of the facade
void CityFacade::DecorationPhase() {

}


// convert shape grammar into mesh objects
void CityFacade::ConvertShapeGrammar(FacadeType facadeType) {
    for (auto& blk : CityFacade::shapeGrammarData) {

        // define and fill new mesh obj
        CityMeshObject meshObj;
        switch(facadeType) {
            case LEFT:
                blk.BlockToMeshObject(&meshObj, LEFT_FACADE_OFFSET, 0.f);
                break;
            case RIGHT:
                blk.BlockToMeshObject(&meshObj, RIGHT_FACADE_OFFSET, 0.f);
                break;
            case BACK:
                blk.BlockToMeshObject(&meshObj, 0.f, BACK_FACADE_OFFSET);
                break;
        }

        // add to data
        CityFacade::data.emplace_back(meshObj);
    }
}
