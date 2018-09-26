#include <cstdio>

#include "I3Test.h"

#include "icetray/OMKey.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "phys-services/geo-selector/I3GeoTrimmers.h"

// Create a phony geometry
static I3OMGeoMapPtr geo() {
  I3OMGeoMapPtr g = I3OMGeoMapPtr(new I3OMGeoMap());
  OMKey dom1(1,1);
  OMKey dom2(1,2);
  OMKey dom3(1,3);
  OMKey dom4(1,4);
  OMKey dom5(2,4);
  OMKey dom6(2,5);
  OMKey dom7(2,61);
  OMKey dom8(2,62);
  OMKey dom9(2,63);
  (*g)[dom1].position=I3Position(0,0,0,I3Position::car);
  (*g)[dom2].position=I3Position(0,0,50,I3Position::car);
  (*g)[dom3].position=I3Position(0,0,100,I3Position::car);
  (*g)[dom4].position=I3Position(0,0,500,I3Position::car);
  (*g)[dom5].position=I3Position(0,50,500,I3Position::car);
  (*g)[dom6].position=I3Position(20,50,500,I3Position::car);
  (*g)[dom7].position=I3Position(20,50,400,I3Position::car);
  (*g)[dom8].position=I3Position(20,50,300,I3Position::car);
  (*g)[dom9].position=I3Position(20,50,200,I3Position::car);
  return g;
}

static I3RecoPulseSeriesMapPtr rpsm() {
  // Create a phony pulse series
  // Hits on DOM's 1-5 only
  I3RecoPulseSeriesMapPtr rp(new I3RecoPulseSeriesMap);
  I3RecoPulse p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
  p0.SetTime(1650);   p0.SetWidth(100);  //close to #1       //coincidence with #1
  p1.SetTime(1850);   p1.SetWidth(100);  //close to #0       //coincidence with #0
  p2.SetTime(0);      p2.SetWidth(100);  //close to #3       //coincidence with #3
  p3.SetTime(550);    p3.SetWidth(0);    //close to #2, #4   //coincidence with #2, #4
  p4.SetTime(800);    p4.SetWidth(0);    //close to #3       //coincidence with #3
  p5.SetTime(3500);   p5.SetWidth(100);  //isolated          //coincidence with #6, #7
  p6.SetTime(3500);   p6.SetWidth(100);  //close to #7, #8   //coincidence with #5, #7
  p7.SetTime(3500);   p7.SetWidth(100);  //close to #6, #8   //coincidence with #5, #6
  p8.SetTime(3500);   p8.SetWidth(100);  //close to #6, #7   //isolated
  p9.SetTime(8000);   p9.SetWidth(3000); //close to #10, #11 //coincidence with #10, #11
  p10.SetTime(9000);  p10.SetWidth(100); //close to #9, #11  //coincidence with #9, #11
  p11.SetTime(9500);  p11.SetWidth(100); //close to #9, #10  //coincidence with #9, #10
  OMKey dom1(1,1);
  OMKey dom2(1,2);
  OMKey dom3(2,63);
  OMKey dom4(1,4);
  OMKey dom5(2,4);
  (*rp)[dom1].push_back(p0);
  (*rp)[dom1].push_back(p1);
  (*rp)[dom1].push_back(p2);
  (*rp)[dom2].push_back(p3);
  (*rp)[dom3].push_back(p4);
  (*rp)[dom3].push_back(p5);
  (*rp)[dom4].push_back(p6);
  (*rp)[dom4].push_back(p7);
  (*rp)[dom5].push_back(p8);
  (*rp)[dom5].push_back(p9);
  (*rp)[dom5].push_back(p10);
  (*rp)[dom5].push_back(p11);
  return rp;
}

static I3VectorOMKey badoms() {
  // Create a phony bad OM list
  I3VectorOMKey bad;
  bad.push_back(OMKey(1,3));
  bad.push_back(OMKey(2,5));
  bad.push_back(OMKey(2,62));
  return bad;
}

