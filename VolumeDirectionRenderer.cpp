/*
 * VolumeDirectionRenderer.cpp
 *
 * Copyright (C) 2013 by Universitaet Stuttgart (VISUS).
 * All rights reserved.
 */

#include "stdafx.h"

#define _USE_MATH_DEFINES 1

#include "VolumeDirectionRenderer.h"
#include "view/CallGetTransferFunction.h"
#include "CoreInstance.h"
#include "utility/ShaderSourceFactory.h"
#include "utility/ColourParser.h"
#include "param/FloatParam.h"
#include "param/IntParam.h"
#include "param/BoolParam.h"
#include "vislib/assert.h"
#include "vislib/String.h"
#include "vislib/StringConverter.h"
#include <GL/glu.h>
#include <omp.h>
#include <cmath>


using namespace megamol;
using namespace megamol::core;
using namespace megamol::protein;


/*
 * protein::VolumeDirectionRenderer::VolumeDirectionRenderer (CTOR)
 */
VolumeDirectionRenderer::VolumeDirectionRenderer(void) : Renderer3DModuleDS (),
    vtiDataCallerSlot("getData", "Connects the arrow rendering with volume data storage"),
    xResParam("numArrXAxis", "Number of arrows along X axis"),
    yResParam("numArrYAxis", "Number of arrows along Y axis"),
    zResParam("numArrZAxis", "Number of arrows along Z axis"),
    lengthScaleParam("arrowLengthScale", "Length scale factor for the arrows"),
    lengthFilterParam("lengthFilter", "Lenght filter for arrow glyphs"),
    minDensityFilterParam("minDensityFilter", "Filter arrow glyphs by minimum density"),
    arrowCount(0), triggerArrowComputation(true),
    getTFSlot("getTransferFunction", "Connects to the transfer function module"),
    greyTF(0)
{
    this->vtiDataCallerSlot.SetCompatibleCall<VTIDataCallDescription>();
    this->MakeSlotAvailable( &this->vtiDataCallerSlot);
    
    this->getTFSlot.SetCompatibleCall<view::CallGetTransferFunctionDescription>();
    this->MakeSlotAvailable(&this->getTFSlot);

    this->xRes = this->yRes = this->zRes = 5;
    this->xResParam.SetParameter(new param::IntParam(this->xRes, 0));
    this->MakeSlotAvailable( &this->xResParam);
    this->yResParam.SetParameter(new param::IntParam(this->yRes, 0));
    this->MakeSlotAvailable( &this->yResParam);
    this->zResParam.SetParameter(new param::IntParam(this->zRes, 0));
    this->MakeSlotAvailable( &this->zResParam);
    
    this->lengthScaleParam.SetParameter(new param::FloatParam( 10.0f, 0.0f));
    this->MakeSlotAvailable( &this->lengthScaleParam);
    
    this->lengthFilterParam.SetParameter(new param::FloatParam( 0.0f, 0.0f));
    this->MakeSlotAvailable( &this->lengthFilterParam);

    this->minDensityFilterParam.SetParameter(new param::FloatParam( 0.0f, 0.0f));
    this->MakeSlotAvailable( &this->minDensityFilterParam);
}


/*
 * protein::VolumeDirectionRenderer::~VolumeDirectionRenderer (DTOR)
 */
VolumeDirectionRenderer::~VolumeDirectionRenderer(void)  {
    this->Release();
}


/*
 * protein::VolumeDirectionRenderer::release
 */
void VolumeDirectionRenderer::release(void) {
    this->arrowShader.Release();
    glDeleteTextures(1, &this->greyTF);
}


/*
 * protein::VolumeDirectionRenderer::create
 */
bool VolumeDirectionRenderer::create(void) {
    if( !glh_init_extensions( "GL_VERSION_2_0") )
        return false;

    if ( !vislib::graphics::gl::GLSLShader::InitialiseExtensions() )
        return false;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_VERTEX_PROGRAM_TWO_SIDE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

    using namespace vislib::sys;
    using namespace vislib::graphics::gl;

    ShaderSource vertSrc;
    ShaderSource fragSrc;
    
    // Load alternative arrow shader (uses geometry shader)
    if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("arrow::vertex", vertSrc)) {
        Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load vertex shader source for arrow shader");
        return false;
    }
    if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("arrow::fragment", fragSrc)) {
        Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load fragment shader source for arrow shader");
        return false;
    }
    this->arrowShader.Create( vertSrc.Code(), vertSrc.Count(), fragSrc.Code(), fragSrc.Count());
    
    // generate greyscale transfer function
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &this->greyTF);
    unsigned char tex[6] = {
        0, 0, 0,  255, 255, 255
    };
    glBindTexture(GL_TEXTURE_1D, this->greyTF);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);

    return true;
}


