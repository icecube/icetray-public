#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <phys-services/I3RandomService.h>

// Functions to bridge to GSL
static unsigned long
i3random_get(void *state)
{
        return ((I3RandomService*)(state))->Integer(INT_MAX);
}

static void
i3random_set(void *state, unsigned long int s)
{
	// Don't actually seed the RNG (hacky)
        return;
}

static double
i3random_get_double(void *state)
{
        return (((I3RandomService*)(state))->Uniform());
}

#if BOOST_VERSION<104700
//Although this urng does have a fixed range, old versions of boost will go
//looking for things that we don't support if this flag isn't false
const bool I3RandomService::has_fixed_range=false;
#endif

I3RandomService::I3RandomService()
{
	gsl_rng_type *type = new gsl_rng_type;

	type->name = typeid(*this).name();
	type->max = INT_MAX;
	type->min = 0;
	type->size = sizeof(I3RandomService *);
	type->set = i3random_set;
	type->get = i3random_get;
	type->get_double = i3random_get_double;

	gsl_ = new gsl_rng_wrap;
	gsl_->type = type;
	gsl_->state = this;
}

I3RandomService::~I3RandomService()
{
	delete gsl_->type;
	delete gsl_;
}

int I3RandomService::Binomial(int ntot, double prob)
{
	return gsl_ran_binomial(gsl_, prob, ntot);
}

double I3RandomService::Exp(double tau)
{
	return gsl_ran_exponential(gsl_, tau);
}

unsigned int I3RandomService::Integer(unsigned int imax)
{
	return (unsigned int)Uniform(imax);
}

int I3RandomService::Poisson(double mean)
{
	return gsl_ran_poisson(gsl_, mean);
}

double I3RandomService::PoissonD(double mean)
{
	return gsl_ran_poisson(gsl_, mean);
}

double I3RandomService::Gaus(double mean, double stddev)
{
	return mean + gsl_ran_gaussian(gsl_, stddev);
}

double I3RandomService::Uniform(double x2)
{
	return Uniform(0, x2);
}

const gsl_rng_wrap *
I3RandomService::GSLRng() const
{
	return gsl_;
}