TEST_GROUP(GeoTrimmers)

TEST(FromRecoPulses)
{
  I3OMGeoMapPtr g = geo();
  I3RecoPulseSeriesMapPtr rp = rpsm();
  I3OMGeoMapPtr result = I3GeoTrimmers::GeoFromPulseSeries(*g, *rp);
  int sz = (int)result->size();
  ENSURE_EQUAL(sz,5,"Wrong number of OM's");
  ENSURE(result->find(OMKey(1,1)) != result->end(), "1-1 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,2)) != result->end(), "1-2 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,63)) != result->end(), "2-63 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,4)) != result->end(), "1-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,4)) != result->end(), "2-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,5)) == result->end(), "2-5 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,61)) == result->end(), "2-61 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,62)) == result->end(), "2-62 Exists but shouldn't");
  ENSURE(result->find(OMKey(1,3)) == result->end(), "1-3 Exists but shouldn't");
}

TEST(FromRecoPulsesWithNeighbors)
{
  I3OMGeoMapPtr g = geo();
  I3RecoPulseSeriesMapPtr rp = rpsm();
  I3OMGeoMapPtr result = I3GeoTrimmers::GeoFromPulseSeriesWithNeighbors(*g, *rp);
  int sz = (int)result->size();
  ENSURE_EQUAL(sz,8,"Wrong number of OM's");
  ENSURE(result->find(OMKey(1,1)) != result->end(), "1-1 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,2)) != result->end(), "1-2 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,3)) != result->end(), "1-3 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,4)) != result->end(), "1-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,4)) != result->end(), "2-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,5)) != result->end(), "2-5 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,61)) == result->end(), "2-61 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,62)) != result->end(), "2-62 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,63)) != result->end(), "2-63 Doesn't exist but should");
}

TEST(NoBadOMs)
{
  I3OMGeoMapPtr g = geo();
  I3VectorOMKey bad = badoms();
  I3OMGeoMapPtr result = I3GeoTrimmers::GeoWithoutBadOMs(*g, bad);
  int sz = (int)result->size();
  ENSURE_EQUAL(sz,6,"Wrong number of OM's");
  ENSURE(result->find(OMKey(1,1)) != result->end(), "1-1 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,2)) != result->end(), "1-2 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,3)) == result->end(), "1-3 Exists but shouldn't");
  ENSURE(result->find(OMKey(1,4)) != result->end(), "1-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,4)) != result->end(), "2-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,5)) == result->end(), "2-5 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,61)) != result->end(), "2-61 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,62)) == result->end(), "2-62 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,63)) != result->end(), "2-63 Doesn't exist but should");

}

TEST(NoIceTop)
{
  I3OMGeoMapPtr g = geo();
  I3OMGeoMapPtr result = I3GeoTrimmers::GeoWithoutIceTop(*g);
  int sz = (int)result->size();
  ENSURE_EQUAL(sz,6,"Wrong number of OM's");
  ENSURE(result->find(OMKey(1,1)) != result->end(), "1-1 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,2)) != result->end(), "1-2 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,3)) != result->end(), "1-3 Doesn't exist but should");
  ENSURE(result->find(OMKey(1,4)) != result->end(), "1-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,4)) != result->end(), "2-4 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,5)) != result->end(), "2-5 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,61)) == result->end(), "2-61 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,62)) == result->end(), "2-62 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,63)) == result->end(), "2-63 Exists but shouldn't");

}