/*
 * protein::VolumeDirectionRenderer::GetCapabilities
 */
bool VolumeDirectionRenderer::GetCapabilities(Call& call) {
    view::AbstractCallRender3D *cr3d = dynamic_cast<view::AbstractCallRender3D *>(&call);
    if (cr3d == NULL) return false;

    cr3d->SetCapabilities(view::AbstractCallRender3D::CAP_RENDER
            | view::AbstractCallRender3D::CAP_LIGHTING
            | view::AbstractCallRender3D::CAP_ANIMATION );

    return true;
}


/*
 * protein::VolumeDirectionRenderer::GetExtents
 */
bool VolumeDirectionRenderer::GetExtents(Call& call) {
    view::AbstractCallRender3D *cr3d = dynamic_cast<view::AbstractCallRender3D *>(&call);
    if( cr3d == NULL ) return false;

    
    VTIDataCall *vti = this->vtiDataCallerSlot.CallAs<VTIDataCall>();
    if( vti == NULL ) return false;
    // set call time
    vti->SetCalltime(cr3d->Time());
    vti->SetFrameID(static_cast<int>(cr3d->Time()));
    // try to call for extent
    if (!(*vti)(VTIDataCall::CallForGetExtent)) return false;
    // try to call for data
    if (!(*vti)(VTIDataCall::CallForGetData)) return false;

    float scale;
    if( !vislib::math::IsEqual( vti->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge(), 0.0f) ) {
        scale = 2.0f / vti->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge();
    } else {
        scale = 1.0f;
    }

    cr3d->AccessBoundingBoxes() = vti->AccessBoundingBoxes();
    cr3d->AccessBoundingBoxes().MakeScaledWorld( scale);
    cr3d->SetTimeFramesCount( vti->FrameCount());

    return true;
}


/**********************************************************************
 * 'render'-functions
 **********************************************************************/

/*
 * protein::VolumeDirectionRenderer::Render
 */
