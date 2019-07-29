#include <phys-services/I3CrossSection.h>
#include <array>
#include <fstream>

namespace{
	double particleMass(I3Particle::ParticleType type){
		I3Particle p(I3Particle::Null,type);
		if(!p.HasMass()){
			log_debug_stream("Treating particle of type " << p.GetTypeString()
			                 << " with unknown mass as massless");
			return(0);
		}
		return(p.GetMass());
	}
	
	///Check whether a given point in phase space is physically realizable.
	///Based on equations 6-8 of http://dx.doi.org/10.1103/PhysRevD.66.113007
	///S. Kretzer and M. H. Reno
	///"Tau neutrino deep inelastic charged current interactions"
	///Phys. Rev. D 66, 113007
	///\param x Bjorken x of the interaction
	///\param y Bjorken y of the interaction
	///\param E Incoming neutrino in energy in the lab frame ($E_\nu$)
	///\param M Mass of the target nucleon ($M_N$)
	///\param m Mass of the secondary lepton ($m_\tau$)
	bool kinematicallyAllowed(double x, double y, double E, double M, double m){
		if(x>1) //Eq. 6 right inequality
			return(false);
		if(x<((m*m)/(2*M*(E-m)))) //Eq. 6 left inequality
			return(false);
		//denominator of a and b
		double d=2*(1+(M*x)/(2*E));
		//the numerator of a (or a*d)
		double ad=1-m*m*((1/(2*M*E*x))+(1/(2*E*E)));
		double term=1-((m*m)/(2*M*E*x));
		//the numerator of b (or b*d)
		double bd=sqrt(term*term-((m*m)/(E*E)));
		return((ad-bd)<=d*y && d*y<=(ad+bd)); //Eq. 7
	}
}

