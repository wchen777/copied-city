#include "generator.h"


#define FIRST_PHASE_SPLITS 16 // for equal splits
#define SECOND_PHASE_MAX_SPLITS 2

#define LEFT_FACADE_OFFSET -50.f
#define RIGHT_FACADE_OFFSET 23.f
#define BACK_FACADE_OFFSET 100.f

#define SPLIT_FRAC_MIN 0.25f
#define SPLIT_FRAC_MAX 0.75f


// get a random float between min and max
inline float randRange(float min, float max) {
    // random number between 0.0 and 1.0
    float fraction = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    // get difference between min and max
    float diff = max - min;

    // scale the difference by fraction, add it to min.
    return min + (fraction * diff);
}


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


// EXTRA, pin random blocks onto walls of the facade
void CityFacade::DecorationPhase() {

}


// convert shape grammar into mesh objects
void CityFacade::ConvertShapeGrammar(FacadeType facadeType) {

    std::cout << CityFacade::shapeGrammarData.size() << "size of shape grammar convert" << std::endl;

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
