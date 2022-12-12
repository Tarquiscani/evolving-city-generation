#ifndef GM_HIP_ROOF_MATRIX_VISUAL_DEBUG_HH
#define GM_HIP_ROOF_MATRIX_VISUAL_DEBUG_HH


#include "settings/debug/visual_debug_hip_roof_matrix_settings.hh"
#if HIPROOFMATRIX_VISUALDEBUG


#include "debug/visual/base/visual_debug.hh"
#include "graphics/algorithms/hip_roof/hip_roof_matrix.hh"


namespace tgm
{



class HipRoofMatrixVisualDebug final : public VisualDebug
{
        public:
        void start(int const map_length, int const map_width);
        void set_matrixDimensions(int const matrix_length, int const matrix_width);
        
        void print_matrix(HipRoofAlgorithm::Matrix const& matrix);

    private:
        virtual bool custom_has_been_activated() const noexcept override { return visualDebug_runtime_openWindowForHipRoofMatrix; }
        virtual void custom_deactivate() const noexcept override { visualDebug_runtime_openWindowForHipRoofMatrix = false; }

        virtual void init_chapterAlias() override { m_chapter_alias = "expansion"; }
        virtual auto window_title() const noexcept -> std::string override { return "Hip Roof OldMatrix Visual Debug"; }
        
        //unused functions
        virtual void custom_stop() override {}
        virtual void init_newChange(int const) override {}
        virtual void custom_goToPreviousStep(int const) override {}
        virtual void custom_goToNextStep(int const) override {}
        virtual void custom_pushVertices(DebugVertices &) const override {}
};



} // namespace tgm


#endif //HIPROOFMATRIX_VISUALDEBUG


#endif //GM_HIP_ROOF_MATRIX_VISUAL_DEBUG_HH