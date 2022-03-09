/**
 *
 * Implementation of class for spline-interpolated cross section model
 *
 * (c) 2016
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3CrossSection.cxx
 * @date $Date$
 * @author cweaver
 *
 */
#include "phys-services/I3CrossSection.h"

#include <array>
#include <cmath>

#include <icetray/I3Logging.h>

namespace
{
    double particleMass(I3Particle::ParticleType type)
    {
        I3Particle p(I3Particle::Null, type);
        if (!p.HasMass())
        {
            log_debug_stream(
                "Treating particle of type " << p.GetTypeString() <<
                " with unknown mass as massless.");

            return 0.;
        }

        return p.GetMass();
    }

    /**
     * @brief Check the kinematics.
     *
     * Check whether a given point in phase space is physically realizable.
     * Based on equations 6-8 of S. Kretzer and M. H. Reno, Phys. Rev. D 66,
     * 113007
     *
     * @param[in] x
     *     Bjorken x of the interaction
     * @param[in] y
     *     Bjorken y of the interaction
     * @param[in] E
     *     Incoming neutrino in energy in the lab frame ($E_{\nu}$)
     * @param[in] M
     *     Mass of the target nucleon ($M_{N}$)
     * @param[in] m
     *     Mass of the secondary lepton ($m_{\tau}$)
     *
     * @return
     *     Result of check on the kinematics
     */
    bool kinematicallyAllowed(double x, double y, double E, double M, double m)
    {
        if (x > 1.)
        {
            // eq. 6 right inequality
            return false ;
        }
        if (x < ((m*m) / (2.*M*(E - m))))
        {
            // eq. 6 left inequality
            return false;
        }

        // denominator of a and b
        double d = 2. * (1. + (M*x) / (2.*E));
        // the numerator of a (or a*d)
        double ad = 1. - m*m*((1./(2.*M*E*x)) + (1./(2.*E*E)));
        double term = 1. - ((m*m) / (2.*M*E*x));
        // the numerator of b (or b*d)
        double bd = std::sqrt(term*term - ((m*m)/(E*E)));

        // eq. 7
        return ((ad - bd) <= d*y && d*y <= (ad + bd));
    }
}


I3CrossSection::finalStateRecord
I3CrossSection::sampleFinalState(double energy,
                                I3Particle::ParticleType scatteredType,
                                boost::shared_ptr<I3RandomService> random) const{
    // calls the DIS function for DIS cases
    // calls the GR function  for GR  cases
    // if it doesn't know what kind of interaction this is, give up

    if (interaction_ ==1 || interaction_==2){
        return( I3CrossSection::sampleFinalState_DIS( energy, scatteredType, random ) );
    }else if(interaction_==3){
        return( I3CrossSection::sampleFinalState_GR(  energy, scatteredType, random ) );
    }else{ //should be easy to modify this to support other interaction cross sections! 
        log_fatal_stream("Unknown interaction number "<<interaction_<<". Your fits files are funky.");
    }
}