TEST(AllIceTop)
{
  I3OMGeoMapPtr g = geo();
  I3OMGeoMapPtr result = I3GeoTrimmers::GeoIceTopOnly(*g);
  int sz = (int)result->size();
  ENSURE_EQUAL(sz,3,"Wrong number of OM's");
  ENSURE(result->find(OMKey(1,1)) == result->end(), "1-1 Exists but shouldn't");
  ENSURE(result->find(OMKey(1,2)) == result->end(), "1-2 Exists but shouldn't");
  ENSURE(result->find(OMKey(1,3)) == result->end(), "1-3 Exists but shouldn't");
  ENSURE(result->find(OMKey(1,4)) == result->end(), "1-4 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,4)) == result->end(), "2-4 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,5)) == result->end(), "2-5 Exists but shouldn't");
  ENSURE(result->find(OMKey(2,61)) != result->end(), "2-61 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,62)) != result->end(), "2-62 Doesn't exist but should");
  ENSURE(result->find(OMKey(2,63)) != result->end(), "2-63 Doesn't exist but should");

}

TEST(Combo1)
{
  I3OMGeoMapPtr g = geo();
  I3RecoPulseSeriesMapPtr rp = rpsm();
  I3VectorOMKey bad = badoms();
  I3OMGeoMapPtr r1 = I3GeoTrimmers::GeoWithoutBadOMs(*g,bad);
  I3OMGeoMapPtr r2 = I3GeoTrimmers::GeoFromPulseSeries(*r1,*rp);
  int sz = (int)r2->size();
  ENSURE_EQUAL(sz,5,"Wrong number of OM's");
  ENSURE(r2->find(OMKey(1,1)) != r2->end(), "1-1 Doesn't exist but should");
  ENSURE(r2->find(OMKey(1,2)) != r2->end(), "1-2 Doesn't exist but should");
  ENSURE(r2->find(OMKey(1,3)) == r2->end(), "1-3 Exists but shouldn't");
  ENSURE(r2->find(OMKey(1,4)) != r2->end(), "1-4 Doesn't exist but should");
  ENSURE(r2->find(OMKey(2,4)) != r2->end(), "2-4 Doesn't exist but should");
  ENSURE(r2->find(OMKey(2,5)) == r2->end(), "2-5 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,61)) == r2->end(), "2-61 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,62)) == r2->end(), "2-62 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,63)) != r2->end(), "2-63 Doesn't exist but should");

}

TEST(Combo2)
{
  I3OMGeoMapPtr g = geo();
  I3RecoPulseSeriesMapPtr rp = rpsm();
  I3VectorOMKey bad = badoms();
  I3OMGeoMapPtr r1 = I3GeoTrimmers::GeoFromPulseSeries(*g,*rp);
  I3OMGeoMapPtr r2 = I3GeoTrimmers::GeoWithoutBadOMs(*r1,bad);
  int sz = (int)r2->size();
  ENSURE_EQUAL(sz,5,"Wrong number of OM's");
  ENSURE(r2->find(OMKey(1,1)) != r2->end(), "1-1 Doesn't exist but should");
  ENSURE(r2->find(OMKey(1,2)) != r2->end(), "1-2 Doesn't exist but should");
  ENSURE(r2->find(OMKey(1,3)) == r2->end(), "1-3 Exists but shouldn't");
  ENSURE(r2->find(OMKey(1,4)) != r2->end(), "1-4 Doesn't exist but should");
  ENSURE(r2->find(OMKey(2,4)) != r2->end(), "2-4 Doesn't exist but should");
  ENSURE(r2->find(OMKey(2,5)) == r2->end(), "2-5 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,61)) == r2->end(), "2-61 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,62)) == r2->end(), "2-62 Exists but shouldn't");
  ENSURE(r2->find(OMKey(2,63)) != r2->end(), "2-63 Doesn't exist but should");

}

