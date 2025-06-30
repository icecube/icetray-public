/**
 *
 * Definition of SPEChargeDistribution class
 *
 * Copyright  (C) 2025 the IceCube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 * @version $Id$
 * @file SPEChargeDistribution.h
 * @date $Date$
 */

#ifndef SPECHARGEDISTRIBUTION_H_INCLUDED
#define SPECHARGEDISTRIBUTION_H_INCLUDED

#include <string>
#include <numeric>
#include <vector>
#include <cmath>

#include <icetray/serialization.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/external/CompareFloatingPoint.h>

#include <icetray/I3Units.h>
#include <icetray/OMKey.h>

static const unsigned SPEChargeDistribution_version_ = 3;
static const unsigned SPEChargeDistribution_PDF_version_ = 0;

/**
 *  @brief: A struct to hold the parameters of an combination of gaussians and exponentials
 *  fit to the SPE charge distribution:
 *  P(q) = sum_i(exp_amp_i * exp(-q/exp_width_i)) +
 *         sum_j(gaus_amp_j * np.exp(-0.5*(q - gaus_mean_j)**2/gaus_width_j**2))
 *
 * The Compensation Factor is simply the mean charge of the TA0003 distribution divided by the
 * average SPE Template charge.
 * CF = (dintt_0^inf q f(q)_ta0003 dq) / (dintt_0^inf q f(q)_spe dq)
 */
struct SPEChargeDistribution
{
  /******************************************************************
   * @brief: A basic structure representing a probability distribution.
   ******************************************************************/
  struct PDF{
    PDF() {}
    virtual ~PDF() {}
    template <class Archive> void serialize(Archive& ar, unsigned version);
    virtual double RelativeNormalization(){return 0;};
    virtual double Probability(double q){return 0;};
    virtual double Weight(double q){return 0;};
    virtual bool IsGaussian(){return false;}
    virtual bool IsExponential(){return false;}
    virtual bool IsValid(){return false;}
    virtual std::string Print(){return "";};
  };
  using PDFPtr = boost::shared_ptr<PDF>;

  /*******************************************************************
   * A Gaussian PDF in the context of the SPE templates.
   ******************************************************************/
  struct Gaussian : PDF{
    Gaussian(): amplitude_(NAN), mean_(NAN), sigma_(NAN) {}
    Gaussian(double amplitude, double mean, double sigma): amplitude_(amplitude), mean_(mean), sigma_(sigma) {}
    ~Gaussian() {};
    template <class Archive> void serialize(Archive& ar, unsigned version);
    double RelativeNormalization(){return amplitude_ * std::sqrt(2*I3Constants::pi) * sigma_;}
    double Probability(double q){
      return Weight(q) / RelativeNormalization();
    }
    double Weight(double q){
      double e = (q - mean_) / sigma_;
      return amplitude_ * exp(-.5 * e *e);
    }
    template<class rng> double Sample(boost::shared_ptr<rng> random) {return random->Gaus(mean_, sigma_);}
    bool IsGaussian(){return true;}
    bool IsExponential(){return false;}
    bool IsValid(){return !std::isnan(amplitude_) && !std::isnan(mean_) && !std::isnan(sigma_);}
    bool operator==(const Gaussian& rhs){
      return ((amplitude_==rhs.amplitude_) && (mean_==rhs.mean_) && (sigma_==rhs.sigma_));
    }
    std::string Print(){
      std::stringstream out;
      out << "Gaussian("
          << "Amp=" << amplitude_ << ", "
          << "Mean=" << mean_ << ", "
          << "Sigma=" << sigma_ << ")";
      return out.str();
    }
    double amplitude_;
    double mean_;
    double sigma_;
  };

  /*******************************************************************
   * An exponential PDF in the context of the templates
   ******************************************************************/
  struct Exponential : PDF{
    Exponential(): amplitude_(NAN), width_(NAN) {}
    Exponential(double amplitude, double width): amplitude_(amplitude), width_(width) {}
    ~Exponential() {}
    template <class Archive> void serialize(Archive& ar, unsigned version);
    double GetNorm() {return amplitude_;}
    double RelativeNormalization(){return amplitude_ * width_;}
    double Probability(double q){
      return Weight(q) / RelativeNormalization();
    }
    double Weight(double q){
      return amplitude_ * exp(-q/width_);
    }
    template<class rng> double Sample(boost::shared_ptr<rng> random) {return random->Exp(width_);}
    bool IsGaussian(){return false;}
    bool IsExponential(){return true;}
    bool IsValid(){return !std::isnan(amplitude_) && !std::isnan(width_);}
    bool operator==(const Exponential& rhs){return ((amplitude_==rhs.amplitude_) & (width_==rhs.width_));}
    std::string Print(){
      std::stringstream out;
      out << "Exponential("
          << "Amp=" << amplitude_ << ", "
          << "Width=" << width_ << ")";
      return out.str();
    }
    double amplitude_;
    double width_;
  };
  
