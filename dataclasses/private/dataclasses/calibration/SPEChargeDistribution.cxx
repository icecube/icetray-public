// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <map>
#include <dataclasses/calibration/SPEChargeDistribution.h>
#include <icetray/I3Units.h>

#include <serialization/export.hpp>
#include <serialization/array.hpp>
#include <gsl/gsl_integration.h>

namespace GSL{  
  ///\brief Compute a one-dimensional integral using GSL.
  ///\param f Function to integrate.
  ///\param a Lower integration limit.
  ///\param b Upper integration limit.
  ///\param rtol Target relative tolerance.
  ///\param max_iter Maximum number of iterations to perform the integral.
  ///\param memory_alloc Number of intervals of scratch space to allocate.
  template<typename FunctionType>
  double integrate(FunctionType&& f,
                   double a,
                   double b,
                   double rtol=1e-7,
                   unsigned int max_iter=10000,
                   size_t memory_alloc=10000){
    using IntegrateWorkspace=std::unique_ptr<gsl_integration_workspace, void(*)(gsl_integration_workspace*)>;
    IntegrateWorkspace ws(gsl_integration_workspace_alloc(memory_alloc), &gsl_integration_workspace_free);

    using FPtr=decltype(&f);
    double (*wrapper)(double,void*)=[](double x, void* params)->double{
      auto& f=*static_cast<FPtr>(params);
      return(f(x));
    };

    double result, error;
    gsl_function F;
    F.function = wrapper;
    F.params = const_cast<typename std::remove_const<typename std::remove_reference<decltype(f)>::type>::type*>(&f);

    gsl_integration_qag(&F, a, b, 0, rtol, max_iter, GSL_INTEG_GAUSS15, ws.get(), &result, &error);

    return(result);
  }

} //namespace GSL

template <class Archive>
void SPEChargeDistribution::PDF::serialize(Archive& ar, unsigned version)
{}
I3_SERIALIZABLE(SPEChargeDistribution::PDF)

template <class Archive>
void SPEChargeDistribution::Gaussian::serialize(Archive& ar, unsigned version)
{
  icecube::serialization::void_cast_register<SPEChargeDistribution::Gaussian, SPEChargeDistribution::PDF>(
    static_cast<SPEChargeDistribution::Gaussian*>(NULL),
    static_cast<SPEChargeDistribution::PDF*>(NULL));
  ar & make_nvp("Amplitude", amplitude_);
  ar & make_nvp("Mean", mean_);
  ar & make_nvp("Sigma", sigma_);
}
I3_SERIALIZABLE(SPEChargeDistribution::Gaussian);

template <class Archive>
void SPEChargeDistribution::Exponential::serialize(Archive& ar, unsigned version)
{
  icecube::serialization::void_cast_register<SPEChargeDistribution::Exponential, SPEChargeDistribution::PDF>(
    static_cast<SPEChargeDistribution::Exponential*>(NULL),
    static_cast<SPEChargeDistribution::PDF*>(NULL));
  ar & make_nvp("Amplitude", amplitude_);
  ar & make_nvp("Width", width_);
}
I3_SERIALIZABLE(SPEChargeDistribution::Exponential);

template <class Archive>
void SPEChargeDistribution::Residuals::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("x", x);
  ar & make_nvp("y", y);
}
I3_SERIALIZABLE(SPEChargeDistribution::Residuals);

template <class Archive>
void
SPEChargeDistribution::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("PDFs", PDFs);
  ar & make_nvp("CompensationFactor", compensation_factor);
  ar & make_nvp("FADCChargeScale", fadc_charge_scale);
  ar & make_nvp("residuals", residuals);
  ar & make_nvp("integral_ul", integral_ul);
}