TEST(MinimumBall2point)
{
    // build a fake geometry with one string
    int stringnr = 1;
    I3OMGeoMap geomap;
    for ( int omnr = 1; omnr<101; ++omnr ){
        OMKey omkey(stringnr,omnr);
        I3OMGeo omgeo;
        omgeo.position=I3Position(0.,0.,1.0*omnr,I3Position::car);
        geomap.insert(std::make_pair(OMKey(stringnr,omnr),omgeo));
    }

    // event data: two hits and some inbetween
    I3RecoPulse pulse;
    I3RecoPulseSeries pulseseries(1,pulse);;
    I3RecoPulseSeriesMap pulsemap;
    int omlo=30;
    int omhi=70;
    OMKey omkey1(stringnr,omlo);
    OMKey omkey2(stringnr,omhi);
    pulsemap.insert( make_pair( omkey1, pulseseries) );
    pulsemap.insert( make_pair( omkey2, pulseseries) );
    for ( int omnr = omlo+2; omnr<omhi; omnr += 3 ){
        OMKey omkey(stringnr,omnr);
        pulsemap.insert(make_pair(omkey,pulseseries));
    }

    // select OMs between hits (inclusive)
    I3OMGeoMapPtr ballmapptr =
        I3GeoTrimmers::GetMinBallGeometryFromData( geomap, pulsemap, 0.1 );

    // should be easy
    unsigned int Nexpected = omhi-omlo+1;
    ENSURE( ballmapptr->size() == Nexpected, "wrong number of selected DOMs" );

    // check all DOMs
    for ( int omnr = 1; omnr<101; ++omnr ){
        OMKey omkey(stringnr,omnr);
        bool outside = ( ballmapptr->find(omkey) == ballmapptr->end());
        bool inside = !outside;
        if ( (omnr < omlo) || (omnr > omhi) ){
            ENSURE( outside, "wrong DOM selected" );
        } else {
            ENSURE( inside, "missed a good DOM" );
        }
    }
}