I3CrossSection::finalStateRecord I3CrossSection::sampleFinalState_DIS(double energy, 
                               I3Particle::ParticleType scatteredType, 
                               boost::shared_ptr<I3RandomService> random) const{
    // Uses Metropolis-Hastings Algorithm! 
    //      useful for cases where we don't know the supremum of our distribution, and the distribution is multi-dimensional 
    
    if (crossSection_->get_ndim()!=3){
        log_fatal_stream("I expected 3 dimensions in the cross section spline, but got "<< crossSection_->get_ndim() <<". Maybe your fits file doesn't have the right 'INTERACTION' key?");
    }

	double m=particleMass(scatteredType);
	//The out-going particle always gets at least enough energy for its rest mass
	double yMax=1-m/energy;
	double logYMax=std::log10(yMax);
	
	//we assume that:
	//the target is stationary so its energy is just its mass
	//the incoming neutrino is massless, so its kinetic energy is its total energy
	double s=targetMass_*targetMass_+2*targetMass_*energy;


	//The minimum allowed value of y occurs when x=1 and Q is minimized
	double yMin=Q2Min_/s;
	double logYMin=log10(yMin);
	//The minimum allowed value of x occurs when y=yMax and Q is minimized
	double xMin=Q2Min_/((s-targetMass_*targetMass_)*yMax);
	double logXMin=log10(xMin);

	bool accept = false;
	
    // kin_vars and its twin are 3-vectors containing [nu-energy, Bjorken X, Bjorken Y]
    std::array<double,3> kin_vars, test_kin_vars;

	// centers of the cross section spline tales. 
    std::array<int,3> spline_table_center, test_spline_table_center;

	// values of cross_section from the splines.  By * Bx * Spline(E,x,y)
    double cross_section, test_cross_section;
    
    // No matter what, we're evaluating at this specific energy. 
	kin_vars[0] = test_kin_vars[0] = log10(energy);

	//check preconditions
	if(kin_vars[0]<crossSection_->lower_extent(0)
	   || kin_vars[0]>crossSection_->upper_extent(0))
    {
        log_fatal_stream("Interaction energy out of cross section table range: ["
                        << std::pow(10.,crossSection_->lower_extent(0)) << " GeV,"
                        << std::pow(10.,crossSection_->upper_extent(0)) << " GeV]");
    }
	//sample an intial point
	do{
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ  __attribute__((unused));
		do{
			kin_vars[1]=random->Uniform(logXMin,0);
			kin_vars[2]=random->Uniform(logYMin,logYMax);
			trialQ=(s-targetMass_*targetMass_)*std::pow(10.,kin_vars[1]+kin_vars[2]);
		}while(trialQ<Q2Min_ || !kinematicallyAllowed(pow(10.,kin_vars[1]),pow(10.,kin_vars[2]),energy,targetMass_,m));

		//sanity check: demand that the sampled point be within the table extents
		accept = (
			kin_vars[1] >= crossSection_->lower_extent(1) &&
			kin_vars[1] <= crossSection_->upper_extent(1) &&
			kin_vars[2] >= crossSection_->lower_extent(2) &&
			kin_vars[2] <= crossSection_->upper_extent(2)
			);

		if(accept)
		{
			// finds the centers in the cross section spline table, returns true if it's successful
			//			also sets the centers
			accept = crossSection_->searchcenters(kin_vars.data(), spline_table_center.data());
		}
	} while(!accept);

	//TODO: better proposal distribution?
	double measure=std::pow(10.,kin_vars[1]+kin_vars[2]); // Bx * By

    // Bx * By * xs(E, x, y)
    // evalutates the differential spline at that point 
	cross_section= measure * std::pow(
        10., crossSection_->ndsplineeval(kin_vars.data(), spline_table_center.data(), 0));
    
    // Metropolis Hastings Algorithm.
    // MCMC method!
    size_t burnin=40; 
	for(size_t j=0; j<=burnin; j++){
		// repeat the sampling from above to get a new valid point
		double trialQ  __attribute__((unused));
		do{
			test_kin_vars[1]=random->Uniform(logXMin,0);
			test_kin_vars[2]=random->Uniform(logYMin,logYMax);

			trialQ=(s-targetMass_*targetMass_)*
                std::pow(10.,test_kin_vars[1]+test_kin_vars[2]);
		}
        while(trialQ<Q2Min_ || !kinematicallyAllowed(
                    std::pow(10.,test_kin_vars[1]), std::pow(10.,test_kin_vars[2]),energy,
                    targetMass_,m));


        accept = (
            kin_vars[1] >= crossSection_->lower_extent(1) &&
            kin_vars[1] <= crossSection_->upper_extent(1) &&
            kin_vars[2] >= crossSection_->lower_extent(2) &&
            kin_vars[2] <= crossSection_->upper_extent(2)
            );


        if (accept){
            accept = crossSection_->searchcenters(test_kin_vars.data(), test_spline_table_center.data());
        }
        if (!accept){
            continue;
        }
		
		double measure=std::pow(10.,test_kin_vars[1]+test_kin_vars[2]);
		double eval=crossSection_->ndsplineeval(test_kin_vars.data(), test_spline_table_center.data(), 0);
		if (std::isnan(eval)){
            continue;
        }
        
		test_cross_section=measure*std::pow(10.,eval);

		double odds=(test_cross_section/cross_section);
		accept=((odds>1.) || random->Uniform(0,1)<odds);

		if(accept){
			kin_vars=test_kin_vars;
			cross_section=test_cross_section;
		}
	}
	//record whatever we sampled
	//log_info_stream(" lx=" << x[1] << " ly=" << x[2]);
	return(finalStateRecord(pow(10.,kin_vars[1]),pow(10.,kin_vars[2])));
}

