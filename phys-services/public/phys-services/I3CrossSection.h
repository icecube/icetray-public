/**
 *
 * Definition of class for spline-interpolated cross section model
 *
 * (c) 2016
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3CrossSection.h
 * @date $Date$
 * @author cweaver
 *
 */
#ifndef I3CROSSSECTION_H
#define I3CROSSSECTION_H

#include <memory>
#include <string>

#include <boost/shared_ptr.hpp>
#include <photospline/splinetable.h>

#include <dataclasses/physics/I3Particle.h>
#include <phys-services/I3RandomService.h>

/**
 * Encapsulates cross section information for one physical process with a
 * two-body final state.
 *
 * Cross sections are stored as a pair of photospline objects: one spline for
 * the differential cross section, and one for the total cross section. The
 * latter could be derived from the former, but is included to avoid users
 * having to repeatedly integrate the differential spline numerically.
 *
 * The differential spline must have three dimensions:
 *  - log10 of incoming neutrino energy in GeV
 *  - log10 of final state x
 *  - log10 of final state y
 *
 * The value of the spline must be the log10 of the cross section. The
 * absolute value of the differential cross section is never used, so its units
 * are not important, however it is advisable that it should be consistent with
 * the total cross section (see below).
 *
 * The total spline must have one dimension, which is log10 of incoming
 * neutrino energy in GeV. The value of the spline must be the log10 of the
 * total cross section in square meters.
 */

class I3CrossSection
{
    public:
        using splinetable_t = photospline::splinetable<>;

        /**
         * Create an instance with empty cross section splines.
         */
        I3CrossSection()
            : crossSection_{std::make_shared<splinetable_t>()},
            totalCrossSection_{std::make_shared<splinetable_t>()},
            Q2Min_{0.},
            targetMass_{0.}
        {};

        /**
         * Create an instance from cross section splines stored in FITS files.
         *
         * @param[in] dd_crossSectionFile
         *     Path to the doubly-differential cross section spline to load
         * @param[in] total_crossSectionFile
         *     Path to the total cross section file to load
         */
        I3CrossSection(
            const std::string& dd_crossSectionFile,
            const std::string& total_crossSectionFile)
            : I3CrossSection()
        {
            load(dd_crossSectionFile, total_crossSectionFile);
        }

        I3CrossSection(const I3CrossSection&)=default;
        I3CrossSection(I3CrossSection&&)=default;

        ~I3CrossSection()=default;

        I3CrossSection& operator=(const I3CrossSection&)=default;
        I3CrossSection& operator=(I3CrossSection&&)=default;

        /**
         * A single final state drawn from a differential cross section
         */
        struct finalStateRecord
        {
            /// Bjorken x
            double x;
            /// Bjorken y
            double y;

            finalStateRecord(double x, double y)
                : x{x}, y{y}
            {}
        };

        /**
         * Sample a final state.
         *
         * @param[in] energy
         *     The energy of the incoming neutrino in GeV
         * @param[in] scatteredType
         *     The type of the outgoing lepton
         * @param[in]
         *     A pseudo-random number generator
         *
         * @return
         *     The sampled final state
         */
        finalStateRecord sampleFinalState(
            double energy,
            I3Particle::ParticleType scatteredType,
            boost::shared_ptr<I3RandomService> random) const;

        ///Sample a final state
        ///\param energy the energy of the incoming neutrino
        ///\param scatteredType the type of the outgoing lepton
        ///\param random a source of random numbers
        finalStateRecord sampleFinalState_DIS(double energy,
                                        I3Particle::ParticleType scatteredType,
                                        boost::shared_ptr<I3RandomService> random) const;

        
        // the GR sampler just returns X=1
        finalStateRecord sampleFinalState_GR(double energy,
                                        I3Particle::ParticleType scatteredType,
                                        boost::shared_ptr<I3RandomService> random) const;

        /**
         * Get the value of the doubly-differential cross section.
         *
         * @param[in] energy
         *     The neutrino energy in GeV
         * @param[in] x
         *     Bjorken x (fraction of the nucleon momentum in the struck
         *     parton)
         * @param[in] y
         *     Bjorken y (fraction of incoming energy transferred to the
         *     nucleon)
         * @param[in] scatteredType
         *     The type of the outgoing lepton
         *
         * @return
         *     The doubly-differential cross section in square meters
         */
        double evaluateCrossSection(
            double energy,
            double x,
            double y,
            I3Particle::ParticleType scatteredType) const;

        /**
         * Get the value of the total cross section.
         *
         * @param[in] energy
         *     The neutrino energy in GeV
         *
         * @return
         *     The total cross section in square meters
         */
        double evaluateTotalCrossSection(double energy) const;

        /**
         * @return
         *     Doubly-differential cross section spline
         */
        const splinetable_t& getCrossSection() const
        {
            return *crossSection_;
        }

        /**
         * @return
         *     Total cross section spline
         */
        const splinetable_t& getTotalCrossSection() const
        {
            return *totalCrossSection_;
        }

        /**
         * Load cross section splines stored in FITS files.
         *
         * @param[in] dd_crossSectionFile
         *     Path to the doubly-differential cross section spline to load
         * @param[in] total_crossSectionFile
         *     Path to the total cross section file to load
         */
        void load(
            const std::string& dd_crossSectionFile,
            const std::string& total_crossSectionFile);

        /**
         * @return
         *     The minimum value of Q^2 for which the cross section was
         *     calculated
         */
        double getQ2Min() const {return Q2Min_;}

        /**
         * @return
         *     The mass of the target nucleon for which the cross section was
         *     calculated
         */
        double getTargetMass() const {return targetMass_;}

        /**
         * @return
         *     The minimum energy included in this cross section table
         */
        double getMinimumEnergy() const;

        /**
         * @return
         *     The maximum energy included in this cross section table
         */
        double getMaximumEnergy() const;

    private:
        /// Total cross section spline
        std::shared_ptr<splinetable_t> crossSection_;
        /// Doubly-differential cross section spline
        std::shared_ptr<splinetable_t> totalCrossSection_;
        ///The minimum value of Q^2 for which the cross section was calculated
        double Q2Min_; 
        ///The mass of the target nucleon
        double targetMass_;
        ///The interaction type, related to dimensionality of the 
        // 1:CC, 2:NC, 3:GR
        int interaction_;
};

#endif // I3CROSSSECTION_H