TEST(MinimumBall3point)
{
    // build a fake rectangular geometry with 81 strings
    I3OMGeoMap geomap;

    const int nx=9;
    const int ny=9;
    const int nz=60;
    for ( int ix = 0; ix <nx; ++ix ){
        for ( int iy = 0; iy <ny; ++iy ){
            for ( int iz = 0; iz<nz; ++iz ){
                int stringnr = 1 + ny*ix + iy;
                int omnr = iz + 1;
                OMKey omkey(stringnr,omnr);
                I3OMGeo omgeo;
                omgeo.position=I3Position(ix,iy,iz,I3Position::car);
                geomap.insert(std::make_pair(OMKey(stringnr,omnr),omgeo));
            }
        }
    }
    log_info("created geomap with(%d*%d*%d  DOMs",nx,ny,nz);

    // event data: three outer hits A, B and C, aranged like a rectangular
    // triangle. The center of the minimum bounding circle
    // lies on the hypothenuse AC. We loop over various values of the
    // z-coordinate of the tip of the triangle, starting with a small
    // enclosed ball and increasing to covering more than half of the
    // "detector".

    // coordinates
    int ixA = 3; int iyA = 0; int izA = 22;
    int ixB = 6; int iyB = 3; int izB = 22;
    int ixC = 6; int iyC = 3;
    
    
    for ( int izC = 28; izC <= 50; izC += 2 ){

        // OM keys (string, omnr)
        OMKey omkeyA( 1 + ny*ixA + iyA, izA+1 );
        OMKey omkeyB( 1 + ny*ixB + iyB, izB+1 );
        OMKey omkeyC( 1 + ny*ixC + iyC, izC+1 );

        // center point M of the enclosing ball
        double xM = ixA + 0.5*(ixC-ixA);
        double yM = iyA + 0.5*(iyC-iyA);
        double zM = izA + 0.5*(izC-izA);

        // maybe add a small number to guard against rounding issues
        // (or choose example such that everything is integer)
        double rA2 = (ixA-xM)*(ixA-xM) +
                     (iyA-yM)*(iyA-yM) +
                     (izA-zM)*(izA-zM);
        double rB2 = (ixB-xM)*(ixB-xM) +
                     (iyB-yM)*(iyB-yM) +
                     (izC-zM)*(izC-zM);
        double radius2 = (ixC-xM)*(ixC-xM) +
                         (iyC-yM)*(iyC-yM) +
                         (izC-zM)*(izC-zM);
        ENSURE_DISTANCE( rA2,radius2,0.0001,"rA2!=rC2");
        ENSURE_DISTANCE( rB2,radius2,0.0001,"rB2!=rC2");
        ENSURE_DISTANCE( rA2,rB2,    0.0001,"rA2!=rB2");

        double margin = 0.1;
        double radius = sqrt(radius2);
        double rm = radius + margin;
        double rm2 = rm*rm;
        log_info( "M(%.1f,%.1f,%.1f), r=%.2f, r2=%.2f, rm=%.2f, rm2=%.2f",
                  xM, yM, zM, radius, radius2, rm, rm2);
        radius2 = rm2;

        // add the three hits to the pulsemap
        I3RecoPulse pulse;
        I3RecoPulseSeries pulseseries(1,pulse);;
        I3RecoPulseSeriesMap pulsemap;
        pulsemap.insert( make_pair( omkeyA, pulseseries) );
        pulsemap.insert( make_pair( omkeyB, pulseseries) );
        pulsemap.insert( make_pair( omkeyC, pulseseries) );

        // manually count included DOMs and add "random" hits to the pulsemap
        unsigned int nsel = 0;
        for ( int ix = 0; ix <nx; ++ix ){
            for ( int iy = 0; iy <ny; ++iy ){
                for ( int iz = 0; iz<nz; ++iz ){
                    double dx = ix-xM;
                    double dy = iy-yM;
                    double dz = iz-zM;
                    double r2 = dx*dx + dy*dy + dz*dz;
                    if ( r2 > radius2 ) continue;
                    nsel++;
                    // continue;
                    // For a pseudorandom subset, add hits.
                    // Points ABC should be the outer points,
                    // so keep other hits within a smaller radius.
                    if ( r2 > 0.9 * radius2 ) continue;
                    int dom = ix + ny*ix + iz*nx*ny;
                    if ( ( (dom%5) == 1 ) || ( (dom%7) == 3 ) ){
                        int stringnr = 1 + ny*ix + iy;
                        int omnr = iz + 1;
                        OMKey omkey(stringnr,omnr);
                        pulsemap.insert(make_pair(omkey,pulseseries));
                    }
                }
            }
        }
        log_info("nsel=%u",nsel);

        // find ball and select OMs between hits (inclusive)
        I3OMGeoMapPtr ballmapptr =
            I3GeoTrimmers::GetMinBallGeometryFromData(
                    geomap, pulsemap, margin );

        // check total number of selected DOMs
        char msg[256]; // sorry
        size_t nball = ballmapptr->size();
        log_info( "got %zu selected DOMs, expected %u", nball, nsel );
        sprintf(msg,"got %zu selected DOMs, expected %u", nball, nsel );
        ENSURE( nball == nsel, msg );

        // check all DOMs individually
        for ( int ix = 0; ix<nx; ++ix ){
            for ( int iy = 0; iy<ny; ++iy ){
                for ( int iz = 0; iz<nz; ++iz ){
                    double dx = ix-xM;
                    double dy = iy-yM;
                    double dz = iz-zM;
                    double r2 = dx*dx + dy*dy + dz*dz;
                    bool should_be_inside = (r2<=radius2);
                    int stringnr = 1 + ny*ix + iy;
                    int omnr = iz + 1;
                    OMKey omkey(stringnr,omnr);
                    bool outside = ( ballmapptr->find(omkey) == ballmapptr->end());
                    bool inside = !outside;
                    if ( should_be_inside ){
                        sprintf(msg, "ball with radius %.3f, DOM at %.3f was "
                                     "rejected but should be included",
                                     sqrt(radius2),sqrt(r2) );
                        ENSURE( inside, msg );
                    } else {
                        sprintf(msg, "ball with radius %.3f, DOM at %.3f was "
                                     "kept but should be excluded",
                                     sqrt(radius2),sqrt(r2) );
                        ENSURE( outside, msg );
                    }
                }
            }
        }
    }
}