  /******************************************************************
   * A visitor function for sampling from the PDFs using I3RandomService
   ******************************************************************/
  template<class rng>
  double SamplePDF(PDFPtr pdf, rng random) const{
    if(pdf->IsGaussian()){
      return dynamic_cast<Gaussian*>(pdf.get())->Sample(random);
    }
    else if(pdf->IsExponential()){
      return dynamic_cast<Exponential*>(pdf.get())->Sample(random);
    }
    else
      log_fatal_stream("Attempted to sample PDF " << pdf << " which is "
                       "neither Gaussian or Exponential.");
  };

  /******************************************************************
   * SPEChargeDistribution::Residuals
   * @brief A class to hold values for the "residuals" calculation
   ******************************************************************/
  struct Residuals{
    Residuals() : x(), y(), max_residual(NAN) {}
    Residuals(std::vector<double> x_arr,
              std::vector<double> y_arr): x(), y(), max_residual(NAN) {Set(x_arr, y_arr);}  
    template <class Archive> void serialize(Archive& ar, unsigned version);
    void Set(std::vector<double> x_arr,
             std::vector<double> y_arr){
      if(x_arr.size() != y_arr.size()){
        log_fatal("SPEChargeDistribution::Residuals objects require len(x_arr)=len(y_arr),"
                  " but len(x_arr)=%d and len(y_arr)=%d.", (uint) x_arr.size(), (uint) y_arr.size());
      }
      if(!std::is_sorted(x.begin(), x.end())){
        log_fatal("SPEChargeDistribution::Residuals objects require the input lists be sorted "
                  " in x (ie, charge).");
      }
      x = x_arr;
      y = y_arr;
      max_residual = NAN;
    }
    void SetPass2(){
      x.clear(); y.clear();
      x.reserve(x_pass2.size());
      y.reserve(y_pass2.size());
      x.assign(x_pass2.begin(), x_pass2.end());
      y.assign(y_pass2.begin(), y_pass2.end());
      max_residual = NAN;
    }    
    const std::vector<double> GetX(){ return x; }
    const std::vector<double> GetY(){ return y; }

    double GetMaxResidual(){ 
      if (x.size() == 0) return 1;
      if (std::isnan(max_residual)){
        max_residual = *std::max_element(y.begin(), y.end());
      }
      return max_residual;
    }
    double ComputeResidual(double q) const{
      // Special case: residuals not set.
      if (x.size() == 0){
        return 1;
      }
      // Special case: constant correction;      
      if (x.size() == 1){
        return y[0];
      }
      
      // find left end of interval for interpolation
      auto iter = std::lower_bound(x.begin(), x.end(), q); 

      // std::lower_bound gives the first x greater than q. We want
      // to back it up by one.
      int i = std::distance(x.begin(), iter);
      if (i>0){
        i--;
      }

      // If q > max(x), we need to back the iterator up further to extrapolate
      if (iter == x.end()){
        i--;
      }
      
      // Do a linear interpolation
      double xL = x[i],   yL = y[i];
      double xR = x[i+1], yR = y[i+1];
      double dydx = ( yR - yL ) / ( xR - xL );
      double y = yL + dydx * ( q - xL );
      return y;
    }

  private:
    friend class icecube::serialization::access;
    std::vector<double> x;
    std::vector<double> y;
    static const std::vector<double> x_pass2;
    static const std::vector<double> y_pass2;

    // We compute this at initialization, so it doesn't need to be stored during serialization
    double max_residual;
  };

  /******************************************************************
   * Now finally start building the SPE charge distribution itself.
   * This object will wrap a vector of PDFs and be used for calculating
   * expected charge and sampling.
   ******************************************************************/
  template <class Archive> void load(Archive& ar, unsigned version);
  template <class Archive> void save(Archive& ar, unsigned version) const;
  I3_SERIALIZATION_SPLIT_MEMBER();

  SPEChargeDistribution() : PDFs(),
                            compensation_factor(NAN),
                            fadc_charge_scale(NAN),
                            residuals(new Residuals()),
                            integral_ul(10),
                            mean_charge(NAN),
                            variance(NAN) {}

  SPEChargeDistribution(double amp_exp1,
                        double width_exp1,
                        double amp_exp2,
                        double width_exp2,
                        double amp_gaus,
                        double mean_gaus,
                        double width_gaus,
                        double factor_compensation,
                        double gaus_mean_SLC) :
    PDFs(),
    compensation_factor(factor_compensation),
    fadc_charge_scale(gaus_mean_SLC/mean_gaus),
    residuals(new Residuals()),
    integral_ul(10*mean_gaus),
    mean_charge(NAN),
    variance(NAN)
    {
      PDFs.push_back(boost::shared_ptr<Exponential>(new Exponential(amp_exp1, width_exp1)));
      PDFs.push_back(boost::shared_ptr<Exponential>(new Exponential(amp_exp2, width_exp2)));
      PDFs.push_back(boost::shared_ptr<Gaussian>(new Gaussian(amp_gaus, mean_gaus, width_gaus)));
      residuals->SetPass2();
    }