I3CrossSection::finalStateRecord
I3CrossSection::sampleFinalState_GR(double energy,
                                I3Particle::ParticleType scatteredType,
                                boost::shared_ptr<I3RandomService> random) const{
    // this does the work for GR interactions.
    // should be like (log(E) vs log(Bjorken Y))
    if (crossSection_->get_ndim()!=2){
        log_fatal_stream("I expected a 2D cross section spline, but got "<<crossSection_->get_ndim()<<" dimensions. Are you sure this is the right fits file? Check the 'INTERACTION' key!");
    }
   
    ///
    /// 
    //      basically, this is just like the DIS function, but there are some changes. I'll highlight the changes
    ///
    ///
    double m=particleMass(scatteredType);
	double yMax=1-m/energy;
	double logYMax=log10(yMax);
    
    // improve the Ymin function
    // maybe set to minimum extent of the Bjorken Y table.  
	double yMin=pow(10.,-4);
	double logYMin=log10(yMin);

	bool accept;
	
    // kin_vars and its twin are now  ***2-vectors*** containing [nu-energy, Bjorkfen Y]
    std::array<double,2> kin_vars, test_kin_vars;

	// centers of the cross section spline tales. 
    std::array<int,2> spline_table_center, test_spline_table_center;

	// values of cross_section from the splines.  By * Spline(E,y)
    double cross_section, test_cross_section;
    
    // No matter what, we're evaluating at this specific energy. 
	kin_vars[0] = test_kin_vars[0] = log10(energy);

	//check preconditions
    if (kin_vars[0] < crossSection_->lower_extent(0) || 
            kin_vars[0]>crossSection_->upper_extent(0))
    {
        log_fatal_stream("Interaction energy (" << energy << 
                        ") is out of cross section table range: ["
	                 << std::pow(10.,crossSection_->lower_extent(0)) << " GeV,"
	                 << std::pow(10.,crossSection_->upper_extent(0)) << " GeV]");
    }
	
	//sample an intial point
	do{
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ	 __attribute__((unused));
		
		kin_vars[1]=random->Uniform(logYMin,logYMax);
				
				
		accept = (
			kin_vars[1] >= crossSection_->lower_extent(1) &&
			kin_vars[1] <= crossSection_->upper_extent(1)
			);


		if(accept){
						accept = crossSection_->searchcenters(kin_vars.data(), spline_table_center.data());
		}

	}
	while(!accept);

	//TODO: better proposal distribution?
	// dropped the extra term
	double measure=std::pow(10.,kin_vars[1]); // By
    
    cross_section = measure*std::pow(
            10., crossSection_->ndsplineeval(kin_vars.data(), spline_table_center.data(), 0));

    size_t burnin=40;
	for(size_t j=0; j<=burnin; j++){
		double trialQ  __attribute__((unused));
		test_kin_vars[1]=random->Uniform(logYMin,logYMax);

        accept = (
            test_kin_vars[1] >= crossSection_->lower_extent(1) &&
            test_kin_vars[1] <= crossSection_->upper_extent(1)
        );

        if (accept){
            accept = crossSection_->searchcenters(test_kin_vars.data(), test_spline_table_center.data());
        }
        if (!accept){
            continue;
        }

		
		double measure=std::pow(10.,test_kin_vars[1]);
		double eval=crossSection_->ndsplineeval(test_kin_vars.data(),test_spline_table_center.data(),0);
		if(std::isnan(eval))
        {
			continue;
        }
		test_cross_section=measure*pow(10.,eval);

		double odds=(test_cross_section/cross_section);
		accept=((odds>1.) || random->Uniform(0,1)<odds);

		if(accept){
			kin_vars=test_kin_vars;
			cross_section=test_cross_section;
		}
	}
	
    //return what we've sampled.
    //this was originally written with DIS in mind,
    //      so we send back the (x,y) pair expected, but set X to 1.0    
    return(finalStateRecord(1.0, pow(10.,kin_vars[1])));

}


