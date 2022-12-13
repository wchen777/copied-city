#include "generator.h"


#define FIRST_PHASE_SPLITS 14 // for equal splits
#define SECOND_PHASE_MAX_SPLITS 2

#define SPLIT_FRAC_MIN 0.25f
#define SPLIT_FRAC_MAX 0.75f



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
 * Go through each block and scale the height by a random factor.
*/
void CityFacade::VaryHeight(std::vector<Block>& blocks) {
    for (auto& b : blocks) {
        b.height = randRange(MIN_HEIGHT_SCALE, MAX_HEIGHT_SCALE) * b.height;
    }
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

        // split it into two blocks.
        float splitFraction = randRange(SPLIT_FRAC_MIN, SPLIT_FRAC_MAX);
        float splitZ = toSplit.zStart + (splitFraction * (toSplit.zEnd - toSplit.zStart));

        // first block is from z start to split z, ceterus paribus
        Block first = Block{
                .height=toSplit.height,
                .zStart=toSplit.zStart,
                .zEnd=splitZ,
                .xStart=toSplit.xStart,
                .xEnd=toSplit.xEnd,
                .xTranslate = toSplit.xTranslate,
                .xScale = toSplit.xScale
        };

        // second block is from split z to z end, ceterus paribus
        Block second = Block{
                .height=toSplit.height,
                .zStart=splitZ,
                .zEnd=toSplit.zEnd,
                .xStart=toSplit.xStart,
                .xEnd=toSplit.xEnd,
                .xTranslate = toSplit.xTranslate,
                .xScale = toSplit.xScale
        };


        // remove the original block from the blocks list
        CityFacade::shapeGrammarData.erase(shapeGrammarData.begin());

        // insert both into descending sorted list by length.

        auto insertIndFirst = 0;
        while (insertIndFirst < CityFacade::shapeGrammarData.size()) {
            auto el = CityFacade::shapeGrammarData[insertIndFirst];
            // if the element's length is shorter than ours, break.
            if ((el.zEnd - el.zStart) < (first.zEnd - first.zStart)) {
                break;
            }
            insertIndFirst++;
        }

        // insert at this index
        CityFacade::shapeGrammarData.insert(CityFacade::shapeGrammarData.begin() + insertIndFirst, first);

        auto insertIndSecond = 0;
        while (insertIndSecond < CityFacade::shapeGrammarData.size()) {
            auto el = CityFacade::shapeGrammarData[insertIndSecond];
            // if the element's length is shorter than ours, break.
            if ((el.zEnd - el.zStart) < (second.zEnd - second.zStart)) {
                break;
            }
            insertIndSecond++;
        }

        // insert at this index
        CityFacade::shapeGrammarData.insert(CityFacade::shapeGrammarData.begin() + insertIndSecond, second);
    }

    // vary height for all blocks
    CityFacade::VaryHeight(CityFacade::shapeGrammarData);
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

    // accumulator for all new blocks
    std::vector<Block> allNewBlocks;

    for (Block& b : CityFacade::shapeGrammarData) {

        std::vector<Block> newBlocks;
        newBlocks.emplace_back(b);

        // random number of possible splits
        int numSplits = arc4random() % (SECOND_PHASE_MAX_SPLITS+1);

        for (int i = 0; i < numSplits; ++i) {

            // grab first thing in the blocks list
            auto toSplit = newBlocks[0];

            // split it into two blocks.
            float splitFraction = randRange(SPLIT_FRAC_MIN, SPLIT_FRAC_MAX);
            float splitX = toSplit.xStart + (splitFraction * (toSplit.xEnd - toSplit.xStart));

            // first block is from x start to split x, ceterus paribus
            Block first = Block{
                    .height=toSplit.height,
                    .zStart=toSplit.zStart,
                    .zEnd=toSplit.zEnd,
                    .xStart=toSplit.xStart,
                    .xEnd=splitX,
                    .xTranslate = toSplit.xTranslate,
                    .xScale = toSplit.xScale
            };

            // second block is from split z to z end, ceterus paribus
            Block second = Block{
                    .height=toSplit.height,
                    .zStart=toSplit.zStart,
                    .zEnd=toSplit.zEnd,
                    .xStart=splitX,
                    .xEnd=toSplit.xEnd,
                    .xTranslate = toSplit.xTranslate,
                    .xScale = toSplit.xScale
            };

            // remove the original block from the new blocks list
            newBlocks.erase(newBlocks.begin());

            // add both new blocks to the new blocks list.
            newBlocks.emplace_back(first);
            newBlocks.emplace_back(second);
        }

        // vary the height for all the new blocks
        CityFacade::VaryHeight(newBlocks);

        // add the new blocks to the accumulator
        for (auto& nb : newBlocks) {
            allNewBlocks.emplace_back(nb);
        }

    }

    // set our block list to be of the new blocks
    CityFacade::shapeGrammarData = allNewBlocks;
}


/*
 * NOT USED RIGHT NOW
 * Perturbation phase for x-axis.
 * Go through each block in the block list and apply a random x scale and random x translate
 * This should happen in between the two subdivision phases.
*/
void CityFacade::PerturbationPhase() {
    for (auto& b : CityFacade::shapeGrammarData) {
        b.xTranslate = randRange(MIN_X_TRANS, MAX_X_TRANS);
        b.xScale = randRange(MIN_X_SCALE, MAX_X_SCALE);
    }
}


// pin random blocks onto walls of the facade. this should be called before we split along the x-axis.
void CityFacade::DecorationPhase(FacadeType facadeType) {
    // if the facade type is left, pin the blocks towards the greater x value
   // if the facade type is right, pin the blocks towards the lesser x value

    for (int i = 0; i < shapeGrammarData.size(); ++i) {

        Block b = shapeGrammarData[i];

        float xSideToPin = facadeType == LEFT ? b.xEnd : b.xStart;

        // roll for number of blocks to generate
        int roll = arc4random() % 10;
        int numBlocks = 0;
        if (roll >= 7) { // 0.1 for 3
            numBlocks = 3;
        } else if (roll >= 5) { // 0.3 for 2
            numBlocks = 2;
        } else if (roll >= 2) { // 0.3 for 1
            numBlocks = 1;
        }

        // generate cube growths
        for (int j = 0; j < numBlocks; ++j) {
            // calculate y and z coordinate on the parent to generate the growth block

            // roll for y coord
            float yCoord = randRange(0.3f * b.height, 0.75 * b.height);
            // roll for z coord
            float zCoord = randRange(b.zStart, b.zEnd);

            // generate directly to the meshData
            CityFacade::GenerateSideRecursiveCubeGrowth(xSideToPin, yCoord, zCoord, facadeType, 0);
        }
    }
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
            case BACK: // not used for right now
                blk.BlockToMeshObject(&meshObj, 0.f, BACK_FACADE_OFFSET);
                break;
        }

        // add to data
        CityFacade::data.emplace_back(meshObj);
    }
}
