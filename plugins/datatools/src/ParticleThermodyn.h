/*
 * ParticleThermodyn.h
 *
 * Copyright (C) 2017 by MegaMol team
 * Alle Rechte vorbehalten.
 */

#pragma once

#include "datatools/PointcloudHelpers.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"
#include "mmcore/param/ParamSlot.h"
#include <Eigen/Eigenvalues>
#include <nanoflann.hpp>
#include <vector>

namespace megamol::datatools {

/**
 * Module overriding global attributes of particles
 */
class ParticleThermodyn : public megamol::core::Module {
public:
    enum searchTypeEnum { RADIUS, NUM_NEIGHBORS };

    enum metricsEnum {
        TEMPERATURE,
        FRACTIONAL_ANISOTROPY,
        DENSITY,
        PRESSURE,
        NEIGHBORS,
        NEAREST_DISTANCE,
        PHASE01,
        PHASE02
    };

    enum phaseEnum { FLUID = 0, GAS = 1 };

    /** Return module class name */
    static const char* ClassName() {
        return "ParticleThermodyn";
    }

    /** Return module class description */
    static const char* Description() {
        return "Computes an intensity from some properties of a particle (compared to its surroundings).";
    }

    /** Module is always available */
    static bool IsAvailable() {
        return true;
    }

    /** Ctor */
    ParticleThermodyn();

    /** Dtor */
    ~ParticleThermodyn() override;

    /**
     * Called when the data is requested by this module
     *
     * @param c The incoming call
     *
     * @return True on success
     */
    bool getDataCallback(megamol::core::Call& c);

    /**
     * Called when the extend information is requested by this module
     *
     * @param c The incoming call
     *
     * @return True on success
     */
    bool getExtentCallback(megamol::core::Call& c);

protected:
    /** Lazy initialization of the module */
    bool create() override;

    /** Resource release */
    void release() override;

private:
    bool assertData(geocalls::MultiParticleDataCall* in, geocalls::MultiParticleDataCall* outMPDC);

    float computeTemperature(
        std::vector<std::pair<size_t, float>>& matches, size_t num_matches, float mass, float freedom);
    float computeFractionalAnisotropy(std::vector<std::pair<size_t, float>>& matches, size_t num_matches);
    float computeDensity(std::vector<std::pair<size_t, float>>& matches, size_t num_matches, float const curPoint[3],
        float radius, vislib::math::Cuboid<float> const& bbox);

    core::param::ParamSlot cyclXSlot;
    core::param::ParamSlot cyclYSlot;
    core::param::ParamSlot cyclZSlot;
    core::param::ParamSlot radiusSlot;
    core::param::ParamSlot numNeighborSlot;
    core::param::ParamSlot searchTypeSlot;
    core::param::ParamSlot minMetricSlot;
    core::param::ParamSlot maxMetricSlot;
    core::param::ParamSlot massSlot;
    core::param::ParamSlot freedomSlot;
    core::param::ParamSlot metricsSlot;
    core::param::ParamSlot removeSelfSlot;
    core::param::ParamSlot findExtremesSlot;
    core::param::ParamSlot extremeValueSlot;
    core::param::ParamSlot fluidDensitySlot;
    core::param::ParamSlot tcSlot;
    core::param::ParamSlot rhocSlot;

    size_t datahash;
    size_t myHash = 0;
    int lastTime;
    std::vector<float> newColors;
    std::vector<size_t> allParts;
    float maxDist;

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigensolver;

    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, simplePointcloud>, simplePointcloud,
        3 /* dim */, std::size_t>
        my_kd_tree_t;

    std::shared_ptr<my_kd_tree_t> particleTree;
    std::shared_ptr<simplePointcloud> myPts;

    /** The slot providing access to the manipulated data */
    megamol::core::CalleeSlot outDataSlot;

    /** The slot accessing the original data */
    megamol::core::CallerSlot inDataSlot;
};

} // namespace megamol::datatools
