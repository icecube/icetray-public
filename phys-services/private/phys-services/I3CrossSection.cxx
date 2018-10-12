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
I3CrossSection::sampleFinalState(double energy, 
                               I3Particle::ParticleType scatteredType, 
                               boost::shared_ptr<I3RandomService> random) const{
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
	std::array<double,3> x, xp;
	std::array<int,3> cx, cxp;
	double px, pxp, propx, propxp;

	x[0] = xp[0] = log10(energy);

	//check preconditions
	if(x[0]<crossSection.extents[0][0]
	   || x[0]>crossSection.extents[0][1])
	log_fatal_stream("Interaction energy out of cross section table range: ["
	                 << pow(10.,crossSection.extents[0][0]) << " GeV,"
	                 << pow(10.,crossSection.extents[0][1]) << " GeV]");
	
	//sample an intial point
	do{
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ;
		do{
			x[1]=random->Uniform(logXMin,0);
			x[2]=random->Uniform(logYMin,logYMax);
			trialQ=(s-targetMass*targetMass)*pow(10.,x[1]+x[2]);
		}while(trialQ<Q2Min || !kinematicallyAllowed(pow(10.,x[1]),pow(10.,x[2]),energy,targetMass,m));

		accept=true;
		//sanity check: demand that the sampled point be within the table extents
		if(x[1]<crossSection.extents[1][0]
		   || x[1]>crossSection.extents[1][1])
			accept=false;
		if(x[2]<crossSection.extents[2][0]
		   || x[2]>crossSection.extents[2][1])
			accept=false;

		if(accept)
			accept=!tablesearchcenters(&crossSection,x.data(),cx.data());
	} while(!accept);

	propx=1; //TODO: better proposal distribution?
	double measure=pow(10.,x[1]+x[2]);
	px=measure*pow(10.,ndsplineeval(&crossSection,x.data(),cx.data(),0));

	const size_t burnin=40;
	for(size_t j=0; j<=burnin; j++){
		//rejection sample a point which is kinematically allowed by calculation limits
		double trialQ;
		do{
			xp[1]=random->Uniform(logXMin,0);
			xp[2]=random->Uniform(logYMin,logYMax);
			trialQ=(s-targetMass*targetMass)*pow(10.,xp[1]+xp[2]);
		}while(trialQ<Q2Min || !kinematicallyAllowed(pow(10.,xp[1]),pow(10.,xp[2]),energy,targetMass,m));

		accept=true;
		//sanity check: demand that the sampled point be within the table extents
		if(x[1]<crossSection.extents[1][0]
		   || x[1]>crossSection.extents[1][1])
			accept=false;
		if(x[2]<crossSection.extents[2][0]
		   || x[2]>crossSection.extents[2][1])
			accept=false;
		if(!accept)
			continue;

		accept=!tablesearchcenters(&crossSection,xp.data(),cxp.data());
		if(!accept)
			continue;
		
		propxp=1;
		double measure=pow(10.,xp[1]+xp[2]);
		double eval=ndsplineeval(&crossSection,xp.data(),cxp.data(),0);
		if(std::isnan(eval))
			continue;
		pxp=measure*pow(10.,eval);

		double odds=(pxp/px)*(propx/propxp);
		accept=((odds>1.) || random->Uniform(0,1)<odds);

		if(accept){
			x=xp;
			propx=propxp;
			px=pxp;
		}
	}
	//record whatever we sampled
	//log_info_stream(" lx=" << x[1] << " ly=" << x[2]);
	return(finalStateRecord(pow(10.,x[1]),pow(10.,x[2])));
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
	if(crossSection.ndim!=3)
		log_fatal_stream("cross section spline has " << crossSection.ndim
		                 << " dimensions, should have 3 (log10(E), log10(x), log10(y))");
	
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
		log_warn("Unable to read TARGETMASS key from cross section spline; assuming an isoscalar mass");
		targetMass=(particleMass(I3Particle::PPlus)+
		            particleMass(I3Particle::Neutron))/2;
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