  bool IsValid() const
  {
    // consider valid only if no value is NaN
    bool valid = true;
    for(auto pdf: PDFs){
      valid &= pdf->IsValid();
    }
    return(valid &&
           !std::isnan(compensation_factor) &&
           !std::isnan(fadc_charge_scale));
  }

  std::string Print() const{
    std::string output;
    output   += "SPEChargeDistribution :: {\n";
    output   += "  PDFs:[\n";
    for(auto pdf: PDFs){
      output += "        " + pdf->Print() + "\n";
    }
    output   += "  ]\n";
    output   += "  Compensation Factor : " + std::to_string(compensation_factor) + "\n";
    if(!std::isnan(fadc_charge_scale)){
      output   += "  FADC Charge Scale : " + std::to_string(fadc_charge_scale) + "\n";
    }
    output   += "}" ;
    return output;
  }

  ///Evaluate the mean and standard deviation of the SPE template distribution
  double Mean() const
  {
    if(std::isnan(mean_charge)){
      mean_charge=ComputeMeanCharge();
    }
    return mean_charge;
  }
  double StdDev() const
  {
    if(std::isnan(variance)){
      variance=ComputeChargeVariance();
    }
    return std::sqrt(variance);
  }

  template <class rng>
  double Sample(unsigned int npe, boost::shared_ptr<rng> random) const{
    // Use gaussian sampling for high charges
    if(npe > 12){
      log_trace("Sampling %d, which is more than 12 PE. Going to try Gaussian sampling.", npe);
      double q = -1;
      while(q<0){
        q = random->Gaus(Mean() * npe, StdDev() * std::sqrt(npe));
      }
      return q;
    }

    // Otherwise directly sample from distributions
    // Find the probability of sampling from each component
    std::vector<double> probabilities = {};
    double running_total = 0;

    for(auto pdf: PDFs){
      running_total += pdf->RelativeNormalization();
      probabilities.push_back(running_total);
    }

    // Normalize
    std::transform(probabilities.begin(), probabilities.end(), probabilities.begin(),
                   [&running_total](double p){return p/running_total;});

    // Start sampling
    double sampledCharge = 0;
    for(unsigned int i=0; i<npe; ++i){
      while(true){
        double q = -1;
        auto pdf_idx = std::lower_bound(probabilities.begin(), probabilities.end(), random->Uniform(1));
        auto pdf = PDFs[std::distance(probabilities.begin(), pdf_idx)];

        // Ensure we have a positive charge
        while(q<0){
          q =  SamplePDF(pdf, random);
        }

        // Get the residual correction
        double correction = residuals->ComputeResidual(q);

        // Rejection sample to get the right corrections
        if(random->Uniform(1) < correction/residuals->GetMaxResidual()){
          sampledCharge += q;
          break;
        }
      }
      log_trace("Sampled %f PE at photoelectron %d of %d", sampledCharge, i, npe);
    }
    return sampledCharge;
  }

  bool vector_equality(const std::vector<PDFPtr>& a, const std::vector<PDFPtr>& b) const;
  bool operator==(const SPEChargeDistribution& rhs) const
  {
    return(vector_equality(PDFs, rhs.PDFs) &&
           CompareFloatingPoint::Compare_NanEqual(compensation_factor, rhs.compensation_factor) &&
           CompareFloatingPoint::Compare_NanEqual(fadc_charge_scale, rhs.fadc_charge_scale));
  }

  bool operator!=(const SPEChargeDistribution& rhs) const
  {
    return !(operator==(rhs));
  }

  double operator()(double q) const{
    double correction = residuals->ComputeResidual(q);
    double p = 0;
    for(auto pdf: PDFs){
      p += pdf->Weight(q);
    }
    return correction*p;
  }

public:
  std::vector<PDFPtr> PDFs;
  double compensation_factor;
  double fadc_charge_scale;

  boost::shared_ptr<Residuals> residuals;

  // These are derived lazily, so we don't store them during serialization.
  mutable double mean_charge;
  mutable double variance;

private:
  friend class icecube::serialization::access;
  double ComputeMeanCharge() const;
  double ComputeChargeVariance() const;
  double integral_ul;
};

I3_CLASS_VERSION(SPEChargeDistribution, SPEChargeDistribution_version_);
I3_CLASS_VERSION(SPEChargeDistribution::PDF, SPEChargeDistribution_PDF_version_);
I3_CLASS_VERSION(SPEChargeDistribution::Gaussian, SPEChargeDistribution_PDF_version_);
I3_CLASS_VERSION(SPEChargeDistribution::Exponential, SPEChargeDistribution_PDF_version_);

typedef std::map<OMKey, SPEChargeDistribution> SPEChargeDistributionMap;
I3_POINTER_TYPEDEFS(SPEChargeDistribution);

std::ostream& operator<<(std::ostream& oss, const SPEChargeDistributionMap& m);

#endif //I3DOMCALIBRATION_H_INCLUDED