double I3CrossSection::evaluateCrossSection(double energy, double x, double y,
                                            I3Particle::ParticleType scatteredType) const{
	double log_energy=std::log10(energy);
	//check preconditions
	if(log_energy<totalCrossSection_->lower_extent(0)
	   || log_energy>totalCrossSection_->upper_extent(0))
    {
		log_fatal_stream("Interaction energy (" << energy << 
						 ") out of cross section table range: ["
						 << std::pow(10.,totalCrossSection_->lower_extent(0)) << " GeV,"
						 << std::pow(10.,totalCrossSection_->upper_extent(0)) << " GeV]");
    }
	if(x<=0 || x>=1)
    {
		log_fatal_stream("Interaction x out of range: " << x);
    }
	if(y<=0 || y>=1)
    {
		log_fatal_stream("Interaction y out of range: " << y);
    }
	//we assume that:
	//the target is stationary so its energy is just its mass
	//the incoming neutrino is massless, so its kinetic energy is its total energy
	double s=targetMass_*targetMass_+2*targetMass_*energy;
	double Q2=(s-targetMass_*targetMass_)*x*y;
	if(Q2<Q2Min_) //cross section not calculated, assumed to be zero
		return 0;
	
	//cross section should be zero, but this check is missing from the original 
	//CSMS calculation, so we must add it here
	if(!kinematicallyAllowed(x, y, energy, targetMass_, particleMass(scatteredType)))
		return 0;
	
	std::array<double,3> coordinates{{log_energy,std::log10(x),std::log10(y)}};
	std::array<int,3> centers;
    if(!crossSection_->searchcenters(coordinates.data(), centers.data()))
    {
        return 0;
    }
    double result=std::pow(10, crossSection_->ndsplineeval(coordinates.data(),centers.data(), 0));
	assert(result>=0);
	return(result);
}

double I3CrossSection::evaluateTotalCrossSection(double energy) const
{
    double log_energy = std::log10(energy);

    // check preconditions
    if (log_energy < totalCrossSection_->lower_extent(0) ||
            log_energy > totalCrossSection_->upper_extent(0))
    {
        log_fatal_stream(
            "Interaction energy (" << energy <<
            ") is out of cross section table range: [" <<
            std::pow(10., totalCrossSection_->lower_extent(0)) << "GeV," <<
            std::pow(10., totalCrossSection_->upper_extent(0)) << "GeV].");
    }

    // evaluate
    int center;
    totalCrossSection_->searchcenters(&log_energy, &center);
    double log_xs = totalCrossSection_->ndsplineeval(&log_energy, &center, 0);

    return std::pow(10., log_xs);
}

void I3CrossSection::load(
    const std::string& dd_crossSectionFile,
    const std::string& total_crossSectionFile)
{
    bool success = crossSection_->read_fits(dd_crossSectionFile);
    if (!success)
    {
        log_fatal_stream(
            "Failed to read cross section data from spline FITS file '"
            << dd_crossSectionFile << "'.");
    }
    if (crossSection_->get_ndim() != 3 && crossSection_->get_ndim() != 2)
    {
        log_fatal_stream(
            "Cross section spline has " << crossSection_->get_ndim()
            << " dimensions, should have 3 (log10(E), log10(x), log10(y)) or 2 (log10(E), log10(y)).");
    }

    success = totalCrossSection_->read_fits(total_crossSectionFile);
    if (!success)
    {
        log_fatal_stream(
            "Failed to read cross section data from spline FITS file '"
            << total_crossSectionFile << "'.");
    }
    if (totalCrossSection_->get_ndim() != 1)
    {
        log_fatal_stream(
            "Total cross section spline has " << totalCrossSection_->get_ndim()
            << " dimensions, should have 1, log10(E).");
    }

    success = crossSection_->read_key("TARGETMASS", targetMass_);
    if (!success)
    {
        if (crossSection_->get_ndim()==3){
            log_warn(
                "Unable to read TARGETMASS key from cross section spline; "
                "assuming an isoscalar mass");

            targetMass_ = (
                particleMass(I3Particle::PPlus) +
                particleMass(I3Particle::Neutron)) / 2.;
        }else if (crossSection_->get_ndim()==2){
            log_warn("Unable to read TARGETMASS key from cross section spline, assuming electron mass");
            targetMass_ = particleMass(I3Particle::EMinus);
        }
    }

    success = crossSection_->read_key("INTERACTION", interaction_);
    if (!success){
        log_warn("Unable to read INTERACTION key from cross section spline, assuming DIS");
        interaction_ = 1;
    }    

    success = crossSection_->read_key("Q2MIN", Q2Min_);
    if (!success)
    {
        log_warn(
            "Unable to read Q2Min key from cross section spline; "
            "assuming 1 GeV^2");

        Q2Min_ = 1.;
    }
}

double I3CrossSection::getMinimumEnergy() const
{
    return std::pow(10., totalCrossSection_->lower_extent(0));
}

double I3CrossSection::getMaximumEnergy() const
{
    return std::pow(10., totalCrossSection_->upper_extent(0));
}
