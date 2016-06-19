#ifndef beam_H
#define beam_H

#include<map>
#include<set>
#include<numeric>
#include<vector>

class beam{
public:
    double rRestraint=0;
    std::map<double,double> loads;
    std::map<double,double> meLoads;
    double leng,Ela,intia;

    beam(double length, double e, double Iz):leng(length), Ela(e), intia(Iz) {};
    virtual ~beam(){}

    virtual double wanju(const double& pos)=0;
    virtual double neili(const double&)=0;
    virtual double bending(const double&)=0;
    //virtual void calc();

    void loadjizhongli(double pos, double force);

    void loadMoment(double pos, double me);

    void loadlin(double& pos1,double& pos2,double& force1,double& force2);

    double thetaA();
    double thetaB();

};


class simpSupBeam:public beam{
public:
    simpSupBeam(double length, double e, double Iz) : beam(length,e,Iz) {};
    double bending(const double& pos);
    double wanju(const double& pos);
    double neili(const double&);
    //double thetaA();
    //double thetaB();

};


class overHangingBeam : public beam{
public:


    overHangingBeam(double length, double e, double Iz,double rRes):beam(length,e,Iz){rRestraint=rRes; }
    overHangingBeam(double length, double e, double Iz):beam(length,e,Iz){rRestraint=length/2.0; }

    double wanju(const double& pos);
    double neili(const double&);

    double bending(const double&);

    //double thetaA();
    //double thetaB();


};

class continBeam{
public:
    continBeam(double length, double e, double Iz):leng(length), Ela(e), intia(Iz)
    {
        resPos.push_back(0.0);
        beams.push_back(new overHangingBeam(1,Ela,intia,.5));
        addRes(length);}
    ~continBeam()
    {
        int size=beams.size();
        for(int i=0;i<size;i++){
            delete beams[i];
        }
    }

    std::vector<double> resPos;
    std::vector<beam*> beams;
    void addRes(double pos);
    void loadjizhongli(double pos, double force);

    void loadMoment(double pos, double me);

    void loadlin(double pos1,double pos2,double force1,double force2);

    double leng,Ela,intia;
    std::set<double> jizhongPos;

    double bend[101]={0};
    double Fsy[101]={0};
    double innerMz[101]={0};
    std::map<double,double> loads;
    std::map<double,double> meLoads;

    void calc();
    double bending(const double& pos);
    double wanju(const double& pos);
    double neili(const double&);

};

#endif // beam_H