I3CrossSection::finalStateRecord 
I3CrossSection::sampleFinalState_DIS(double energy, 
                               I3Particle::ParticleType scatteredType, 
                               boost::shared_ptr<I3RandomService> random) const{
    // Uses Metropolis-Hastings Algorithm! 
    //      useful for cases where we don't know the supremum of our distribution, and the distribution is multi-dimensional 
    
    if (crossSection.ndim!=3){
        log_fatal_stream("I expected 3 dimensions in the cross section spline, but got "<< crossSection.ndim <<". Maybe your fits file doesn't have the right 'INTERACTION' key?");
    }

	double m=particleMass(scatteredType);
	//The out-going particle always gets at least enough energy for its rest mass
	double yMax=1-m/energy;
	double logYMax=log10(yMax);
	
	//we assume that:
	//the target is stationary so its energy is just its mass
	//the incoming neutrino is massless, so its kinetic energy is its total energy
	double s=targetMass*targetMass+2*targetMass*energy;


	//The minimum allowed value of y occurs when x=1 and Q is minimized
	double yMin=Q2Min/s;
	double logYMin=log10(yMin);
	//The minimum allowed value of x occurs when y=yMax and Q is minimized
	double xMin=Q2Min/((s-targetMass*targetMass)*yMax);
	double logXMin=log10(xMin);

	bool accept;
	
    // kin_vars and its twin are 3-vectors containing [nu-energy, Bjorken X, Bjorken Y]
    std::array<double,3> kin_vars, test_kin_vars;

	// centers of the cross section spline tales. 
    std::array<int,3> spline_table_center, test_spline_table_center;

	// values of cross_section from the splines.  By * Bx * Spline(E,x,y)
    double cross_section, test_cross_section;
    
    // No matter what, we're evaluating at this specific energy. 
	kin_vars[0] = test_kin_vars[0] = log10(energy);

	//check preconditions
	if(kin_vars[0]<crossSection.extents[0][0]
	   || kin_vars[0]>crossSection.extents[0][1])
	log_fatal_stream("Interaction energy out of cross section table range: ["
	                 << pow(10.,crossSection.extents[0][0]) << " GeV,"
	                 << pow(10.,crossSection.extents[0][1]) << " GeV]");
	
	//sample an intial point
	do{
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ;
		do{
			kin_vars[1]=random->Uniform(logXMin,0);
			kin_vars[2]=random->Uniform(logYMin,logYMax);
			trialQ=(s-targetMass*targetMass)*pow(10.,kin_vars[1]+kin_vars[2]);
		}while(trialQ<Q2Min || !kinematicallyAllowed(pow(10.,kin_vars[1]),pow(10.,kin_vars[2]),energy,targetMass,m));

		accept=true;
		//sanity check: demand that the sampled point be within the table extents
		if(kin_vars[1]<crossSection.extents[1][0]
		   || kin_vars[1]>crossSection.extents[1][1])
			accept=false;
        if(kin_vars[2]<crossSection.extents[2][0]
		   || kin_vars[2]>crossSection.extents[2][1])
			accept=false;
        

		if(accept)
            // finds the centers in the cross section spline table, returns true if it's successful
            //      also sets the centers
			accept=!tablesearchcenters(&crossSection,kin_vars.data(),spline_table_center.data());
	} while(!accept);

	//TODO: better proposal distribution?
	double measure=pow(10.,kin_vars[1]+kin_vars[2]); // Bx * By

    // Bx * By * xs(E, x, y)
    // evalutates the differential spline at that point? 
	cross_section=measure*pow(10.,ndsplineeval(&crossSection,kin_vars.data(),spline_table_center.data(),0)); 
    
    // this is the magic part. Metropolis Hastings Algorithm.
    // MCMC method! 
	const size_t burnin=40; // converges to the correct distribution over multiple samplings. 
                            // big number means more accurate, but slower
	for(size_t j=0; j<=burnin; j++){
		// repeat the sampling from above to get a new valid point
        double trialQ;
		do{
			test_kin_vars[1]=random->Uniform(logXMin,0);
			test_kin_vars[2]=random->Uniform(logYMin,logYMax);
			trialQ=(s-targetMass*targetMass)*pow(10.,test_kin_vars[1]+test_kin_vars[2]);
		}while(trialQ<Q2Min || !kinematicallyAllowed(pow(10.,test_kin_vars[1]),pow(10.,test_kin_vars[2]),energy,targetMass,m));

		accept=true;
        if(test_kin_vars[1]<crossSection.extents[1][0]
		   || test_kin_vars[1]>crossSection.extents[1][1])
			accept=false;
		if(test_kin_vars[2]<crossSection.extents[2][0]
		   || test_kin_vars[2]>crossSection.extents[2][1])
			accept=false;
		if(!accept)
			continue;

		accept=!tablesearchcenters(&crossSection,test_kin_vars.data(),test_spline_table_center.data());
		if(!accept)
			continue;
		
		double measure=pow(10.,test_kin_vars[1]+test_kin_vars[2]);
		double eval=ndsplineeval(&crossSection,test_kin_vars.data(),test_spline_table_center.data(),0);
		if(std::isnan(eval))
			continue;
		test_cross_section=measure*pow(10.,eval);

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
    if (crossSection.ndim!=2){
        log_fatal_stream("I expected a 2D cross section spline, but got "<<crossSection.ndim<<" dimensions. Are you sure this is the right fits file? Check the 'INTERACTION' key!");
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
	if(kin_vars[0]<crossSection.extents[0][0]
	   || kin_vars[0]>crossSection.extents[0][1])
	log_fatal_stream("Interaction energy out of cross section table range: ["
	                 << pow(10.,crossSection.extents[0][0]) << " GeV,"
	                 << pow(10.,crossSection.extents[0][1]) << " GeV]");
	
	//sample an intial point
	do{
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ;
		
        // all values of Y and energy  are valid, we won't find an incompatible option here
	    kin_vars[1]=random->Uniform(logYMin,logYMax);
	    	
        
		accept=true;
        //only one dimension to ckeck in this case
		if(kin_vars[1]<crossSection.extents[1][0]
		   || kin_vars[1]>crossSection.extents[1][1])
			accept=false;	

		if(accept)
			accept=!tablesearchcenters(&crossSection,kin_vars.data(),spline_table_center.data());
	} while(!accept);

	//TODO: better proposal distribution?
    // dropped the extra term
	double measure=pow(10.,kin_vars[1]); // By

	cross_section=measure*pow(10.,ndsplineeval(&crossSection,kin_vars.data(),spline_table_center.data(),0)); 
    
	const size_t burnin=40; // converges to the correct distribution over multiple samplings. 
	for(size_t j=0; j<=burnin; j++){
        double trialQ;
		test_kin_vars[1]=random->Uniform(logYMin,logYMax);

		accept=true;
        if(test_kin_vars[1]<crossSection.extents[1][0]
		   || test_kin_vars[1]>crossSection.extents[1][1])
			accept=false;
		if(!accept)
			continue;

		accept=!tablesearchcenters(&crossSection,test_kin_vars.data(),test_spline_table_center.data());
		if(!accept)
			continue;
		
		double measure=pow(10.,test_kin_vars[1]);
		double eval=ndsplineeval(&crossSection,test_kin_vars.data(),test_spline_table_center.data(),0);
		if(std::isnan(eval))
			continue;
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

I3CrossSection::finalStateRecord
I3CrossSection::sampleFinalState(double energy,
                                I3Particle::ParticleType scatteredType,
                                boost::shared_ptr<I3RandomService> random) const{
    // calls the DIS function for DIS cases
    // calls the GR function  for GR  cases
    // if it doesn't know what kind of interaction this is, give up

    if (interaction ==1 || interaction==2){
        return( I3CrossSection::sampleFinalState_DIS( energy, scatteredType, random ) );
    }else if(interaction==3){
        return( I3CrossSection::sampleFinalState_GR(  energy, scatteredType, random ) );
    }else{ //should be easy to modify this to support other interaction cross sections! 
        log_fatal_stream("Unknown interaction number "<<interaction<<". Your fits files are funky.");
    }
}


double I3CrossSection::evaluateCrossSection(double energy, double x, double y,
                                            I3Particle::ParticleType scatteredType) const{
	double log_energy=log10(energy);
	//check preconditions
	if(log_energy<totalCrossSection.extents[0][0]
	   || log_energy>totalCrossSection.extents[0][1])
		log_fatal_stream("Interaction energy (" << energy << 
						 ") out of cross section table range: ["
						 << pow(10.,totalCrossSection.extents[0][0]) << " GeV,"
						 << pow(10.,totalCrossSection.extents[0][1]) << " GeV]");
	if(x<=0 || x>=1)
		log_fatal_stream("Interaction x out of range: " << x);
	if(y<=0 || y>=1)
		log_fatal_stream("Interaction y out of range: " << y);
	
	//we assume that:
	//the target is stationary so its energy is just its mass
	//the incoming neutrino is massless, so its kinetic energy is its total energy
	double s=targetMass*targetMass+2*targetMass*energy;
	double Q2=(s-targetMass*targetMass)*x*y;
	if(Q2<Q2Min) //cross section not calculated, assumed to be zero
		return 0;
	
	//cross section should be zero, but this check is missing from the original 
	//CSMS calculation, so we must add it here
	if(!kinematicallyAllowed(x, y, energy, targetMass, particleMass(scatteredType)))
		return 0;
	
	std::array<double,3> coordinates{{log_energy,log10(x),log10(y)}};
	std::array<int,3> centers;
	if(tablesearchcenters(&crossSection,coordinates.data(),centers.data()))
		return 0;
	double result=pow(10.,ndsplineeval(&crossSection,coordinates.data(),centers.data(),0));
	assert(result>=0);
	return(result);
}

double I3CrossSection::evaluateTotalCrossSection(double energy) const{
	double log_energy=log10(energy);
	//check preconditions
	if(log_energy<totalCrossSection.extents[0][0]
	   || log_energy>totalCrossSection.extents[0][1])
	log_fatal_stream("Interaction energy out of cross section table range: ["
	                 << pow(10.,totalCrossSection.extents[0][0]) << " GeV,"
	                 << pow(10.,totalCrossSection.extents[0][1]) << " GeV]");
	//evaluate
	int center;
	tablesearchcenters(&totalCrossSection,&log_energy,&center);
	double log_xs=ndsplineeval(&totalCrossSection,&log_energy,&center,0);
	return(pow(10.,log_xs));
}

void I3CrossSection::load(std::string dd_crossSectionFile, std::string total_crossSectionFile){
	int status=readsplinefitstable(dd_crossSectionFile.c_str(),&crossSection);
	if(status!=0)
		log_fatal_stream("Failed to read cross section data from spline FITS file '"
		                 << dd_crossSectionFile << "': error code " << status);
	if(crossSection.ndim!=3 && crossSection.ndim!=2)
		log_fatal_stream("cross section spline has " << crossSection.ndim
		                 << " dimensions, should have either 3 (log10(E), log10(x), log10(y)) or 2 (log10(E), log10(y))");
	
	status=readsplinefitstable(total_crossSectionFile.c_str(),&totalCrossSection);
	if(status!=0)
		log_fatal_stream("Failed to read cross section data from spline FITS file '"
		                 << total_crossSectionFile << "': error code " << status);
	if(totalCrossSection.ndim!=1)
		log_fatal_stream("Total cross section spline has " << totalCrossSection.ndim
		                 << " dimensions, should have 1, log10(E)");
	
	int err=0;
	err=splinetable_read_key(&crossSection, SPLINETABLE_DOUBLE, "TARGETMASS", &targetMass);
	if(err){
        // TODO: have it use the interaction type to set the masses as a backup instead of the dimensionality 
        if(crossSection.ndim==3){
		    log_warn("Unable to read TARGETMASS key from cross section spline, using isoscalar mass");
    		targetMass=(particleMass(I3Particle::PPlus)+
		                particleMass(I3Particle::Neutron))/2;
        }else if(crossSection.ndim==2){
            log_warn("Unable to read TARGETMASS key from cross section spline, using electron mass");
            targetMass=particleMass(I3Particle::EMinus);
        }else{
            log_fatal_stream("Logic error. This point should be unreachable!");
        }
	}
    err=splinetable_read_key(&crossSection, SPLINETABLE_INT, "INTERACTION", &interaction);
    if(err){
        // assume DIS to preserve compatability with previous versions!
        log_warn("Unable to read INTERACTION key from cross section spline, assuming DIS");
        interaction=1;
    }

	err=splinetable_read_key(&crossSection, SPLINETABLE_DOUBLE, "Q2MIN", &Q2Min);
	if(err){
		log_warn("Unable to read Q2Min key from cross section spline; assuming 1 GeV^2");
		Q2Min=1;
	}
}

double I3CrossSection::GetMinimumEnergy() const{
	return(pow(10.,totalCrossSection.extents[0][0]));
}

double I3CrossSection::GetMaximumEnergy() const{
	return(pow(10.,totalCrossSection.extents[0][1]));
}