template <class Archive>
void
SPEChargeDistribution::load(Archive& ar, unsigned version)
{
  if (version>SPEChargeDistribution_version_)
    log_fatal("Attempting to read version %u from file but running version %u of SPEChargeDistribution class.",version,SPEChargeDistribution_version_);

  double exp1_amp, exp1_width;
  double exp2_amp, exp2_width;
  double gaus_amp, gaus_width;
  double atwd_gaus_mean, fadc_gaus_mean;

  if (version == 1){
    ar & make_nvp("ExpAmp", exp1_amp);
    ar & make_nvp("ExpWidth", exp1_width);
    ar & make_nvp("GausAmp", gaus_amp);
    ar & make_nvp("GausMean", atwd_gaus_mean);
    ar & make_nvp("GausWidth", gaus_width);
    PDFs.push_back(boost::shared_ptr<Exponential>(new Exponential(exp1_amp, exp1_width)));
    PDFs.push_back(boost::shared_ptr<Gaussian>(new Gaussian(gaus_amp, atwd_gaus_mean, gaus_width)));

    compensation_factor  = 1.;
    fadc_charge_scale = 1.;
    integral_ul = 10. * atwd_gaus_mean;
    
    // No residuals here, so just leave it with the default constructor.
  }
  else if (version == 2){
    ar & make_nvp("Exp2Amp", exp2_amp);
    ar & make_nvp("Exp2Width", exp2_width);
    ar & make_nvp("Exp1Amp", exp1_amp);
    ar & make_nvp("Exp1Width", exp1_width);
    ar & make_nvp("CompensationFactor", compensation_factor);
    ar & make_nvp("SLCGausMean", fadc_gaus_mean);
    ar & make_nvp("GausAmp", gaus_amp);
    ar & make_nvp("GausMean", atwd_gaus_mean);
    ar & make_nvp("GausWidth", gaus_width);
    PDFs.push_back(boost::shared_ptr<Exponential>(new Exponential(exp1_amp, exp1_width)));
    PDFs.push_back(boost::shared_ptr<Exponential>(new Exponential(exp2_amp, exp2_width)));
    PDFs.push_back(boost::shared_ptr<Gaussian>(new Gaussian(gaus_amp, atwd_gaus_mean, gaus_width)));
    fadc_charge_scale = fadc_gaus_mean / atwd_gaus_mean;
    integral_ul = 10. * atwd_gaus_mean;

    // Pass2 residuals here.
    residuals->SetPass2();
  }
  if (version == 3){
    ar & make_nvp("PDFs", PDFs);
    ar & make_nvp("CompensationFactor", compensation_factor);
    ar & make_nvp("FADCChargeScale", fadc_charge_scale);
    ar & make_nvp("residuals", residuals);
    ar & make_nvp("integral_ul", integral_ul);
  }

}
I3_SPLIT_SERIALIZABLE(SPEChargeDistribution);

double SPEChargeDistribution::ComputeMeanCharge() const{
  //Integral to calculate the mean expected charge from the distribution
  //This might seems like a very loose tolerance, but the GSL integrator tends to
  //fail for any tighter setting.
  //However, it also tends to substantially overestimate its error, at least for
  //these functions, as checking all charge distributions in
  //GeoCalibDetectorStatus_2020.Run134142.Pass2_V0.i3.gz against another, less touchy
  //algorithm indicates that it produces an average relative error in the computed
  //mean of only 4.2e-06, and the worst observed error is 2.4e-4.
  const double int_tol = 1e-3;
  double norm = GSL::integrate(*this, 0, integral_ul, int_tol);
  auto moment_integrand = [this](double x){ return x*this->operator()(x); };
  double mom1 = GSL::integrate(moment_integrand, 0, integral_ul, int_tol);
  return mom1/norm;
}

double SPEChargeDistribution::ComputeChargeVariance() const{
  // Tolerance here is finicky. Tighter tolerances (< 1e-4) run into roundoff error,
  // which leads to GSL errors. Looser tolerances (> 1e-2) return values
  // close to 0. This tolerance seems to be around the only usable region.
  // Tests with a few DOMs show errors of around 1e-5 when compared to directly
  // sampling from the distributions (like in DOMLauncher::PMT's SampleCharge).
  // This is well within the tolerance requirements needed for our purposes.
  const double int_tol = 1e-3;
  double norm = GSL::integrate(*this, 0, integral_ul, int_tol);
  auto moment_integrand = [this](double x){ return x*x*this->operator()(x); };
  double mom2 = GSL::integrate(moment_integrand, 0, integral_ul, int_tol);
  return mom2/norm - std::pow(Mean(), 2);
}

std::ostream& operator<<(std::ostream& oss, const SPEChargeDistribution& spe)
{
  oss << spe.Print();
  return oss;
}

using Gaussian  = SPEChargeDistribution::Gaussian;
using Exponential = SPEChargeDistribution::Exponential;
using PDFPtr = boost::shared_ptr<SPEChargeDistribution::PDF>;
static bool element_equality(const PDFPtr& a, const PDFPtr& b){
  bool equal = true;
  if(typeid(a)!=typeid(b))
    equal = false;
  else if(a->IsGaussian()){
    Gaussian* gaus_a = dynamic_cast<Gaussian*>(a.get());
    Gaussian* gaus_b = dynamic_cast<Gaussian*>(b.get());
    equal = *gaus_a==*gaus_b;
  }
  else{
    Exponential* expo_a = dynamic_cast<Exponential*>(a.get());
    Exponential* expo_b = dynamic_cast<Exponential*>(b.get());
    equal = *expo_a==*expo_b;
  }
  return equal;
}

bool SPEChargeDistribution::vector_equality(const std::vector<PDFPtr>& a,
                                            const std::vector<PDFPtr>& b) const{
  if (a.size() != b.size()){
    return false;
  }
  return std::equal(a.begin(), a.end(), b.begin(), element_equality);
}