bool VolumeDirectionRenderer::Render(Call& call) {
    // cast the call to Render3D
    view::AbstractCallRender3D *cr = dynamic_cast<view::AbstractCallRender3D *>(&call);
    if( cr == NULL ) return false;

    // get camera information
    this->cameraInfo = cr->GetCameraParameters();

    float callTime = cr->Time();

    // get pointer to MolecularDataCall
    VTIDataCall *vti = this->vtiDataCallerSlot.CallAs<VTIDataCall>();
    if( vti == NULL) return false;
    
    // set call time
    vti->SetCalltime(callTime);
    // set frame ID
    vti->SetFrameID(static_cast<int>( callTime));
    // try to call for data
    if (!(*vti)(VTIDataCall::CallForGetData)) return false;
    
    glPushMatrix();
    // compute scale factor and scale world
    float scale;
    if( !vislib::math::IsEqual( vti->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge(), 0.0f) ) {
        scale = 2.0f / vti->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge();
    } else {
        scale = 1.0f;
    }
    glScalef( scale, scale, scale);

    // ---------- update parameters ----------
    this->UpdateParameters(vti);
    
    // ---------- prepare data ---------- 
    if( this->triggerArrowComputation ) {
        vislib::math::Vector<float, 3> gridSize = vti->GetGridsize();
        //this->arrowCount = xRes * yRes * zRes;
        this->arrowCount = gridSize.X() * gridSize.Y() * gridSize.Z();
        this->vertexArray.SetCount( arrowCount * 4);
        this->colorArray.SetCount( arrowCount);
        this->dirArray.SetCount( arrowCount * 3);
        
        float deltaX = vti->AccessBoundingBoxes().ObjectSpaceBBox().Width() / static_cast<float>(xRes+1);
        float deltaY = vti->AccessBoundingBoxes().ObjectSpaceBBox().Height() / static_cast<float>(yRes+1);
        float deltaZ = vti->AccessBoundingBoxes().ObjectSpaceBBox().Depth() / static_cast<float>(zRes+1);
        
        float xPos = vti->AccessBoundingBoxes().ObjectSpaceBBox().Left();
        float yPos = vti->AccessBoundingBoxes().ObjectSpaceBBox().Bottom();
        float zPos = vti->AccessBoundingBoxes().ObjectSpaceBBox().Back();

        /*
        unsigned int numPieces = vti->GetNumberOfPieces();
        unsigned int numArrays = vti->GetArrayCntOfPiecePointData(0);
        unsigned int arr0Size = vti->GetPiecePointArraySize(0, 0); 
        unsigned int arr1Size = vti->GetPiecePointArraySize(1, 0);
        unsigned int arr0Type = vti->GetPiecePointArrayType(0, 0);
        unsigned int arr1Type = vti->GetPiecePointArrayType(1, 0);
        unsigned int arr0NC = vti->GetPointDataArrayNumberOfComponents(0, 0);
        unsigned int arr1NC = vti->GetPointDataArrayNumberOfComponents(1, 0);
        const float *arr0data = (const float*)(vti->GetPointDataByIdx(0, 0));
        */
        // TODO check for errors!!! (wrong data etc.)
        const float *densityData = (const float*)(vti->GetPointDataByIdx(0, 0));
        
        float minDensity = vti->GetPointDataArrayMin( 0, 0);
        float maxDensity = vti->GetPointDataArrayMax( 0, 0);

        const float *dirData = (const float*)(vti->GetPointDataByIdx(1, 0));
        
        this->minC = vti->GetPointDataArrayMin( 1, 0);
        this->maxC = vti->GetPointDataArrayMax( 1, 0);

        unsigned int idx = 0;
        //for( unsigned int xIdx = 0; xIdx < xRes; xIdx++) {
        for( unsigned int xIdx = 0; xIdx < gridSize.X(); xIdx++) {
            xPos += deltaX;
            //for( unsigned int yIdx = 0; yIdx < yRes; yIdx++) {
            for( unsigned int yIdx = 0; yIdx < gridSize.Y(); yIdx++) {
                yPos += deltaY;
                //for( unsigned int zIdx = 0; zIdx < zRes; zIdx++) {
                for( unsigned int zIdx = 0; zIdx < gridSize.Z(); zIdx++) {
                    zPos += deltaZ;
                    // set position
                    this->vertexArray[idx*4+0] = xPos;
                    this->vertexArray[idx*4+1] = yPos;
                    this->vertexArray[idx*4+2] = zPos;
                    this->vertexArray[idx*4+3] = 0.1f;
                    // set direction
                    vislib::math::Vector<float, 3> posn(
                        (xPos - vti->AccessBoundingBoxes().ObjectSpaceBBox().Left()) / vti->AccessBoundingBoxes().ObjectSpaceBBox().Width(),
                        (yPos - vti->AccessBoundingBoxes().ObjectSpaceBBox().Bottom()) / vti->AccessBoundingBoxes().ObjectSpaceBBox().Height(),
                        (zPos - vti->AccessBoundingBoxes().ObjectSpaceBBox().Back()) / vti->AccessBoundingBoxes().ObjectSpaceBBox().Depth());
                    posn *= gridSize;
                    posn.SetX( floorf(posn.X() + 0.5f));
                    posn.SetY( floorf(posn.Y() + 0.5f));
                    posn.SetZ( floorf(posn.Z() + 0.5f));
                    //unsigned int gridIdx = static_cast<unsigned int>(gridSize.X() * (gridSize.Y() * posn.Z() + posn.Y()) + posn.X());
                    //vislib::math::Vector<float, 3> dir( dirData[3*gridIdx], dirData[3*gridIdx+1], dirData[3*gridIdx+2]);
                    //this->dirArray[idx*3+0] = dir.X();
                    //this->dirArray[idx*3+1] = dir.Y();
                    //this->dirArray[idx*3+2] = dir.Z();
                    unsigned int gridIdx = static_cast<unsigned int>(gridSize.X() * (gridSize.Y() * zIdx + yIdx) + xIdx);
                    vislib::math::Vector<float, 3> dir( dirData[3*gridIdx], dirData[3*gridIdx+1], dirData[3*gridIdx+2]);
                    this->dirArray[idx*3+0] = dir.X();
                    this->dirArray[idx*3+1] = dir.Y();
                    this->dirArray[idx*3+2] = dir.Z();
                    this->vertexArray[idx*4+0] = (float(xIdx)/float(gridSize.X())) * vti->AccessBoundingBoxes().ObjectSpaceBBox().Width() + vti->AccessBoundingBoxes().ObjectSpaceBBox().Left();
                    this->vertexArray[idx*4+1] = (float(yIdx)/float(gridSize.Y())) * vti->AccessBoundingBoxes().ObjectSpaceBBox().Height() + vti->AccessBoundingBoxes().ObjectSpaceBBox().Bottom();
                    this->vertexArray[idx*4+2] = (float(zIdx)/float(gridSize.Z())) * vti->AccessBoundingBoxes().ObjectSpaceBBox().Depth() + vti->AccessBoundingBoxes().ObjectSpaceBBox().Back();
                    float test = (densityData[gridIdx] < this->minDensityFilterParam.Param<param::FloatParam>()->Value()) ? 0.0f : 1.0f;
                    this->vertexArray[idx*4+3] = 1.0f * 
                        ((dir.Length() - this->minC) / (this->maxC - this->minC)) * 
                        ((densityData[gridIdx] - minDensity) / (maxDensity - minDensity)) *
                        this->lengthScaleParam.Param<param::FloatParam>()->Value() * test;
                    // set color
                    this->colorArray[idx] = dir.Length();
                    idx++;
                }
                zPos = vti->AccessBoundingBoxes().ObjectSpaceBBox().Back();
            }
            yPos = vti->AccessBoundingBoxes().ObjectSpaceBBox().Bottom();
        }
        
        this->triggerArrowComputation = false;
    }

    // ---------- render ---------
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    float viewportStuff[4];
    ::glGetFloatv(GL_VIEWPORT, viewportStuff);
    glPointSize(vislib::math::Max(viewportStuff[2], viewportStuff[3]));
    if (viewportStuff[2] < 1.0f) viewportStuff[2] = 1.0f;
    if (viewportStuff[3] < 1.0f) viewportStuff[3] = 1.0f;
    viewportStuff[2] = 2.0f / viewportStuff[2];
    viewportStuff[3] = 2.0f / viewportStuff[3];

    this->arrowShader.Enable();

    glUniform4fvARB(this->arrowShader.ParameterLocation("viewAttr"), 1, viewportStuff);
    glUniform3fvARB(this->arrowShader.ParameterLocation("camIn"), 1, cr->GetCameraParameters()->Front().PeekComponents());
    glUniform3fvARB(this->arrowShader.ParameterLocation("camRight"), 1, cr->GetCameraParameters()->Right().PeekComponents());
    glUniform3fvARB(this->arrowShader.ParameterLocation("camUp"), 1, cr->GetCameraParameters()->Up().PeekComponents());
    this->arrowShader.SetParameter("lengthScale", this->lengthScaleParam.Param<param::FloatParam>()->Value());
    //this->arrowShader.SetParameter("lengthFilter", lengthFilter);
    this->arrowShader.SetParameter("lengthFilter", this->lengthFilterParam.Param<param::FloatParam>()->Value());

    unsigned int cial = glGetAttribLocationARB(this->arrowShader, "colIdx");
    unsigned int tpal = glGetAttribLocationARB(this->arrowShader, "dir");
    unsigned int colTabSize = 0;
    
    // colour
    //glEnableClientState(GL_COLOR_ARRAY);
    //glColorPointer(3, GL_FLOAT, 0, this->colorArray.PeekElements());
    glEnableVertexAttribArrayARB(cial);
    glVertexAttribPointerARB(cial, 1, GL_FLOAT, GL_FALSE, 0, this->colorArray.PeekElements());
    glEnable(GL_TEXTURE_1D);
    view::CallGetTransferFunction *cgtf = this->getTFSlot.CallAs<view::CallGetTransferFunction>();
    if ((cgtf != NULL) && ((*cgtf)())) {
        glBindTexture(GL_TEXTURE_1D, cgtf->OpenGLTexture());
        colTabSize = cgtf->TextureSize();
    } else {
        glBindTexture(GL_TEXTURE_1D, this->greyTF);
        colTabSize = 2;
    }
    glUniform1iARB(this->arrowShader.ParameterLocation("colTab"), 0);
    glColor3ub(127, 127, 127);

    // radius and position
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(4, GL_FLOAT, 0, this->vertexArray.PeekElements());

    // direction
    glEnableVertexAttribArrayARB(tpal);
    glVertexAttribPointerARB(tpal, 3, GL_FLOAT, GL_FALSE, 0, this->dirArray.PeekElements());
    
    glUniform4fARB(this->arrowShader.ParameterLocation("inConsts1"), -1.0f, this->minC, this->maxC, float(colTabSize));

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(this->arrowCount-1));

    //glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArrayARB(cial);
    glDisableVertexAttribArrayARB(tpal);

    this->arrowShader.Disable();
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDisable(GL_TEXTURE_1D);
    
    // unlock the current frame
    vti->Unlock();
    glPopMatrix();
    return true;
}


/*
 * update parameters
 */
void VolumeDirectionRenderer::UpdateParameters(const VTIDataCall *vti) {
    if( this->xResParam.IsDirty() ) {
        this->xRes = this->xResParam.Param<param::IntParam>()->Value();
        this->xResParam.ResetDirty();
        this->triggerArrowComputation = true;
    }
    if( this->yResParam.IsDirty() ) {
        this->yRes = this->yResParam.Param<param::IntParam>()->Value();
        this->yResParam.ResetDirty();
        this->triggerArrowComputation = true;
    }
    if( this->zResParam.IsDirty() ) {
        this->zRes = this->zResParam.Param<param::IntParam>()->Value();
        this->zResParam.ResetDirty();
        this->triggerArrowComputation = true;
    }
    if( this->minDensityFilterParam.IsDirty() ) {
        this->minDensityFilterParam.ResetDirty();
        this->triggerArrowComputation = true;
    }
    if( this->lengthScaleParam.IsDirty() ) {
        this->lengthScaleParam.ResetDirty();
        this->triggerArrowComputation = true;
    }
}
