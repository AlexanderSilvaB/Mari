#ifndef COMBINED_FOVEA_HPP
#define COMBINED_FOVEA_HPP

#include "perception/vision/Fovea.hpp"

struct CombinedFovea {

public:
    CombinedFovea(Fovea* top, Fovea* bot) : top_(top), bot_(bot) {}
    ~CombinedFovea() { free(top_); free(bot_); }
    void generate(const CombinedFrame& this_frame, 
    		ColourClassifier& colour_classifier_top, ColourClassifier& colour_classifier_bot) {
        top_->generate(this_frame, colour_classifier_top);
        bot_->generate(this_frame, colour_classifier_bot);
    }
    Fovea* top_;
    Fovea* bot_;
};

#endif