const std::vector<double> SPEChargeDistribution::Residuals::x_pass2 = {0.0, 0.0025, 0.0125, 0.0225, 0.0325, 0.042499999999999996,
                                                                       0.052500000000000005, 0.0625, 0.07250000000000001, 0.0825,
                                                                       0.0925, 0.10250000000000001, 0.1125, 0.1225, 0.1325, 0.14250000000000002,
                                                                       0.1525, 0.1625, 0.17250000000000001, 0.1825, 0.1925, 0.2025, 0.2125,
                                                                       0.2225, 0.2325, 0.2425, 0.2525, 0.2625, 0.2725, 0.28250000000000003,
                                                                       0.2925, 0.3025, 0.3125, 0.3225, 0.3325, 0.3425, 0.35250000000000004,
                                                                       0.3625, 0.3725, 0.3825, 0.3925, 0.4025, 0.41250000000000003, 0.4225,
                                                                       0.4325, 0.4425, 0.4525, 0.4625, 0.47250000000000003, 0.4825, 0.4925,
                                                                       0.5025, 0.5125, 0.5225, 0.5325, 0.5425, 0.5525, 0.5625, 0.5725, 0.5825,
                                                                       0.5925, 0.6025, 0.6125, 0.6225, 0.6325000000000001, 0.6425000000000001,
                                                                       0.6525000000000001, 0.6625000000000001, 0.6725000000000001, 0.6825000000000001,
                                                                       0.6925000000000001, 0.7025, 0.7124999999999999, 0.7224999999999999,
                                                                       0.7324999999999999, 0.7424999999999999, 0.7525, 0.7625, 0.7725, 0.7825,
                                                                       0.7925, 0.8025, 0.8125, 0.8225, 0.8325, 0.8425, 0.8525, 0.8625, 0.8725,
                                                                       0.8825000000000001, 0.8925000000000001, 0.9025000000000001, 0.9125000000000001,
                                                                       0.9225000000000001, 0.9325000000000001, 0.9425000000000001, 0.9525000000000001,
                                                                       0.9624999999999999, 0.9724999999999999, 0.9824999999999999, 0.9924999999999999, 5.0};

const std::vector<double> SPEChargeDistribution::Residuals::y_pass2 = {1.0, 1.0, 1.0, 1.0,
                                                                       1.0, 1.0, 1.0, 1.0,
                                                                       1.0, 1.0, 1.0, 1.0,
                                                                       1.0, 1.0, 1.0, 1.0,
                                                                       1.0078185220290956, 1.0042299812007673, 0.9935908344034875, 0.9850454543557308,
                                                                       0.9726915954383835, 0.9697828027792773, 0.9652743986792529, 0.9678607318783602,
                                                                       0.9710489835385085, 0.9779325024066567, 0.9851805409942587, 0.9917270505917856,
                                                                       0.9996406452735341, 1.0068567044292451, 1.017735687531788, 1.0267751591351273,
                                                                       1.0370993434166467, 1.044744857399972, 1.0573247280549134, 1.0625672697852353,
                                                                       1.0746265958307744, 1.0811289116634608, 1.0895461845600405, 1.0969954086913443,
                                                                       1.102224227160381, 1.1080791739726892, 1.1124879641850856, 1.1167398762419734,
                                                                       1.1191118871969707, 1.1225845994808588, 1.1228561156113273, 1.1214733538285444,
                                                                       1.1201466033159877, 1.119017891198713, 1.117034774252846, 1.114784420031669,
                                                                       1.1106707843428119, 1.1046761249652928, 1.1004126244552774, 1.0971385151354633,
                                                                       1.0917037242585172, 1.0851977267453712, 1.0803033012303216, 1.072758138133644,
                                                                       1.067524998209676, 1.0625850059075657, 1.0555095359234306, 1.0509622181928957,
                                                                       1.0440128255959997, 1.0395139814809693, 1.0354914927244894, 1.0310352119539496,
                                                                       1.0267994142473922, 1.0218812500846572, 1.01668830547869, 1.0147294646372143,
                                                                       1.0109961001746626, 1.0088071581605818, 1.0056045189846852, 1.0042154120061508,
                                                                       1.0010489798770452, 0.9986921110064382, 0.9988728971422061, 0.9970854046567016,
                                                                       0.9968106821162841, 0.9963693247219644, 0.9941273798848427, 0.9958665014503302,
                                                                       0.9949889009733639, 0.9959272341615341, 0.9955380608193667, 0.996792440853806,
                                                                       0.9961095310945263, 0.9961005008737484, 0.9973811405595548, 0.9988258893905738,
                                                                       0.9980287475191254, 0.9991263792520673, 1.000596123699185, 1.0017720700471255,
                                                                       1.0030773292152362, 1.0016553887740836, 1.0026409396829936, 1.0025133670051922,
                                                                       1.0035085128801953, 1.0};
