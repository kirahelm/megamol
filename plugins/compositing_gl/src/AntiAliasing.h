/*
 * AntiAliasing.h
 *
 * Copyright (C) 2021 by Universitaet Stuttgart (VISUS).
 * All rights reserved.
 */

#ifndef ANTI_ALIASING_H_INCLUDED
#define ANTI_ALIASING_H_INCLUDED

#include "mmcore/Module.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "vislib/graphics/gl/GLSLComputeShader.h"

#include "glowl/BufferObject.hpp"
#include "glowl/Texture2D.hpp"

namespace megamol {
namespace compositing {

class AntiAliasing : public core::Module {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName() { return "AntiAliasing"; }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description() { return "Compositing module that compute a screen space effect"; }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable() { return true; }

    AntiAliasing();
    ~AntiAliasing();

protected:
    /**
     * Implementation of 'Create'.
     *
     * @return 'true' on success, 'false' otherwise.
     */
    bool create();

    /**
     * Implementation of 'Release'.
     */
    void release();

    /**
     * TODO
     */
    bool getDataCallback(core::Call& caller);

    /**
     * TODO
     */
    bool getMetaDataCallback(core::Call& caller);

private:
    typedef vislib::graphics::gl::GLSLComputeShader GLSLComputeShader;

    uint32_t m_version;

    /** Shader program for fxaa */
    std::unique_ptr<GLSLComputeShader> m_fxaa_prgm;

    /** Shader program for smaa */
    std::unique_ptr<GLSLComputeShader> m_smaa_prgm;

    /** Texture that the combination result will be written to */
    std::shared_ptr<glowl::Texture2D> m_output_texture;

    /** Texture that can store intermediate results for multi-pass effect, e.g. ssao with blur */
    std::shared_ptr<glowl::Texture2D> m_intermediate_texture;

    /** Hash value to keep track of update to the output texture */
    size_t m_output_texture_hash;

    /** Parameter for selecting the screen space effect that is computed, e.g. ssao, fxaa,... */
    megamol::core::param::ParamSlot m_mode;

    /** Slot for requesting the output textures from this module, i.e. lhs connection */
    megamol::core::CalleeSlot m_output_tex_slot;

    /** Slot for optionally querying an input texture, i.e. a rhs connection */
    megamol::core::CallerSlot m_input_tex_slot;
};

} // namespace compositing
} // namespace megamol

#endif // !ANTI_ALIASING_H_INCLUDED
